# 本地依赖

本目录随 Git 保存实际源码，不使用 Git 子模块。正常配置和构建不下载第三方库。

| 目录 | 来源 | 版本或来源记录 | 构建方式 |
|---|---|---|---|
| glfw | https://github.com/glfw/glfw | 3.4，提交 7b6aead9fb88b3623e3b3725ebb42670cbe4c579 | add_subdirectory，静态库 |
| glad | https://glad.dav1d.de | 原项目生成的 OpenGL Core C/C++ 加载器 | 编译本地 glad.c，静态库 |
| stb | https://github.com/nothings/stb | 见 stb/README.md 和头文件版本 | 编译本地 stb_image.cpp，静态库 |

GLFW 源码通过其 3.4 提交的 git archive 导入，未修改上游代码，并保留 LICENSE.md 及其他版权声明。
GLAD 和 stb 的许可及来源声明保留在各自目录及源码中。

OpenGL 驱动、操作系统 SDK、CMake 与 C/C++ 编译器属于系统开发环境，不是可随源码替代的第三方库。Windows 使用 Visual Studio 的“使用 C++ 的桌面开发”工作负载，并安装“适用于 Windows 的 C++ CMake 工具”。
