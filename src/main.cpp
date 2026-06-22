//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
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

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "trail.h"
#include "globals.h"


// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso não existam.
glm::mat4 CalcularTaco(glm::vec3 posicao_bola, glm::vec3 direcao_taco = glm::vec3(0.0f, 0.0f, -1.0f), float distancia_offset = 0.2f); //calcula a posição do taco
void LoadTextureImage(const char* filename); // Função que carrega imagens de textura
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Função para debugging
float RaycastTrackHeight(float x, float z, float fallbackY);
void RotacionarTaco(GLFWwindow* window); // atualiza a animação do taco

// Funções do sistema de menu
void MenuInit();
void MenuUpdate(GLFWwindow* window, float delta_time);
void MenuRenderOverlay(GLFWwindow* window);
void MenuRenderMainMenu(GLFWwindow* window);
void MenuRenderLevelSelect(GLFWwindow* window);
void MenuRenderSettings(GLFWwindow* window);
void MenuHandleClick(GLFWwindow* window);
void DrawHudQuad(float cx, float cy, float hw, float hh);
bool IsMouseOverRect(GLFWwindow* w, float cx, float cy, float hw, float hh);
bool RenderButton(GLFWwindow* window, float cx, float cy, float hw, float hh,
                  const char* label, float r1, float g1, float b1, float r2, float g2, float b2,
                  bool enabled);
void RenderSlider(GLFWwindow* window, float cx, float cy, float hw, float hh, float value);
GLuint LoadTextureImageRGBA(const char* filename, int* outW, int* outH);
GLuint CompileHudShaders();


// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// função para avançar para o próximo nível
void ProxNivel(GLFWwindow* window);

// função para atualizar as posições das bolas de golfe
void AtualizarFisicaBola(float delta_time);
void AtualizarFisicaBolaTwo(float delta_time);

// função para atualizar a posição da câmera
void AtualizarCamera();

// Extern para acessar o shader de texto (definido em textrendering.cpp)
extern GLuint textprogram_id;

static float RaycastTrackHeight(const std::vector<TrackTriangle>& tris, float x, float current_y, float z, float fallbackY);

// Áudio Globals
#include "miniaudio.h"
ma_engine g_audioEngine;
ma_sound g_musicMenu;
ma_sound g_ambientSound;
ma_sound g_hitSound;
ma_sound g_victorySound;
ma_sound g_wallSound;
bool g_audioInitialized = false;

// --- Zeppelin & Bezier ---
float g_ZeppelinT = 0.0f;

