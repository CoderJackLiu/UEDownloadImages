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

/**
 * @class UXDownloadManager
 * @brief Class for managing image downloading tasks asynchronously.
 *
 * This class provides functionality to download images asynchronously using a task-based approach.
 */
UCLASS()
class XDOWNLOADER_API UXDownloadManager : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

private:
	void InitParas(const FString& InSaveGameSlotName);

public:
	/**
	 * @brief This variable is an event dispatcher that will be triggered whenever there is a change in the total download progress.
	 *
	 * The OnTotalDownloadProgress event allows other objects or components to register and handle the download progress change events in the XDownload category. Any changes to the total
	 * download progress will trigger this event, providing a way to track and react to the progress in real-time.
	 *
	 * @see FOnDownloadProgressChanged
	 * @see XDownload
	 *
	 * BlueprintAssignable: This macro allows the event to be assignable in Blueprints, allowing for Blueprint scripting integration.
	 *
	 * Category: This variable is categorised under "XDownload" category, indicating its relevance to the XDownload component or system.
	 */
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadProgressChanged OnTotalDownloadProgress;

	/**
	 * @brief A delegate that is called when the total download for a specific task has succeeded.
	 *
	 * This delegate is used in the "XDownload" category to notify when the total download for a specific task has been completed successfully.
	 */
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadStatusChanged OnTotalDownloadSucceed;

	/**
	 * @brief A delegate used to notify when the total download fails in XDownload.
	 * @details This delegate is BlueprintAssignable and can be bound in Unreal Engine's Blueprint system.
	 *
	 * - Category: XDownload
	 * - Bindable: Yes
	 *
	 * @see FOnDownloadStatusChanged
	 */
	UPROPERTY(BlueprintAssignable, Category="XDownload")
	FOnDownloadStatusChanged OnTotalDownloadFailed;


	int32 MaxRetryTimes;

	float DownloadTimeoutSecond;

	/**
	 * Downloads images from the given tasks and saves them to the specified save game slot.
	 *
	 * @param Tasks The array of image download tasks.
	 * @param InSaveGameSlotName The name of the save game slot to save the downloaded images.
	 *
	 * @return An instance of UXDownloadManager that handles the image download process.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "XDownload")
	static UXDownloadManager* DownloadImages(const TArray<FImageDownloadTask>& Tasks, FString InSaveGameSlotName = "");

	/**
	 * @brief Starts the download of image tasks.
	 *
	 * This method starts the download of a given array of image tasks.
	 *
	 * @param Tasks The array of image download tasks to start.
	 * @param MaxDownloads The maximum number of concurrent downloads (default value is 3).
	 */
	void ExecuteTask(const TArray<FImageDownloadTask>& Tasks, int32 MaxDownloads = 3);

	/**
	 * Invoked when a sub-task of downloading an image is finished.
	 *
	 * @param HttpRequest - Pointer to the HTTP request object.
	 * @param Response - Pointer to the HTTP response object.
	 * @param bWasSuccessful - Flag indicating whether the sub-task was successful or not.
	 * @param ImageID - The ID of the image being downloaded.
	 * @param ImageURL - The URL of the image being downloaded.
	 */
	void OnSubTaskFinished(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, FString ImageID, FString ImageURL);

	/**
	 * @brief Maximum number of parallel downloads allowed.
	 *
	 * This variable represents the maximum number of download operations that can be performed in parallel when using the UXDownloadManager class.
	 * It is a static member variable of the UXDownloadManager class.
	 * Changes to this value will affect all instances of the UXDownloadManager class. The default value is 5.
	 */
	static int32 MaxParallelDownloads;


	/**
	 * @brief The number of parallel downloads currently being processed.
	 *
	 * This variable tracks the number of parallel downloads that are currently
	 * being processed by the UXDownloadManager class. It is a static variable
	 * shared among all instances of the class, and is initialized to 0.
	 *
	 * @note The value of this variable should only be modified via the
	 *       UXDownloadManager class methods.
	 */
	static int32 CurrentParallelDownloads;

	/**
	 * @brief A static variable representing the task queue for image download tasks.
	 *
	 * The TaskQueue variable is of type TQueue<FImageDownloadTask> and is used to store and manage the image download tasks.
	 * It is a static member of the UXDownloadManager class, which means that it is shared among all instances of the class.
	 * The TaskQueue variable can be accessed and modified by any member function of the UXDownloadManager class.
	 * It is recommended to use appropriate synchronization mechanisms when accessing or modifying the TaskQueue variable in a multi-threaded environment.
	 */
	static TQueue<FImageDownloadTask> TaskQueue;

	/**
	 * @brief DownloadFailNum
	 *
	 * Variable to keep track of the number of download failures.
	 *
	 * The DownloadFailNum variable is an integer used to store the number
	 * of times a download has failed. It is initially set to 0 and is
	 * incremented whenever a download fails. The variable can be queried or
	 * modified to obtain or update the current count of download failures.
	 *
	 * @see DownloadFailNum if you need to track failed downloads.
	 */
	int32 DownloadFailNum = 0;

	/**
	 * Executes a download task.
	 *
	 * This method is responsible for executing a download task for a given image. If the image is already
	 * cached, the task is considered completed and the result is returned with the cached image data. Otherwise,
	 * a new HTTP request is created to download the image from the given URL. The progress and completion
	 * callbacks are set, and the request is processed.
	 *
	 * @param Task The download task information, including the image ID and URL.
	 */
	void ExecuteDownloadTask(const FImageDownloadTask& Task);

	/**
	 * @brief Initializes the download task.
	 *
	 * This method clears the event delegates for successful, failed, and progress
	 * downloads. It also clears the task queue and resets the current parallel
	 * downloads, download fail count, and total download result.
	 *
	 */
	void InitTask();

	/**
	 * @brief Represents the result of a total download.
	 *
	 * The TotalDownloadResult structure encapsulates information about the result
	 * of a total download operation. It contains the total number of bytes downloaded,
	 * as well as any additional metadata related to the download.
	 *
	 * @note This structure should be used after a total download has been completed.
	 */
	FTotalDownloadResult TotalDownloadResult;

