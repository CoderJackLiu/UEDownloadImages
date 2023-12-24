// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "XDownloaderTypes.h"
#include "XDownloaderSaveGame.generated.h"

/**
 * @class UXDownloaderSaveGame
 * @brief A save game class used by the UXDownloader for caching image data.
 */
UCLASS()
class XDOWNLOADER_API UXDownloaderSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//image caches data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UXDownloaderSaveGame")
	TArray<FXDownloadImageCached> ImageCaches;

	//init slot name
	void SetSlotName(const FString& InSlotName);

	/**
	 * Adds an image cache to the save game object.
	 *
	 * @param IMageInstance The image cache object to add.
	 * @param NewSlotName (Optional) The name of the slot to save the game. If not provided, the default slot name will be used.
	 */
	void AddImageCache(const FXDownloadImageCached& IMageInstance, FString NewSlotName = "");

	/**
	 * Retrieves an image cache with the specified ImageID.
	 *
	 * @param ImageID The ID of the image cache to retrieve.
	 * @return A pointer to the image cache with the specified ImageID, or nullptr if no cache is found.
	 */
	FXDownloadImageCached* GetImageCache(const FString& ImageID);

	/**
	 * Checks if an image cache with the specified ImageID exists.
	 *
	 * @param ImageID The ID of the image cache to check.
	 * @return True if an image cache with the specified ImageID exists, false otherwise.
	 */
	bool HasImageCache(const FString& ImageID) const;


	/**
	 * Release the save game object by removing all image caches.
	 *
	 * @see FXDownloadImageCached
	 */
	void ReleaseSaveGame(bool bClearImageCaches = false);

	void SaveImageCacheData();
	
	FString SlotNameOverride;
	
	FString DefaultSlotName = "XDownloaderImagesCaches";
	
	static int32 UserIndex;

private:
	//UXDownloaderSaveGame* XDownloaderSaveGame;
};
