#include <iostream>
#include <exception>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Camera.h>
#include <Buffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture2D.h>

int
main()
{
    const GLint WIDTH = 800, HEIGHT = 600;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "n", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    try
    {
        Shader shader{
            HELLO_SHADER_DIR "/basic.vert",
            HELLO_SHADER_DIR "/basic.frag"
        };

        GLuint indices[] =
        {
            0, 1, 2,  0, 2, 3,
            4, 5, 6,  4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        GLfloat vertices[] =
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
            -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 

             0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 
             0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 

            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 
            -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f, 
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f, 

            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
             0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
        };

        {
            Buffer vertexBuffer{GL_ARRAY_BUFFER};
            Buffer indexBuffer{GL_ELEMENT_ARRAY_BUFFER};
            VertexArray vertexArray;

            vertexArray.bind();
            vertexBuffer.upload(sizeof(vertices), vertices);
            indexBuffer.upload(sizeof(indices), indices);

            vertexBuffer.bind();
            vertexArray.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
            vertexArray.setAttribute(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 3*sizeof(GLfloat));
            vertexArray.setAttribute(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 5*sizeof(GLfloat));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            VertexArray::unbind();

            Texture2D texture{HELLO_TEXTURE_DIR "/uv-test.png"};

            std::cout << "Texture loaded: " << texture.getWidth()
                      << "x" << texture.getHeight() << std::endl;
            shader.setInt("imageTexture", 0);


            Camera camera{Vec3{0.0f, 0.0f, 5.0f}};

            float lastFrame = (float)glfwGetTime();

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double lastMouseX = 0.0;
            double lastMouseY = 0.0;
            bool firstMouse = true;
            bool reloadWasPressed = false;


            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();

                bool reloadPressed = glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS;
                if (reloadPressed && !reloadWasPressed)
                    shader.reload();
                reloadWasPressed = reloadPressed;

                float currentFrame = (float)glfwGetTime();
                float radius = 8.0f;
                float angularSpeed = 1.0f;
                float angle = currentFrame*angularSpeed;

                Vec3 lightPos{
                    radius * std::cos(angle),
                    8.0f,
                    radius * std::sin(angle)
                };
                float deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                double mouseX;
                double mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                if (firstMouse)
                {
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                    firstMouse = false;
                }

                float offsetX = static_cast<float>(mouseX - lastMouseX);
                float offsetY = static_cast<float>(lastMouseY - mouseY);

                lastMouseX = mouseX;
                lastMouseY = mouseY;

                camera.rotate(offsetX, offsetY);

                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, GLFW_TRUE);

                float forwardInput = 0.0f;
                float rightInput = 0.0f;

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    forwardInput += 1.0f;

                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    forwardInput -= 1.0f;

                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    rightInput -= 1.0f;

                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    rightInput += 1.0f;

                camera.move(forwardInput, rightInput, deltaTime);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glEnable(GL_DEPTH_TEST);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                shader.use();

                float t = (float)glfwGetTime();
                Mat4 model = Mat4::identity();
                //Mat4 model = Mat4::multiply(
                //    Mat4::rotateY(t),
                //    Mat4::rotateX(t * 0.6f)
                //);
                Mat4 view = camera.getViewMatrix();
                Mat4 projection = Mat4::perspective(
                    45.0f * 3.14159265f / 180.0f,
                    800.0f / 600.0f,
                    0.1f,
                    100.0f
                );
                Mat4 mvp = Mat4::multiply(
                    projection,
                    Mat4::multiply(view, model)
                );

                shader.setMat4("transform", mvp);
                shader.setMat4("model", model);
                shader.setVec3("lightPos", lightPos);
                texture.bind(0);
                shader.setInt("imageTexture", 0);
                vertexArray.bind();
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                VertexArray::unbind();
                glfwSwapBuffers(window);
            }
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}
