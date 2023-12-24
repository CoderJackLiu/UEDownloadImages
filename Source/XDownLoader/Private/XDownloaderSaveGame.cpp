// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSaveGame.h"

#include "Kismet/GameplayStatics.h"

int32 UXDownloaderSaveGame::UserIndex = 0;

void UXDownloaderSaveGame::SetSlotName(const FString& InSlotName)
{
	DefaultSlotName = InSlotName;
}

void UXDownloaderSaveGame::AddImageCache(const FXDownloadImageCached& IMageInstance, FString NewSlotName)
{
	SlotNameOverride = NewSlotName.IsEmpty() ? DefaultSlotName : NewSlotName;
	ImageCaches.AddUnique(IMageInstance);
}

FXDownloadImageCached* UXDownloaderSaveGame::GetImageCache(const FString& ImageID)
{
	return ImageCaches.FindByKey(ImageID);
}

bool UXDownloaderSaveGame::HasImageCache(const FString& ImageID) const
{
	return ImageCaches.Contains(ImageID);
}

void UXDownloaderSaveGame::ReleaseSaveGame(bool bClearImageCaches)
{
	for (const FXDownloadImageCached& ImageCached : ImageCaches)
	{
		ImageCached.Texture->RemoveFromRoot();
	}
	if (bClearImageCaches)
	{
		ImageCaches.Empty();
		UGameplayStatics::SaveGameToSlot(this, SlotNameOverride, UserIndex);
	}
}

void UXDownloaderSaveGame::SaveImageCacheData()
{
	UGameplayStatics::AsyncSaveGameToSlot(this, SlotNameOverride, UserIndex);
}
