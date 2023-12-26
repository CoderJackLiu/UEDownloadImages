// UDownloadManager.cpp
#include "XDownloadManager.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "XDownloaderSaveGame.h"
#include "XDownloaderSettings.h"
#include "XDownloaderSubsystem.h"
#include "Engine/Texture2DDynamic.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

int32 UXDownloadManager::MaxParallelDownloads = 5;
int32 UXDownloadManager::CurrentParallelDownloads = 0;
static FCriticalSection ExecutingXDownloadTaskPoolLock;
TQueue<FImageDownloadTask> UXDownloadManager::TaskQueue;
UWorld* UXDownloadManager::GameWorld = nullptr;
TArray<UXDownloadManager*> UXDownloadManager::DownloadManagers;

void UXDownloadManager::InitParas(const FString& InSaveGameSlotName)
{
	DownloaderSubsystem = UGameInstance::GetSubsystem<UXDownloaderSubsystem>(GameWorld->GetGameInstance());
	if (!DownloaderSubsystem)
	{
		//log
		UE_LOG(LogTemp, Error, TEXT("DownloaderSubsystem is null,you need run the XDownloader at Runtime!!!,if you need editor mode , please contect me by github issuse!!!"));
		return;
	}
	SaveGameSlotName = InSaveGameSlotName.IsEmpty() ? DownloaderSubsystem->GetXDownloadSettings()->GetSaveGameDefaultSlotName() : InSaveGameSlotName;
	CacheType = DownloaderSubsystem->GetXDownloadSettings()->GetCacheType();
	DownloaderSaveGame = DownloaderSubsystem->GetSaveGame(SaveGameSlotName);
	MaxParallelDownloads = DownloaderSubsystem->GetXDownloadSettings()->GetMaxParallelDownloads();
	MaxRetryTimes = DownloaderSubsystem->GetXDownloadSettings()->GetMaxRetryTimes();
	DownloadTimeoutSecond = DownloaderSubsystem->GetXDownloadSettings()->GetDownloadTimeout();
	DownloadImageDefaultPath = DownloaderSubsystem->GetXDownloadSettings()->GetDownloadImageDefaultPath();
}

UXDownloadManager* UXDownloadManager::DownloadImages(const TArray<FImageDownloadTask>& Tasks, FString InSaveGameSlotName)
{
	GameWorld = GetGameWorld();
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	UXDownloadManager* DownloadMgr = NewObject<UXDownloadManager>();
	DownloadMgr->AddToRoot(); // 防止垃圾回收
	DownloadManagers.Add(DownloadMgr);
	DownloadMgr->InitTask();
	DownloadMgr->InitParas(InSaveGameSlotName);
	DownloadMgr->ExecuteTask(Tasks);
	return DownloadMgr;
}

void UXDownloadManager::ExecuteTask(const TArray<FImageDownloadTask>& Tasks, int32 MaxDownloads)
{
	//get game instance subsystem
	if (!IsGameWorldValid())
	{
		return;
	}
	TotalDownloadResult.TotalNum = Tasks.Num();
	CurrentTasks = Tasks;
	// FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	for (auto ImageDownloadTask : Tasks)
	{
		TaskQueue.Enqueue(ImageDownloadTask);
	}
	// const int32 CanDownloadNum = FMath::Min(FMath::Min(MaxParallelDownloads - CurrentParallelDownloads, Tasks.Num()), MaxDownloads);
	int32 CanDownloadNum = FMath::Min(MaxParallelDownloads - CurrentParallelDownloads, Tasks.Num());
	for (int i = 0; i < CanDownloadNum; ++i)
	{
		FImageDownloadTask Task;
		if (TaskQueue.Dequeue(Task))
		{
			if (const FImageDownloadTask* FinishItem = CurrentTasks.FindByKey(Task.ImageID))
			{
				const FImageDownloadTask RemoveItem = *FinishItem;
				CurrentTasks.Remove(RemoveItem);
				ExecuteDownloadTask(Task);
			}
		}
	}
}

