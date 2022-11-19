#ifndef LOADER_HPP
#define LOADER_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

struct Vertex
{
    Vertex(glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _norm)
        : pos { std::move(_pos) }
        , uv { std::move(_uv) }
        , normal { std::move(_norm) }
    {}

    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct ObjectBufferData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

ObjectBufferData loadObjFile(const std::string_view& filepath);

#endif // LOADER_HPP