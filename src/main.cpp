#include <iostream>
#include <exception>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Camera.h>
#include <Buffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture2D.h>
#include <Light.h>
#include <ObjLoader.h>
#include <Mesh.h>
#include <Framebuffer.h>

void 
glInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int
main()
{
    const GLint WIDTH = 800, HEIGHT = 600;
    glInit();

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
        Shader shader{ HELLO_SHADER_DIR "/basic.vert", HELLO_SHADER_DIR "/basic.frag" };
        Shader lightShader{ HELLO_SHADER_DIR "/light.vert", HELLO_SHADER_DIR "/light.frag" };
        Shader screenShader{ HELLO_SHADER_DIR "/screen.vert", HELLO_SHADER_DIR "/screen.frag" };

        Framebuffer sceneFramebuffer;

        const float screenVertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };

        Buffer screenVBO{GL_ARRAY_BUFFER};
        VertexArray screenVAO;
        screenVAO.bind();
        screenVBO.upload(sizeof(screenVertices), screenVertices);
        screenVAO.setAttribute(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
        screenVAO.setAttribute(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 2*sizeof(float));
        VertexArray::unbind();

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

            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f, 0.0f, 
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f,  1.0f, 0.0f, 
             0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f, 0.0f, 
        };

        
        VertexArray vertexArray;
        Buffer vertexBuffer{GL_ARRAY_BUFFER};
        Buffer indexBuffer{GL_ELEMENT_ARRAY_BUFFER};

        vertexArray.bind();
        vertexBuffer.upload(sizeof(vertices), vertices);
        indexBuffer.upload(sizeof(indices), indices);

        vertexBuffer.bind();
        vertexArray.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
        vertexArray.setAttribute(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 3*sizeof(GLfloat));
        vertexArray.setAttribute(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 5*sizeof(GLfloat));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        VertexArray::unbind();

        Texture2D texture{HELLO_TEXTURE_DIR "/uv-test.png", true, Texture2D::ColorSpace::SRGB};
        Texture2D specularTexture{HELLO_TEXTURE_DIR "/specular-mask.png", true, Texture2D::ColorSpace::Linear};
        std::cout << "Texture loaded: " << texture.getWidth()<< "x" << texture.getHeight() << std::endl;
        shader.setInt("imageTexture", 0);
        shader.setInt("specularMap", 1);

        std::vector<Vertex> vertex = loadObj(HELLO_MODELS_DIR "/pyramid.obj");
        Mesh pyramid{vertex};


        Camera camera{Vec3{0.0f, 0.0f, 5.0f}};

        float lastFrame = (float)glfwGetTime();

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        bool firstMouse = true;
        bool reloadWasPressed = false;
        int effectMode = 0;

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            int previousEffect = effectMode;
            for (int mode = 0; mode < 5; ++mode)
            {
                if (glfwGetKey(window, GLFW_KEY_1 + mode) == GLFW_PRESS)
                    effectMode = mode;
            }

            if (effectMode != previousEffect)
            {
                const char* names[] = { "Original", "Invert", "Grayscale", "Gaussian blur", "Sharpen" };
                std::cout << "Post-process: " << names[effectMode] << std::endl;
            }

            bool reloadPressed = glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS;
            if (reloadPressed && !reloadWasPressed)
            {
                shader.reload();
                lightShader.reload();
                screenShader.reload();
            }
            reloadWasPressed = reloadPressed;

            float currentFrame = (float)glfwGetTime();
            float radius = 8.0f;
            float angularSpeed = 1.0f;
            float angle = currentFrame*angularSpeed;

            float radiusLight = 2.0f;

            Vec3 lightPos{ radiusLight*std::cos(angle), 2.0f, radiusLight *std::sin(angle) };
            Vec3 lightColour{ 1.0f, 0.5f, 0.2f }; 
			Light light{ lightPos, lightColour, 2.0f };
            Light secondLight{ Vec3{ -2.0f, -1.0f, 2.0f }, Vec3{ 0.15f, 0.4f, 1.0f }, 1.5f };

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

            glfwGetFramebufferSize(window, &width, &height);
            if (width == 0 || height == 0)
                continue;

            sceneFramebuffer.resize(width, height);
            sceneFramebuffer.bind();

            glViewport(0, 0, width, height);
            glEnable(GL_CULL_FACE);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();

            float t = (float)glfwGetTime();
            //Mat4 model = Mat4::scale(2.0f, 1.0f, 0.5f);
            Mat4 model = Mat4::identity();
            //Mat4 model = Mat4::multiply(
            //    Mat4::rotateY(t),
            //    Mat4::rotateX(t * 0.6f)
            //);
            Mat4 view = camera.getViewMatrix();
            Mat4 projection = Mat4::perspective(
                45.0f * 3.14159265f / 180.0f,
                static_cast<float>(width) / static_cast<float>(height),
                0.1f,
                100.0f
            );
            Mat4 mvp = Mat4::multiply(
                projection,
                Mat4::multiply(view, model)
            );

            shader.setMat4("transform", mvp);
            shader.setMat4("model", model);
            shader.setVec3("lightPos", light.getPosition());
            shader.setVec3("lightColour", light.getColor());
            shader.setVec3("cameraPos", camera.getPosition());
            texture.bind(0);
            shader.setInt("imageTexture", 0);
            specularTexture.bind(1);
            shader.setInt("specularMap", 1);
            shader.setFloat("lightIntensity", light.getIntensity());
            shader.setVec3("secondLightPos", secondLight.getPosition());
            shader.setVec3("secondLightColour", secondLight.getColor());
            shader.setFloat("secondLightIntensity", secondLight.getIntensity());
            pyramid.draw();

            vertexArray.bind();
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            for (const Light* visibleLight: { &light, &secondLight } )
            {
                Vec3 position = visibleLight->getPosition();
                Mat4 lightModel = Mat4::multiply(Mat4::translate(position.x, position.y, position.z), Mat4::scale(0.1f));
                Mat4 lightMvp = Mat4::multiply( projection, Mat4::multiply(view, lightModel));

                lightShader.setMat4("transform", lightMvp);
                lightShader.setVec3("lightColour", visibleLight->getColor());
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

            VertexArray::unbind();

            Framebuffer::bindDefault();
            glViewport(0, 0, width, height);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_FRAMEBUFFER_SRGB);
            screenShader.use();
            sceneFramebuffer.bindColorTexture(0);
            screenShader.setInt("screenTexture", 0);
            screenShader.setInt("effectMode", effectMode);
            screenVAO.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            VertexArray::unbind();

            glfwSwapBuffers(window);
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
