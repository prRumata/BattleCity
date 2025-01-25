#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Resources/ResourceManager.h"
#include "Renderer/ShaderProgram.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Sprite.h"

glm::ivec2 g_windowSize(640, 480);

void glfwWindowSizeCallback(GLFWwindow * pWindow, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    glViewport(0, 0, g_windowSize.x, g_windowSize.y);
}

void glfwKeyCallback(GLFWwindow * pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}

int main(int argc, char** argv)
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cerr << "ERROR: glfwInit failed!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Take monitor resolution */
    g_windowSize.x = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    g_windowSize.y = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* pWindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Battle City", glfwGetPrimaryMonitor(), nullptr);
    if (!pWindow)
    {
        std::cerr << "ERROR: glfwCreateWindow failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    /* Make the window's context current */
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cerr << "ERROR: Cannot load GLAD!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(0.15, 0.15, 0.15, 1);

    {
        ResourceManager resourceManager(argv[0]);
        auto pSpriteShaderProgram = resourceManager.loadShaders("SpriteShader", "res/shaders/sprite.vert", "res/shaders/sprite.frag");
        if (!pSpriteShaderProgram)
        {
            std::cerr << "Can't create shader program: " << "SpriteShader" << std::endl;
            return -1;
        }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/kirpichi.png");
        if (!tex)
        {
            std::cerr << "Can't loading texture: " << "DefaultTexture" << std::endl;
            return -1;
        }

        auto pSprite = resourceManager.loadSprite("NewSprite", "DefaultTexture", "SpriteShader", 500, 500);
        if (!pSprite)
        {
            std::cerr << "Can't loading sprite: " << "NewSprite" << std::endl;
            return -1;
        }
        pSprite->setPosition(glm::vec2(500, 300));

        pSpriteShaderProgram->use();
        pSpriteShaderProgram->setInt("tex", 0);

        glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(g_windowSize.x), 0.f, static_cast<float>(g_windowSize.y), -100.f, 100.f);

        pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(pWindow))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            pSprite->render();

            /* Swap front and back buffers */
            glfwSwapBuffers(pWindow);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
