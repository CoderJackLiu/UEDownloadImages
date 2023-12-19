// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderSubsystem.h"

#include "XDownloaderSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UXDownloaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UXDownloaderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UXDownloaderSaveGame* UXDownloaderSubsystem::LoadSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(UXDownloaderSaveGame::SlotName, UXDownloaderSaveGame::UserIndex))
	{
		if (USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(UXDownloaderSaveGame::StaticClass()))
		{
			UGameplayStatics::SaveGameToSlot(SaveGame, UXDownloaderSaveGame::SlotName, UXDownloaderSaveGame::UserIndex);
			return Cast<UXDownloaderSaveGame>(SaveGame);
		}
	}
	else
	{
		if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(UXDownloaderSaveGame::SlotName, UXDownloaderSaveGame::UserIndex))
		{
			return Cast<UXDownloaderSaveGame>(SaveGame);
		}
	}
	return nullptr;
}

UXDownloaderSaveGame* UXDownloaderSubsystem::GetSaveGame()
{
	if (!XDownloaderSaveGame)
	{
		XDownloaderSaveGame = LoadSaveGame();
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

bool UXDownloaderSubsystem::SaveSaveGame()
{
	return UGameplayStatics::SaveGameToSlot(GetSaveGame(), UXDownloaderSaveGame::SlotName, UXDownloaderSaveGame::UserIndex);
}
