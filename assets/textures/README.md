# 图片纹理练习

`uv-test.png` 是本项目的方向测试图：顶部“上 TOP”，底部“下 BOTTOM”，四角 TL/TR/BL/BR，左上蓝、右上黄、左下红、右下绿。

## 加载流程

1. CMake 的 HELLO_TEXTURE_DIR 给出本目录的绝对路径，不依赖程序启动目录。
2. stbi_set_flip_vertically_on_load(true) 让解码结果按本课 UV 约定从底行开始。
3. stbi_load 读取 PNG/JPEG 等图像，返回解码后的像素，不是压缩文件字节。
4. imageWidth、imageHeight 是解码得到的宽高；channelsInFile 表示源图通道数。
5. STBI_rgb_alpha 要求输出始终为四个通道，即使源图只有 RGB，返回的像素也会补齐 alpha。
6. glTexImage2D 使用实际宽高、GL_RGBA8 内部格式、GL_RGBA 输入格式、GL_UNSIGNED_BYTE 分量类型。
7. 上传后 stbi_image_free(pixels) 释放 CPU 内存；OpenGL 已接收数据，不再依赖这个地址。
8. glDeleteTextures 删除 GPU 纹理。这与释放 CPU 像素是两件不同的事。

本课返回的像素每行 width*4 字节，符合默认 GL_UNPACK_ALIGNMENT=4。以后改为原样加载 RGB 三通道时，需要重新考虑每行对齐。
过滤暂时保留 GL_NEAREST，以便观察像素和图案；这里没有生成 Mipmap。

每帧绘制前重新选择纹理单元、绑定纹理，并设置 imageTexture=0，因此 F5 成功重载程序后也能继续采样。
F5 目前只重载 GLSL，不重新读取 PNG；修改图片后需要重启程序。

## 方向观察

当前仍然是旋转立方体，各面 UV 单独映射。背面和部分侧面从外侧观察可能呈镜像或不同转向，因为沿用了原先各面的 UV 方向；不代表 PNG 加载时又翻转了。
要单独观察图片上下方向，可暂时设 Model 为单位矩阵，先看朝相机的 +Z 面。
关闭加载翻转会让该面上的上下标签颠倒。纹理并没有“必须翻转”的普遍规则，要让解码行顺序和 UV 约定一致。

## 替换自己的图片

将 PNG/JPEG 放进本目录，把主程序 texturePath 的文件名换成新文件名，再运行。无需手动填写新图片宽高。
该目录路径目前按开发环境配置；部署程序时要另行安排资源目录。

## 测试图来源

使用内置 image_gen 工具生成，未使用 CLI。生成提示词：

> Use case: scientific-educational. Asset type: square OpenGL UV orientation test texture PNG. Create a perfectly flat, front-facing square diagnostic chart, no perspective, no mockup, no shadows. Four large colored quadrants: top left blue, top right yellow, bottom left red, bottom right green. Clear dark or white high-contrast corner labels "TL", "TR", "BL", "BR" in their corresponding corners. At top center large Chinese text "上 TOP" with upward arrow. At bottom center "下 BOTTOM" with downward arrow. A subtle regular square grid across the entire chart. Center text "UV TEST". Bold readable clean sans serif typography; exact short labels; straight grid lines. Fill the full square with the chart, no outer scene, no watermark. Opaque background.
