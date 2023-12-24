// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XDownloaderTypes.h"
#include "UObject/Object.h"
#include "XDownloaderSettings.generated.h"

/**
 * @class UXDownloaderSettings
 * @brief Represents the settings for the XDownloader module.
 *
 * This class derives from UObject and provides configurable properties for the XDownloader module in project settings.
 */
UCLASS(Config=Game, defaultconfig)
class XDOWNLOADER_API UXDownloaderSettings : public UObject
{
	GENERATED_BODY()

	UXDownloaderSettings();

protected:
#if WITH_EDITOR
	//edit property changed
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	//获取缓存方式
	ECacheType GetCacheType() const { return CacheType; }

	//获取SaveGame默认缓存路径
	FString GetSaveGameDefaultSlotName() const { return SaveGameDefaultSlotName; }

	//获取下载图片默认缓存路径
	FString GetDownloadImageDefaultPath() const { return DownloadImageDefaultPath.Path; }

	//获取下载图片的最大并发数
	int32 GetMaxParallelDownloads() const { return MaxParallelDownloads; }

	//获取下载图片的最大重试次数
	int32 GetMaxRetryTimes() const { return MaxRetryTimes; }

	//获取下载图片的超时时间
	int32 GetDownloadTimeout() const { return DownloadTimeoutSecond; }

private:
	//缓存方式
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true))
	TEnumAsByte<enum ECacheType> CacheType = ECacheType::CT_SaveGame;

	//SaveGame默认缓存路径
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true, EditCondition="CacheType==ECacheType::CT_SaveGame||CacheType==ECacheType::CT_BothSaveGameAndFile", EditConditionHides))
	FString SaveGameDefaultSlotName;

	//下载图片默认缓存路径
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true, EditCondition="CacheType==ECacheType::CT_LocalFile||CacheType==ECacheType::CT_BothSaveGameAndFile", EditConditionHides))
	FDirectoryPath DownloadImageDefaultPath;

	//下载图片的最大并发数
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true, ClampMin=1, ClampMax=8))
	int32 MaxParallelDownloads = 5;

	//下载图片的最大重试次数
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true, ClampMin=1, ClampMax=5))
	int32 MaxRetryTimes = 3;

	//下载图片的超时时间
	UPROPERTY(Config, EditAnywhere, Category = "XDownloader", meta=(AllowPrivateAccess=true, ClampMin=10, ClampMax=300))
	int32 DownloadTimeoutSecond = 10;
};
