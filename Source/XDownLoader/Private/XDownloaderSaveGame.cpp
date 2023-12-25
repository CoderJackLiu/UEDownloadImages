// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSaveGame.h"

#include "ImageUtils.h"
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
	// FScopeLock Lock(&ImportBufferLock);
	FXDownloadImageCached* Cached = ImageCaches.FindByKey(ImageID);
	if (Cached->Texture == nullptr)
	{
		Cached->Texture = FImageUtils::ImportBufferAsTexture2D(Cached->ImageData);
		Cached->Texture->AddToRoot();
	}
	return Cached;
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
