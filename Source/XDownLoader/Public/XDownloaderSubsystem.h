// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XDownloaderTypes.h"
#include "XDownloaderSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XDownloaderSubsystem.generated.h"

class UXDownloaderSettings;
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
	static UXDownloaderSaveGame* LoadSaveGame(const FString& InSlotName);

	/**
	 * @brief Retrieves the save game data.
	 *
	 * This method retrieves the save game data from the UXDownloaderSubsystem.
	 * If the save game data is not loaded yet, it loads the save game data and initializes the image caches.
	 *
	 * @return A pointer to the UXDownloaderSaveGame object containing the save game data.
	 */
	UXDownloaderSaveGame* GetSaveGame(const FString& InSlotName="");

	/**
	 * @brief Retrieves the XDownload settings.
	 *
	 * This method retrieves the XDownload settings from the UXDownloaderSubsystem.
	 * If the settings have not been set yet, it initializes the settings with the default values.
	 *
	 * @return A pointer to the UXDownloaderSettings object containing the XDownload settings.
	 */
	UXDownloaderSettings* GetXDownloadSettings();

private:
	/**
	 * @struct FXDownloadImageCached
	 * This struct represents the cached data for a downloaded image.
	 */
	UPROPERTY(BlueprintReadOnly, Category="XDownloader", meta=(AllowPrivateAccess=true))
	TArray<USaveGame*> XDownloaderSaveGames;

	/**
	 * @class XDownloaderSettings
	 * @brief Represents the settings for the XDownloader subsystem.
	 *
	 * The XDownloaderSettings class is responsible for storing and managing the settings for the XDownloader subsystem.
	 */
	UPROPERTY(BlueprintReadOnly, Category="XDownloader", meta=(AllowPrivateAccess=true))
	UXDownloaderSettings* XDownloaderSettings;

	UXDownloaderSaveGame* FindOrLoadSaveGame(const FString& InSlotName);
};
