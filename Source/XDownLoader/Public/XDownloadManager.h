// UDownloadManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XDownloaderTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "XDownloadManager.generated.h"

// 声明下载状态改变的事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDownloadStatusChanged, const FTotalDownloadResult&, DownloadResult);

// 声明下载进度改变的事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDownloadProgressChanged, const FTotalDownloadResult&, DownloadProgress);

// 下载管理类
UCLASS()
class XDOWNLOADER_API UXDownloadManager : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// 总任务下载进度改变的事件
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadProgressChanged OnTotalDownloadProgress;

	// 总任务下载成功
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadStatusChanged OnTotalDownloadSucceed;

	//总任务下载失败
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadStatusChanged OnTotalDownloadFailed;

	//todo 子任务下载进度事件
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "XDownload")
	static UXDownloadManager* DownloadImages(const TArray<FImageDownloadTask>& Tasks);

	// 启动下载的方法,
	// 参数为下载任务列表 以及最大并行下载数量
	void StartDownload(const TArray<FImageDownloadTask>& Tasks, int32 MaxDownloads = 3);
	void OnSubTaskFinished(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, FString ImageID, FString ImageURL);

	//最大下载数量
	static int32 MaxParallelDownloads;

	
	//当前下载数量
	static int32 CurrentParallelDownloads;

	// 下载任务队列
	static TQueue<FImageDownloadTask> TaskQueue;

	// TArray<FImageDownloadTask> CurrentTasks;

	int32 DownloadFailNum = 0;

	//执行下载任务
	void ExecuteDownloadTask(const FImageDownloadTask& Task);
	void InitTask();


	FTotalDownloadResult TotalDownloadResult;

private:
	//Destroy Task
	void DestroyTask();

	bool bStopDownload = false;

	// 当下载完成时调用的回调方法
	void MakeSubTaskSucceed(const FDownloadResult& InTask);

	// 处理下载错误的方法
	void MakeSubTaskError(const FDownloadResult& InTaskResult);

	// 更新下载进度的方法
	void UpdateAllProgress();

	// 总任务下载完成方法
	void MakeAllTaskFinished();

	// 当请求进度更新时调用的方法
	void MakeSubTaskProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived, FString ImageID) ;

	static UWorld* GameWorld;

	static bool IsGameWorldValid();
	static UWorld* GetGameWorld();

	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> DownLoadRequests;

	static UTexture2DDynamic* LoadImageFromBuffer(const TArray<uint8>& ImageBuffer);



	UPROPERTY(Transient)
	class UXDownloaderSaveGame* DownloaderSaveGame;

};
