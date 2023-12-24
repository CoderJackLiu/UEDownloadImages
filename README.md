# UEDownloadImages
UE5 Download Images plugin. for cache and multi download images instead of download image blueprint API from unreal engin.



## Introduction
This plugin is designed to address the limitations of Unreal Engine's native single-image download capability. The built-in `downloadImage` API only supports downloading one image at a time and struggles with multiple images, often resulting in the latter overwriting the former. Moreover, this API lacks a caching feature. Our plugin overcomes these issues by offering the `DownloadImages` API and image caching.

## Features
- **Multiple Image Download**: The `DownloadImages` API allows users to download multiple images simultaneously. This requires an array of structures composed of image IDs and URLs.
- **Image Caching**: Utilizes Unreal Engine's `SaveGame` for image caching. The same image ID will not be downloaded repeatedly. Note that if the server-side image ID is updated, the corresponding URL should also be updated.
- **Customizability**: The plugin supports customized settings, such as the default location for caching images.
- **Multiple Caching Modes**: Users can choose to cache using the `SaveGame` method, cache local files, or use both methods simultaneously.

![ProjectSettings](Resources/Settings.png)
![DownloadImages](Resources/Node.png)
![Demo](Resources/Demo.png)

## Compatibility
- Developed using Unreal Engine 5.1, this plugin should be easily adaptable to Unreal Engine 4 and higher versions of Unreal Engine 5. Developers interested in contributing can add branches for other versions.

## Example
- Open the `UEDownloadImagesExample` map located in the Content directory of the plugin. Run the game, and click to execute to see the effect. For specific usage, please refer to the BP_UEDownloadImagesController under GameMode.

## Feedback and Suggestions
- For additional feature requests or suggestions, feel free to leave a message in the issues section.

## Future Updates
- **Single Image Download and Caching**: We will add a `DownloadImageWithCache` API for downloading a single image with support for automatic caching or cache retrieval.
- **Enhanced Project Settings**: More options for project settings will be provided to enhance user customization.

## Use Cases
While not common in game development, this plugin can quickly address specific needs, especially suitable for scenarios requiring the download and management of multiple images.

# 虚幻引擎图片下载插件

## 简介
本插件旨在解决虚幻引擎仅提供单一图片下载的局限性。虚幻引擎内置的 `downloadImage` API只能同时下载一个图片，且存在多图下载时后续图片会覆盖前者的问题。此外，该API不支持图片缓存功能。本插件通过提供 `DownloadImages` API和图片缓存功能，克服了这些限制。

## 功能
- **多图下载**: `DownloadImages` API允许用户同时下载多张图片，需输入图片ID和URL组成的结构体数组。
- **图片缓存**: 利用虚幻引擎的 `SaveGame` 实现图片缓存，同一图片ID不会重复下载。请注意，若服务端对应的图片ID更新，则URL也应相应更新。
- **定制性**: 插件支持定制化设置，例如缓存图片的默认位置。
- **多模式**：可以选择使用saveGame的方式缓存，也可以使用缓存本地文件的方式缓存，也可以两个同时缓存；

## 兼容性
- 本插件使用虚幻引擎5.1开发，由于未广泛使用API，因此应易于修改以适用于虚幻4和虚幻5的更高版本。对此感兴趣的开发者可参与添加其他版本的分支。

## 示例
- 打开插件Content目录下的 `UEDownloadImagesExample` 地图，运行游戏，点击运行即可看到效果，具体调用请参阅GameMode下面的BP_UEDownloadImagesController。


## 交流与建议
- 如对本插件有更多的功能需求或建议，欢迎在issues区留言讨论。

## 未来更新
- **单图下载与缓存**: 将增加 `DownloadImageWithCache` API，支持单张图片下载及自动缓存或读取缓存功能。
- **项目设置优化**: 提供更多项目设置选项，增强用户自定义体验。

## 应用场景
虽然在游戏开发中不常见，但本插件可快速解决特定需求，特别适用于需要下载和管理多张图片的场景。





