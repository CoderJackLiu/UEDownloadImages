// UDownloadManager.cpp
#include "XDownloadManager.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "XDownloaderSaveGame.h"
#include "XDownloaderSubsystem.h"
#include "Engine/Texture2DDynamic.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

int32 UXDownloadManager::MaxParallelDownloads = 5;
int32 UXDownloadManager::CurrentParallelDownloads = 0;
static FCriticalSection ExecutingXDownloadTaskPoolLock;
TQueue<FImageDownloadTask> UXDownloadManager::TaskQueue;
UWorld* UXDownloadManager::GameWorld = nullptr;

UXDownloadManager* UXDownloadManager::DownloadImages(const TArray<FImageDownloadTask>& Tasks)
{
	GameWorld = GetGameWorld();
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	UXDownloadManager* DownloadMgr = NewObject<UXDownloadManager>();
	DownloadMgr->AddToRoot(); // 防止垃圾回收
	DownloadMgr->InitTask();
	DownloadMgr->StartDownload(Tasks);
	return DownloadMgr;
}

void UXDownloadManager::StartDownload(const TArray<FImageDownloadTask>& Tasks, int32 MaxDownloads)
{
	//get game instance subsystem
	UXDownloaderSubsystem* DownloaderSubsystem = UGameInstance::GetSubsystem<UXDownloaderSubsystem>(GetGameWorld()->GetGameInstance());
	DownloaderSaveGame = DownloaderSubsystem->GetSaveGame();

	// CurrentTasks = Tasks;
	// 当前整个类下载数量
	CurrentParallelDownloads = 0;

	TotalDownloadResult.TotalNum = Tasks.Num();
	for (auto ImageDownloadTask : Tasks)
	{
		TaskQueue.Enqueue(ImageDownloadTask);
	}
	const int32 CanDownloadNum = FMath::Min(FMath::Min(MaxParallelDownloads - CurrentParallelDownloads, Tasks.Num()), MaxDownloads);
	for (int i = 0; i < CanDownloadNum; ++i)
	{
		FImageDownloadTask Task;
		if (TaskQueue.Dequeue(Task))
		{
			FPlatformAtomics::InterlockedIncrement(&CurrentParallelDownloads);
			ExecuteDownloadTask(Task);
		}
	}
}

void UXDownloadManager::OnSubTaskFinished(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, FString ImageID, FString ImageURL)
{
	AsyncTask(ENamedThreads::GameThread, [this, Response, bWasSuccessful,ImageID,ImageURL]()
	{
		if (bStopDownload)
		{
			bStopDownload = true;
			return;
		}
		// ...
		// 下载完成的处理逻辑
		// ...
		FDownloadResult Result;
		Result.ImageID = ImageID;
		Result.ImageURL = ImageURL;
		if (bWasSuccessful)
		{
			if (Response.IsValid() && Response->GetContentLength() > 0)
			{
				const TArray<uint8> Content = Response->GetContent();
				Result.ImageData = Content;
				//save to disk
				const FString FilePath = FPaths::ProjectSavedDir() + TEXT("DownloadImages/") + ImageID + TEXT(".png");
				FFileHelper::SaveArrayToFile(Content, *FilePath);
				Result.Status = EDownloadStatus::Success;

				FXDownloadImageCached ImageCached;
				ImageCached.ImageID = ImageID;
				ImageCached.ImageURL = ImageURL;
				ImageCached.ImageData = Content;
				DownloaderSaveGame->AddImageCache(ImageCached);
				//todo Result.Texture
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
	});
}

void UXDownloadManager::ExecuteDownloadTask(const FImageDownloadTask& Task)
{

	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this,Task]()
	{
		if (DownloaderSaveGame->HasImageCache(Task.ImageID))
		{
			if (const FXDownloadImageCached* Cache = DownloaderSaveGame->GetImageCache(Task.ImageID))
			{
				FDownloadResult Result;
				Result.ImageID = Task.ImageID;
				Result.ImageURL = Task.ImageURL;
				Result.Status = EDownloadStatus::Failed;
				Result.ImageData = Cache->ImageData;
				Result.Texture = Cache->Texture;
				MakeSubTaskSucceed(Result);
				return;
			}
		}
		else
		{
			// ...
		   // 下载逻辑
		   // ...
		   const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		   DownLoadRequests.Add(HttpRequest);
		   HttpRequest->SetURL(Task.ImageURL);
		   HttpRequest->SetVerb(TEXT("GET"));

		   HttpRequest->OnRequestProgress().BindUObject(this, &UXDownloadManager::MakeSubTaskProgress, Task.ImageID);

		   HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXDownloadManager::OnSubTaskFinished, Task.ImageID, Task.ImageURL);
		   HttpRequest->SetHeader("ImageID", Task.ImageID);
		   HttpRequest->SetHeader("ImageURL", Task.ImageURL);
		   HttpRequest->ProcessRequest();
		}
	});
}