// Curva de Bezier Cúbica
glm::vec3 CalculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 p = uuu * p0;
    p += 3 * uu * t * p1;
    p += 3 * u * tt * p2;
    p += ttt * p3;
    return p;
}
int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047 - Gabriel & Leonardo - Trabalho Final Computação Gráfica e Visualização", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    // Carregamos duas imagens para serem utilizadas como textura
    LoadTextureImage("../../data/red_brick_diff_1k.jpg");      // TextureImage0
    LoadTextureImage("../../data/rocky_terrain_02_diff_1k.jpg"); // TextureImage1
    LoadTextureImage("../../data/golf_fbx_golf_club_BaseColor.png"); // TextureImage2
    LoadTextureImage("../../data/t7.jpg");        // TextureImage3
    LoadTextureImage("../../data/tronco.jpg");    // TextureImage4
    LoadTextureImage("../../data/grass.jpg");     // TextureImage5
    LoadTextureImage("../../data/track.jpg");     // TextureImage6
    LoadTextureImage("../../data/zeppelin.png");  // TextureImage7

    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ObjModel spheremodel("../../data/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel("../../data/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);

    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    // Construímos o Taco de Golfe
    ObjModel tacomodel("../../data/golf_club_and_ball_obj.obj");
    ComputeNormals(&tacomodel);
    BuildTrianglesAndAddToVirtualScene(&tacomodel);

    // Construímos a Bola de Golfe
    ObjModel bolamodel("../../data/golf_ball.obj");
    ComputeNormals(&bolamodel);
    BuildTrianglesAndAddToVirtualScene(&bolamodel);

    // Construímos a Pista com loop
    ObjModel PistaLoopmodel("../../data/PistaLoop.obj");
    ComputeNormals(&PistaLoopmodel);
    BuildTrianglesAndAddToVirtualScene(&PistaLoopmodel);

    // Construímos a bandeira
    ObjModel bandeiramodel("../../data/bandeira.obj");
    ComputeNormals(&bandeiramodel);
    BuildTrianglesAndAddToVirtualScene(&bandeiramodel);

    // Construímos a Pista cCurva
    ObjModel PistaCurvamodel("../../data/PistaCurva.obj");
    ComputeNormals(&PistaCurvamodel);
    BuildTrianglesAndAddToVirtualScene(&PistaCurvamodel);

    // Extrair triângulos da PistaCurva para heightmap (world space: scale=0.25, Y offset=0.064)
    ObjModel Zeppelinmodel("../../data/zeppelin.obj");
    ComputeNormals(&Zeppelinmodel);
    BuildTrianglesAndAddToVirtualScene(&Zeppelinmodel);
    // APENAS triângulos com normal apontando pra cima (chão), excluindo paredes e teto
    {
        const auto& attrib = PistaCurvamodel.attrib;
        float scale = 0.25f;
        float y_off = 0.064f;
        for (const auto& shape : PistaCurvamodel.shapes) {
            size_t idx_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                if (fv >= 3) {
                    glm::vec3 v0(
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+0]*scale,
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+1]*scale + y_off,
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+2]*scale);
                    for (int t = 1; t < fv - 1; t++) {
                        glm::vec3 v1(
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+0]*scale,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+1]*scale + y_off,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+2]*scale);
                        glm::vec3 v2(
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+0]*scale,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+1]*scale + y_off,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+2]*scale);
                        // Guardar todos os triângulos para física 3D
                        g_PistaCurvaAllTriangles.push_back({v0, v1, v2});
                        // Filtrar: apenas triângulos com normal apontando pra cima e que não sejam borda
                        bool isBorda = (shape.name.find("borda") != std::string::npos);
                        glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
                        float len = glm::length(normal);
                        if (!isBorda && len > 1e-7f && (normal.y / len) > 0.3f) {
                            g_PistaCurvaTriangles.push_back({v0, v1, v2});
                        }
                    }
                }
                idx_offset += fv;
            }
        }
        printf("PistaCurva: %zu triângulos de chão extraídos para heightmap.\n", g_PistaCurvaTriangles.size());
    }

    // Extrair triângulos da PistaLoop para heightmap (world space: scale=1.0, Y offset=1.12)
    // APENAS triângulos com normal apontando pra cima (chão da pista)
    {
        const auto& attrib = PistaLoopmodel.attrib;
        float scale = 1.0f;
        float y_off = 1.12f;
        for (const auto& shape : PistaLoopmodel.shapes) {
            size_t idx_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                if (fv >= 3) {
                    glm::vec3 v0(
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+0]*scale,
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+1]*scale + y_off,
                        attrib.vertices[3*shape.mesh.indices[idx_offset+0].vertex_index+2]*scale);
                    for (int t = 1; t < fv - 1; t++) {
                        glm::vec3 v1(
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+0]*scale,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+1]*scale + y_off,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t].vertex_index+2]*scale);
                        glm::vec3 v2(
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+0]*scale,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+1]*scale + y_off,
                            attrib.vertices[3*shape.mesh.indices[idx_offset+t+1].vertex_index+2]*scale);
                        // Filtrar: apenas triângulos com normal apontando pra cima
                        glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
                        float len = glm::length(normal);
                        bool isBorda = (shape.name.find("borda") != std::string::npos);
                        if (!isBorda && len > 1e-7f) {
                            g_PistaLoopAllTriangles.push_back({v0, v1, v2}); // Guarda apenas chão do loop
                            if ((normal.y / len) > 0.3f) {
                                g_PistaLoopTriangles.push_back({v0, v1, v2}); // Guarda apenas chão para física planar
                            }
                        }
                    }
                }
                idx_offset += fv;
            }
        }
        printf("PistaLoop: %zu triângulos de chão extraídos para heightmap.\n", g_PistaLoopTriangles.size());
    }

        // Caminho do Loop (Path Following Waypoints)
        g_LoopWaypoints.clear();
        // Curva extraída analiticamente do centro exato da geometria PistaLoop.obj
        // O loop possui formato 'corkscrew' e torce os eixos X entre 1.070 e 3.320.
        g_LoopWaypoints.push_back(glm::vec3(3.689f, 0.05f + 0.025f, -1.800f));
        g_LoopWaypoints.push_back(glm::vec3(3.685f, 0.05f + 0.025f, -1.380f));
        g_LoopWaypoints.push_back(glm::vec3(3.670f, 0.05f + 0.025f, -0.960f));
        g_LoopWaypoints.push_back(glm::vec3(2.738f, 0.05f + 0.025f, -0.540f));
        g_LoopWaypoints.push_back(glm::vec3(2.210f, 0.05f + 0.025f, -0.120f));
        g_LoopWaypoints.push_back(glm::vec3(2.576f, 0.293f, 0.166f));
        g_LoopWaypoints.push_back(glm::vec3(3.320f, 0.425f, 0.424f));
        g_LoopWaypoints.push_back(glm::vec3(3.177f, 0.629f, 0.628f));
        g_LoopWaypoints.push_back(glm::vec3(2.963f, 0.887f, 0.760f));
        g_LoopWaypoints.push_back(glm::vec3(2.756f, 1.173f, 0.805f));
        g_LoopWaypoints.push_back(glm::vec3(2.512f, 1.459f, 0.760f));
        g_LoopWaypoints.push_back(glm::vec3(2.381f, 1.717f, 0.628f));
        g_LoopWaypoints.push_back(glm::vec3(2.267f, 1.921f, 0.424f));
        g_LoopWaypoints.push_back(glm::vec3(2.216f, 2.053f, 0.166f));
        g_LoopWaypoints.push_back(glm::vec3(2.203f, 2.098f, -0.120f));
        g_LoopWaypoints.push_back(glm::vec3(2.194f, 2.053f, -0.406f));
        g_LoopWaypoints.push_back(glm::vec3(2.138f, 1.921f, -0.664f));
        g_LoopWaypoints.push_back(glm::vec3(2.035f, 1.717f, -0.868f));
        g_LoopWaypoints.push_back(glm::vec3(1.893f, 1.459f, -1.000f));
        g_LoopWaypoints.push_back(glm::vec3(1.648f, 1.173f, -1.045f));
        g_LoopWaypoints.push_back(glm::vec3(1.417f, 0.887f, -1.000f));
        g_LoopWaypoints.push_back(glm::vec3(1.245f, 0.629f, -0.868f));
        g_LoopWaypoints.push_back(glm::vec3(1.070f, 0.425f, -0.664f));
        g_LoopWaypoints.push_back(glm::vec3(1.649f, 0.293f, -0.406f));
        g_LoopWaypoints.push_back(glm::vec3(2.263f, 0.248f, -0.120f));
        g_LoopWaypoints.push_back(glm::vec3(1.693f, 0.05f + 0.025f, 0.285f));
        g_LoopWaypoints.push_back(glm::vec3(0.802f, 0.05f + 0.025f, 0.690f));
        g_LoopWaypoints.push_back(glm::vec3(0.826f, 0.05f + 0.025f, 1.095f));
        g_LoopWaypoints.push_back(glm::vec3(0.744f, 0.05f + 0.025f, 1.500f));

    // ArVore Baixa
    ObjModel ArvoreBaixamodel("../../data/ArvoreBaixa.obj");
    ComputeNormals(&ArvoreBaixamodel);
    BuildTrianglesAndAddToVirtualScene(&ArvoreBaixamodel);

    // Arvore Alta
    ObjModel ArvoreAltamodel("../../data/ArvoreAlta.obj");
    ComputeNormals(&ArvoreAltamodel);
    BuildTrianglesAndAddToVirtualScene(&ArvoreAltamodel);

    if ( argc > 1 ) {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Inicializamos o sistema de menu (HUD shader, logo, VAO/VBO)
    MenuInit();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK); glFrontFace(GL_CCW);

    float previous_time = (float)glfwGetTime();

    if (ma_engine_init(NULL, &g_audioEngine) == MA_SUCCESS) {
        g_audioInitialized = true;
        ma_sound_init_from_file(&g_audioEngine, "../../data/menu.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &g_musicMenu);
        ma_sound_set_looping(&g_musicMenu, MA_TRUE);
        ma_sound_init_from_file(&g_audioEngine, "../../data/ambiente.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &g_ambientSound);
        ma_sound_set_looping(&g_ambientSound, MA_TRUE);

        // Efeitos sonoros na memória
        ma_sound_init_from_file(&g_audioEngine, "../../data/hit.wav", 0, NULL, NULL, &g_hitSound);
        ma_sound_init_from_file(&g_audioEngine, "../../data/vitoria.mp3", 0, NULL, NULL, &g_victorySound);
        ma_sound_init_from_file(&g_audioEngine, "../../data/wall.wav", 0, NULL, NULL, &g_wallSound);

        // Volumes iniciais
        ma_sound_set_volume(&g_musicMenu, g_MusicVolume);
        ma_sound_set_volume(&g_ambientSound, g_AmbientVolume);
        ma_sound_set_volume(&g_hitSound, g_AmbientVolume);
        ma_sound_set_volume(&g_victorySound, g_AmbientVolume);
        ma_sound_set_volume(&g_wallSound, g_AmbientVolume);

        // Inicia música do menu
        if (g_CurrentState != PLAYING) {
            ma_sound_start(&g_musicMenu);
            ma_sound_set_fade_in_milliseconds(&g_musicMenu, 0.0f, 1.0f, 3000);
        }
    }

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        float current_time = (float)glfwGetTime();
        float delta_time = current_time - previous_time;
        previous_time = current_time;

        // ===== MENU MODE: renderiza menu e pula gameplay =====
        if (g_CurrentState != PLAYING && g_CurrentState != MENU_LEVEL_COMPLETE && g_CurrentState != MENU_PAUSE) {
            glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            MenuUpdate(window, delta_time);
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // Se estiver no MENU_LEVEL_COMPLETE ou MENU_PAUSE, queremos que a cena do jogo ainda seja desenhada por trás do overlay!
        // Então não damos 'continue', deixamos o render normal acontecer, e só bloqueamos as atualizações físicas.

        if (g_MultiplayerAtivo && g_TerminouJogada && (glfwGetTime() - g_TempoDesdeEspaco) > 0.2f) {
            if (!g_JogadorAtual && glm::length(g_VelocidadeBola) < 0.02f) {
                g_TerminouJogada = false;
                if (!g_BolaNoBuracoTwo) {
                    g_JogadorAtual = true;
                    g_BolaEmFocoAtual = true;
                }
            } else if (g_JogadorAtual && glm::length(g_VelocidadeBolaTwo) < 0.02f) {
                g_TerminouJogada = false;
                if (!g_BolaNoBuraco) {
                    g_JogadorAtual = false;
                    g_BolaEmFocoAtual = false;
                }
            }
        }

        // Atualização da física da bola
        if (g_CurrentState == PLAYING) {
            if (!g_BolaNoBuraco && !g_JogadorAtual) {
                AtualizarFisicaBola(delta_time);
                // Atualiza a trilha da bola 1
                if (glm::length(g_VelocidadeBola) > 0.01f) {
                    UpdateTrail(g_PosBola, g_LastTrailPosBola, delta_time);
                }
            } else if (!g_BolaNoBuracoTwo && g_JogadorAtual)
            {
                AtualizarFisicaBolaTwo(delta_time);
                // Atualiza a trilha da bola 2
                if (glm::length(g_VelocidadeBolaTwo) > 0.01f) {
                    UpdateTrail2(g_PosBolaTwo, g_LastTrailPosBola2, delta_time);
                }
            }
        }

        // Atualiza a idade dos segmentos de trilha
        UpdateTrailSegments(delta_time);

        // lógica de vitória e troca de níveis
        if (g_CurrentState == PLAYING) {
            bool won = false;
            if (!g_MultiplayerAtivo) {
                if (g_BolaNoBuraco) won = true;
            } else if (g_BolaNoBuraco && g_BolaNoBuracoTwo) {
                won = true;
            }
            if (won) {
                g_CurrentState = MENU_LEVEL_COMPLETE;
            }
        }

        // Aqui executamos as operações de renderização
        glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(g_GpuProgramID);

        // Envia uniforms de seleção de textura para o shader
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaGramaPista"), g_TexturaPistaGrama);
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaParedesPista"), g_TexturaPistaParede);
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaBola"), g_TexturaBola);
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaTaco"), g_TexturaTaco);
        glUniform1f(glGetUniformLocation(g_GpuProgramID, "u_Time"), glfwGetTime());

        g_TempoDesdeTacada = glfwGetTime() - g_TempoDesdeEspaco;
        //printf("Tempo desde a ultima tacada: %.2f \n", g_TempoDesdeTacada);

        // Calcular a força acumulada se a barra de espaço estiver sendo pressionada
        bool active_ball_stopped = false;
        if (!g_MultiplayerAtivo) active_ball_stopped = (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco);
        else active_ball_stopped = (!g_JogadorAtual) ? (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco) : (glm::length(g_VelocidadeBolaTwo) < 0.02f && !g_BolaNoBuracoTwo);
        bool turn_ready = (!g_MultiplayerAtivo || !g_TerminouJogada);

        if (g_EspacoPressionado && active_ball_stopped && turn_ready && g_TempoRotacaoTaco < 0.0) {
            double tempo_segurando = glfwGetTime() - g_InicioEspaco;
            // Limita a força (ex: 2 segundos para força máxima)
            g_ForcaTacada = std::min((float)tempo_segurando * 5.0f, 15.0f);
        }

        // Controle suave de mira do taco (apenas se a bola estiver parada)
        // Controle suave de mira do taco (apenas se a bola estiver parada e for seu turno)
        if (active_ball_stopped && turn_ready) {
            float delta_mira = M_PI * delta_time; // velocidade de rotação da mira
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                g_TacoRotacao += delta_mira;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                g_TacoRotacao -= delta_mira;
            }
        }

        RotacionarTaco(window); // chamamos a função que rotaciona o taco

        AtualizarCamera(); // chamamos a função que atualiza a câmera

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -50.0f; // Posição do "far plane"

                if (g_UsePerspectiveProjection)
        {
            // Projeção Perspectiva.
            // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.


            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && field_of_view < M_PI / 1.75f) { //Zoom in máximo (π/1.75 rad = 102.85 graus)
                field_of_view = field_of_view * 1.01f; //Taxa de zoom in
            }

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && field_of_view > M_PI / 5.0f) { //Zoom out máximo (π/5 rad  = 36 graus)
                field_of_view = field_of_view / 1.01f; //Taxa de zoom out
            }

            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        }
        else
        {
            // Projeção Ortográfica.
            // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortográfico, computamos o valor de "t"
            // utilizando a variável g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && field_of_view < M_PI / 1.75f) { //Zoom in máximo (π/1.75 rad = 102.85 graus)
            field_of_view = field_of_view * 1.01f; //Taxa de zoom in
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && field_of_view > M_PI / 5.0f) { //Zoom out máximo (π/5 rad  = 36 graus)
            field_of_view = field_of_view / 1.01f; //Taxa de zoom out
        }

        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);


        glm::mat4 model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));



        // Desabilitamos Culling para desenhar as paredes de todos os lados
        glDisable(GL_CULL_FACE);

        // Chão de GRAMA (Terreno Aberto)

        model = Matrix_Translate(0.0f, -0.01f, 0.0f) * Matrix_Scale(50.0f, 1.0f, 50.0f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, GRAMA);
        DrawVirtualObject("the_plane");

        // Pista base (planos) — apenas no nível 1
        if (g_nivelAtual == 1) {
            // Chão da pista
            model = Matrix_Translate(0.0f,0.0f,0.0f) * Matrix_Scale(2.0f, 1.0f, 5.0f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTA_CHAO);
            DrawVirtualObject("the_plane");

            // Parede Esquerda
            model = Matrix_Translate(2.0f, 0.1f, 0.0f) * Matrix_Rotate_Z(M_PI/2.0f) * Matrix_Scale(0.1f, 1.0f, 5.0f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTA_PAREDE);
            DrawVirtualObject("the_plane");

            // Parede Direita
            model = Matrix_Translate(-2.0f, 0.1f, 0.0f) * Matrix_Rotate_Z(M_PI/2.0f) * Matrix_Scale(0.1f, 1.0f, 5.0f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTA_PAREDE);
            DrawVirtualObject("the_plane");

            // Parede Fundo
            model = Matrix_Translate(0.0f, 0.1f, 5.0f) * Matrix_Rotate_X(M_PI/2.0f) * Matrix_Scale(2.0f, 1.0f, 0.1f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTA_PAREDE);
            DrawVirtualObject("the_plane");

            // Parede Frente
            model = Matrix_Translate(0.0f, 0.1f, -5.0f) * Matrix_Rotate_X(M_PI/2.0f) * Matrix_Scale(2.0f, 1.0f, 0.1f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTA_PAREDE);
            DrawVirtualObject("the_plane");
        }

        // Reativamos o Culling para os objetos 3D normais
        glEnable(GL_CULL_FACE);

        // Trajetória da bola (renderizada apenas se estiver parada e puder jogar)
        if (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco && !g_JogadorAtual && turn_ready) {
            glUniform3f(glGetUniformLocation(g_GpuProgramID, "u_TrailColor"), 1.0f, 1.0f, 1.0f);
            float cosseno = cos(g_TacoRotacao);
            float seno = sin(g_TacoRotacao);
            glm::vec3 dir_arremesso = glm::vec3(-cosseno, 0.0f, -seno);

            float forca_base = g_EspacoPressionado ? g_ForcaTacada : 2.0f;
            int num_dots = (int)(forca_base * 4.0f);
            float step = 0.1f;

            glUniform1i(g_object_id_uniform, TRAJETORIA);
            for (int i = 1; i <= num_dots; i++) {
                glm::vec3 pos_dot = g_PosBola + dir_arremesso * (i * step);

                float ray_y = g_PosBola.y;
                if (g_nivelAtual == 3) ray_y = RaycastTrackHeight(g_PistaLoopTriangles, pos_dot.x, g_PosBola.y, pos_dot.z, g_PosBola.y);
                else if (g_nivelAtual == 2) ray_y = RaycastTrackHeight(g_PistaCurvaTriangles, pos_dot.x, g_PosBola.y, pos_dot.z, g_PosBola.y);

                model = Matrix_Translate(pos_dot.x, ray_y + 0.01f, pos_dot.z) * Matrix_Scale(0.015f, 0.015f, 0.015f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                DrawVirtualObject("the_sphere");
            }

        } else if (glm::length(g_VelocidadeBolaTwo) < 0.02f && !g_BolaNoBuracoTwo && g_JogadorAtual && turn_ready) {
            glUniform3f(glGetUniformLocation(g_GpuProgramID, "u_TrailColor"), 1.0f, 1.0f, 1.0f);
            float cosseno = cos(g_TacoRotacao);
            float seno = sin(g_TacoRotacao);
            glm::vec3 dir_arremesso = glm::vec3(-cosseno, 0.0f, -seno);

            float forca_base = g_EspacoPressionado ? g_ForcaTacada : 2.0f;
            int num_dots = (int)(forca_base * 4.0f);
            float step = 0.1f;

            glUniform1i(g_object_id_uniform, TRAJETORIA);
            for (int i = 1; i <= num_dots; i++) {
                glm::vec3 pos_dot = g_PosBolaTwo + dir_arremesso * (i * step);

                float ray_y = g_PosBolaTwo.y;
                if (g_nivelAtual == 3) ray_y = RaycastTrackHeight(g_PistaLoopTriangles, pos_dot.x, g_PosBolaTwo.y, pos_dot.z, g_PosBolaTwo.y);
                else if (g_nivelAtual == 2) ray_y = RaycastTrackHeight(g_PistaCurvaTriangles, pos_dot.x, g_PosBolaTwo.y, pos_dot.z, g_PosBolaTwo.y);

                model = Matrix_Translate(pos_dot.x, ray_y + 0.01f, pos_dot.z) * Matrix_Scale(0.015f, 0.015f, 0.015f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                DrawVirtualObject("the_sphere");
            }
        }


        // Passa a posição do buraco para o shader para descartar os fragmentos da pista e criar o buraco físico
        glUniform3f(glGetUniformLocation(g_GpuProgramID, "u_HolePosition"), g_HolePosition.x, g_HolePosition.y, g_HolePosition.z);

        // Montanha de grama que sobe para abraçar o buraco por fora (esconde a protuberância)
        if (g_nivelAtual == 2 || g_nivelAtual == 3) {
            model = Matrix_Translate(g_HolePosition.x, g_HolePosition.y - 0.15f, g_HolePosition.z) * Matrix_Scale(0.6f, 0.14f, 0.6f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, GRAMA);
            DrawVirtualObject("the_sphere");
        }

        // Buraco (Copo de esfera invertida)
        glEnable(GL_CULL_FACE); // Permitir ver a parte de dentro da esfera mas esconder o fundo de fora
        glCullFace(GL_FRONT);
        model = Matrix_Translate(g_HolePosition.x, g_HolePosition.y + 0.01f, g_HolePosition.z) * Matrix_Scale(0.12f, 0.12f, 0.12f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, BURACO);
        DrawVirtualObject("the_sphere");
        glCullFace(GL_BACK); // Reativar o culling normal

        // Renderiza a trilha da bola
        glUseProgram(g_GpuProgramID);
        RenderTrail();
        if (g_MultiplayerAtivo) {
            RenderTrail2();
        }

        bool draw_club = false;
        if (!g_MultiplayerAtivo) {
            draw_club = (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco);
        } else if (!g_TerminouJogada) { // Only draw club if it's actively your turn
            if (!g_JogadorAtual) draw_club = (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco);
            else draw_club = (glm::length(g_VelocidadeBolaTwo) < 0.02f && !g_BolaNoBuracoTwo);
        }

        // O taco só é desenhado se a bola estiver (quase) parada e não estiver no buraco
        if (draw_club) {
            if(!g_JogadorAtual){
                model = CalcularTaco(g_PosBola, g_PosTaco, g_DistanciaTaco);
            } else if(g_JogadorAtual){
                model = CalcularTaco(g_PosBolaTwo, g_PosTaco, g_DistanciaTaco);
            }
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, TACO);
            DrawVirtualObject("golf_club.002_Cube.003");
        }

        // desenhamos a Bola de Golfe perfeitamente centralizada e no tamanho correto (0.025)
        model = Matrix_Translate(g_PosBola.x, g_PosBola.y, g_PosBola.z)
              * g_BolaRotationMatrix
              * Matrix_Scale(0.025f, 0.025f, 0.025f); // 0.025 encaixa no novo colisor de minigolf
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, BOLA);

        int inactive1 = (g_MultiplayerAtivo && g_JogadorAtual) ? 1 : 0;
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_BolaInativa"), inactive1);
        if (inactive1) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
        if (!g_BolaNoBuraco) DrawVirtualObject("the_sphere");
        if (inactive1) { glDisable(GL_BLEND); }

        if(g_MultiplayerAtivo){
            // se MP ativo desenhamos a Bola do segundo jogador
            model = Matrix_Translate(g_PosBolaTwo.x, g_PosBolaTwo.y, g_PosBolaTwo.z)
                * g_BolaRotationMatrixTwo
                * Matrix_Scale(0.025f, 0.025f, 0.025f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, BOLA);

            int inactive2 = (!g_JogadorAtual) ? 1 : 0;
            glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_BolaInativa"), inactive2);
            if (inactive2) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
            if (!g_BolaNoBuracoTwo) DrawVirtualObject("the_sphere");
            if (inactive2) { glDisable(GL_BLEND); }
        }

        // Reseta u_BolaInativa para não afetar outros objetos
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_BolaInativa"), 0);


        // Desenha a pista curva (nível 2)
        if(g_nivelAtual == 2){
            glEnable(GL_CULL_FACE);
            // PistaCurva: bbox_min.y = -0.2556. At scale 0.25, Y offset = 0.2556 * 0.25 = 0.064
            // para que o ponto mais baixo da mesh fique em Y=0 (nível do chão)
            model = Matrix_Translate(0.0f, 0.064f, 0.0f) * Matrix_Scale(0.25f, 0.25f, 0.25f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTACURVA);
            DrawVirtualObject("PistaCurva");

            model = Matrix_Translate(0.0f, 0.064f, 0.0f) * Matrix_Scale(0.25f, 0.25f, 0.25f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, BORDASCURVA);
            DrawVirtualObject("PistaCurva_bordas");
        }

        // Desenha a Pista em loop (nível 3)
        if(g_nivelAtual == 3){
            glEnable(GL_CULL_FACE);
            // PistaLoop: bbox_min.y = -1.12. At scale 1.0, Y offset = 1.12
            // para que o chão da pista fique em Y=0
            model = Matrix_Translate(0.0f, 1.12f, 0.0f) * Matrix_Scale(1.0f, 1.0f, 1.0f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, PISTALOOP);
            DrawVirtualObject("loop");

            model = Matrix_Translate(0.0f, 1.12f, 0.0f) * Matrix_Scale(1.0f, 1.0f, 1.0f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, BORDASLOOP);
            DrawVirtualObject("bordas_loop");
        }

        // ==========================================
        // Desenha o Zeppelin Voador com Curva de Bézier Cúbica (Loop Fechado)
        // ==========================================
        g_ZeppelinT += 0.1f * delta_time;
        if (g_ZeppelinT > 2.0f) {
            g_ZeppelinT -= 2.0f;
        }

        // Ponto A0: Frente direita da pista
        glm::vec3 pA0(20.0f, 6.0f, 15.0f);
        // Ponto A1: Passando alto atrás da pista
        glm::vec3 pA1(5.0f, 12.0f, -20.0f);
        // Ponto A2: Dando a volta na lateral esquerda
        glm::vec3 pA2(-15.0f, 10.0f, -5.0f);
        // Ponto A3: Fim do primeiro trajeto
        glm::vec3 pA3(-15.0f, 8.0f, 15.0f);

        // Pontos de controle da segunda metade para fechar o oval suavemente
        glm::vec3 pB0 = pA3;
        // Vetor de continuidade C1
        glm::vec3 tangentA = pA3 - pA2;
        glm::vec3 pB1 = pA3 + tangentA;
        // Tangente inversa para voltar pro ponto A0 suavemente
        glm::vec3 tangentB = pA0 - glm::vec3(15.0f, 6.0f, 20.0f);
        glm::vec3 pB2 = pA0 - tangentB;
        glm::vec3 pB3 = pA0;

        glm::vec3 posZeppelin;
        glm::vec3 nextPosZeppelin;

        if (g_ZeppelinT <= 1.0f) {
            posZeppelin = CalculateBezierPoint(g_ZeppelinT, pA0, pA1, pA2, pA3);
            nextPosZeppelin = CalculateBezierPoint(std::min(1.0f, g_ZeppelinT + 0.01f), pA0, pA1, pA2, pA3);
            if (g_ZeppelinT > 0.99f) nextPosZeppelin = CalculateBezierPoint(0.01f, pB0, pB1, pB2, pB3);
        } else {
            float t = g_ZeppelinT - 1.0f;
            posZeppelin = CalculateBezierPoint(t, pB0, pB1, pB2, pB3);
            nextPosZeppelin = CalculateBezierPoint(std::min(1.0f, t + 0.01f), pB0, pB1, pB2, pB3);
            if (t > 0.99f) nextPosZeppelin = CalculateBezierPoint(0.01f, pA0, pA1, pA2, pA3);
        }

        glm::vec3 dirZeppelin = glm::normalize(nextPosZeppelin - posZeppelin);
        float zeppelinYaw = atan2(-dirZeppelin.z, dirZeppelin.x);

        model = Matrix_Translate(posZeppelin.x, posZeppelin.y, posZeppelin.z)
              * Matrix_Rotate_Y(zeppelinYaw)
              * Matrix_Scale(1.5f, 1.5f, 1.5f); // Escala uniforme (o modelo ja foi esticado na base)

        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, ZEPPELIN);
        DrawVirtualObject("Zeppelin");

        // Cenário: Árvores
        auto DrawTree = [&](float x, float y, float z, float scale, bool isTall) {
            glm::mat4 m = Matrix_Translate(x, y, z) * Matrix_Scale(scale, scale, scale);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(m));
            if (isTall) {
                glUniform1i(g_object_id_uniform, ARVORE_ALTA);
                DrawVirtualObject("Arvore-Alta");
                glUniform1i(g_object_id_uniform, TRONCO);
                DrawVirtualObject("Tronco_Alto.001");
            } else {
                glUniform1i(g_object_id_uniform, ARVORE_BAIXA);
                DrawVirtualObject("Arvore-Baixa");
                glUniform1i(g_object_id_uniform, TRONCO);
                DrawVirtualObject("Cube.001");
            }
        };

        if (g_nivelAtual >= 1 && g_nivelAtual <= 3) {
            // Floresta densa mais proxima da pista
            float cx = 0.0f;
            float cz = 0.0f;
            if (g_nivelAtual == 1) { cx = 0.0f; cz = 0.5f; }
            else if (g_nivelAtual == 2) { cx = 1.365f; cz = -2.15f; }
            else if (g_nivelAtual == 3) { cx = 2.5f; cz = 1.35f; }
            for (int i = 0; i < 60; i++) {
                float angle = (i / 60.0f) * 2.0f * M_PI;
                // Raio variavel adaptado pro tamanho da fase
                float radius = 10.0f + (i % 5) * 1.5f;
                float tx = cx + cos(angle) * radius;
                float tz = cz + sin(angle) * radius;

                bool tall = (i % 2 == 0);
                float scale = tall ? 0.15f + (i % 4)*0.03f : 0.1f + (i % 3)*0.02f;
                DrawTree(tx, 0.0f, tz, scale, tall);
            }

            // Segunda camada de árvores para dar mais volume visual
            for (int i = 0; i < 40; i++) {
                float angle = (i / 40.0f) * 2.0f * M_PI + 0.1f;
                float radius = 16.0f + (i % 4) * 2.0f;
                float tx = cx + cos(angle) * radius;
                float tz = cz + sin(angle) * radius;

                bool tall = (i % 3 != 0);
                float scale = tall ? 0.18f + (i % 3)*0.02f : 0.12f + (i % 2)*0.02f;
                DrawTree(tx, 0.0f, tz, scale, tall);
            }
        }

        // Desenha a Bandeira (acompanha a posição do buraco)
        model = Matrix_Translate(g_HolePosition.x + 0.3f, g_HolePosition.y, g_HolePosition.z) * Matrix_Scale(0.1f, 0.1f, 0.1f);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, MASTRO);
        DrawVirtualObject("Bandeira_Mastro");

        glDisable(GL_CULL_FACE); // A bandeira é um plano 2D, queremos vê-la dos dois lados
        glUniform1i(g_object_id_uniform, BANDEIRA);
        DrawVirtualObject("Blandeira");
        glEnable(GL_CULL_FACE);


        // Desenha a HUD da Barra de Força em NDC (Tela 2D)
        if (g_EspacoPressionado) {
            glDisable(GL_DEPTH_TEST); // Garante que a HUD seja desenhada por cima de tudo

            glm::mat4 identity = Matrix_Identity();
            glUniformMatrix4fv(g_view_uniform, 1, GL_FALSE, glm::value_ptr(identity));
            glUniformMatrix4fv(g_projection_uniform, 1, GL_FALSE, glm::value_ptr(identity));
            glUniform1i(g_object_id_uniform, HUD_BARRA);

            float percent = std::min(g_ForcaTacada / 15.0f, 1.0f);
            GLint g_forca_percent_uniform = glGetUniformLocation(g_GpuProgramID, "u_ForcaPercent");
            glUniform1f(g_forca_percent_uniform, percent);

            // O the_plane original geralmente vai de -1 a 1 (largura 2).
            // Queremos centralizar a barra embaixo: y = -0.8
            // Para crescer da esquerda pra direita, o centro X precisa transladar
            float max_width = 0.4f; // Tamanho máximo da barra
            float scale_x = percent * max_width;
            float pos_x = -0.5f + scale_x; // Começa em -0.5 (X=-0.5), e se move pra direita

            model = Matrix_Translate(pos_x, -0.8f, 0.0f) * Matrix_Scale(scale_x, 0.05f, 1.0f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            DrawVirtualObject("the_plane");

            glEnable(GL_DEPTH_TEST); // Restaura
        }

        //mandamos a posição da luz para o fragment_shader para ser utilizada na iluminação
        glUseProgram(g_GpuProgramID);
        glUniform3f( glGetUniformLocation(g_GpuProgramID, "g_PosLuz"), g_PosLuz.x, g_PosLuz.y, g_PosLuz.z );

        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.
        TextRendering_ShowEulerAngles(window);

        // Imprimimos na informação sobre a matriz de projeção sendo utilizada.
        TextRendering_ShowProjection(window);

            // Imprimimos na tela informação sobre o número de quadros renderizados
            // por segundo (frames per second).
            TextRendering_ShowFramesPerSecond(window);

            // Se o nível foi concluído, renderizar a UI do menu de fase por cima de tudo
            if (g_CurrentState == MENU_LEVEL_COMPLETE || g_CurrentState == MENU_PAUSE) {
                MenuRenderOverlay(window);
            }

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    if (g_audioInitialized) {
        ma_sound_uninit(&g_musicMenu);
        ma_sound_uninit(&g_ambientSound);
        ma_sound_uninit(&g_hitSound);
        ma_sound_uninit(&g_victorySound);
        ma_sound_uninit(&g_wallSound);
        ma_engine_uninit(&g_audioEngine);
    }
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Setamos as variáveis "bbox_min" e "bbox_max" do fragment shader
    // com os parâmetros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");

    // Variáveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage3"), 3);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage4"), 4);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage5"), 5);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage6"), 6);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage7"), 7);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage8"), 8);
    glUseProgram(0);
}

// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice e que pertencem ao mesmo "smoothing group".

    // Obtemos a lista dos smoothing groups que existem no objeto
    std::set<unsigned int> sgroup_ids;
    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        assert(model->shapes[shape].mesh.smoothing_group_ids.size() == num_triangles);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);
            unsigned int sgroup = model->shapes[shape].mesh.smoothing_group_ids[triangle];
            assert(sgroup >= 0);
            sgroup_ids.insert(sgroup);
        }
    }

    size_t num_vertices = model->attrib.vertices.size() / 3;
    model->attrib.normals.reserve( 3*num_vertices );

    // Processamos um smoothing group por vez
    for (const unsigned int & sgroup : sgroup_ids)
    {
        std::vector<int> num_triangles_per_vertex(num_vertices, 0);
        std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

        // Acumulamos as normais dos vértices de todos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                glm::vec4  vertices[3];
                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                    const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                    const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                    vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
                }

                const glm::vec4  a = vertices[0];
                const glm::vec4  b = vertices[1];
                const glm::vec4  c = vertices[2];
                const glm::vec4  n = crossproduct(b-a,c-a);

                for (size_t vertex = 0; vertex < 3; ++vertex) {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    num_triangles_per_vertex[idx.vertex_index] += 1;
                    vertex_normals[idx.vertex_index] += n;
                }
            }
        }

        // Computamos a média das normais acumuladas
        std::vector<size_t> normal_indices(num_vertices, 0);

        for (size_t vertex_index = 0; vertex_index < vertex_normals.size(); ++vertex_index)
        {
            if (num_triangles_per_vertex[vertex_index] == 0)
                continue;

            glm::vec4 n = vertex_normals[vertex_index] / (float)num_triangles_per_vertex[vertex_index];
            n /= norm(n);

            model->attrib.normals.push_back( n.x );
            model->attrib.normals.push_back( n.y );
            model->attrib.normals.push_back( n.z );

            size_t normal_index = (model->attrib.normals.size() / 3) - 1;
            normal_indices[vertex_index] = normal_index;
        }

        // Escrevemos os índices das normais para os vértices dos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index =
                        normal_indices[ idx.vertex_index ];
                }
            }
        }

    }
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name           = model->shapes[shape].name;
        theobject.first_index    = first_index; // Primeiro índice
        theobject.num_indices    = last_index - first_index + 1; // Número de indices
        theobject.rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// função que calcula a posição e orientação do taco de golfe baseado na posição da bola

//  posicao_bola: posição  da bola de golfe
//  direcao_taco: direção emque o taco está apontando
//  distancia: distância entre a bola e o  taco

