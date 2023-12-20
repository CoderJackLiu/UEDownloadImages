// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XDownloaderTypes.h"
#include "XDownloaderSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XDownloaderSubsystem.generated.h"

/**
 * @class UXDownloaderSubsystem
 * @brief A class representing the downloader subsystem of the XDownloader API in Unreal Engine.
 *
 * This class is responsible for managing the downloading functionality in Unreal Engine. It inherits from UGameInstanceSubsystem,
 * which means it is designed to be instantiated and managed by a UGameInstance object.
 *
 * Usage:
 * - Initialize the subsystem by calling the Initialize() function.
 * - Load and get the saved game with LoadSaveGame() and GetSaveGame() functions respectively.
 * - Save the current game state with the SaveSaveGame() function.
 */
UCLASS()
class XDOWNLOADER_API UXDownloaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

public:
	static UXDownloaderSaveGame* LoadSaveGame();

	UXDownloaderSaveGame* GetSaveGame();

	bool SaveSaveGame();

private:
	UPROPERTY(BlueprintReadOnly, Category="XDownloader", meta=(AllowPrivateAccess=true))
	UXDownloaderSaveGame* XDownloaderSaveGame;
};
