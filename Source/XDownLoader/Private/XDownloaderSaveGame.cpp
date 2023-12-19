// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSaveGame.h"

#include "Kismet/GameplayStatics.h"

FString UXDownloaderSaveGame::SlotName = "XDownloaderImagesCahes";
int32 UXDownloaderSaveGame::UserIndex = 0;

void UXDownloaderSaveGame::AddImageCache(const FXDownloadImageCached& IMageInstance)
{
	ImageCaches.AddUnique(IMageInstance);
	UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
}

FXDownloadImageCached* UXDownloaderSaveGame::GetImageCache(const FString& ImageID)
{
	return ImageCaches.FindByKey(ImageID);
}

bool UXDownloaderSaveGame::HasImageCache(const FString& ImageID) const
{
	TArray<FXDownloadImageCached> LocalImageCaches = ImageCaches;
	for (FXDownloadImageCached& ImageCached : LocalImageCaches)
	{
		UE_LOG(LogTemp, Error, TEXT("ImageCach.ImageID: %s"), *ImageCached.ImageID);
	}
	return ImageCaches.Contains(ImageID);
}

void UXDownloaderSaveGame::ReleaseSaveGame()
{
	for (FXDownloadImageCached& ImageCach : ImageCaches)
	{
		ImageCach.Texture->RemoveFromRoot();
	}
}