//IMPOTANTE: é OBRIGATÓRIO usar mat4 se não o código quebra, demorei para entender isso
//segundo um cara no stackoverflow "matrizes 4x4 são as únicas capazes de combinar translação, rotação e escala em uma única estrutura matemática"
glm::mat4 CalcularTaco(glm::vec3 posicao_bola, glm::vec3 direcao_taco, float distancia)
{
    // 1. Escala original do taco
    glm::mat4 m_scale = Matrix_Scale(0.05f, 0.05f, 0.05f);

    // 2. Gira 90 graus POSITIVO para alinhar a face plana do taco para frente (direção do impacto, eixo -X)
    glm::mat4 m_rot_y_mesh = Matrix_Rotate_Y(M_PI / 2.0f);

    // 3. Ajuste fino do impacto. O raio da bola é 0.025. Queremos que a face do taco toque exatamente em X=+0.025.
    glm::mat4 m_ajuste_impacto = Matrix_Translate(0.017f, -0.01f, -0.046f);

    // 4. Pivô do Pêndulo. O jogador segura o taco no Grip (altura Y=0.34).
    glm::mat4 m_pivot = Matrix_Translate(-0.017f, -0.34f, +0.046f);

    // 5. Swing (Pêndulo no eixo Z local).
    // Ângulo POSITIVO rotaciona a cabeça do taco para +X (puxando para TRÁS, para a câmera).
    glm::mat4 m_swing = Matrix_Rotate_Z(g_TacoRotacaoVertical);

    // 6. Retorna o grip para a posição exata de stance.
    glm::mat4 m_unpivot = Matrix_Translate(0.017f, 0.34f, -0.046f);

    // 7. Rotação do jogador (Mira). Gira toda a mecânica ao redor do centro da bola (eixo Y).
    // O SINAL DEVE SER NEGATIVO (-g_TacoRotacao) PARA ACOMPANHAR A CÂMERA DO JOGADOR!
    glm::mat4 m_mira = Matrix_Rotate_Y(-g_TacoRotacao);

    // 8. Coloca na posição final do mundo
    glm::mat4 m_pos = Matrix_Translate(posicao_bola.x, posicao_bola.y, posicao_bola.z);

    glm::mat4 modelo = m_pos * m_mira * m_unpivot * m_swing * m_pivot * m_ajuste_impacto * m_rot_y_mesh * m_scale;

    return modelo;
}

// Função que atualiza a animação do taco
void RotacionarTaco(GLFWwindow* window)
{
    static bool bola_atingida = false;

    if (g_TempoRotacaoTaco < 0.0)
    {
        bola_atingida = false;
        if (g_EspacoPressionado) {
            float percent = std::min(g_ForcaTacada / 15.0f, 1.0f);
            g_TacoRotacaoVertical = percent * g_AnguloRotacaoTaco; // Puxa pra trás
        } else {
            g_TacoRotacaoVertical = 0.0f; // Fica travado na bola
        }
        return;
    }

    double Atual = glfwGetTime();
    double Total = Atual - g_TempoRotacaoTaco;

    float duracao_descida = 0.15f;

    float percent_forca = std::min(g_ForcaTacada / 15.0f, 1.0f);
    float angulo_inicial = percent_forca * g_AnguloRotacaoTaco;

    // Termina a tacada logo após o impacto para garantir que ele pare encostado!
    if (Total >= duracao_descida + 0.1f)
    {
        g_TacoRotacaoVertical = 0.0f;
        g_TempoRotacaoTaco = -1.0;
        return;
    }

    if (Total <= duracao_descida) {
        // Fase 1: Descida (Aceleração Quadrática até bater no 0)
        float t = (float)(Total / duracao_descida);
        g_TacoRotacaoVertical = angulo_inicial * (1.0f - (t * t));
    } else {
        // Fase 2: Impacto Físico
        bool buraco_atual = false;
        if (!g_MultiplayerAtivo) buraco_atual = g_BolaNoBuraco;
        else buraco_atual = (!g_JogadorAtual) ? g_BolaNoBuraco : g_BolaNoBuracoTwo;

        if (!bola_atingida && !buraco_atual) {
            bola_atingida = true;

            if (g_audioInitialized) {
                ma_sound_set_volume(&g_hitSound, g_AmbientVolume);
                ma_sound_seek_to_pcm_frame(&g_hitSound, 0);
                ma_sound_start(&g_hitSound);
            }

            float forca = std::max(2.0f, g_ForcaTacada);
            float cosseno = cos(g_TacoRotacao);
            float seno = sin(g_TacoRotacao);

            if(!g_JogadorAtual){
                g_VelocidadeBola = glm::vec3(-cosseno * forca, 0.0f, -seno * forca);
                g_BolaEmFocoAtual = false;
                g_TacadasPlayer1++;
            } else {
                g_VelocidadeBolaTwo = glm::vec3(-cosseno * forca, 0.0f, -seno * forca);
                g_BolaEmFocoAtual = true;
                g_TacadasPlayer2++;
            }
        }

        // Mantém o taco em repouso perfeito na posição inicial (encostado onde a bola estava)
        g_TacoRotacaoVertical = 0.0f;
    }
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        fprintf(stderr, "%s", output.c_str());
    }
    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);
    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        // No menu, processar clique nos botões
        if (g_CurrentState != PLAYING) {
            MenuHandleClick(window);
            return;
        }

        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;

        rotacao_camera = true; //habilitamos a rotação da camera

    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;

        rotacao_camera = false; //desabilitamos a rotação da camera

    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

    // No menu, não rotacionar a câmera
    if (g_CurrentState != PLAYING) return;


    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da câmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   += 0.01f*dy;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre um valor mínimo acima do chão e +pi/2.
        float phimax = M_PI/2;
        float phimin = 0.1f;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_TorsoPositionX += 0.01f*dx;
        g_TorsoPositionY -= 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset){}

void Correcao_KeyCallback(int key, int action, int mod);

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    if (g_CurrentState == MENU_MAIN || g_CurrentState == MENU_LEVELS || g_CurrentState == MENU_SETTINGS || g_CurrentState == MENU_LEVEL_COMPLETE || g_CurrentState == MENU_PAUSE) {
        MenuRenderOverlay(window);
        return;
    }
    // =======================
    // Não modifique esta chamada! Ela é utilizada para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    Correcao_KeyCallback(key, action, mod);
    // =======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (g_CurrentState == PLAYING) {
            g_CurrentState = MENU_PAUSE;
            if (g_audioInitialized) {
                ma_sound_stop(&g_ambientSound);
                ma_sound_set_volume(&g_musicMenu, g_MusicVolume);
                ma_sound_start(&g_musicMenu);
                ma_sound_set_fade_in_milliseconds(&g_musicMenu, 0.0f, 1.0f, 2000);
            }
            g_LeftMouseButtonPressed = false;
            g_RightMouseButtonPressed = false;
        } else if (g_CurrentState == MENU_PAUSE) {
            g_CurrentState = PLAYING;
            if (g_audioInitialized) {
                ma_sound_stop(&g_musicMenu);
                ma_sound_set_volume(&g_ambientSound, g_AmbientVolume);
                ma_sound_start(&g_ambientSound);
            }
        } else if (g_CurrentState == MENU_LEVELS || g_CurrentState == MENU_SETTINGS) {
            g_CurrentState = MENU_MAIN;
        } else {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    // variaveis para rotacionar o taco de golfe
    float delta = M_PI / 16; // 22.5 graus radianos. podemos mudar depois

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    // Lógica para controle da força com Espaço
    if (key == GLFW_KEY_SPACE) {
        bool pode_jogar = false;
        if (!g_MultiplayerAtivo) {
            pode_jogar = (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco);
        } else {
            if (!g_JogadorAtual) {
                pode_jogar = (glm::length(g_VelocidadeBola) < 0.02f && !g_BolaNoBuraco);
            } else {
                pode_jogar = (glm::length(g_VelocidadeBolaTwo) < 0.02f && !g_BolaNoBuracoTwo);
            }
        }

        if (action == GLFW_PRESS && pode_jogar && g_TempoRotacaoTaco < 0.0) {
            g_EspacoPressionado = true;
            g_InicioEspaco = glfwGetTime();
            g_ForcaTacada = 0.0f;
        }
        else if (action == GLFW_RELEASE && g_EspacoPressionado) {
            g_EspacoPressionado = false; // Inicia a animação da tacada
            g_TempoRotacaoTaco = glfwGetTime();
            g_TempoDesdeEspaco = g_TempoRotacaoTaco;
            g_TerminouJogada = true; // Marca que a jogada terminou
        }
    }

    // Se o usuário apertar a tecla backspace, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
        g_ForearmAngleX = 0.0f;
        g_ForearmAngleZ = 0.0f;
        g_TorsoPositionX = 0.0f;
        g_TorsoPositionY = 0.0f;
        g_TacoRotacao = 0.0f;
        g_TacoRotacaoVertical = 0.0f;
    }

        // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        g_UsePerspectiveProjection = true;
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        g_UsePerspectiveProjection = false;
    }


    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        g_ShowInfoText = !g_ShowInfoText;
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f ) {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);

    // Mostra a força da tacada atual de forma visual
    if (g_ShowInfoText && !g_BolaNoBuraco) {
        if (!g_EspacoPressionado && g_TempoRotacaoTaco < 0.0) {
            TextRendering_PrintString(window, "Mire com [A] / [D]. Zoom com [W] / [S]", -1.0f+charwidth, 1.0f-2.5f*lineheight, 1.2f);
            TextRendering_PrintString(window, "Mova a camera com o [M1]. Resetar com [M2]", -1.0f+charwidth, 1.0f-5.0f*lineheight, 1.2f);
            TextRendering_PrintString(window, "Segure [Espaco] para bater", -1.0f+charwidth, 1.0f-7.5f*lineheight, 1.2f);
        }
    }

}

// Função para debugging: imprime no terminal todas informações de um modelo
// geométrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

// =============================================
// IMPLEMENTAÇÃO DO SISTEMA DE MENU
// =============================================

GLuint LoadTextureImageRGBA(const char* filename, int* outW, int* outH)
{
    printf("Carregando imagem RGBA \"%s\"... ", filename);
    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char* data = stbi_load(filename, &w, &h, &ch, 4);
    if (!data) {
        fprintf(stderr, "ERRO: Nao foi possivel abrir \"%s\".\n", filename);
        return 0;
    }
    printf("OK (%dx%d).\n", w, h);
    if (outW) *outW = w;
    if (outH) *outH = h;

    // IMPORTANTE: Salvar a textura ativa para não clobber os game texture units
    GLint prevActiveUnit = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveUnit);
    GLint prevBoundTex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevBoundTex);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Restaurar binding anterior (evita clobber do golf club texture em unit 2)
    glBindTexture(GL_TEXTURE_2D, prevBoundTex);

    stbi_image_free(data);
    return tex;
}

GLuint CompileHudShaders()
{
    const char* vs_src =
        "#version 330 core\n"
        "layout(location=0) in vec2 aPos;\n"
        "layout(location=1) in vec2 aUV;\n"
        "out vec2 vUV;\n"
        "void main(){\n"
        "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "   vUV = aUV;\n"
        "}\n";
    const char* fs_src =
        "#version 330 core\n"
        "in vec2 vUV;\n"
        "uniform int u_Mode;\n"
        "uniform sampler2D u_Texture;\n"
        "uniform vec4 u_Color1;\n"
        "uniform vec4 u_Color2;\n"
        "uniform float u_Hover;\n"
        "uniform float u_Value;\n"
        "uniform float u_Radius;\n"
        "uniform vec2 u_Size;\n"
        "out vec4 fragColor;\n"
        "void main(){\n"
        "   if(u_Mode==0){\n"
        "       fragColor = texture(u_Texture, vUV);\n"
        "   } else if(u_Mode==1){\n"
        "       vec3 c = mix(u_Color1.rgb, u_Color2.rgb, vUV.y);\n"
        "       vec2 p = vUV * u_Size;\n"
        "       vec2 h2 = u_Size*0.5;\n"
        "       vec2 d = abs(p - h2) - (h2 - vec2(u_Radius));\n"
        "       float dist = length(max(d,vec2(0.0))) - u_Radius;\n"
        "       float a = 1.0 - smoothstep(-1.0, 1.0, dist);\n"
        "       c = c + vec3(u_Hover * 0.12);\n"
        "       fragColor = vec4(c, a * u_Color1.a);\n"
        "   } else if(u_Mode==2){\n"
        "       vec2 p = vUV * u_Size;\n"
        "       vec2 h2 = u_Size*0.5;\n"
        "       vec2 d = abs(p - h2) - (h2 - vec2(u_Radius));\n"
        "       float dist = length(max(d,vec2(0.0))) - u_Radius;\n"
        "       float a = 1.0 - smoothstep(-1.0, 1.0, dist);\n"
        "       float f = step(vUV.x, u_Value);\n"
        "       vec3 c = mix(vec3(0.15,0.15,0.2), u_Color1.rgb, f);\n"
        "       fragColor = vec4(c, a * 0.9);\n"
        "   }\n"
        "}\n";
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

void MenuInit()
{
    g_HudShaderProgram = CompileHudShaders();
    glGenVertexArrays(1, &g_HudVAO);
    glGenBuffers(1, &g_HudVBO);
    glBindVertexArray(g_HudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_HudVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    g_LogoTextureID = LoadTextureImageRGBA("../../GOLFinho-removebg-preview.png", &g_LogoWidth, &g_LogoHeight);

    // Bind to unit 8 for the flag shader
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, g_LogoTextureID);
    glActiveTexture(GL_TEXTURE0);
}

void DrawHudQuad(float cx, float cy, float hw, float hh)
{
    float x0=cx-hw, y0=cy-hh, x1=cx+hw, y1=cy+hh;
    float verts[] = {
        x0,y0, 0,0,  x1,y0, 1,0,  x1,y1, 1,1,
        x0,y0, 0,0,  x1,y1, 1,1,  x0,y1, 0,1
    };
    glBindVertexArray(g_HudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_HudVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool IsMouseOverRect(GLFWwindow* w, float cx, float cy, float hw, float hh)
{
    double mx, my;
    glfwGetCursorPos(w, &mx, &my);
    int ww, wh;
    glfwGetWindowSize(w, &ww, &wh);
    float nx = (2.0f*(float)mx/ww)-1.0f;
    float ny = 1.0f-(2.0f*(float)my/wh);
    return nx>=cx-hw && nx<=cx+hw && ny>=cy-hh && ny<=cy+hh;
}

// Helpers para setar uniforms do HUD shader
static void SHU1i(const char* n, int v) { glUniform1i(glGetUniformLocation(g_HudShaderProgram, n), v); }
static void SHU1f(const char* n, float v) { glUniform1f(glGetUniformLocation(g_HudShaderProgram, n), v); }
static void SHU2f(const char* n, float a, float b) { glUniform2f(glGetUniformLocation(g_HudShaderProgram, n), a, b); }
static void SHU4f(const char* n, float a, float b, float c, float d) { glUniform4f(glGetUniformLocation(g_HudShaderProgram, n), a, b, c, d); }

bool RenderButton(GLFWwindow* window, float cx, float cy, float hw, float hh,
                  const char* label, float r1, float g1, float b1, float r2, float g2, float b2,
                  bool enabled)
{
    bool hovered = enabled && IsMouseOverRect(window, cx, cy, hw, hh);
    glUseProgram(g_HudShaderProgram);
    SHU1i("u_Mode", 1);
    if (enabled) {
        SHU4f("u_Color1", r1, g1, b1, 0.92f);
        SHU4f("u_Color2", r2, g2, b2, 0.92f);
    } else {
        SHU4f("u_Color1", 0.35f, 0.35f, 0.38f, 0.7f);
        SHU4f("u_Color2", 0.25f, 0.25f, 0.28f, 0.7f);
    }
    SHU1f("u_Hover", hovered ? 1.0f : 0.0f);
    SHU1f("u_Radius", 10.0f);
    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh);
    SHU2f("u_Size", hw*(float)ww, hh*(float)wh);
    DrawHudQuad(cx, cy, hw, hh);

    // Texto branco sobre botão
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    float charw = TextRendering_CharWidth(window);
    float textLen = strlen(label) * charw * 1.5f;
    float textX = cx - textLen / 2.0f;
    float textY = cy - TextRendering_LineHeight(window) * 0.75f;
    TextRendering_PrintString(window, label, textX, textY, 1.5f);
    // Re-ativar blend (PrintString desativa)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Resetar cor do texto para preto
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 0.0f, 0.0f, 0.0f);
    return hovered;
}

void RenderSlider(GLFWwindow* window, float cx, float cy, float hw, float hh, float value)
{
    glUseProgram(g_HudShaderProgram);
    SHU1i("u_Mode", 2);
    SHU4f("u_Color1", 0.2f, 0.7f, 0.35f, 1.0f);
    SHU1f("u_Value", value);
    SHU1f("u_Radius", 6.0f);
    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh);
    SHU2f("u_Size", hw*(float)ww, hh*(float)wh);
    DrawHudQuad(cx, cy, hw, hh);
}

