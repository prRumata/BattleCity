#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"

#include <sstream>
#include <fstream>
#include <iostream>

ResourceManager::ResourceManager(const std::string& executablePath)
{
    size_t found = executablePath.find_last_of("/\\");
    m_path = executablePath.substr(0, found);
}

std::string ResourceManager::getFileString(const std::string& relativeFilePath) const
{
    std::ifstream f;
    f.open(m_path + "/" + relativeFilePath.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
    {
        std::cerr << "Failed to open file: " << relativeFilePath << std::endl;
        return std::string{};
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexString = getFileString(vertexPath);
    if (vertexPath.empty())
    {
        std::cerr << "No vertex shader!" << std::endl;
        return nullptr;
    }

    std::string fragmentString = getFileString(fragmentPath);
    if (fragmentPath.empty())
    {
        std::cerr << "No fragment shader!" << std::endl;
        return nullptr;
    }

    std::shared_ptr<Renderer::ShaderProgram>& newShader = m_shaderPrograms.emplace(shaderName, std::make_shared<Renderer::ShaderProgram>(vertexString, fragmentString)).first->second;
    if (!newShader->isCompiled())
    {
        std::cerr << "Can't load shader program:\n"
                << "Vertex: " << vertexPath << "\n"
                << "Fragment: " << fragmentPath << std::endl;
        return nullptr;
    }

    return newShader;
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName)
{
    ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
    if (it == m_shaderPrograms.end())
    {
        std::cerr << "Can't find the shader program: " << shaderName << std::endl;
        return nullptr;
    }
    return it->second;
}