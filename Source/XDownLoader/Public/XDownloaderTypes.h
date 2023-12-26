// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XDownloaderTypes.generated.h"


/**
 * @enum ECacheType
 * @brief Enumerates the different cache types.
 *
 * The ECacheType enum class defines the different cache types that can be used in the application.
 * It is used to indicate the type of cache to be used for storing data.
 */
UENUM(BlueprintType)
enum class ECacheType : uint8
{
	CT_SaveGame UMETA(DisplayName = "SaveGame"),
	CT_LocalFile UMETA(DisplayName = "LocalFile"),
	CT_BothSaveGameAndFile UMETA(DisplayName = "Both")
};

/**
 * @struct FDownloadProgress
 * @brief Data structure representing the download progress of an image.
 *
 * This struct is used to store the ID of an image being downloaded and the percentage
 * of completion of the download process.
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

/**
 * \struct FImageDownloadTask
 * \brief Represents a task for downloading an image.
 *
 * This struct contains the necessary information to download an image, such as the URL and ID of the image.
 *
 * \var FImageDownloadTask::ImageURL
 *     The URL of the image to download.
 *
 * \var FImageDownloadTask::ImageID
 *     The ID of the image.
 *
 * \see FImageDownloader
 */
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
	//operator ==
	bool operator==(const FString& InID) const
	{
		UE_LOG(LogTemp, Warning, TEXT("maybe find by key ImageID: %s"), *ImageID);
		return ImageID == InID;
	}
};

/**
 * @enum EDownloadStatus
 * @brief Represents the status of a download operation.
 *
 * This enumeration provides predefined values to represent different stages of a download process.
 * Each value is associated with a display name that can be used for user-friendly purposes.
 */
UENUM(BlueprintType)
enum class EDownloadStatus : uint8
{
	Pending UMETA(DisplayName = "Pending"),
	InProgress UMETA(DisplayName = "InProgress"),
	Success UMETA(DisplayName = "Success"),
	Failed UMETA(DisplayName = "Failed")
};

/**
 * @struct FDownloadResult
 * @brief Represents the result of a download operation.
 *
 * FDownloadResult stores relevant information about a downloaded image, including its status, error message (if any),
 * image ID, URL, downloaded image data, and the resulting texture.
 */
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

/**
 * @struct FTotalDownloadResult
 * @brief Structure representing the result of a total download operation
 */
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

/**
 * @struct FXDownloadImageCached
 * @brief Represents a cached image for download
 *
 * This structure is used to store cached image information for downloads. It includes the image ID, URL, optional time, image data, and texture.
 */
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
		// UE_LOG(LogTemp, Warning, TEXT("ImageID: %s"), *ImageID);
		return ImageID == Other.ImageID;
	}

	//operator ==
	bool operator==(const FString& InID) const
	{
		// UE_LOG(LogTemp, Warning, TEXT("ImageID: %s"), *ImageID);
		return ImageID == InID;
	}

	//load texture from image data
	void LoadTextureFromImageData();
};
