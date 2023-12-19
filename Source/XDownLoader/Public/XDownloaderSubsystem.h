// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XDownloaderTypes.h"
#include "XDownloaderSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XDownloaderSubsystem.generated.h"

/**
 * 
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
