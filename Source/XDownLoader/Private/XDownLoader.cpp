// Copyright Epic Games, Inc. All Rights Reserved.

#include "XDownLoader.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "XDownloaderSettings.h"
#endif

#define LOCTEXT_NAMESPACE "FXDownLoaderModule"

void FXDownLoaderModule::RegisterXDownloaderSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Project", "XDownloaderSettings",
		                                 NSLOCTEXT("XDownloaderSettings", "XDownloaderSettingsName", "XDownloaderSettings"),
		                                 NSLOCTEXT("XDownloaderSettings", "XDownloader Settings", "Configure XDownloaderSettings Plugin settings"),
		                                 GetMutableDefault<UXDownloaderSettings>()
		);
	}
#endif
}

void FXDownLoaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// 注册设置
	RegisterXDownloaderSettings();
}

void FXDownLoaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXDownLoaderModule, XDownLoader)