void MenuRenderMainMenu(GLFWwindow* window)
{
    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh);
    float aspect = (float)ww / (float)wh;

    // Logo com transparência
    // IMPORTANTE: Salvar a textura original do unit 0 (brick) antes de substituir pelo logo
    glUseProgram(g_HudShaderProgram);
    SHU1i("u_Mode", 0);
    GLint prevTex0 = 0;
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex0);
    glBindTexture(GL_TEXTURE_2D, g_LogoTextureID);
    SHU1i("u_Texture", 0);
    float logoAspect = (float)g_LogoWidth / (float)g_LogoHeight;
    float logoHH = 0.28f;
    float logoHW = logoHH * logoAspect / aspect;
    DrawHudQuad(0.0f, 0.52f, logoHW, logoHH);
    // Restaurar a textura original do unit 0 (brick)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, prevTex0);

    // Botões do menu principal
    float btnHW = 0.22f;
    float btnHH = 0.055f;
    g_HoverJogar = RenderButton(window, 0.0f, 0.05f, btnHW, btnHH, "1 JOGADOR",
        0.18f, 0.72f, 0.35f,  0.10f, 0.50f, 0.22f, true);
    g_HoverMultiplayer = RenderButton(window, 0.0f, -0.10f, btnHW, btnHH, "2 JOGADORES",
        0.25f, 0.50f, 0.85f,  0.15f, 0.35f, 0.65f, true);
    g_HoverConfig = RenderButton(window, 0.0f, -0.25f, btnHW, btnHH, "CONFIGURACOES",
        0.85f, 0.65f, 0.18f,  0.65f, 0.45f, 0.10f, true);
    g_HoverSair = RenderButton(window, 0.0f, -0.40f, btnHW, btnHH, "SAIR",
        0.85f, 0.22f, 0.18f,  0.65f, 0.12f, 0.10f, true);
}

