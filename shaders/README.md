# GLSL 着色器文件

- `basic.vert`：顶点着色器，处理位置变换并输出顶点颜色。
- `basic.frag`：片段着色器，输出插值后的颜色。
- 后续几何着色器使用 `.geom` 扩展名；目前没有加入几何阶段。

程序启动时读取文件，再交给 OpenGL 驱动编译和链接。C++ 编译成功不等于 GLSL 编译成功，运行时请检查控制台日志。

## 编辑与重新加载

1. 启动程序。
2. 修改着色器文件并保存。
3. 切回 OpenGL 窗口，按 F5。
4. 编译和链接成功后立即替换程序；失败时打印文件路径及错误日志，保留之前可用的程序。

这是手动触发的热重载，不是自动监控保存事件。F5 是运行中 OpenGL 窗口处理的按键；如果焦点在 Visual Studio 中，F5 的行为由 IDE 决定。

可以将 basic.frag 的输出暂时改为 `color = vec4(1.0, 0.0, 0.0, 1.0);`，保存后回到程序按 F5，观察立方体变红。然后改回原来的输出。

## 路径

CMake 通过 HELLO_SHADER_DIR 传入当前项目 shaders 目录的绝对路径。因此从不同工作目录启动程序都能找到源码，并且重新加载读取的是你编辑的这份文件。
这是开发阶段的配置；移动整个项目后重新运行 CMake。发布独立程序时，需要改成随程序部署资源文件的方案。

## 编辑器检查

当前语言是 GLSL，不是 HLSL。Visual Studio 可查看 GLSL language integration 扩展，安装前确认它支持当前 VS 版本。
编辑器提示不能替代当前驱动上的编译与跨阶段链接检查。离线检查也可使用 Khronos glslang 工具。

## C++ 调用

```cpp
Shader shader{
    HELLO_SHADER_DIR "/basic.vert",
    HELLO_SHADER_DIR "/basic.frag"
};

shader.use();
shader.setMat4("transform", mvp);
```

Shader 构造函数第三个参数可选，用于几何着色器文件路径。更换程序后 uniform 位置可能变化，setMat4 会根据当前程序重新查询位置并上传矩阵。
Shader 拥有程序对象，禁止拷贝、支持移动，在有效 OpenGL 上下文销毁之前析构。