void UXDownloadManager::OnSubTaskFinished(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, FString ImageID, FString ImageURL)
{
	if (bStopDownload)
	{
		bStopDownload = true;
		return;
	}
	FDownloadResult Result;
	Result.ImageID = ImageID;
	Result.ImageURL = ImageURL;
	if (bWasSuccessful)
	{
		if (Response.IsValid() && Response->GetContentLength() > 0)
		{
			const TArray<uint8> Content = Response->GetContent();
			Result.ImageData = Content;
			Result.Status = EDownloadStatus::Success;
			Result.Texture = FImageUtils::ImportBufferAsTexture2D(Content);
			if (CacheType == ECacheType::CT_LocalFile)
			{
				//save to disk
				const FString FilePath = FPaths::Combine(DownloadImageDefaultPath, ImageID);
				FFileHelper::SaveArrayToFile(Content, *FilePath);
			}
			else if (CacheType == ECacheType::CT_SaveGame)
			{
				//save to disk
				FXDownloadImageCached ImageCached;
				ImageCached.ImageID = ImageID;
				ImageCached.ImageURL = ImageURL;
				ImageCached.ImageData = Content;
				ImageCached.Texture = Result.Texture;
				DownloaderSaveGame->AddImageCache(ImageCached, SaveGameSlotName);
			}
			else
			{
				const FString FilePath = FPaths::Combine(DownloadImageDefaultPath, ImageID);
				FFileHelper::SaveArrayToFile(Content, *FilePath);
				//save to disk
				FXDownloadImageCached ImageCached;
				ImageCached.ImageID = ImageID;
				ImageCached.ImageURL = ImageURL;
				ImageCached.ImageData = Content;
				ImageCached.Texture = Result.Texture;
				DownloaderSaveGame->AddImageCache(ImageCached, SaveGameSlotName);
			}
			MakeSubTaskSucceed(Result);
		}
		else
		{
			Result.Status = EDownloadStatus::Failed;
			MakeSubTaskError(Result);
		}
	}
	else
	{
		Result.Status = EDownloadStatus::Failed;
		MakeSubTaskError(Result);
	}
}

void UXDownloadManager::ExecuteDownloadTask(const FImageDownloadTask& Task)
{
	FPlatformAtomics::InterlockedIncrement(&CurrentTaskDownloadingNum);
	FPlatformAtomics::InterlockedIncrement(&CurrentParallelDownloads);
	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this,Task]()
	{
		const FString FilePath = FPaths::Combine(DownloadImageDefaultPath, Task.ImageID);
		TArray<uint8> Content;
		FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
		{
			bool HasCache = false;
			switch (CacheType)
			{
			case ECacheType::CT_SaveGame:

				if (const FXDownloadImageCached* Cache = DownloaderSaveGame->GetImageCache(Task.ImageID))
				{
					FDownloadResult Result;
					Result.ImageID = Task.ImageID;
					Result.ImageURL = Task.ImageURL;
					Result.Status = EDownloadStatus::Success;
					Result.ImageData = Cache->ImageData;
					Result.Texture = Cache->Texture;
					HasCache = true;
					MakeSubTaskSucceed(Result);
				}
				else
				{
					HasCache = false;
				}
				break;
			case ECacheType::CT_LocalFile:
				if (ImageHasCached(Task.ImageID))
				{
					FFileHelper::LoadFileToArray(Content, *FilePath);
					FDownloadResult Result;
					Result.ImageID = Task.ImageID;
					Result.ImageURL = Task.ImageURL;
					Result.Status = EDownloadStatus::Success;
					Result.ImageData = Content;
					Result.Texture = FImageUtils::ImportBufferAsTexture2D(Content);
					HasCache = true;
					MakeSubTaskSucceed(Result);
				}
				else
				{
					HasCache = false;
				}
				break;
			case ECacheType::CT_BothSaveGameAndFile:
				if (ImageHasCached(Task.ImageID))
				{
					FFileHelper::LoadFileToArray(Content, *FilePath);
					FDownloadResult Result;
					Result.ImageID = Task.ImageID;
					Result.ImageURL = Task.ImageURL;
					Result.Status = EDownloadStatus::Success;
					Result.ImageData = Content;
					Result.Texture = FImageUtils::ImportBufferAsTexture2D(Content);
					if (!DownloaderSaveGame->HasImageCache(Task.ImageID))
					{
						FXDownloadImageCached ImageCached;
						ImageCached.ImageID = Task.ImageID;
						ImageCached.ImageURL = Task.ImageURL;
						ImageCached.ImageData = Content;
						ImageCached.Texture = Result.Texture;
						DownloaderSaveGame->AddImageCache(ImageCached, SaveGameSlotName);
					}
					HasCache = true;
					MakeSubTaskSucceed(Result);
				}
				else if (const FXDownloadImageCached* Cache = DownloaderSaveGame->GetImageCache(Task.ImageID))
				{
					FDownloadResult Result;
					Result.ImageID = Task.ImageID;
					Result.ImageURL = Task.ImageURL;
					Result.Status = EDownloadStatus::Success;
					Result.ImageData = Cache->ImageData;
					Result.Texture = Cache->Texture;
					{
						//image file cache
						Content = Cache->ImageData;
						FFileHelper::SaveArrayToFile(Content, *FilePath);
					}
					HasCache = true;
					MakeSubTaskSucceed(Result);
				}
				else
				{
					HasCache = false;
				}
				break;
			}
			if (!HasCache)
			{
				DownloadImage(Task.ImageURL, Task.ImageID);
			}
		}
	});
}

