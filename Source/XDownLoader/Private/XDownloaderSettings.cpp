// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSettings.h"

#include "HAL/FileManagerGeneric.h"

UXDownloaderSettings::UXDownloaderSettings()
{
	LoadConfig();
	if (SaveGameDefaultSlotName.IsEmpty())
	{
		SaveGameDefaultSlotName = TEXT("XDownload/DownloaderSaveGame");
	}
	if (DownloadImageDefaultPath.Path.IsEmpty())
	{
		DownloadImageDefaultPath.Path = IFileManager::Get().ConvertToRelativePath(*FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("XDownload/DownloadImages")));
	}
}

#if WITH_EDITOR
void UXDownloaderSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if (SaveGameDefaultSlotName.IsEmpty())
	{
		SaveGameDefaultSlotName = TEXT("XDownload/DownloaderSaveGame");
	}
	if (DownloadImageDefaultPath.Path.IsEmpty())
	{
		DownloadImageDefaultPath.Path = IFileManager::Get().ConvertToRelativePath(*FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("XDownload/DownloadImages")));
	}
	SaveConfig();
}
#endif
