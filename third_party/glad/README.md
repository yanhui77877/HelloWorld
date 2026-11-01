# glad 放置说明

1. 打开 https://glad.dav1d.de/ （老版生成器，不是 glad2）
2. 选择：
   - Language: C/C++
   - Specification: OpenGL
   - API / gl: Version 4.6
   - Profile: Core
3. 点 Generate，下载 zip 并解压
4. 把 zip 里的内容按下面的结构放好：

```
third_party/glad/
├── include/
│   ├── glad/glad.h
│   └── KHR/khrplatform.h
└── src/
    └── glad.c
```

CMakeLists.txt 已经把 glad 编成静态库并接到主程序，
不要只复制头文件——glad.c 必须参与编译。