void UXDownloadManager::InitTask()
{
	OnTotalDownloadSucceed.Clear();
	OnTotalDownloadFailed.Clear();
	OnTotalDownloadProgress.Clear();
	DownloadFailNum = 0;
	TotalDownloadResult = FTotalDownloadResult();
}

void UXDownloadManager::DestroyTask()
{
	for (const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> DownLoadRequest : DownLoadRequests)
	{
		DownLoadRequest->OnProcessRequestComplete().Unbind();
		DownLoadRequest->OnRequestProgress().Unbind();
		DownLoadRequest.Get().CancelRequest();
		UE_LOG(LogTemp, Warning, TEXT("DownloadManager http request unbind  url is  %s !!!!"), *DownLoadRequest->GetURL());
	}
	DownLoadRequests.Empty();
	DownloadManagers.Remove(this);
	UE_LOG(LogTemp, Warning, TEXT("DownloadManager Destroy!!!"));
	RemoveFromRoot();
}

void UXDownloadManager::MakeSubTaskSucceed(const FDownloadResult& InTaskResult)
{
	FPlatformAtomics::InterlockedDecrement(&CurrentParallelDownloads);
	FPlatformAtomics::InterlockedDecrement(&CurrentTaskDownloadingNum);
	//log succeed
	UE_LOG(LogTemp, Warning, TEXT("Download Succeed!!! ImageID :%s ,URL:%s"), * InTaskResult.ImageID, *InTaskResult.ImageURL);
	UpdateAllProgress();
	TotalDownloadResult.SubTaskDownloadResults.Add(InTaskResult);

	if (!IsGameWorldValid())
	{
		DestroyTask();
		return;
	}
	if (CurrentTasks.Num() == 0)
	{
		if (CurrentTaskDownloadingNum == 0)
		{
			MakeAllTaskFinished();
		}
	}
	else
	{
		ExecuteNextTask();
	}
}

void UXDownloadManager::MakeSubTaskError(const FDownloadResult& InTaskResult)
{
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	{
		//log error
		UE_LOG(LogTemp, Error, TEXT("Download failed!!!"));
		FPlatformAtomics::InterlockedIncrement(&DownloadFailNum);
		FPlatformAtomics::InterlockedDecrement(&CurrentParallelDownloads);
		FPlatformAtomics::InterlockedDecrement(&CurrentTaskDownloadingNum);
		TotalDownloadResult.SubTaskDownloadResults.Add(InTaskResult);
		UpdateAllProgress();
		//log error  log InTaskResult.ImageID
		UE_LOG(LogTemp, Error, TEXT("Download failed!!! ImageID :%s ,URL:%s"), * InTaskResult.ImageID, *InTaskResult.ImageURL);

		if (CurrentTasks.Num() == 0)
		{
			if (CurrentTaskDownloadingNum == 0)
			{
				MakeAllTaskFinished();
			}
		}
		else
		{
			ExecuteNextTask();
		}
	}
}

void UXDownloadManager::UpdateAllProgress()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Download progress!!!"));
		OnTotalDownloadProgress.Broadcast(TotalDownloadResult);
	});
}

void UXDownloadManager::MakeAllTaskFinished()
{
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			DownloaderSaveGame->SaveImageCacheData();
			if (DownloadFailNum)
			{
				OnTotalDownloadFailed.Broadcast(TotalDownloadResult);
				//log failed
				UE_LOG(LogTemp, Error, TEXT("Download total failed!!!"));
			}
			else
			{
				OnTotalDownloadSucceed.Broadcast(TotalDownloadResult);
				//log succeed
				UE_LOG(LogTemp, Warning, TEXT("Download total succeed!!!"));
			}
			DestroyTask();
		});
	}
	ExecuteNextTask();
}

