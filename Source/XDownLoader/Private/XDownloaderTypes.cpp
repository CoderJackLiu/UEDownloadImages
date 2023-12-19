// Fill out your copyright notice in the Description page of Project Settings.


#include "XDownloaderTypes.h"

#include "ImageUtils.h"

void FXDownloadImageCached::LoadTextureFromImageData()
{
	Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
}
