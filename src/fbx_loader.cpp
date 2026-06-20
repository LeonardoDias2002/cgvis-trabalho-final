// Carregador de animações FBX com Assimp
// Este módulo é responsável por carregar modelos 3D no formato FBX usando a biblioteca

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

#include "fbx_loader.h"
#include "globals.h"

// HEaders do Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//necessario para a LoadFBX pois as outras do OpenGL não funcionaram
#include <cstdio>
#include <glm/glm.hpp>
#include <algorithm>
//#include <limits>

// função para construir um modelo FBX a partir de uma estrutura MeshSOA e adicioná-lo à cena virtual
void BuildFBXAndAddToVirtualScene(const MeshSOA& mesh, const std::string& object_name)
{
    // Aqui você pode criar um VAO, VBOs e configurar os atributos de vértice
    // para o modelo FBX usando os dados contidos na estrutura MeshSOA.
    // Depois, crie um SceneObject e adicione-o à g_VirtualScene com o nome fornecido.;
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);
    GLuint position_vbo;

    glGenBuffers(1,&position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,position_vbo);
    glBufferData( GL_ARRAY_BUFFER, mesh.positions.size()*sizeof(glm::vec3), mesh.positions.data(), GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT,GL_FALSE,0,nullptr);
    glEnableVertexAttribArray(0);
    GLuint normal_vbo;

    glGenBuffers(1,&normal_vbo);

    glBindBuffer(GL_ARRAY_BUFFER,normal_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.normals.size()*sizeof(glm::vec3),
        mesh.normals.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer( 1, 3, GL_FLOAT,GL_FALSE,0,nullptr);

    glEnableVertexAttribArray(1);

    GLuint uv_vbo;

    glGenBuffers(1,&uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,uv_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.texcoords.size()*sizeof(glm::vec2),
        mesh.texcoords.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(2 , 2, GL_FLOAT, GL_FALSE , 0 , nullptr);

    glEnableVertexAttribArray(2);

    GLuint bone_vbo;

    glGenBuffers(1,&bone_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,bone_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.boneIds.size()*sizeof(glm::ivec4),
        mesh.boneIds.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribIPointer(
    4,
    4,
    GL_INT,
    0,
    nullptr
);

    glEnableVertexAttribArray(4);

    GLuint weight_vbo;

    glGenBuffers(1,&weight_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,weight_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.weights.size()*sizeof(glm::vec4),
        mesh.weights.data(),
        GL_STATIC_DRAW
    );

        glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(5);

        GLuint ebo;

    glGenBuffers(1,&ebo);

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        ebo
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.indices.size()*sizeof(uint32_t),
        mesh.indices.data(),
        GL_STATIC_DRAW
    );

    SceneObject obj;

    obj.name = object_name;

    obj.first_index = 0;
    obj.num_indices = mesh.indices.size();

    obj.rendering_mode = GL_TRIANGLES;

    obj.vertex_array_object_id = vertex_array_object_id;

    glm::vec3 bbox_min(0.0f);
    glm::vec3 bbox_max(0.0f);

    if (!mesh.positions.empty())
    {
        bbox_min = mesh.positions[0];
        bbox_max = mesh.positions[0];

        for (const auto& p : mesh.positions)
        {
            bbox_min.x = std::min(bbox_min.x, p.x);
            bbox_min.y = std::min(bbox_min.y, p.y);
            bbox_min.z = std::min(bbox_min.z, p.z);

            bbox_max.x = std::max(bbox_max.x, p.x);
            bbox_max.y = std::max(bbox_max.y, p.y);
            bbox_max.z = std::max(bbox_max.z, p.z);
        }
    }

    obj.bbox_min = bbox_min;
    obj.bbox_max = bbox_max;

    printf(
    "bbox = (%f %f %f) -> (%f %f %f)\n",
    bbox_min.x,
    bbox_min.y,
    bbox_min.z,
    bbox_max.x,
    bbox_max.y,
    bbox_max.z
    );

    printf("Criando objeto %s\n", object_name.c_str());
    printf("Vertices: %zu\n", mesh.positions.size());
    printf("Indices : %zu\n", mesh.indices.size());

    g_VirtualScene[object_name] = obj;

    printf("Objeto salvo. VAO=%u indices=%zu\n",
        obj.vertex_array_object_id,
        obj.num_indices);





}

//função responsável por ler um arquivo .fbx do disco e converter seus dados para o formato usado pelo MeshSOA
MeshSOA LoadFBX(const std::string& filename)
{
    MeshSOA mesh;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs
    );

    if (!scene)
    {
        fprintf(stderr, "Erro Assimp: %s\n",
                importer.GetErrorString());
        return mesh;
    }

    if (scene->mNumMeshes == 0)
    {
        fprintf(stderr, "FBX sem meshes.\n");
        return mesh;
    }

    printf("Numero de meshes: %u\n", scene->mNumMeshes);

    for (unsigned i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* m = scene->mMeshes[i];

        printf(
            "Mesh %u -> vertices=%u faces=%u nome='%s'\n",
            i,
            m->mNumVertices,
            m->mNumFaces,
            m->mName.C_Str()
        );
    }

    aiMesh* ai_mesh = scene->mMeshes[1]; // teste

    // Escolha qual mesh carregar

    // Vértices
    mesh.positions.resize(ai_mesh->mNumVertices);
    mesh.normals.resize(ai_mesh->mNumVertices);
    mesh.texcoords.resize(ai_mesh->mNumVertices);

    for (unsigned i = 0; i < ai_mesh->mNumVertices; i++)
    {
        // posição
        mesh.positions[i] =
            glm::vec3(
                ai_mesh->mVertices[i].x,
                ai_mesh->mVertices[i].y,
                ai_mesh->mVertices[i].z
            );

        // normal
        if (ai_mesh->HasNormals())
        {
            mesh.normals[i] =
                glm::vec3(
                    ai_mesh->mNormals[i].x,
                    ai_mesh->mNormals[i].y,
                    ai_mesh->mNormals[i].z
                );
        }

        // UV
        if (ai_mesh->HasTextureCoords(0))
        {
            mesh.texcoords[i] =
                glm::vec2(
                    ai_mesh->mTextureCoords[0][i].x,
                    ai_mesh->mTextureCoords[0][i].y
                );
        }
    }

    // Índices
    for (unsigned f = 0; f < ai_mesh->mNumFaces; f++)
    {
        const aiFace& face = ai_mesh->mFaces[f];

        for (unsigned j = 0; j < face.mNumIndices; j++)
        {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }
    printf("Numero de meshes: %u\n", scene->mNumMeshes);
    return mesh;
}