void MenuRenderLevelSelect(GLFWwindow* window)
{
    // Título
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "SELECIONAR NIVEL", -0.40f, 0.70f, 2.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Botões de nível
    float btnSize = 0.075f;
    float startX = -0.40f;
    float spacing = 0.20f;
    for (int i = 0; i < 5; i++) {
        float bx = startX + i * spacing;
        bool enabled = (i < 3);
        char label[4];
        snprintf(label, 4, "%d", i + 1);
        RenderButton(window, bx, 0.15f, btnSize, btnSize, label,
            0.18f, 0.72f, 0.35f,  0.10f, 0.50f, 0.22f, enabled);
        if (!enabled) {
            glUseProgram(textprogram_id);
            glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 0.9f, 0.3f, 0.3f);
            float cw = TextRendering_CharWidth(window);
            TextRendering_PrintString(window, "BLOQ.", bx - cw*3.0f, 0.01f, 1.0f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
    // Botão voltar
    g_HoverVoltar = RenderButton(window, -0.72f, 0.78f, 0.14f, 0.04f, "< VOLTAR",
        0.45f, 0.45f, 0.50f,  0.30f, 0.30f, 0.35f, true);
    // Resetar cor do texto
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 0.0f, 0.0f, 0.0f);
}

void MenuRenderSettings(GLFWwindow* window)
{
    const char* gramaOpcoes[] = {"Terreno Rochoso", "Tijolo Vermelho", "Textura Realista", "Verde Solido"};
    const char* paredeOpcoes[] = {"Terreno Rochoso", "Tijolo Vermelho", "Cinza Solido"};
    const char* bolaOpcoes[] = {"Branca", "Tijolo", "Rochosa"};
    const char* tacoOpcoes[] = {"Metal Cinza", "Texturizado", "Tijolo"};

    // Título
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "CONFIGURACOES", -0.35f, 0.75f, 2.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Volume Musica
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Musica:", -0.55f, 0.55f, 1.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderSlider(window, 0.15f, 0.55f, 0.30f, 0.025f, g_MusicVolume);
    char volMusBuf[8];
    snprintf(volMusBuf, 8, "%d%%", (int)(g_MusicVolume*100));
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, volMusBuf, 0.50f, 0.53f, 1.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Volume Ambiente
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Ambiente:", -0.55f, 0.45f, 1.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderSlider(window, 0.15f, 0.45f, 0.30f, 0.025f, g_AmbientVolume);
    char volAmbBuf[8];
    snprintf(volAmbBuf, 8, "%d%%", (int)(g_AmbientVolume*100));
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, volAmbBuf, 0.50f, 0.43f, 1.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Seletores de textura
    float settY = 0.30f;
    float settStep = 0.15f;
    float arrowBtn = 0.028f;

    // --- Grama da Pista ---
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Grama Pista:", -0.55f, settY, 1.3f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverGramaL = RenderButton(window, 0.08f, settY, arrowBtn, arrowBtn, "<",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 0.95f, 0.7f);
    TextRendering_PrintString(window, gramaOpcoes[g_TexturaPistaGrama], 0.15f, settY-0.015f, 1.1f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverGramaR = RenderButton(window, 0.60f, settY, arrowBtn, arrowBtn, ">",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);

    settY -= settStep;
    // --- Paredes da Pista ---
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Paredes Pista:", -0.55f, settY, 1.3f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverParedeL = RenderButton(window, 0.08f, settY, arrowBtn, arrowBtn, "<",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 0.95f, 0.7f);
    TextRendering_PrintString(window, paredeOpcoes[g_TexturaPistaParede], 0.15f, settY-0.015f, 1.1f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverParedeR = RenderButton(window, 0.60f, settY, arrowBtn, arrowBtn, ">",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);

    settY -= settStep;
    // --- Textura da Bola ---
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Textura Bola:", -0.55f, settY, 1.3f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverBolaL = RenderButton(window, 0.08f, settY, arrowBtn, arrowBtn, "<",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 0.95f, 0.7f);
    TextRendering_PrintString(window, bolaOpcoes[g_TexturaBola], 0.15f, settY-0.015f, 1.1f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverBolaR = RenderButton(window, 0.60f, settY, arrowBtn, arrowBtn, ">",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);

    settY -= settStep;
    // --- Textura do Taco ---
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
    TextRendering_PrintString(window, "Textura Taco:", -0.55f, settY, 1.3f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverTacoL = RenderButton(window, 0.08f, settY, arrowBtn, arrowBtn, "<",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 0.95f, 0.7f);
    TextRendering_PrintString(window, tacoOpcoes[g_TexturaTaco], 0.15f, settY-0.015f, 1.1f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_HoverTacoR = RenderButton(window, 0.60f, settY, arrowBtn, arrowBtn, ">",
        0.3f, 0.5f, 0.75f, 0.2f, 0.35f, 0.55f, true);

    // Resetar cor do texto
    glUseProgram(textprogram_id);
    glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 0.0f, 0.0f, 0.0f);

    // Botão voltar
    g_HoverVoltar = RenderButton(window, -0.72f, 0.78f, 0.14f, 0.04f, "< VOLTAR",
        0.45f, 0.45f, 0.50f,  0.30f, 0.30f, 0.35f, true);
}

void MenuRenderOverlay(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Overlay escuro semi-transparente para legibilidade
    glUseProgram(g_HudShaderProgram);
    SHU1i("u_Mode", 1);
    SHU4f("u_Color1", 0.0f, 0.02f, 0.05f, 0.45f);
    SHU4f("u_Color2", 0.0f, 0.0f, 0.0f, 0.55f);
    SHU1f("u_Hover", 0.0f);
    SHU1f("u_Radius", 0.0f);
    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh);
    SHU2f("u_Size", (float)ww, (float)wh);
    DrawHudQuad(0.0f, 0.0f, 1.0f, 1.0f);

    if (g_CurrentState == MENU_MAIN) {
        MenuRenderMainMenu(window);
    } else if (g_CurrentState == MENU_LEVELS) {
        MenuRenderLevelSelect(window);
    } else if (g_CurrentState == MENU_SETTINGS) {
        MenuRenderSettings(window);
    } else if (g_CurrentState == MENU_LEVEL_COMPLETE) {
        float btnW = 0.5f;
        float btnH = 0.08f;

        glUseProgram(textprogram_id);
        glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
        TextRendering_PrintString(window, "Nivel Completo!", -0.3f, 0.5f, 1.8f);

        char buffer[100];
        if (g_MultiplayerAtivo) {
            int winner = (g_TacadasPlayer1 < g_TacadasPlayer2) ? 1 : (g_TacadasPlayer2 < g_TacadasPlayer1) ? 2 : 0;
            if (winner == 1) sprintf(buffer, "Player 1 Venceu!");
            else if (winner == 2) sprintf(buffer, "Player 2 Venceu!");
            else sprintf(buffer, "Empate!");
            TextRendering_PrintString(window, buffer, -0.2f, 0.35f, 1.2f);

            sprintf(buffer, "Player 1: %d tacadas", g_TacadasPlayer1);
            TextRendering_PrintString(window, buffer, -0.3f, 0.25f, 1.0f);

            sprintf(buffer, "Player 2: %d tacadas", g_TacadasPlayer2);
            TextRendering_PrintString(window, buffer, -0.3f, 0.15f, 1.0f);
        } else {
            sprintf(buffer, "Tacadas: %d", g_TacadasPlayer1);
            TextRendering_PrintString(window, buffer, -0.15f, 0.3f, 1.2f);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        g_HoverProxPista = RenderButton(window, 0.0f, -0.05f, btnW, btnH,
                                        g_nivelAtual < 3 ? "Proxima Pista" : "Finalizar",
                                        0.2f, 0.8f, 0.4f, 0.15f, 0.7f, 0.3f, true);

        g_HoverMenuCompleto = RenderButton(window, 0.0f, -0.25f, btnW, btnH, "Menu Principal",
                                        0.8f, 0.3f, 0.3f, 0.7f, 0.2f, 0.2f, true);
    } else if (g_CurrentState == MENU_PAUSE) {
        float btnW = 0.5f;
        float btnH = 0.08f;

        glUseProgram(textprogram_id);
        glUniform3f(glGetUniformLocation(textprogram_id, "textColor"), 1.0f, 1.0f, 1.0f);
        TextRendering_PrintString(window, "JOGO PAUSADO", -0.25f, 0.3f, 1.8f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        g_HoverContinuar = RenderButton(window, 0.0f, 0.05f, btnW, btnH, "Continuar",
                                        0.2f, 0.8f, 0.4f, 0.15f, 0.7f, 0.3f, true);

        g_HoverSairPause = RenderButton(window, 0.0f, -0.2f, btnW, btnH, "Sair para o Menu",
                                        0.8f, 0.3f, 0.3f, 0.7f, 0.2f, 0.2f, true);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void MenuUpdate(GLFWwindow* window, float delta_time)
{
    g_MenuCameraAngle += delta_time * 0.3f;
    float cam_dist = 6.0f;
    float cam_height = 3.0f;
    glm::vec4 cam_pos = glm::vec4(
        cos(g_MenuCameraAngle)*cam_dist, cam_height,
        sin(g_MenuCameraAngle)*cam_dist, 1.0f);
    glm::vec4 cam_look = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 cam_view_vec = cam_look - cam_pos;
    glm::vec4 cam_up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::mat4 mv = Matrix_Camera_View(cam_pos, cam_view_vec, cam_up);
    glm::mat4 mp = Matrix_Perspective(M_PI/3.0f, g_ScreenRatio, -0.1f, -50.0f);

    glUseProgram(g_GpuProgramID);
    glUniformMatrix4fv(g_view_uniform, 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(g_projection_uniform, 1, GL_FALSE, glm::value_ptr(mp));
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaGramaPista"), g_TexturaPistaGrama);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaParedesPista"), g_TexturaPistaParede);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaBola"), g_TexturaBola);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "u_TexturaTaco"), g_TexturaTaco);
    glUniform3f(glGetUniformLocation(g_GpuProgramID, "g_PosLuz"), g_PosLuz.x, g_PosLuz.y, g_PosLuz.z);

    glm::mat4 model;
    glDisable(GL_CULL_FACE);

    // Grama global
    model = Matrix_Translate(0.0f, -0.01f, 0.0f) * Matrix_Scale(50.0f, 1.0f, 50.0f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, GRAMA);
    DrawVirtualObject("the_plane");

    // Chão da pista
    model = Matrix_Translate(0.0f,0.0f,0.0f) * Matrix_Scale(2.0f, 1.0f, 5.0f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, PISTA_CHAO);
    DrawVirtualObject("the_plane");

    // Paredes
    model = Matrix_Translate(2.0f, 0.1f, 0.0f) * Matrix_Rotate_Z(M_PI/2.0f) * Matrix_Scale(0.1f, 1.0f, 5.0f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, PISTA_PAREDE);
    DrawVirtualObject("the_plane");
    model = Matrix_Translate(-2.0f, 0.1f, 0.0f) * Matrix_Rotate_Z(M_PI/2.0f) * Matrix_Scale(0.1f, 1.0f, 5.0f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, PISTA_PAREDE);
    DrawVirtualObject("the_plane");
    model = Matrix_Translate(0.0f, 0.1f, 5.0f) * Matrix_Rotate_X(M_PI/2.0f) * Matrix_Scale(2.0f, 1.0f, 0.1f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, PISTA_PAREDE);
    DrawVirtualObject("the_plane");
    model = Matrix_Translate(0.0f, 0.1f, -5.0f) * Matrix_Rotate_X(M_PI/2.0f) * Matrix_Scale(2.0f, 1.0f, 0.1f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, PISTA_PAREDE);
    DrawVirtualObject("the_plane");

    glEnable(GL_CULL_FACE);

    // Passa a posição do buraco para o shader para descartar os fragmentos da pista e criar o buraco físico
    glm::vec3 hp(0.0f, 0.0f, 4.0f);
    glUniform3f(glGetUniformLocation(g_GpuProgramID, "u_HolePosition"), hp.x, hp.y, hp.z);

    // Buraco
    glDisable(GL_CULL_FACE);
    model = Matrix_Translate(hp.x, hp.y+0.01f, hp.z) * Matrix_Scale(0.12f, 0.12f, 0.12f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, BURACO);
    DrawVirtualObject("the_sphere");
    glEnable(GL_CULL_FACE);

    // Bandeira
    model = Matrix_Translate(hp.x + 0.3f, hp.y, hp.z) * Matrix_Scale(0.1f, 0.1f, 0.1f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, MASTRO);
    DrawVirtualObject("Bandeira_Mastro");

    glDisable(GL_CULL_FACE);
    glUniform1i(g_object_id_uniform, BANDEIRA);
    DrawVirtualObject("Blandeira");
    glEnable(GL_CULL_FACE);

    // Bola estática para visualização
    model = Matrix_Translate(0.0f, 0.025f, -3.0f) * Matrix_Scale(0.025f, 0.025f, 0.025f);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, BOLA);
    DrawVirtualObject("the_sphere");

    // Renderiza overlay do menu
    MenuRenderOverlay(window);
}

void MenuHandleClick(GLFWwindow* window)
{
    if (g_CurrentState == MENU_LEVEL_COMPLETE) {
        if (g_HoverProxPista) ProxNivel(window);
        if (g_HoverMenuCompleto) {
            g_CurrentState = MENU_MAIN;
            g_nivelAtual = 1;
            if (g_audioInitialized) {
                ma_sound_stop(&g_ambientSound);
                ma_sound_set_volume(&g_musicMenu, g_MusicVolume);
                ma_sound_start(&g_musicMenu);
                ma_sound_set_fade_in_milliseconds(&g_musicMenu, 0.0f, 1.0f, 2000);
            }
        }
    }
    else if (g_CurrentState == MENU_PAUSE) {
        if (g_HoverContinuar) {
            g_CurrentState = PLAYING;
            if (g_audioInitialized) {
                ma_sound_stop(&g_musicMenu);
                ma_sound_set_volume(&g_ambientSound, g_AmbientVolume);
                ma_sound_start(&g_ambientSound);
            }
        }
        if (g_HoverSairPause) {
            g_CurrentState = MENU_MAIN;
            g_nivelAtual = 1;
        }
    }
    else if (g_CurrentState == MENU_MAIN) {
        if (g_HoverJogar) {
            g_CurrentState = MENU_LEVELS;
            g_MultiplayerAtivo = false;
        }
        else if (g_HoverMultiplayer) {
            g_CurrentState = MENU_LEVELS;
            g_MultiplayerAtivo = true;
        }
        else if (g_HoverConfig) g_CurrentState = MENU_SETTINGS;
        else if (g_HoverSair) glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (g_CurrentState == MENU_LEVELS) {
        if (g_HoverVoltar) g_CurrentState = MENU_MAIN;

        // Verifica clique nos botões de nível (3 habilitados)
        float startX = -0.40f;
        float spacing = 0.20f;
        float btnSize = 0.075f;
        for (int i = 0; i < 3; i++) {
            float bx = startX + i * spacing;
            if (IsMouseOverRect(window, bx, 0.15f, btnSize, btnSize)) {
                g_CurrentState = PLAYING;
                if (g_audioInitialized) {
                    ma_sound_stop(&g_musicMenu);
                    ma_sound_set_volume(&g_ambientSound, g_AmbientVolume);
                    ma_sound_start(&g_ambientSound);
                }
                g_nivelAtual = i + 1;

                // Reset do estado do jogo
                g_BolaNoBuraco = false;
                g_BolaNoBuracoTwo = false;
                g_TacadasPlayer1 = 0;
                g_TacadasPlayer2 = 0;
                g_TerminouJogada = false;
                g_JogadorAtual = false;
                g_BolaEmFocoAtual = false;
                g_BolaRotationMatrix = glm::mat4(1.0f);
                g_BolaRotationMatrixTwo = glm::mat4(1.0f);
                g_TacoRotacao = 0.0f;
                g_VelocidadeBola = glm::vec3(0.0f);
                g_VelocidadeBolaTwo = glm::vec3(0.0f);
                g_BolaNoCaminho = false;
                g_IndiceCaminho = 0;
                g_ProgressoCaminho = 0.0f;
                g_BolaNoCaminhoTwo = false;
                g_IndiceCaminhoTwo = 0;
                g_ProgressoCaminhoTwo = 0.0f;

                // Posições dependem do nível (coordenadas verificadas via análise do mesh)
                if (g_nivelAtual == 1) {
                    g_PosBola = glm::vec3(0.0f, 0.025f, -3.0f);
                    g_PosBolaTwo = glm::vec3(0.5f, 0.025f, -3.0f);
                    g_HolePosition = glm::vec3(0.0f, 0.0f, 4.0f);
                } else if (g_nivelAtual == 2) {
                    // PistaCurva: spawn no centro da seção larga, hole no final
                    g_PosBola = glm::vec3(3.0f, 0.1f, -1.0f);
                    g_PosBolaTwo = glm::vec3(2.7f, 0.1f, -1.0f);
                    g_HolePosition = glm::vec3(-0.27f, 0.07f, -3.3f);
                } else if (g_nivelAtual == 3) {
                    // PistaLoop: spawn no início, hole no fim
                    g_PosBola = glm::vec3(3.7f, 0.15f, -6.5f);
                    g_PosBolaTwo = glm::vec3(3.2f, 0.15f, -6.5f);
                    g_HolePosition = glm::vec3(1.3f, 0.12f, 9.2f);
                }

                break;
            }
        }
    }
    else if (g_CurrentState == MENU_SETTINGS) {
        if (g_HoverVoltar) g_CurrentState = MENU_MAIN;
        // Volume sliders clique
        if (IsMouseOverRect(window, 0.15f, 0.55f, 0.30f, 0.025f) || IsMouseOverRect(window, 0.15f, 0.45f, 0.30f, 0.025f)) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            int ww, wh;
            glfwGetWindowSize(window, &ww, &wh);
            float ndcX = (2.0f*(float)mx/ww)-1.0f;
            float ndcY = 1.0f - (2.0f*(float)my/wh);
            float sliderLeft = 0.15f - 0.30f;
            float sliderRight = 0.15f + 0.30f;
            float newVal = (ndcX - sliderLeft) / (sliderRight - sliderLeft);
            newVal = std::max(0.0f, std::min(1.0f, newVal));

            if (ndcY > 0.50f) { // Music slider
                g_MusicVolume = newVal;
                if (g_audioInitialized) ma_sound_set_volume(&g_musicMenu, g_MusicVolume);
            } else { // Ambient slider
                g_AmbientVolume = newVal;
                if (g_audioInitialized) ma_sound_set_volume(&g_ambientSound, g_AmbientVolume);
            }
        }
        // Setas de textura
        if (g_HoverGramaL) g_TexturaPistaGrama = (g_TexturaPistaGrama + 3) % 4;
        if (g_HoverGramaR) g_TexturaPistaGrama = (g_TexturaPistaGrama + 1) % 4;
        if (g_HoverParedeL) g_TexturaPistaParede = (g_TexturaPistaParede + 2) % 3;
        if (g_HoverParedeR) g_TexturaPistaParede = (g_TexturaPistaParede + 1) % 3;
        if (g_HoverBolaL) g_TexturaBola = (g_TexturaBola + 2) % 3;
        if (g_HoverBolaR) g_TexturaBola = (g_TexturaBola + 1) % 3;
        if (g_HoverTacoL) g_TexturaTaco = (g_TexturaTaco + 2) % 3;
        if (g_HoverTacoR) g_TexturaTaco = (g_TexturaTaco + 1) % 3;
    }
}

void ProxNivel(GLFWwindow* window) {
    if (g_nivelAtual < 3) {
        g_nivelAtual++;
        g_CurrentState = PLAYING;
    } else {
        // Voltou pro menu após completar todos os níveis
        g_CurrentState = MENU_MAIN;
        g_nivelAtual = 1;
        if (g_audioInitialized) {
            ma_sound_stop(&g_ambientSound);
            ma_sound_set_volume(&g_musicMenu, g_MusicVolume);
            ma_sound_start(&g_musicMenu);
            ma_sound_set_fade_in_milliseconds(&g_musicMenu, 0.0f, 1.0f, 2000);
        }
    }

    g_BolaNoBuraco = false;
    g_BolaNoBuracoTwo = false;
    g_TacadasPlayer1 = 0;
    g_TacadasPlayer2 = 0;
    g_TerminouJogada = false;
    g_JogadorAtual = false;
    g_BolaEmFocoAtual = false;
    g_VelocidadeBola = glm::vec3(0.0f);
    g_VelocidadeBolaTwo = glm::vec3(0.0f);
    g_BolaNoCaminho = false;
    g_IndiceCaminho = 0;
    g_ProgressoCaminho = 0.0f;
    g_BolaNoCaminhoTwo = false;
    g_IndiceCaminhoTwo = 0;
    g_ProgressoCaminhoTwo = 0.0f;
    g_BolaRotationMatrix = glm::mat4(1.0f);
    g_BolaRotationMatrixTwo = glm::mat4(1.0f);
    g_TacoRotacao = 0.0f;

    if (g_nivelAtual == 1) {
        g_PosBola = glm::vec3(0.0f, 0.025f, -3.0f);
        g_PosBolaTwo = glm::vec3(0.5f, 0.025f, -3.0f);
        g_HolePosition = glm::vec3(0.0f, 0.0f, 4.0f);
    } else if (g_nivelAtual == 2) {
        g_PosBola = glm::vec3(3.0f, 0.1f, -1.0f);
        g_PosBolaTwo = glm::vec3(2.7f, 0.1f, -1.0f);
        g_HolePosition = glm::vec3(-0.27f, 0.07f, -3.3f);
    } else if (g_nivelAtual == 3) {
        g_PosBola = glm::vec3(3.7f, 0.15f, -6.5f);
        g_PosBolaTwo = glm::vec3(3.2f, 0.15f, -6.5f);
        g_HolePosition = glm::vec3(1.3f, 0.12f, 9.2f);
    }
}

// Forward declaration (definição completa abaixo)
static float RaycastTrackHeight(const std::vector<TrackTriangle>& tris,
                                float x, float current_y, float z, float fallbackY);

// =============================================
// CONSTANTES DO LOOP — Rail Physics (world space)
// =============================================
// Geometria analítica do loop cilíndrico, extraída da análise do mesh PistaLoop.obj.
// O loop é um círculo vertical no plano YZ, centrado em LOOP_CENTER, com raio LOOP_RADIUS.
// A bola percorre o círculo em coordenadas polares: θ=0 → base (6 horas), θ=π → topo (12 horas).
static const float LOOP_CENTER_X  = 2.202f;   // Posição lateral (travada enquanto no rail)
static const float LOOP_CENTER_Y  = 1.196f;   // Centro vertical do loop
static const float LOOP_CENTER_Z  = -0.13f;   // Centro em profundidade
static const float LOOP_RADIUS    = 1.03f;    // Raio do círculo
static const float GRAVITY        = 9.81f;    // Gravidade realista (m/s²)
static const float BALL_RADIUS    = 0.025f;
static const float LOOP_FRICTION  = 0.02f;    // Atrito leve no rail para estabilidade

// --- AABB Trigger Volume ---
// Caixa invisível na rampa de entrada do loop. Engloba toda a largura da curva.
static const glm::vec3 TRIGGER_AABB_MIN = glm::vec3(3.0f, -0.5f, -2.5f);
static const glm::vec3 TRIGGER_AABB_MAX = glm::vec3(4.2f, 1.5f, -1.0f);
static const float     TRIGGER_MIN_SPEED = 0.5f; // Velocidade mínima reduzida para garantir ativação

// --- AABB Exit Volume ---
// Caixa na saída do loop, onde a bola retorna ao estado de chão.
static const glm::vec3 EXIT_AABB_MIN = glm::vec3(1.2f, 0.0f, 0.3f);
static const glm::vec3 EXIT_AABB_MAX = glm::vec3(3.2f, 0.5f, 1.2f);

/**
 * @brief Testa se um ponto está dentro de uma AABB.
 */
static inline bool PointInsideAABB(const glm::vec3& p,
                                   const glm::vec3& aabb_min,
                                   const glm::vec3& aabb_max)
{
    return p.x >= aabb_min.x && p.x <= aabb_max.x &&
           p.y >= aabb_min.y && p.y <= aabb_max.y &&
           p.z >= aabb_min.z && p.z <= aabb_max.z;
}

static void ApplyTrackCollision(glm::vec3& pos, glm::vec3& vel,
                                float min_x, float max_x,
                                float min_z, float max_z)
{
    const float B = BALL_RADIUS;
    bool hit = false;
    float impact_vel = 0.0f;

    if (pos.x > max_x - B) { pos.x = max_x - B; impact_vel = std::max(impact_vel, std::abs(vel.x)); vel.x *= -0.5f; hit = true; }
    if (pos.x < min_x + B) { pos.x = min_x + B; impact_vel = std::max(impact_vel, std::abs(vel.x)); vel.x *= -0.5f; hit = true; }
    if (pos.z > max_z - B) { pos.z = max_z - B; impact_vel = std::max(impact_vel, std::abs(vel.z)); vel.z *= -0.5f; hit = true; }
    if (pos.z < min_z + B) { pos.z = min_z + B; impact_vel = std::max(impact_vel, std::abs(vel.z)); vel.z *= -0.5f; hit = true; }

    if (hit && g_audioInitialized && impact_vel > 0.5f) {
        ma_sound_set_volume(&g_wallSound, g_AmbientVolume * 0.5f);
        ma_sound_seek_to_pcm_frame(&g_wallSound, 0);
        ma_sound_start(&g_wallSound);
    }
}


/**
 * Calculates the closest point on a 3D triangle to a given coordinate.
 * Uses Voronoi region separation to evaluate the closest geometric feature (vertex, edge, or face)
 * to maintain strict constraints for non-planar surface traversal.
 *
 * @param p The target spatial coordinate.
 * @param a Triangle vertex 0.
 * @param b Triangle vertex 1.
 * @param c Triangle vertex 2.
 * @param out_normal Output parameter returning the face normal of the evaluated triangle.
 * @return The closest point on the triangle's surface to p.
 */
static glm::vec3 ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, glm::vec3& out_normal) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = p - a;

    out_normal = glm::normalize(glm::cross(ab, ac));

    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    glm::vec3 bp = p - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    float vc = d1*d4 - d3*d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    glm::vec3 cp = p - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vb = d5*d2 - d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + w * ac;
    }

    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

/**
 * Executes sub-stepped physics integration for Level 3 (Loop Track).
 * Dynamically switches between planar raycast constraints and universal 3D mesh collisions
 * to allow navigation through complex topologies without predefined waypoints.
 *
 * @param pos Current positional vector of the ball.
 * @param vel Current velocity vector of the ball.
 * @param no_caminho Deprecated waypoint state flag (maintained for signature compatibility).
 * @param indice Deprecated waypoint state flag.
 * @param progresso Deprecated waypoint state flag.
 * @param rot_matrix Reference to the rotation matrix for visual representation.
 * @param dt Delta time for the current frame.
 */
static void AtualizarFisicaLoop(glm::vec3& pos, glm::vec3& vel,
                                bool& no_caminho, int& indice, float& progresso,
                                glm::mat4& rot_matrix, float dt)
{
    const int N = std::max(1, (int)(dt / 0.001f));
    float sub = dt / N;

    for (int i = 0; i < N; i++) {
        vel.y -= GRAVITY * sub;

        glm::vec3 next_pos = pos + vel * sub;

        if (!g_PistaLoopAllTriangles.empty()) {
            bool fallback_to_planar = false;
            glm::vec3 best_p = next_pos;
            glm::vec3 best_n = glm::vec3(0.0f, 1.0f, 0.0f);
            float min_dist2 = 1e9f;

            for (const auto& tri : g_PistaLoopAllTriangles) {
                float min_z = std::min({tri.v0.z, tri.v1.z, tri.v2.z});
                float max_z = std::max({tri.v0.z, tri.v1.z, tri.v2.z});
                if (next_pos.z < min_z - 0.5f || next_pos.z > max_z + 0.5f) continue;

                glm::vec3 n;
                glm::vec3 cp = ClosestPointOnTriangle(next_pos, tri.v0, tri.v1, tri.v2, n);

                // BACKFACE CULLING: Evita colar por baixo do chão na base do loop
                if (pos.y < 1.0f && n.y < -0.05f) continue;

                glm::vec3 diff = next_pos - cp;
                float dist2 = glm::dot(diff, diff);

                if (dist2 < min_dist2 && dist2 < 0.25f) {
                    min_dist2 = dist2;
                    best_p = cp;
                    best_n = n;
                }
            }

            if (min_dist2 < 0.25f) {
                glm::vec3 diff = next_pos - best_p;
                float normal_dist = glm::dot(diff, best_n);
                glm::vec3 diff_tangent = diff - best_n * normal_dist;
                float lateral = glm::length(diff_tangent);

                if (lateral > 0.01f) {
                    glm::vec3 wall_normal = diff_tangent / lateral;
                    float vel_into_wall = glm::dot(vel, wall_normal);
                    if (vel_into_wall > 0.0f) {
                        vel -= wall_normal * vel_into_wall * 1.5f;
                        if (g_audioInitialized && vel_into_wall > 0.5f) {
                            ma_sound_set_volume(&g_wallSound, g_AmbientVolume * 0.5f);
                            ma_sound_seek_to_pcm_frame(&g_wallSound, 0);
                            ma_sound_start(&g_wallSound);
                        }
                    }
                }

                pos = best_p + best_n * BALL_RADIUS;

                float vel_normal = glm::dot(vel, best_n);
                if (vel_normal < 0.0f) {
                    vel -= best_n * vel_normal;
                }

                vel -= vel * LOOP_FRICTION * sub;

                float speed = glm::length(vel);
                if (best_n.y < 0.5f && speed < 1.5f && pos.y > 0.5f) {
                    vel.y -= GRAVITY * sub * 2.0f;
                    pos = next_pos;
                    fallback_to_planar = true;
                }
            } else {
                fallback_to_planar = true;
            }

            if (fallback_to_planar) {
                float checkY = RaycastTrackHeight(g_PistaLoopTriangles, next_pos.x, pos.y + 2.0f, next_pos.z, -1e9f);
                if (checkY < -1e8f) {
                    float min_d2_edge = 1e9f;
                    glm::vec3 best_cp = next_pos;
                    for (const auto& tri : g_PistaLoopTriangles) {
                        glm::vec3 n;
                        glm::vec3 cp = ClosestPointOnTriangle(next_pos, tri.v0, tri.v1, tri.v2, n);
                        float dx = next_pos.x - cp.x;
                        float dy = pos.y - cp.y;
                        float dz = next_pos.z - cp.z;
                        float d2 = dx*dx + dy*dy + dz*dz;
                        if (d2 < min_d2_edge) {
                            min_d2_edge = d2;
                            best_cp = cp;
                        }
                    }
                    if (min_d2_edge > 0.0004f) {
                        glm::vec2 diff(next_pos.x - best_cp.x, next_pos.z - best_cp.z);
                        float dist = glm::length(diff);
                        if (dist > 1e-4f) {
                            glm::vec2 wall_normal = diff / dist;
                            glm::vec2 vel_xz(vel.x, vel.z);
                            float vel_into_wall = glm::dot(vel_xz, wall_normal);
                            if (vel_into_wall > 0.0f) {
                                vel.x -= wall_normal.x * vel_into_wall * 1.5f;
                                vel.z -= wall_normal.y * vel_into_wall * 1.5f;
                                if (g_audioInitialized && vel_into_wall > 0.5f) {
                                    ma_sound_set_volume(&g_wallSound, g_AmbientVolume * 0.5f);
                                    ma_sound_seek_to_pcm_frame(&g_wallSound, 0);
                                    ma_sound_start(&g_wallSound);
                                }
                            }
                        }
                        next_pos.x = best_cp.x;
                        next_pos.z = best_cp.z;
                    }
                }

                vel.x -= vel.x * 0.9f * sub;
                vel.z -= vel.z * 0.9f * sub;
                if (glm::length(glm::vec3(vel.x, 0.0f, vel.z)) < 0.05f) { vel.x = 0.0f; vel.z = 0.0f; }

                pos = next_pos;

                float sY = RaycastTrackHeight(g_PistaLoopTriangles, pos.x, pos.y + 2.0f, pos.z, -1e9f);
                if (sY < -1e8f && !g_PistaLoopTriangles.empty()) {
                    float min_d2 = 1e9f;
                    for (const auto& tri : g_PistaLoopTriangles) {
                        glm::vec3 n;
                        glm::vec3 cp = ClosestPointOnTriangle(pos, tri.v0, tri.v1, tri.v2, n);
                        float d2 = (pos.x - cp.x)*(pos.x - cp.x) + (pos.z - cp.z)*(pos.z - cp.z);
                        if (d2 < min_d2) { min_d2 = d2; sY = cp.y; }
                    }
                }

                if (sY > -1e8f && pos.y <= sY + BALL_RADIUS) {
                    pos.y = sY + BALL_RADIUS;
                    if (vel.y < 0.0f) vel.y = 0.0f;
                }
            }
        }
    }

    float d = glm::length(vel * dt);
    if (d > 1e-4f) {
        glm::vec4 ax(vel.z, 0, -vel.x, 0);
        float al = norm(ax);
        if (al > 1e-4f) rot_matrix = Matrix_Rotate(d / BALL_RADIUS, ax / al) * rot_matrix;
    }
}


static void AtualizarFisicaSubstep(glm::vec3& pos, glm::vec3& vel,
                                   glm::mat4& rot_matrix, float dt, int nivel)
{
    const int N = std::max(1, (int)(dt / 0.001f));
    float sub = dt / N;
    for (int i = 0; i < N; i++) {
        pos += vel * sub;
        vel -= vel * 0.9f * sub;
        if (glm::length(vel) < 0.05f) vel = glm::vec3(0.0f);
        if (nivel == 1) ApplyTrackCollision(pos, vel, -2.0f, 2.0f, -5.0f, 5.0f);
    }
    float d = glm::length(vel * dt);
    if (d > 1e-4f) {
        glm::vec4 ax(vel.z, 0, -vel.x, 0);
        float al = norm(ax);
        if (al > 1e-4f) rot_matrix = Matrix_Rotate(d/BALL_RADIUS, ax/al) * rot_matrix;
    }
}

static void AtualizarFisicaMalha3D(glm::vec3& pos, glm::vec3& vel,
                                   glm::mat4& rot_matrix, float dt,
                                   const std::vector<TrackTriangle>& allTriangles,
                                   const std::vector<TrackTriangle>& floorTriangles)
{
    const int N = std::max(1, (int)(dt / 0.001f));
    float sub = dt / N;

    for (int i = 0; i < N; i++) {
        vel.y -= GRAVITY * sub;
        glm::vec3 next_pos = pos + vel * sub;

        // Planar boundary containment (Invisible Walls)
        // Elevado para +2.0f para ignorar problemas de tunneling
        float checkY = RaycastTrackHeight(floorTriangles, next_pos.x, pos.y + 2.0f, next_pos.z, -1e9f);

        if (checkY < -1e8f) {
            float min_d2_edge = 1e9f;
            glm::vec3 best_cp = next_pos;
            for (const auto& tri : floorTriangles) {
                glm::vec3 n;
                glm::vec3 cp = ClosestPointOnTriangle(next_pos, tri.v0, tri.v1, tri.v2, n);
                float dx = next_pos.x - cp.x;
                float dy = pos.y - cp.y;
                float dz = next_pos.z - cp.z;
                float d2 = dx*dx + dy*dy + dz*dz;
                if (d2 < min_d2_edge) {
                    min_d2_edge = d2;
                    best_cp = cp;
                }
            }

            if (min_d2_edge > 0.0004f) {
                glm::vec2 diff(next_pos.x - best_cp.x, next_pos.z - best_cp.z);
                float dist = glm::length(diff);
                if (dist > 1e-4f) {
                    glm::vec2 wall_normal = diff / dist;
                    glm::vec2 vel_xz(vel.x, vel.z);
                    float vel_into_wall = glm::dot(vel_xz, wall_normal);

                    if (vel_into_wall > 0.0f) {
                        vel.x -= wall_normal.x * vel_into_wall * 1.5f;
                        vel.z -= wall_normal.y * vel_into_wall * 1.5f;

                        if (g_audioInitialized && vel_into_wall > 0.5f) {
                            ma_sound_set_volume(&g_wallSound, g_AmbientVolume * 0.5f);
                            ma_sound_seek_to_pcm_frame(&g_wallSound, 0);
                            ma_sound_start(&g_wallSound);
                        }
                    }
                }
                next_pos.x = best_cp.x;
                next_pos.z = best_cp.z;
            }
        }

        pos = next_pos;

        // Perfect floor following with gap filler!
        float sY = RaycastTrackHeight(floorTriangles, pos.x, pos.y + 2.0f, pos.z, -1e9f);

        if (sY < -1e8f && !floorTriangles.empty()) {
            float min_d2 = 1e9f;
            for (const auto& tri : floorTriangles) {
                glm::vec3 n;
                glm::vec3 cp = ClosestPointOnTriangle(pos, tri.v0, tri.v1, tri.v2, n);
                float dx = pos.x - cp.x;
                float dz = pos.z - cp.z;
                float d2 = dx*dx + dz*dz;
                if (d2 < min_d2) {
                    min_d2 = d2;
                    sY = cp.y;
                }
            }
        }

        if (sY > -1e8f) {
            if (pos.y <= sY + BALL_RADIUS) {
                pos.y = sY + BALL_RADIUS;
                if (vel.y < 0.0f) {
                    vel.y = 0.0f;
                }
            }
        }

        // Apply continuous floor friction
        vel.x -= vel.x * 0.8f * sub;
        vel.z -= vel.z * 0.8f * sub;

        if (glm::length(vel) < 0.05f) vel = glm::vec3(0.0f);
    }

    // Rotação visual
    float d = glm::length(vel * dt);
    if (d > 1e-4f) {
        glm::vec4 ax(vel.z, 0, -vel.x, 0);
        float al = norm(ax);
        if (al > 1e-4f) rot_matrix = Matrix_Rotate(d / BALL_RADIUS, ax / al) * rot_matrix;
    }
}

void AtualizarFisicaBola(float delta_time) {
    glm::vec2 diff = glm::vec2(g_PosBola.x - g_HolePosition.x, g_PosBola.z - g_HolePosition.z);
    float distToHole = glm::length(diff);

    if (distToHole < 0.11f && g_PosBola.y <= g_HolePosition.y + 0.10f) {
        // Snap to center and kill horizontal velocity so it can't escape
        g_PosBola.x = g_HolePosition.x;
        g_PosBola.z = g_HolePosition.z;
        g_VelocidadeBola.x = 0.0f;
        g_VelocidadeBola.z = 0.0f;

        // Freefall perfectly straight down
        g_VelocidadeBola.y -= 9.8f * delta_time;
        g_PosBola.y += g_VelocidadeBola.y * delta_time;

        // Stop exactly at the bottom and end level instantly
        float bottom_y = g_HolePosition.y - 0.10f;
        if (g_PosBola.y <= bottom_y + BALL_RADIUS) {
            g_PosBola.y = bottom_y + BALL_RADIUS;
            g_VelocidadeBola = glm::vec3(0.0f);
            if (!g_BolaNoBuraco && g_audioInitialized) {
                ma_sound_set_volume(&g_victorySound, g_AmbientVolume);
                ma_sound_seek_to_pcm_frame(&g_victorySound, 0);
                ma_sound_start(&g_victorySound);
            }
            g_BolaNoBuraco = true;
            ClearTrail();
        }
    } else {
        if (g_nivelAtual == 3) {
            bool inside_loop_zone = (g_PosBola.z > -2.5f && g_PosBola.z < 1.5f);
            if (inside_loop_zone) {
                AtualizarFisicaLoop(g_PosBola, g_VelocidadeBola, g_BolaNoCaminho,
                                    g_IndiceCaminho, g_ProgressoCaminho,
                                    g_BolaRotationMatrix, delta_time);
            } else {
                AtualizarFisicaMalha3D(g_PosBola, g_VelocidadeBola,
                                       g_BolaRotationMatrix, delta_time,
                                       g_PistaLoopAllTriangles, g_PistaLoopTriangles);
            }
        }
        else if (g_nivelAtual == 2)
            AtualizarFisicaMalha3D(g_PosBola, g_VelocidadeBola,
                                   g_BolaRotationMatrix, delta_time,
                                   g_PistaCurvaAllTriangles, g_PistaCurvaTriangles);
        else
            AtualizarFisicaSubstep(g_PosBola, g_VelocidadeBola,
                                   g_BolaRotationMatrix, delta_time, g_nivelAtual);
    }
}

void AtualizarFisicaBolaTwo(float delta_time) {
    glm::vec2 diff = glm::vec2(g_PosBolaTwo.x - g_HolePosition.x, g_PosBolaTwo.z - g_HolePosition.z);
    float distToHole = glm::length(diff);

    if (distToHole < 0.11f && g_PosBolaTwo.y <= g_HolePosition.y + 0.10f) {
        // Snap to center and kill horizontal velocity so it can't escape
        g_PosBolaTwo.x = g_HolePosition.x;
        g_PosBolaTwo.z = g_HolePosition.z;
        g_VelocidadeBolaTwo.x = 0.0f;
        g_VelocidadeBolaTwo.z = 0.0f;

        // Freefall perfectly straight down
        g_VelocidadeBolaTwo.y -= 9.8f * delta_time;
        g_PosBolaTwo.y += g_VelocidadeBolaTwo.y * delta_time;

        // Stop exactly at the bottom and end level instantly
        float bottom_y = g_HolePosition.y - 0.10f;
        if (g_PosBolaTwo.y <= bottom_y + BALL_RADIUS) {
            g_PosBolaTwo.y = bottom_y + BALL_RADIUS;
            g_VelocidadeBolaTwo = glm::vec3(0.0f);
            if (!g_BolaNoBuracoTwo && g_audioInitialized) {
                ma_sound_set_volume(&g_victorySound, g_AmbientVolume);
                ma_sound_seek_to_pcm_frame(&g_victorySound, 0);
                ma_sound_start(&g_victorySound);
            }
            g_BolaNoBuracoTwo = true;
            ClearTrail2();
        }
    } else {
        if (g_nivelAtual == 3) {
            bool inside_loop_zone = (g_PosBolaTwo.z > -2.5f && g_PosBolaTwo.z < 1.5f);
            if (inside_loop_zone) {
                AtualizarFisicaLoop(g_PosBolaTwo, g_VelocidadeBolaTwo, g_BolaNoCaminhoTwo,
                                    g_IndiceCaminhoTwo, g_ProgressoCaminhoTwo,
                                    g_BolaRotationMatrixTwo, delta_time);
            } else {
                AtualizarFisicaMalha3D(g_PosBolaTwo, g_VelocidadeBolaTwo,
                                       g_BolaRotationMatrixTwo, delta_time,
                                       g_PistaLoopAllTriangles, g_PistaLoopTriangles);
            }
        }
        else if (g_nivelAtual == 2)
            AtualizarFisicaMalha3D(g_PosBolaTwo, g_VelocidadeBolaTwo,
                                   g_BolaRotationMatrixTwo, delta_time,
                                   g_PistaCurvaAllTriangles, g_PistaCurvaTriangles);
        else
            AtualizarFisicaSubstep(g_PosBolaTwo, g_VelocidadeBolaTwo,
                                   g_BolaRotationMatrixTwo, delta_time, g_nivelAtual);
    }
}



// =============================================
// RAYCAST HEIGHTMAP: Encontra a Y da superfície em (x, z)
// Lança um raio vertical para baixo de (x, alto, z) contra cada
// triângulo. Retorna o Y mais alto encontrado, ou fallbackY.
// Usa o teste de interseção Möller–Trumbore.
// =============================================
static float RaycastTrackHeight(const std::vector<TrackTriangle>& tris,
                                float x, float current_y, float z, float fallbackY)
{
    // Raio: origem = (x, current_y + 0.1f, z), direção = (0, -1, 0)
    // O offset de +0.1f garante que o raio saia imediatamente acima da bola,
    // ignorando qualquer malha de teto que esteja muito mais alto.
    glm::vec3 orig(x, current_y + 0.1f, z);
    glm::vec3 dir(0.0f, -1.0f, 0.0f);
    float best_y = -1e9f;
    bool hit = false;

    for (const auto& tri : tris) {
        float minX = tri.v0.x; if (tri.v1.x < minX) minX = tri.v1.x; if (tri.v2.x < minX) minX = tri.v2.x;
        if (x < minX) continue;
        float maxX = tri.v0.x; if (tri.v1.x > maxX) maxX = tri.v1.x; if (tri.v2.x > maxX) maxX = tri.v2.x;
        if (x > maxX) continue;

        float minZ = tri.v0.z; if (tri.v1.z < minZ) minZ = tri.v1.z; if (tri.v2.z < minZ) minZ = tri.v2.z;
        if (z < minZ) continue;
        float maxZ = tri.v0.z; if (tri.v1.z > maxZ) maxZ = tri.v1.z; if (tri.v2.z > maxZ) maxZ = tri.v2.z;
        if (z > maxZ) continue;

        glm::vec3 e1 = tri.v1 - tri.v0;
        glm::vec3 e2 = tri.v2 - tri.v0;

        glm::vec3 h = glm::cross(dir, e2);
        float a = glm::dot(e1, h);
        if (fabs(a) < 1e-7f) continue; // Paralelo

        float f = 1.0f / a;
        glm::vec3 s = orig - tri.v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) continue;

        glm::vec3 q = glm::cross(s, e1);
        float v = f * glm::dot(dir, q);
        if (v < 0.0f || u + v > 1.0f) continue;

        float t = f * glm::dot(e2, q);
        if (t > 0.0f) {
            float hitY = orig.y + dir.y * t; // = 100 - t
            if (hitY > best_y) {
                best_y = hitY;
                hit = true;
            }
        }
    }
    return hit ? best_y : fallbackY;
}

void AtualizarCamera(){
    if(!rotacao_camera){
            // A câmera segue a bola mantendo o POV do golfista
            // Aproximamos a câmera para o estilo 8 Ball Pool
            g_CameraDistance = 1.0f;
            float camera_height = 0.35f;
            glm::vec4 camera_position_c  = glm::vec4( g_PosBola.x + cos(g_TacoRotacao) * g_CameraDistance, g_PosBola.y + camera_height, g_PosBola.z + sin(g_TacoRotacao) * g_CameraDistance, 1.0f );
            glm::vec4 camera_lookat_l    = glm::vec4(g_PosBola.x, g_PosBola.y, g_PosBola.z, 1.0f);
            glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c;
            glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);

            if(!g_JogadorAtual){
                camera_position_c  = glm::vec4(
                    g_PosBola.x + cos(g_TacoRotacao) * g_CameraDistance,
                    g_PosBola.y + camera_height,
                    g_PosBola.z + sin(g_TacoRotacao) * g_CameraDistance,
                    1.0f
                );
                camera_lookat_l    = glm::vec4(g_PosBola.x, g_PosBola.y, g_PosBola.z, 1.0f);
                camera_view_vector = camera_lookat_l - camera_position_c;
            } else if(g_JogadorAtual){
                camera_position_c  = glm::vec4(
                    g_PosBolaTwo.x + cos(g_TacoRotacao) * g_CameraDistance,
                    g_PosBolaTwo.y + camera_height,
                    g_PosBolaTwo.z + sin(g_TacoRotacao) * g_CameraDistance,
                    1.0f
                );
                camera_lookat_l    = glm::vec4(g_PosBolaTwo.x, g_PosBolaTwo.y, g_PosBolaTwo.z, 1.0f);
                camera_view_vector = camera_lookat_l - camera_position_c;
                camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);
            }
            // Computamos a matriz "View" utilizando os parâmetros da câmera para
            // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
            view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);
        }
        else{
            if(!g_JogadorAtual){

                g_CameraDistance = 1.0f;

                // Coordenadas esféricas
                float camera_x = g_PosBola.x + g_CameraDistance * cos(g_CameraPhi) * cos(g_CameraTheta);
                float camera_y = g_PosBola.y + g_CameraDistance * sin(g_CameraPhi);
                float camera_z = g_PosBola.z + g_CameraDistance * cos(g_CameraPhi) * sin(g_CameraTheta);

                glm::vec4 camera_position_c = glm::vec4(camera_x, camera_y + 0.3f, camera_z, 1.0f);
                glm::vec4 camera_lookat_l = glm::vec4(g_PosBola.x, g_PosBola.y, g_PosBola.z, 1.0f);
                glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c;
                glm::vec4 camera_up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f);


                view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

            } else if(g_JogadorAtual) {

                g_CameraDistance = 1.0f;

                // Coordenadas esféricas
                float camera_x = g_PosBolaTwo.x + g_CameraDistance * cos(g_CameraPhi) * cos(g_CameraTheta);
                float camera_y = g_PosBolaTwo.y + g_CameraDistance * sin(g_CameraPhi);
                float camera_z = g_PosBolaTwo.z + g_CameraDistance * cos(g_CameraPhi) * sin(g_CameraTheta);

                glm::vec4 camera_position_c = glm::vec4(camera_x, camera_y + 0.3f, camera_z, 1.0f);
                glm::vec4 camera_lookat_l = glm::vec4(g_PosBolaTwo.x, g_PosBolaTwo.y, g_PosBolaTwo.z, 1.0f);
                glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c;
                glm::vec4 camera_up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f);


                view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

            }
        }
}