void UXDownloadManager::MakeSubTaskProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived, FString ImageID)
{
	if (!IsGameWorldValid())
	{
		DestroyTask();
		return;
	}
	const float Progress = static_cast<float>(BytesReceived) / static_cast<float>(Request->GetResponse()->GetContentLength());
}

bool UXDownloadManager::IsGameWorldValid()
{
	return GameWorld && IsValid(GameWorld);
}

UWorld* UXDownloadManager::GetGameWorld()
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();

	//编辑器世界
#if WITH_EDITOR
	for (const FWorldContext& Context : WorldContexts)
	{
		if (Context.World() != nullptr)
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				return Context.World();
			}
			else if (Context.WorldType == EWorldType::Game)
			{
				return Context.World();
			}
		}
	}
	return nullptr;
#else
	//游戏世界
	for (const FWorldContext& Context : WorldContexts)
	{
		if (Context.World() != nullptr)
		{
			if (Context.WorldType == EWorldType::Game)
			{
				return Context.World();
			}
			if (Context.WorldType == EWorldType::GameRPC)
			{
				return Context.World();
			}
		}
	}
	return nullptr;
#endif
}

UTexture2DDynamic* UXDownloadManager::LoadImageFromBuffer(const TArray<uint8>& ImageBuffer)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrappers[3] =
	{
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
	};

	for (auto ImageWrapper : ImageWrappers)
	{
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageBuffer.GetData(), ImageBuffer.Num()))
		{
			TArray<uint8> RawData;
			const ERGBFormat InFormat = ERGBFormat::BGRA;
			if (ImageWrapper->GetRaw(InFormat, 8, RawData))
			{
				if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
				{
					Texture->SRGB = true;
					Texture->UpdateResource();
					FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
					if (TextureResource)
					{
						ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
							[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
							{
								TextureResource->WriteRawToTexture_RenderThread(RawData);
							});
					}
					return Texture;
				}
			}
		}
	}
	return nullptr;
}

bool UXDownloadManager::ImageHasCached(FString FileName)
{
	const FString fileFullName = FPaths::Combine(DownloadImageDefaultPath, FileName);
	//log file full name
	UE_LOG(LogTemp, Warning, TEXT("ImageHasCached,file name is %s"), *fileFullName);
	return FPaths::FileExists(fileFullName);
}

void UXDownloadManager::DownloadImage(const FString& ImageURL, const FString& ImageID)
{
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	{
		DownLoadRequests.Add(HttpRequest);
	}
	HttpRequest->SetURL(ImageURL);
	HttpRequest->SetTimeout(DownloadTimeoutSecond);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->OnRequestProgress().BindUObject(this, &UXDownloadManager::MakeSubTaskProgress, ImageID);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXDownloadManager::OnSubTaskFinished, ImageID, ImageURL);
	HttpRequest->SetHeader("ImageID", ImageID);
	HttpRequest->SetHeader("ImageURL", ImageURL);
	HttpRequest->ProcessRequest();
}

void UXDownloadManager::ExecuteNextTask()
{
	FImageDownloadTask Task;
	if (TaskQueue.Peek(Task))
	{
		for (UXDownloadManager* DownloadManager : DownloadManagers)
		{
			if (const FImageDownloadTask* FinishItem = DownloadManager->CurrentTasks.FindByKey(Task.ImageID))
			{
				const FImageDownloadTask RemoveItem = *FinishItem;
				if (DownloadManager->CurrentTasks.Remove(RemoveItem))
				{
					if (!TaskQueue.Dequeue(Task))
					{
						//log error dequeue failed but alone task object not finish 
						UE_LOG(LogTemp, Error, TEXT("dequeue failed but alone task object not finish!!!"));
					}
					DownloadManager->ExecuteDownloadTask(Task);
				}
				return;
			}
			else
			{
				//log this task does not contains this sub task
				UE_LOG(LogTemp, Warning, TEXT("this task does not contains this sub task!!!"));
			}
		}
	}
	else
	{
		//log alone task object not finish but queue is empty
		UE_LOG(LogTemp, Warning, TEXT("TaskQueue peek failed, all task finished dequeue!!!"));
	}
}
