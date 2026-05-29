/**
 * @file gpu_utils.h
 * @brief Protótipos de funções relacionadas à GPU: carregamento de shaders,
 *        texturas, construção de VAOs e renderização de objetos.
 */
#ifndef GPU_UTILS_H
#define GPU_UTILS_H

#include "globals.h"

/** @brief Salva a matriz M no topo da pilha de matrizes de modelagem. */
void PushMatrix(glm::mat4 M);

/** @brief Remove a matriz do topo da pilha e armazena em M. */
void PopMatrix(glm::mat4& M);

/**
 * @brief Constrói a representação de um ObjModel como malha de triângulos
 *        e adiciona à cena virtual (g_VirtualScene).
 *
 * Para cada shape do modelo, cria VBOs com posições, normais e coordenadas
 * de textura, e um VAO que os agrupa. Os objetos ficam acessíveis pelo
 * nome da shape (e.g. "the_sphere", "the_plane").
 */
void BuildTrianglesAndAddToVirtualScene(ObjModel*);

/**
 * @brief Computa normais de um ObjModel caso não existam no .obj.
 *
 * Utiliza o método de Gouraud: a normal de cada vértice é a média
 * das normais das faces que o compartilham dentro do mesmo smoothing group.
 */
void ComputeNormals(ObjModel* model);

/**
 * @brief Carrega uma imagem do disco e envia para a GPU como textura.
 *
 * Usa stb_image para decodificar. A textura recebe a unidade de textura
 * g_NumLoadedTextures (incrementado automaticamente).
 *
 * @param filename Caminho para o arquivo de imagem (JPG, PNG, etc.)
 */
void LoadTextureImage(const char* filename);

/**
 * @brief Carrega e compila os shaders de vértice e fragmento,
 *        criando o programa de GPU (g_GpuProgramID).
 *
 * Também obtém os uniform locations das matrizes model/view/projection
 * e dos samplers de textura.
 */
void LoadShadersFromFiles();

/**
 * @brief Renderiza um objeto armazenado em g_VirtualScene.
 *
 * Liga o VAO correspondente, seta os uniforms de bounding box,
 * e chama glDrawElements.
 *
 * @param object_name Nome do objeto (chave em g_VirtualScene)
 */
void DrawVirtualObject(const char* object_name);

/** @brief Carrega um vertex shader de um arquivo GLSL. */
GLuint LoadShader_Vertex(const char* filename);

/** @brief Carrega um fragment shader de um arquivo GLSL. */
GLuint LoadShader_Fragment(const char* filename);

/** @brief Lê, compila e verifica um shader a partir de um arquivo. */
void LoadShader(const char* filename, GLuint shader_id);

/**
 * @brief Cria um programa de GPU linkando vertex e fragment shader.
 * @return ID do programa de GPU criado.
 */
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

/** @brief Imprime no terminal informações detalhadas de um ObjModel (para debugging). */
void PrintObjModelInfo(ObjModel*);

#endif // GPU_UTILS_H
