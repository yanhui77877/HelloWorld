# uniform.cpp 学习笔记

本笔记对应当前的彩色旋转立方体练习。源码后续可能变化，文末的带注释版本是本次迁移时的快照，不是需要一起编译的文件。

## 1. 程序整体流程

初始化 GLFW → 创建 OpenGL 3.3 Core 上下文 → 加载 GLAD → 设置视口 → 编译和链接着色器 → 上传顶点与索引 → 配置 VAO → 循环更新 MVP 并绘制 → 释放资源。

- GLFW 负责窗口、上下文、事件和时间。
- GLAD 负责加载 OpenGL 函数地址；必须在上下文成为当前上下文之后加载。
- iostream 用来打印日志，cmath 提供 sin、cos、tan，角度使用弧度。
- GLuint 等是 OpenGL 类型；对象编号不是 CPU 内存中的对象指针。

## 2. GLSL 与顶点属性

C++ 中相邻的字符串字面量会拼接成完整的着色器源码。字符串里的 `\n` 是 GLSL 的换行；C++ 字符串后面的注释不会传入着色器。

- location=0：位置 XYZ。
- location=1：颜色 RGB。
- 顶点着色器将位置补成四维点 (x,y,z,1)，乘 transform，输出裁剪坐标 gl_Position。
- 顶点颜色经光栅化阶段插值后输入片段着色器，片段着色器输出 RGBA。
- alpha=1 不等于自动开启或关闭混合，混合由 OpenGL 状态控制。

## 3. Mat4 与组合变换

矩阵使用列主序：`m[col*4+row]`，row、col 从 0 开始。

```text
数学行列布局与数组索引：
m[0]  m[4]  m[8]   m[12]
m[1]  m[5]  m[9]   m[13]
m[2]  m[6]  m[10]  m[14]
m[3]  m[7]  m[11]  m[15]
```

- 构造函数将 16 个元素清零，不是单位矩阵。
- identity 将四个对角元素设为 1。
- scale 修改 m[0]、m[5]、m[10]，保持 m[15]=1。若把四个对角元素都乘相同系数，透视除法会抵消缩放。
- translate 把平移放在 m[12]、m[13]、m[14]。输入点 w=1 受平移影响；方向 w=0 不受平移影响。
- 三个旋转函数采用右手规则，sin/cos 的输入为弧度。
- multiply(a,b) 返回 a*b。每个结果元素累加四个乘积，所以使用 +=，并要求初值为零。
- const 引用参数避免复制，并禁止通过这些引用修改输入矩阵。
- static 成员函数可以通过 Mat4:: 调用；返回值是矩阵本身。

列向量的变换从右向左：
- T*R：先旋转，再平移。
- R*T：先平移，再绕原点旋转。
- R*S：先缩放，再旋转。
- 非等比缩放时 S*R 一般不同于 R*S；等比缩放与旋转可交换。

## 4. MVP、投影与 w

`MVP = Projection * View * Model`。

- Model：局部坐标到世界坐标；当前先绕 X 轴旋转 t*0.6，再绕 Y 轴旋转 t。
- View：世界坐标到观察坐标；translate(0,0,-5) 对应相机在世界坐标 (0,0,5)，朝 -Z 看。
- Projection：观察坐标到裁剪坐标。垂直视野 45 度先转成弧度，aspect=800/600，near=0.1，far=100。
- 本实现要求 0<fov<π、aspect>0、0<near<far，采用 OpenGL 默认 NDC 深度范围 [-1,1]。

令 q=1/tan(fov/2)，n=near，f=far：

```text
投影矩阵（按数学上的行显示）：
q/aspect  0       0                  0
0         q       0                  0
0         0    -(f+n)/(f-n)      -2fn/(f-n)
0         0      -1                  0
```

