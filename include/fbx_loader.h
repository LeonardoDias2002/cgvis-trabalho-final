//Header do loader.cpp aí
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>



struct MeshSOA
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    std::vector<glm::ivec4> boneIds;
    std::vector<glm::vec4> weights;

    std::vector<unsigned int> indices;
};

MeshSOA LoadFBX(const std::string& filename);

void BuildFBXAndAddToVirtualScene(const MeshSOA& mesh, const std::string& object_name);