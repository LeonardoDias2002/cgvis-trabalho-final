/**
 * @file globals.h
 * @brief Declarações centrais do projeto GOLFinho: structs, enums, constantes
 *        de object_id e variáveis globais compartilhadas entre módulos.
 *
 * Todas as variáveis declaradas aqui como `extern` são instanciadas em
 * globals.cpp. Isso evita duplicação e garante uma única definição.
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_obj_loader.h>
#include <stb_image.h>

#include "matrices.h"
#include "utils.h"

// =============================================
// STRUCTS
// =============================================

/*
Representa um modelo geométrico carregado de um arquivo ".obj".
Usa a biblioteca tinyobjloader para parsear vértices, normais, texturas
e materiais. Veja: https://en.wikipedia.org/wiki/Wavefront_.obj_file
Estrutura que representa um modelo geométrico carregado a partir de um
arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file . */
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, então setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretório dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};


/*
Cada SceneObject armazena o nome, intervalo de índices no VBO,
modo de rasterização e a bounding box (AABB) do modelo. */
struct SceneObject
{
    std::string  name;                  ///< Nome do objeto
    size_t       first_index;           ///< Índice do primeiro vértice no vetor de índices
    size_t       num_indices;           ///< Número de índices do objeto
    GLenum       rendering_mode;        ///< Modo de rasterização (GL_TRIANGLES, etc.)
    GLuint       vertex_array_object_id;///< ID do VAO com os atributos do modelo
    glm::vec3    bbox_min;              ///< Mínimo da Axis-Aligned Bounding Box
    glm::vec3    bbox_max;              ///< Máximo da Axis-Aligned Bounding Box
};

// =============================================
// CONSTANTES DE OBJECT ID
// Usadas no fragment shader para distinguir objetos.
// IMPORTANTE: Devem ser mantidas em sincronia com
// shader_fragment.glsl.
// =============================================
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define TACO   3
#define BOLA   4
#define BURACO 5
#define TRAJETORIA 6
#define BANDEIRA 7
#define MASTRO 8
#define HUD_BARRA 9
#define GRAMA 10
#define PISTALOOP 11


#define ARVORE_ALTA 15
#define ARVORE_BAIXA 16

#define PISTACURVA 18
#define PISTASIMPLES 19
#define BORDASSIMPLES 20
#define BORDASCURVA 21
#define BORDASLOOP 22
#define TRONCO 23
#define ZEPPELIN 24
#define PISTAQUATRO 25
#define PISTAQUATRO2 26 
#define BORDASQUATRO 27
#define BORDASQUATRO2 28
#define ESPINHOS 29
#define SQZO 30
#define PISTAQUATRO3 31
// =============================================
// ENUM DE ESTADO DO JOGO
// =============================================


enum GameState { MENU_MAIN, MENU_LEVELS, MENU_SETTINGS, PLAYING, MENU_LEVEL_COMPLETE, MENU_PAUSE };

// =============================================
// VARIÁVEIS GLOBAIS (extern — definidas em globals.cpp)
// =============================================

// --- Cena Virtual e Pilha de Matrizes ---
extern std::map<std::string, SceneObject> g_VirtualScene;
extern std::stack<glm::mat4> g_MatrixStack;
extern float g_ScreenRatio;

// --- Ângulos de Euler (rotação de objetos de debug) ---
extern float g_AngleX;
extern float g_AngleY;
extern float g_AngleZ;

// --- Estado dos botões do mouse ---
extern bool g_LeftMouseButtonPressed;
extern bool g_RightMouseButtonPressed;
extern bool g_MiddleMouseButtonPressed;

// --- Câmera (coordenadas esféricas) ---
extern float g_CameraTheta;
extern float g_CameraPhi;
extern float g_CameraDistance;
extern glm::mat4 view;

// Variável que controla o nível atual do jogo 
extern int g_nivelAtual;

// --- Controle de antebraço/torso (do código base da disciplina) ---
extern float g_ForearmAngleZ;
extern float g_ForearmAngleX;
extern float g_TorsoPositionX;
extern float g_TorsoPositionY;

// --- Bola de Golfe (Jogador 1) ---
extern glm::vec3 g_PosBola;
extern glm::vec3 g_PosTaco;
extern float g_DistanciaTaco;
extern float g_TacoRotacao;
extern float g_TacoRotacaoVertical;

// --- Animação do Taco ---
extern double g_TempoRotacaoTaco;
extern double g_TempoDesdeEspaco;
extern double g_TempoDesdeTacada;
extern float g_DuracaoRotacaoTaco;
extern float g_AnguloRotacaoTaco;

// --- Física da Bola (Jogador 1) ---
extern glm::vec3 g_VelocidadeBola;
extern bool g_BolaParada;
extern bool g_BolaNoBuraco;
extern glm::mat4 g_BolaRotationMatrix;