private:
	/**
	 * @brief Destroys the task.
	 *
	 * This method is responsible for destroying the task. It unbinds the callbacks for request completion and request progress, cancels the request, clears the list of download requests
	 *, sets the reference to the game world to nullptr, removes the task from the root, and prints a log message indicating that the task has been destroyed.
	 *
	 * Use this method to properly clean up and destroy a task.
	 *
	 * @see InitTask()
	 */
	void DestroyTask();

	/**
	 * @brief Flag indicating whether the download should be stopped.
	 *
	 * This flag is a boolean variable that indicates whether the download process
	 * should be stopped or not. When the flag is set to true, the download process
	 * will be stopped.
	 *
	 * @note The initial value of this flag is false, which means the download will
	 *       not be stopped by default.
	 */
	bool bStopDownload = false;

	/**
	 * Makes a sub task succeed.
	 *
	 * This method is called when a sub task of a download operation has successfully completed.
	 * It updates the download progress, adds the sub task result to the total download result, and checks if all tasks have finished.
	 * If all tasks have finished, the necessary actions are taken to mark all tasks as finished.
	 *
	 * @param InTask The download result of the sub task that succeeded.
	 */
	void MakeSubTaskSucceed(const FDownloadResult& InTask);

	/**
	 * @brief Makes a subtask error.
	 *
	 * This method is called when a subtask of a download task encounters an error. It handles logging the error,
	 * updating the download fail count, decrementing the number of current parallel downloads, and adding the subtask
	 * result to the total download result. If there are no more parallel downloads, it calls MakeAllTaskFinished(),
	 * otherwise it updates the progress and dequeues the next task from the task queue to execute it.
	 *
	 * @param InTaskResult The result of the subtask that encountered the error.
	 */
	void MakeSubTaskError(const FDownloadResult& InTaskResult);

	/**
	 * @brief Updates the progress of all downloads.
	 *
	 * This method is responsible for updating the progress of all downloads in the UXDownloadManager.
	 * It should be called periodically to keep track of the ongoing downloads and notify the progress changes.
	 *
	 */
	void UpdateAllProgress();

	/**
	 * @brief Executes all remaining tasks in the download manager.
	 *
	 * This method executes all remaining tasks in the download manager.
	 * If any tasks failed, it broadcasts an event notifying the failure.
	 * If all tasks succeed, it broadcasts an event notifying the success.
	 * Finally, it destroys the tasks.
	 *
	 */
	void MakeAllTaskFinished();

	/**
	 * Updates the progress of a sub-task within the download manager.
	 *
	 * This method is called to update the progress of a sub-task within the download manager. It calculates the progress based on the number of bytes sent and received, and updates the
	 * corresponding sub-task's progress accordingly. If the game world is not valid, the sub-task and its associated resources are destroyed.
	 *
	 * @param Request The HTTP request associated with the sub-task.
	 * @param BytesSent The number of bytes sent for the sub-task.
	 * @param BytesReceived The number of bytes received for the sub-task.
	 * @param ImageID The ID of the image associated with the sub-task.
	 */
	void MakeSubTaskProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived, FString ImageID);

	/**
	 * @brief The GameWorld variable represents the Unreal Engine world the game is running in.
	 *
	 * @details The GameWorld variable is a static member of the UXDownloadManager class and it is used to store a pointer to the UWorld object.
	 * This variable is of type UWorld*, which is a pointer to the Unreal Engine World object.
	 *
	 * The GameWorld variable is declared as static to allow easy access to it from different parts of the codebase, without having to pass the UWorld object as a parameter.
	 *
	 * The initial value of the GameWorld variable is nullptr, which means that it is not pointing to any valid UWorld object.
	 *
	 * Note that being a static member, the GameWorld variable is shared among all instances of the UXDownloadManager class.
	 * Changing its value in one instance will affect all instances.
	 *
	 * In order to use the GameWorld variable, you should include the "UXDownloadManager.h" header file in your code.
	 *
	 * Example usage:
	 * @code
	 * // Set the GameWorld variable to the current Unreal Engine world
	 * UXDownloadManager::GameWorld = GetWorld();
	 *
	 * // Check if the GameWorld variable is pointing to a valid UWorld object
	 * if (!IsGameWorldValid())
	 * {
	 *     // Do something with the GameWorld object
	 * }
	 * @endcode
	 */
	static UWorld* GameWorld;

	/**
	 * @brief Checks if the game world is valid.
	 *
	 * @return true if the game world is valid, false otherwise.
	 */
	static bool IsGameWorldValid();
	/**
	 * Retrieves the game world.
	 *
	 * @return A pointer to the game world if found, otherwise nullptr.
	 */
	static UWorld* GetGameWorld();

	/**
	 * @brief A variable holding an array of shared references to HTTP download requests.
	 *
	 * This variable is used to store a collection of shared references to
	 * `IHttpRequest` objects, representing HTTP download requests. Each element of
	 * the array maintains a thread-safe reference to an `IHttpRequest` object.
	 *
	 * Note that this variable is not thread-safe, and concurrent access to it should
	 * be synchronized externally to avoid race conditions.
	 */
	TArray<TSharedRef<IHttpRequest, ESPMode::ThreadSafe>> DownLoadRequests;

	/**
	 * Loads an image from a byte buffer and returns a UTexture2DDynamic object.
	 * The image format can be PNG, JPEG, or BMP.
	 *
	 * @param ImageBuffer The byte buffer containing the image data.
	 * @return A UTexture2DDynamic object if the image is successfully loaded, nullptr otherwise.
	 */
	static UTexture2DDynamic* LoadImageFromBuffer(const TArray<uint8>& ImageBuffer);

	/**
	 * @class DownloaderSaveGame
	 *
	 * @brief The DownloaderSaveGame class represents a save game object for the downloader.
	 *
	 * This class is used to store information related to the save game as part of the downloader functionality.
	 * It inherits from the UObject class and is declared as a UPROPERTY with the Transient specifier.
	 *
	 * @see UXDownloaderSaveGame
	 *
	 */
	UPROPERTY(Transient)
	class UXDownloaderSaveGame* DownloaderSaveGame;

	ECacheType CacheType = ECacheType::CT_SaveGame;

	UPROPERTY(Transient)
	class UXDownloaderSubsystem* DownloaderSubsystem;

	FString SaveGameSlotName;
	
	FString DownloadImageDefaultPath;


	bool ImageHasCached(FString FileName);

	//download image
	void DownloadImage(const FString& ImageURL, const FString& ImageID);
	
};
