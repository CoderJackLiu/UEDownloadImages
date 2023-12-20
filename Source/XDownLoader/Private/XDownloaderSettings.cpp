// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSettings.h"

UXDownloaderSettings::UXDownloaderSettings()
{
	LoadConfig();
	if (SaveGameDefaultPath.IsEmpty())
	{
		SaveGameDefaultPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DownloaderSaveGame"));
	}
	if (DownloadImageDefaultPath.IsEmpty())
	{
		DownloadImageDefaultPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DownloadImages"));
	}
}
#if WITH_EDITOR

void UXDownloaderSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig();
}
#endif