// --- Mecânica de Força da Tacada ---
extern bool g_EspacoPressionado;
extern double g_InicioEspaco;
extern float g_ForcaTacada;

// --- Iluminação ---
extern glm::vec3 g_PosLuz;

// --- Projeção ---
extern float field_of_view;
extern bool rotacao_camera;
extern bool g_UsePerspectiveProjection;

// --- HUD / Texto ---
extern bool g_ShowInfoText;

// --- Programa de GPU ---
extern GLuint g_GpuProgramID;
extern GLint g_model_uniform;
extern GLint g_view_uniform;
extern GLint g_projection_uniform;
extern GLint g_object_id_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;

// --- Texturas ---
extern GLuint g_NumLoadedTextures;

// --- Multiplayer ---
extern bool g_MultiplayerAtivo;
extern bool g_JogadorAtual;
extern bool g_BolaNoBuracoTwo;
extern bool g_TerminouJogada;
extern bool g_BolaEmFocoAtual;

extern glm::vec3 g_PosBolaTwo;
extern glm::vec3 g_PosTacoTwo;
extern glm::vec3 g_VelocidadeBolaTwo;
extern glm::mat4 g_BolaRotationMatrixTwo;

extern int g_TacadasPlayer1;
extern int g_TacadasPlayer2;

// --- Loop Physics State Machine ---
// Controla a física por Path Following (Waypoints) do loop (PistaLoop)
extern bool g_BolaNoCaminho;
extern int g_IndiceCaminho;
extern float g_ProgressoCaminho;

extern bool g_BolaNoCaminhoTwo;
extern int g_IndiceCaminhoTwo;
extern float g_ProgressoCaminhoTwo;

extern std::vector<glm::vec3> g_LoopWaypoints;

// Posição do buraco depende do nível
extern glm::vec3 g_HolePosition;

// --- Heightmap: Triângulos da pista para raycasting ---
struct TrackTriangle {
    glm::vec3 v0, v1, v2;
};
extern std::vector<TrackTriangle> g_PistaCurvaTriangles;
extern std::vector<TrackTriangle> g_PistaCurvaAllTriangles; // ALL triangles for 3D physics
extern std::vector<TrackTriangle> g_PistaLoopTriangles;
extern std::vector<TrackTriangle> g_PistaLoopAllTriangles; // ALL triangles for 3D physics
extern std::vector<TrackTriangle> g_PistaQuatroTriangles;
extern std::vector<TrackTriangle> g_PistaQuatroAllTriangles;

// --- Sistema de Menu ---
extern GameState g_CurrentState;
extern float g_MenuCameraAngle;
extern float g_MusicVolume;
extern float g_AmbientVolume;
extern int g_TexturaPistaGrama;
extern int g_TexturaPistaParede;
extern int g_TexturaBola;
extern int g_TexturaTaco;
extern GLuint g_HudShaderProgram;
extern GLuint g_HudVAO;
extern GLuint g_HudVBO;
extern GLuint g_LogoTextureID;
extern int g_LogoWidth, g_LogoHeight;

// --- Hover States dos Botões do Menu ---
extern bool g_HoverJogar, g_HoverMultiplayer;
extern bool g_HoverConfig, g_HoverSair;
extern bool g_HoverVoltar;
extern bool g_HoverGramaL, g_HoverGramaR;
extern bool g_HoverParedeL, g_HoverParedeR;
extern bool g_HoverBolaL, g_HoverBolaR;
extern bool g_HoverTacoL, g_HoverTacoR;
extern bool g_HoverProxPista, g_HoverMenuCompleto;
extern bool g_HoverContinuar, g_HoverSairPause;

// --- Posição do cursor (para cálculo de delta no mouse) ---
extern double g_LastCursorPosX, g_LastCursorPosY;

// --- Shader de texto (definido em textrendering.cpp) ---
extern GLuint textprogram_id;

// --- Sistema de Trilha da Bola ---
struct TrailSegment
{
    glm::vec3 posStart;   ///< Posição inicial do segmento
    glm::vec3 posEnd;     ///< Posição final do segmento
    float opacidade;        ///< Opacidade do segmento (desaparece gradualmente)
    float tempoMax;         ///< Tempo máximo de vida do segmento
    float tempoAtual;     ///< Tempo de vida atual do segmento
};

extern std::vector<TrailSegment> g_TrailSegmentsBola;
extern std::vector<TrailSegment> g_TrailSegmentsBola2;
extern glm::vec3 g_TrailColor;
extern float g_Trailopacidade;
extern float g_TrailSegmentLength;
extern float g_TrailtempoMax;
extern float g_TrailThickness;
extern glm::vec3 g_LastTrailPosBola;
extern glm::vec3 g_LastTrailPosBola2;


void DrawVirtualObject(const char* object_name);


#endif // GLOBALS_H