# Texture2D 使用与阅读顺序

文件保留你创建的 texture.h / texture.cpp 名称，类名为 Texture2D。

```cpp
#include <texture.h>

Texture2D texture{HELLO_TEXTURE_DIR "/uv-test.png"};
texture.bind(0);
shader.setInt("imageTexture", 0);
```

构造时解码图像、按需上下翻转、转换为 RGBA、创建纹理、上传像素并生成 Mipmap。
默认使用三线性缩小过滤、线性放大过滤和 S/T 重复。
第二个构造参数为 false 时不翻转图片：Texture2D texture{path, false}。

bind 的参数是纹理单元索引，例如 bind(3) 对应 GL_TEXTURE0+3，随后采样器应设置为 3。
setFiltering(min, mag) 和 setWrapping(s, t) 会先把自身绑定到单元 0；每次绘制前仍应按需 bind(unit)。
放大过滤只允许 GL_NEAREST / GL_LINEAR，错误时抛出异常。其他枚举值需要调用方传入合法值。

## 资源生命周期

CPU 像素由带自定义删除器的 unique_ptr 管理，离开构造函数时调用 stbi_image_free；失败抛异常时也能清理已取得的像素。
GPU 纹理由 Texture2D 独占，析构时调用 glDeleteTextures；禁止拷贝，允许移动。
std::exchange(other.id, 0) 返回原 id，并把 other.id 设为 0，相当于“取走编号后清空源对象”。
移动赋值会先释放目标对象原来的纹理。移动后的源对象仅用于析构或重新赋值，不再用于绑定或参数设置。

构造、操作、移动赋值释放旧资源和析构时，都应有有效且合适的 OpenGL 上下文。
当前主程序对象的作用域在 glfwTerminate 之前结束。

## 这一版的范围

使用 RGBA8、8 位分量、默认像素传输状态；当前项目没有绑定像素上传缓冲，行对齐保持默认 4。
不包括图片自动热重载、sRGB 转换、多种内部格式、线程并发加载或无损保存外部绑定状态。
stbi 的翻转选项在这里按本次构造参数设置，适用于当前单线程加载练习。
