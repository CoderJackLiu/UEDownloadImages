// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSubsystem.h"

#include "XDownloaderSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "XDownloaderSettings.h"

void UXDownloaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UXDownloaderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UXDownloaderSaveGame* UXDownloaderSubsystem::LoadSaveGame(const FString& InSlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(InSlotName, UXDownloaderSaveGame::UserIndex))
	{
		if (USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(UXDownloaderSaveGame::StaticClass()))
		{
			UGameplayStatics::SaveGameToSlot(SaveGame, InSlotName, UXDownloaderSaveGame::UserIndex);
			return Cast<UXDownloaderSaveGame>(SaveGame);
		}
	}
	else
	{
		if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(InSlotName, UXDownloaderSaveGame::UserIndex))
		{
			return Cast<UXDownloaderSaveGame>(SaveGame);
		}
	}
	return nullptr;
}

UXDownloaderSaveGame* UXDownloaderSubsystem::GetSaveGame(const FString& InSlotName)
{
	if (!XDownloaderSaveGame)
	{
		XDownloaderSaveGame = LoadSaveGame(InSlotName);
		for (FXDownloadImageCached& ImageCached : XDownloaderSaveGame->ImageCaches)
		{
			ImageCached.LoadTextureFromImageData();
			ImageCached.Texture->AddToRoot();
			//log ImageCached.Texture->GetName();
			UE_LOG(LogTemp, Error, TEXT("ImageCached.Texture->GetName(): %s"), *ImageCached.Texture->GetName());
		}
	}
	return XDownloaderSaveGame;
}

UXDownloaderSettings* UXDownloaderSubsystem::GetXDownloadSettings()
{
	if (!XDownloaderSettings)
	{
		XDownloaderSettings = GetMutableDefault<UXDownloaderSettings>();
	}
	return XDownloaderSettings;
}
