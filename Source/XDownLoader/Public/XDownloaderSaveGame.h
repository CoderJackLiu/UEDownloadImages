// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "XDownloaderTypes.h"
#include "XDownloaderSaveGame.generated.h"

/**
 *
 */
UCLASS()
class XDOWNLOADER_API UXDownloaderSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//image caches data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UXDownloaderSaveGame")
	TArray<FXDownloadImageCached> ImageCaches;

	//add image cache
	void AddImageCache(const FXDownloadImageCached& IMageInstance);

	//get image cache
	FXDownloadImageCached* GetImageCache(const FString& ImageID);

	//has image cache
	bool HasImageCache(const FString& ImageID) const;
	
	
	//release save game
	void ReleaseSaveGame();
	
	static FString SlotName;
	static int32 UserIndex;

private:
	//UXDownloaderSaveGame* XDownloaderSaveGame;
};
