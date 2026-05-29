/**
 * @file globals.cpp
 * @brief Instanciação (definição) de todas as variáveis globais do projeto.
 *
 * Cada variável declarada como `extern` em globals.h é definida aqui
 * com seu valor inicial. Este arquivo é compilado uma única vez e
 * linkado com todos os outros módulos.
 */
#include "globals.h"

// --- Cena Virtual e Pilha de Matrizes ---
std::map<std::string, SceneObject> g_VirtualScene;
std::stack<glm::mat4>  g_MatrixStack;

// --- Janela ---
float g_ScreenRatio = 1.0f;

// --- Ângulos de Euler ---
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// --- Estado dos botões do mouse ---
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;
bool g_MiddleMouseButtonPressed = false;

// --- Câmera (coordenadas esféricas) ---
float g_CameraTheta = 0.0f;
float g_CameraPhi = 0.0f;
float g_CameraDistance = 3.5f;

glm::mat4 view = Matrix_Camera_View(
    glm::vec4(0.0f, 0.0f, 3.5f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
);

// --- Controle de antebraço/torso ---
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// --- Bola de Golfe (Jogador 1) ---
glm::vec3 g_PosBola = glm::vec3(0.0f, 0.025f, -3.0f);
glm::vec3 g_PosTaco = glm::vec3(0.0f, 0.0f, -1.0f);
float g_DistanciaTaco = 0.2f;
float g_TacoRotacao = 0.0f;
float g_TacoRotacaoVertical = 0.0f;

// --- Animação do Taco ---
double g_TempoRotacaoTaco = -1.0;
double g_TempoDesdeEspaco = 0.0;
double g_TempoDesdeTacada = 0.0;
float g_DuracaoRotacaoTaco = 0.5f;
float g_AnguloRotacaoTaco = M_PI / 2.5f;

// --- Física da Bola (Jogador 1) ---
glm::vec3 g_VelocidadeBola = glm::vec3(0.0f, 0.0f, 0.0f);
bool g_BolaParada = true;
bool g_BolaNoBuraco = false;
glm::mat4 g_BolaRotationMatrix = glm::mat4(1.0f);

// --- Mecânica de Força da Tacada ---
bool g_EspacoPressionado = false;
double g_InicioEspaco = 0.0;
float g_ForcaTacada = 0.0f;

// --- Iluminação ---
glm::vec3 g_PosLuz(0.0f, 10.0f, 0.0f);

// --- Projeção ---
float field_of_view = M_PI / 3.0f;
bool rotacao_camera = false;
bool g_UsePerspectiveProjection = true;

// --- HUD / Texto ---
bool g_ShowInfoText = true;

// --- Programa de GPU ---
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

// --- Texturas ---
GLuint g_NumLoadedTextures = 0;

// --- Multiplayer ---
bool g_MultiplayerAtivo = false;
bool g_JogadorAtual = false;
bool g_BolaNoBuracoTwo = false;
bool g_TerminouJogada = false;
bool g_BolaEmFocoAtual = false;

glm::vec3 g_PosBolaTwo = glm::vec3(1.0f, 0.025f, -3.0f);
glm::vec3 g_PosTacoTwo = glm::vec3(1.0f, 0.0f, -1.0f);
glm::vec3 g_VelocidadeBolaTwo = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 g_BolaRotationMatrixTwo = glm::mat4(1.0f);

// --- Sistema de Menu ---
GameState g_CurrentState = MENU_MAIN;
float g_MenuCameraAngle = 0.0f;
float g_MasterVolume = 0.8f;
int g_TexturaPistaGrama = 0;
int g_TexturaPistaParede = 0;
int g_TexturaBola = 0;
int g_TexturaTaco = 0;
GLuint g_HudShaderProgram = 0;
GLuint g_HudVAO = 0;
GLuint g_HudVBO = 0;
GLuint g_LogoTextureID = 0;
int g_LogoWidth = 1, g_LogoHeight = 1;

// --- Hover States ---
bool g_HoverJogar = false, g_HoverNiveis = false;
bool g_HoverConfig = false, g_HoverSair = false;
bool g_HoverVoltar = false;
bool g_HoverGramaL = false, g_HoverGramaR = false;
bool g_HoverParedeL = false, g_HoverParedeR = false;
bool g_HoverBolaL = false, g_HoverBolaR = false;
bool g_HoverTacoL = false, g_HoverTacoR = false;

// --- Posição do cursor ---
double g_LastCursorPosX, g_LastCursorPosY;