矩阵乘点 (x,y,z,1)：
- x_clip=(q/aspect)*x。
- y_clip=q*y。
- z_clip=A*z+B，其中 A=-(f+n)/(f-n)，B=-2fn/(f-n)。
- w_clip=-z，因为最后一行是 [0,0,-1,0]，不是 [0,0,0,-z]。

m[11]=-1 实现输出 w=-z，m[15] 必须保持 0。漏写 m[11] 会让 w 为零，使本练习的几何无法正常显示。

GPU 在图元裁剪后进行透视除法：x_ndc=x_clip/w_clip，y_ndc=y_clip/w_clip，z_ndc=z_clip/w_clip。
这样 X、Y 除以前向距离 -z，产生近大远小；A、B 保证 z=-near 映射到 -1，z=-far 映射到 +1。
默认裁剪条件是每个 clip 分量位于 [-w,w]。裁剪针对图元，跨边界的三角形可保留可见部分。
NDC 再经视口变换进入窗口坐标；默认深度范围把 z_ndc 从 [-1,1] 映射到 [0,1]。

## 5. 着色器编译与 uniform

创建 shader → 提交源码 → 编译 → 查询编译状态与日志。
创建 program → 附加顶点、片段 shader → 链接 → 查询链接状态与日志。

编译查询用 glGetShaderiv；链接查询用 glGetProgramiv。
当前程序打印编译/链接错误后还会继续执行，调试时应先检查控制台日志。
glDeleteShader 标记独立 shader 删除；链接后的程序仍可使用。
glGetUniformLocation 查询 transform 的位置，返回 -1 表示未找到或没有活动 uniform。
glUseProgram 激活程序后，glUniformMatrix4fv 上传一个列主序矩阵；GL_FALSE 表示不转置。

## 6. 立方体数据与 VAO/VBO/EBO

当前 8 个顶点，每个顶点 6 个 float：XYZ+RGB。
每个面两个三角形，6 个面共 12 个三角形、36 个索引。
索引 0~3 位于 z=-0.5 的面，4~7 位于 z=+0.5 的面。

VBO 保存顶点数据，EBO 保存索引，VAO 记录顶点属性读取方式以及 EBO 绑定。
glGen* 获取编号；绑定后配置对象。glBufferData 分配存储并复制 CPU 数组数据。
GL_STATIC_DRAW 是预计较少修改、用于绘制的使用提示，不是禁止修改。

- 位置属性：3 个 float，步长 6*sizeof(GLfloat)，字节偏移 0。
- 颜色属性：3 个 float，同一步长，字节偏移 3*sizeof(GLfloat)。
- glVertexAttribPointer 的最后一个参数在绑定 VBO 时表示缓冲中的字节偏移，不是要解引用的 CPU 地址。
- glVertexAttribPointer 记录当前 VBO 与属性的关联；随后解绑 GL_ARRAY_BUFFER 不会清除此关联。
- EBO 绑定属于 VAO 状态，不要在配置当前 VAO 时误解绑 EBO。
- glEnableVertexAttribArray 启用对应属性数组。
- glDrawElements 的 36 是索引个数，GL_UNSIGNED_INT 对应 GLuint，末尾的 0 表示 EBO 字节偏移零。

## 7. 渲染循环与深度测试

glfwPollEvents 处理输入和关闭事件。
glClearColor 仅设置清除颜色；glClear 才执行清除。
glEnable(GL_DEPTH_TEST) 开启深度测试；当前放在循环内，功能正确，通常放初始化阶段一次即可。
默认比较 GL_LESS，默认深度写入开启、清除深度为 1；更近的片段可以覆盖远处片段。
每帧同时清除颜色和深度，防止前一帧深度干扰当前帧。
透视投影负责位置映射；深度测试负责遮挡，两者职责不同。

glfwGetTime 返回秒数；当前用它直接驱动旋转，而不是每帧增加固定角度。
glfwSwapBuffers 交换前后颜色缓冲，把这一帧呈现到窗口。

## 8. 清理与当前限制

