# HelloWorld — OpenGL 学习项目

C++17 / OpenGL 3.3 Core 学习项目。目前包含贴图立方体、飞行相机、绕 Y 轴移动的点光源与基础漫反射，以及着色器文件读取和 F5 热重载。

## 目录

- `src/uniform.cpp`：程序入口、场景数据与渲染循环。
- `src/camera`：相机移动、转向和 View 矩阵。
- `src/buffer`、`src/vertex_array`：缓冲和 VAO 的 RAII 封装。
- `src/shader`：GLSL 文件加载、编译链接、uniform 上传和热重载。
- `src/texture`：图片加载、纹理上传、Mipmap 与资源管理。
- `src/math`：向量和矩阵计算。
- `shaders`：GLSL 顶点、片段着色器。
- `assets/textures`：纹理图片及学习说明。
- `third_party`：随项目保存的 GLFW 3.4、GLAD 和 stb_image，全部从本地源码构建。

## 构建

需要支持 OpenGL 3.3 的显卡驱动、CMake 3.24 或更新版本，以及支持 C++17 的编译器。Windows 可使用 Visual Studio 的“使用 C++ 的桌面开发”工作负载和 CMake 支持。

第三方库已经完整纳入仓库，无需另行下载、设置库目录、运行子模块命令，也不依赖任何仓库外的 `third_party` 目录。

### Visual Studio 2022 / 2026

1. 安装“使用 C++ 的桌面开发”工作负载，包括 Windows SDK、MSVC、CMake 和 Ninja 工具。
2. 克隆仓库，用 VS 的“打开文件夹”打开包含 CMakeLists.txt 的仓库根目录。
3. 选择 `Windows x64 Debug` 配置预设，等待 CMake 配置完成。
4. 启动项选择 `hello_window.exe`，生成并运行。

若旧目录曾因缺少 GLFW 配置失败，使用“删除缓存并重新配置”，或切换到新增的预设。不要复制其他目录的 out 或 .vs 缓存。

### 命令行

在 Visual Studio 的 x64 Native Tools 命令提示符中执行：

```powershell
git clone https://github.com/yanhui77877/HelloWorld.git
cd HelloWorld
cmake --preset windows-debug
cmake --build --preset windows-debug
.\out\build\windows-debug\hello_window.exe
```

也可以使用其他本机 CMake 生成器：`cmake -S . -B build`，然后 `cmake --build build --config Debug`。程序入口当前是 `uniform.cpp`。

开发时资源路径由 CMake 定位到仓库中的 `shaders` 和 `assets/textures`，与程序启动目录无关。移动仓库后重新配置 CMake；发布独立程序时需要另行部署资源。

## 操作

- 鼠标：转头。
- WASD：相对于镜头方向移动。
- F5（OpenGL 窗口获得焦点时）：重新读取、编译和链接 GLSL；失败保留旧程序。
- ESC：退出。

图片修改后需要重启程序，F5 当前只重载着色器。第三方代码的许可见其文件内或对应目录的说明。
