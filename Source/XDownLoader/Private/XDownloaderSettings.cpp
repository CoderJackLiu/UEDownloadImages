// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSettings.h"

#include "HAL/FileManagerGeneric.h"

UXDownloaderSettings::UXDownloaderSettings()
{
	LoadConfig();
	if (SaveGameDefaultPath.Path.IsEmpty())
	{
		SaveGameDefaultPath.Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DownloaderSaveGame"));
	}
	else
	{
		SaveGameDefaultPath.Path = IFileManager::Get().ConvertToRelativePath(*SaveGameDefaultPath.Path);
	}
	if (DownloadImageDefaultPath.Path.IsEmpty())
	{
		DownloadImageDefaultPath.Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DownloadImages"));
	}
	else
	{
		DownloadImageDefaultPath.Path = FFileManagerGeneric::DefaultConvertToRelativePath(*DownloadImageDefaultPath.Path);
	}
}

#if WITH_EDITOR
void UXDownloaderSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if (!SaveGameDefaultPath.Path.IsEmpty())
	{
		SaveGameDefaultPath.Path = IFileManager::Get().ConvertToRelativePath(*SaveGameDefaultPath.Path);
	}
	if (!DownloadImageDefaultPath.Path.IsEmpty())
	{
		DownloadImageDefaultPath.Path = FFileManagerGeneric::DefaultConvertToRelativePath(*DownloadImageDefaultPath.Path);
	}
	SaveConfig();
}
#endif