在上下文仍然有效时删除 VAO、EBO、VBO、program，最后 glfwTerminate。
删除 VAO 不会自动删除关联的缓冲。

当前仅初始化时设置视口，并使用固定投影宽高比；以后窗口缩放需同步更新。
glfwInit 返回值尚未检查，GLAD 失败分支尚未调用 glfwTerminate。
CMake 中 /utf-8 明确指定 MSVC 的源文件及执行字符集，避免中文注释按代码页 936 被误读。

## 9. 迁移时的带注释源码快照

保留本次迁移前已有的逐行注释和历史实验，便于查阅。以项目中的 uniform.cpp 为后续实际运行版本。

```cpp
#include <iostream> 

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <cmath>

// Window dimensions

// Vertex shader source (GLSL)
const GLchar* vertexShaderSource =  
"#version 330 core\n" 
"layout (location = 0) in vec3 position;\n" 
"layout(location = 1) in vec3 colour;\n" 
"uniform mat4 transform;\n"
"out vec3 vColour;\n" 
"void main()\n" 
"{\n" 
"    gl_Position = transform*vec4(position.x, position.y, position.z, 1.0);\n"
"    vColour = colour;\n"
"}\0"; 

// Fragment shader source (GLSL)
const GLchar* fragmentShaderSource = 
"#version 330 core\n" 
"in vec3 vColour;\n"
"out vec4 color;\n"
"void main()\n" 
"{\n" 
"    color = vec4(vColour, 1.0f);\n" 
"}\n\0"; 

struct Mat4 
{ 
    float m[16];

    Mat4() 
    {
        for (int i = 0; i < 16; ++i) 
            m[i] = 0.0f; 
    };

    static Mat4  
    identity() 
    { 
        Mat4 ret{}; // 调用构造函数，创建全零矩阵。
        ret.m[0] = ret.m[5] = ret.m[10] = ret.m[15] = 1.0f; // 将四个对角元素设为 1，形成单位矩阵。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。

    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    scale(float scale) // 等比缩放重载：X、Y、Z 使用相同系数。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[0] = ret.m[5] = ret.m[10] = scale; // 缩放 X、Y、Z；保持 m[15]=1，避免缩放在透视除法时被抵消。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。
    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    scale(float x,  // 非等比缩放重载：x 是 X 轴缩放系数。
          float y, // Y 轴缩放系数。
          float z) // Z 轴缩放系数。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[0] = x; // 第 1 行第 1 列：X 轴缩放。
        ret.m[5] = y; // 第 2 行第 2 列：Y 轴缩放。
        ret.m[10] = z; // 第 3 行第 3 列：Z 轴缩放。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。

    static Mat4 
    multiply(const Mat4 &a,
             const Mat4 &b)
    { 
        Mat4 result{}; 
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col) 
                for (int k = 0; k < 4; ++k) 
                    result.m[col*4 + row] += a.m[k*4 + row]*b.m[col*4 + k];
        return result; 
    }

    static Mat4 
    perspective(float fov,  
                float aspectRatio, 
                float near,
                float far)
    {
        Mat4 result{}; 
        float f = 1.0f / tan(fov / 2.0f); 
        result.m[0] = f / aspectRatio; 
        result.m[5] = f; 
        result.m[10] = (far + near) / (near - far); 
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * far * near) / (near - far); 
        return result; 
    }

    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    translate(float tx, float ty, float tz) // 平移矩阵：分别沿 X、Y、Z 移动。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[12] = tx; // 第 1 行第 4 列：点的 x 加上 tx（输入 w=1）。
        ret.m[13] = ty; // 第 2 行第 4 列：点的 y 加上 ty。
        ret.m[14] = tz; // 第 3 行第 4 列：点的 z 加上 tz；方向向量 w=0 时不受平移影响。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。

    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    rotateZ(float angle) // 绕 Z 轴旋转；angle 为弧度，使用右手规则。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
		Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[0] = cos(angle); // 旋转矩阵第 1 行第 1 列的余弦系数。
        ret.m[1] = sin(angle); // Z 旋转：y 输出中来自 x 的系数。
        ret.m[4] = -sin(angle); // Z 旋转：x 输出中来自 y 的系数。
        ret.m[5] = cos(angle); // 旋转矩阵第 2 行第 2 列的余弦系数。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。
    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    rotateY(float angle) // 绕 Y 轴旋转；改变 X、Z，保留 Y。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
		Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[0] = cos(angle); // 旋转矩阵第 1 行第 1 列的余弦系数。
        ret.m[2] = -sin(angle); // Y 旋转：z 输出中来自 x 的系数。
        ret.m[8] = sin(angle); // Y 旋转：x 输出中来自 z 的系数。
        ret.m[10] = cos(angle); // 旋转矩阵第 3 行第 3 列的余弦系数。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。
    static Mat4  // 静态成员函数返回 Mat4，无需先创建对象即可通过 Mat4:: 调用。
    rotateX(float angle) // 绕 X 轴旋转；改变 Y、Z，保留 X。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
		Mat4 ret = identity(); // 以单位矩阵为基础修改，保留未修改轴和齐次分量。
        ret.m[5] = cos(angle); // 旋转矩阵第 2 行第 2 列的余弦系数。
        ret.m[6] = sin(angle); // X 旋转：z 输出中来自 y 的系数。
        ret.m[9] = -sin(angle); // X 旋转：y 输出中来自 z 的系数。
        ret.m[10] = cos(angle); // 旋转矩阵第 3 行第 3 列的余弦系数。
        return ret; // 按值返回矩阵；编译器通常可以消除不必要的复制。
    } // 结束当前作用域。
}; // 结束当前定义或初始化列表。

int  // main 的返回类型：整数退出状态。
main() // 程序入口。
{ // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
    const GLint WIDTH = 800, HEIGHT = 600; // 初始窗口尺寸；实际帧缓冲像素尺寸随后查询。
    glfwInit(); // 初始化 GLFW；当前代码未检查返回值。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 请求 OpenGL 主版本 3。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 请求 OpenGL 次版本 3，即 3.3。
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 请求核心模式，使用现代可编程渲染管线。

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "n", nullptr, nullptr); // 创建标题为 n 的窗口和上下文；不全屏、不与其他上下文共享资源。
    if(window == nullptr) // 检查窗口是否创建失败。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        std::cout << "Failed to create GLFW window" << std::endl; // 输出窗口创建错误，并刷新控制台输出。
        glfwTerminate(); // 释放 GLFW 管理的窗口等资源；结束 GLFW 使用。
        return -1; // 以失败状态退出程序。
    } // 结束当前作用域。

    glfwMakeContextCurrent(window); // 将此窗口的 OpenGL 上下文绑定到当前线程。

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // 从当前上下文加载 OpenGL 函数地址；失败时进入分支。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        std::cout << "Failed to initialize GLAD" << std::endl; // 输出 OpenGL 函数加载失败的信息。
        return -1; // 以失败状态退出程序。
    } // 结束当前作用域。

    int width, height; // 接收实际帧缓冲的像素宽高，高 DPI 下可能不同于窗口尺寸。
    glfwGetFramebufferSize(window, &width, &height); // 通过输出指针取得帧缓冲尺寸。
    glViewport(0, 0, width, height); // 设置 NDC 到窗口坐标的映射区域；当前只在初始化时设置。

    GLint success; // 保存编译或链接的成功状态。
    GLchar infoLog[512]; // 保存最多 512 字节的错误日志（含终止符空间）。

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // 创建顶点着色器对象，返回其编号。
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 提交一段源码；NULL 表示由字符串终止符确定长度。
    glCompileShader(vertexShader); // 编译顶点着色器 GLSL。
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // 查询顶点着色器编译状态。
    if (!success) // 状态为失败时打印日志；当前分支不会立即终止程序。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // 读取顶点着色器编译日志；不接收实际日志长度。
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl; // 打印顶点着色器错误详情。
    } // 结束当前作用域。

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // 创建片段着色器对象。
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // 将片段着色器源码提交给 OpenGL。
    glCompileShader(fragmentShader); // 编译片段着色器。
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // 查询片段着色器编译状态。
    if (!success) // 状态为失败时打印日志；当前分支不会立即终止程序。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); // 读取片段着色器编译日志。
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl; // 打印片段着色器错误详情。
    } // 结束当前作用域。

    GLuint shaderProgram = glCreateProgram(); // 创建程序对象，用来链接各着色器阶段。
    glAttachShader(shaderProgram, vertexShader); // 将顶点着色器附加到程序。
    glAttachShader(shaderProgram, fragmentShader); // 将片段着色器附加到程序。
    glLinkProgram(shaderProgram); // 链接程序，并检查各阶段接口是否兼容。
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // 查询链接状态；程序状态使用 glGetProgramiv。
    if (!success) // 状态为失败时打印日志；当前分支不会立即终止程序。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog); // 读取程序链接日志。
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl; // 打印程序链接错误详情。
    } // 结束当前作用域。
    glDeleteShader(vertexShader); // 标记删除独立顶点着色器；已链接程序仍可使用，附加关系可能延迟其实际销毁。
    glDeleteShader(fragmentShader); // 标记删除独立片段着色器；不删除已链接的程序。


    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform"); // 查询 uniform 位置；未找到或被优化掉时返回 -1。

    GLuint indices[] =
    {
        4, 5, 6,  4, 6, 7, // +Z 面
        0, 3, 2,  0, 2, 1, // -Z 面
        0, 4, 7,  0, 7, 3, // -X 面
        1, 2, 6,  1, 6, 5, // +X 面
        3, 7, 6,  3, 6, 2, // +Y 面
        0, 1, 5,  0, 5, 4  // -Y 面
    };

    GLfloat vertices[] =
    {
        // 位置                   // 颜色
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, // 0
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f, // 1
         0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f, // 2
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 0.0f, // 3
    
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 1.0f, // 4
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 1.0f, // 5
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 1.0f, // 6
        -0.5f,  0.5f,  0.5f,     0.2f, 0.2f, 0.2f  // 7
    };

    GLuint VBO, VAO, EBO; // 分别保存顶点缓冲、顶点数组对象和索引缓冲的编号。
    glGenVertexArrays(1, &VAO); // 生成一个 VAO 编号；VAO 用于记录顶点输入配置。
    glGenBuffers(1, &VBO); // 生成一个缓冲对象编号，随后作为 VBO 使用。
    glGenBuffers(1, &EBO); // 生成一个缓冲对象编号，随后作为 EBO 使用。

    glBindVertexArray(VAO); // 绑定 VAO；配置时记录属性和 EBO 关联，绘制时恢复这些配置。
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将 VBO 绑定到顶点数据缓冲目标。
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 分配并复制全部顶点数据；STATIC_DRAW 是预计很少修改、用于绘制的提示。

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // 绑定索引缓冲；该绑定会保存在当前 VAO 中。
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // 将全部索引数据复制到 EBO。

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0); // 属性 0 读 3 个 float；步长 6 个 float，字节偏移为 0；记录当前 VBO 关联。
    glEnableVertexAttribArray(0); // 启用位置属性 0；否则不会按数组读取此属性。

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat))); // 属性 1 读 RGB；步长相同，起点偏移 3 个 float；此处指针值表示缓冲内字节偏移。
    glEnableVertexAttribArray(1); // 启用颜色属性 1，对应 GLSL location=1。

    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解除 ARRAY_BUFFER 当前绑定；不会清除已由属性指针记录的 VBO 关联。
    glBindVertexArray(0); // 解除当前 VAO 绑定；该 VAO 已保存的配置不变。

    while (!glfwWindowShouldClose(window)) // 只要窗口没有被要求关闭，就继续渲染下一帧。
    { // 开始当前函数、类型、循环、条件分支或初始化列表的作用域。
        glfwPollEvents(); // 处理操作系统事件，包括关闭窗口、键盘和鼠标事件。

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置颜色缓冲清除值（RGBA）；本行本身不清除画面。 
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram); // 激活程序；后续 glUniform 和绘制使用此程序。

        //Mat4 scaleMat = Mat4::scale(1.0f + 0.5f * sin((float)glfwGetTime()));
        //Mat4 finalMat = Mat4::multiply(scaleMat, transMat);
        float t = (float)glfwGetTime(); // 取得 GLFW 计时器的秒数并转为 float，用作旋转角度，约 1 弧度/秒。
        //Mat4 transMat = Mat4::translate(1, 0.0f, 0.0f);
        //Mat4 rotMat = Mat4::rotateZ(t);                              // 持续旋转
        //Mat4 scaleMat = Mat4::scale(1.0f + 0.5f * sin(t));           // 呼吸
        //Mat4 scaleMat = Mat4::scale(0.5f, 0.2f, 0.1f);           // 非等比缩放：历史实验，当前不执行
        //Mat4 finalMat = Mat4::multiply(rotMat, scaleMat);            // R × S
        //Mat4 finalMat = Mat4::multiply(scaleMat, rotMat);            // S × R
        //Mat4 finalMat = Mat4::multiply(transMat, rotMat);            // T × R
        //Mat4 finalMat = Mat4::multiply(rotMat, transMat);            // R × T
        Mat4 model = Mat4::multiply(
            Mat4::rotateY(t),
            Mat4::rotateX(t * 0.6f)
        );
        Mat4 view = Mat4::translate(0.0f, 0.0f, -5.0f); // View：世界到观察空间；相当于相机位于 (0,0,5) 并朝 -Z 看。
        Mat4 projection = Mat4::perspective( // Projection：观察到裁剪空间；以下参数构造透视投影。
            45.0f * 3.14159265f / 180.0f, // 将垂直视野角 45 度转成弧度。
            800.0f / 600.0f, // 初始宽高比 4/3；使用浮点除法，窗口改变尺寸时也应更新此值。
            0.1f, // 近裁剪面距离 0.1；相机前方对应观察空间 z=-0.1。
            100.0f // 远裁剪面距离 100；对应观察空间 z=-100。
        ); // 结束本次函数调用。
        Mat4 mvp = Mat4::multiply( // 组合 MVP=P*V*M；列向量按从右到左的顺序变换。
            projection, // 左乘投影矩阵，最后执行投影。
            Mat4::multiply(view, model) // 先计算 V*M，使点先执行 Model，再执行 View。
        ); // 结束本次函数调用。

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mvp.m); // 向当前程序上传 1 个 mat4；GL_FALSE 不转置，数组按列主序解释。
        glBindVertexArray(VAO); // 绑定 VAO；配置时记录属性和 EBO 关联，绘制时恢复这些配置。
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // 解除当前 VAO 绑定；该 VAO 已保存的配置不变。
        glfwSwapBuffers(window); // 交换前后颜色缓冲，让刚画好的画面显示到窗口。
    } // 结束当前作用域。

    glDeleteVertexArrays(1, &VAO); // 释放 VAO；不会连带删除 VBO、EBO。
    glDeleteBuffers(1, &EBO); // 删除索引缓冲对象。
    glDeleteBuffers(1, &VBO); // 删除顶点缓冲对象。
    glDeleteProgram(shaderProgram); // 删除着色器程序对象；资源清理在上下文销毁前进行。
    glfwTerminate(); // 释放 GLFW 管理的窗口等资源；结束 GLFW 使用。
    return 0; // 以成功状态结束程序。
} // 结束当前作用域。
```

