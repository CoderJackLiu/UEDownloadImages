// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XDownloaderTypes.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FDownloadProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	int32 ImageID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	float PercentComplete; // 进度百分比
};

USTRUCT(BlueprintType)
struct FImageDownloadTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageID;


	//override operator ==
	bool operator==(const FImageDownloadTask& Other) const
	{
		return ImageID == Other.ImageID;
	}
};

UENUM(BlueprintType)
enum class EDownloadStatus : uint8
{
	Pending UMETA(DisplayName = "Pending"),
	InProgress UMETA(DisplayName = "InProgress"),
	Success UMETA(DisplayName = "Success"),
	Failed UMETA(DisplayName = "Failed")
};

USTRUCT(BlueprintType)
struct FDownloadResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	EDownloadStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ErrorMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageID;

	//url
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	TArray<uint8> ImageData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	UTexture2D* Texture = nullptr;
};

//total download Result
USTRUCT(BlueprintType)
struct FTotalDownloadResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	TArray<FDownloadResult> SubTaskDownloadResults;

	//total num
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	int32 TotalNum = 0;
};

//image cached
USTRUCT(BlueprintType)
struct FXDownloadImageCached
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	FString ImageURL;

	//optional image time
	FDateTime ImageTime = FDateTime::Now();

	//optional image data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	TArray<uint8> ImageData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Download")
	UTexture2D* Texture = nullptr;

	//operator ==
	bool operator==(const FXDownloadImageCached& Other) const
	{
		//log error imageID
		UE_LOG(LogTemp, Error, TEXT("ImageID: %s"), *ImageID);
		return ImageID == Other.ImageID;
	}

	//operator ==
	bool operator==(const FString& InID) const
	{
		UE_LOG(LogTemp, Error, TEXT("ImageID: %s"), *ImageID);

		return ImageID == InID;
	}

	//load texture from image data
	void LoadTextureFromImageData();
};


UCLASS()
class XDOWNLOADER_API UImageDownloaderTypes : public UObject
{
	GENERATED_BODY()
};