void UXDownloadManager::InitTask()
{
	OnTotalDownloadSucceed.Clear();
	OnTotalDownloadFailed.Clear();
	OnTotalDownloadProgress.Clear();
	TaskQueue.Empty();
	CurrentParallelDownloads = 0;
	DownloadFailNum = 0;
	TotalDownloadResult = FTotalDownloadResult();
}

void UXDownloadManager::DestroyTask()
{
	// ...
	// 销毁逻辑
	// ...
	InitTask();
	for (const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> DownLoadRequest : DownLoadRequests)
	{
		DownLoadRequest->OnProcessRequestComplete().Unbind();
		DownLoadRequest->OnRequestProgress().Unbind();
		DownLoadRequest.Get().CancelRequest();
		//log
		UE_LOG(LogTemp, Warning, TEXT("DownloadManager http request unbind  url is  %s !!!!"), *DownLoadRequest->GetURL());
	}
	DownLoadRequests.Empty();
	GameWorld = nullptr;
	//log destroy func
	UE_LOG(LogTemp, Warning, TEXT("DownloadManager Destroy!!!"));
	RemoveFromRoot();
}

void UXDownloadManager::MakeSubTaskSucceed(const FDownloadResult& InTaskResult)
{
	// ...
	// 完成的处理逻辑
	// ...
	//log succeed
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	{
		FPlatformAtomics::InterlockedDecrement(&CurrentParallelDownloads);
		UE_LOG(LogTemp, Warning, TEXT("Download Succeed!!! ImageID :%s ,URL:%s"), * InTaskResult.ImageID, *InTaskResult.ImageURL);
		UpdateAllProgress();
		TotalDownloadResult.SubTaskDownloadResults.Add(InTaskResult);

		if (!IsGameWorldValid())
		{
			DestroyTask();

			return;
		}


		if (CurrentParallelDownloads == 0)
		{
			MakeAllTaskFinished();
		}
		else
		{
			FImageDownloadTask Task;
			if (TaskQueue.Dequeue(Task))
			{
				FPlatformAtomics::InterlockedIncrement(&CurrentParallelDownloads);
				ExecuteDownloadTask(Task);
				// CurrentTasks.Remove(Task);
			}
		}
	}
}

void UXDownloadManager::MakeSubTaskError(const FDownloadResult& InTaskResult)
{
	// ...
	// 错误处理逻辑
	// 继续执行下一个任务
	// ...
	FScopeLock ScopeLock(&ExecutingXDownloadTaskPoolLock);
	{
		//log error
		UE_LOG(LogTemp, Error, TEXT("Download failed!!!"));
		FPlatformAtomics::InterlockedIncrement(&DownloadFailNum);
		FPlatformAtomics::InterlockedDecrement(&CurrentParallelDownloads);
		TotalDownloadResult.SubTaskDownloadResults.Add(InTaskResult);
		//log error  log InTaskResult.ImageID
		UE_LOG(LogTemp, Error, TEXT("Download failed!!! ImageID :%s ,URL:%s"), * InTaskResult.ImageID, *InTaskResult.ImageURL);

		if (CurrentParallelDownloads == 0)
		{
			MakeAllTaskFinished();
		}
		else
		{
			UpdateAllProgress();
			FImageDownloadTask Task;
			if (TaskQueue.Dequeue(Task))
			{
				ExecuteDownloadTask(Task);
				// CurrentTasks.Remove(Task);
			}
		}
	}
}

void UXDownloadManager::UpdateAllProgress()
{
	// ...
	// 进度更新逻辑
	// ...
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		//log progress
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
}

void UXDownloadManager::MakeSubTaskProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived, FString ImageID)
{
	// ...
	// 请求进度更新逻辑
	// ...

	//log progress
	if (!IsGameWorldValid())
	{
		DestroyTask();
		return;
	}
	const float Progress = static_cast<float>(BytesReceived) / static_cast<float>(Request->GetResponse()->GetContentLength());
	// UE_LOG(LogTemp, Warning, TEXT("Download progress!!! ImageID :%s ,Progress:%f"), *ImageID, Progress);
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
