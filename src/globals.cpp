/**
 * @file globals.cpp
 * @brief Instanciação (definição) de todas as variáveis globais do projeto.
 *
 * Cada variável declarada como `extern` em globals.h é definida aqui
 * com seu valor inicial. Este arquivo é compilado uma única vez e
 * linkado com todos os outros módulos.
 */
#include "globals.h"

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;
bool g_MiddleMouseButtonPressed = false;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

glm::mat4 view = Matrix_Camera_View(
    glm::vec4(0.0f, 0.0f, 3.5f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
);

// Variável que controla o nível atual do jogo 
int g_nivelAtual = 0;

// Variáveis que controlam rotação do antebraço
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variáveis que controlam translação do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Variáveis que rastreiam as posições da bola e taco de golfe (vou usar para posicionar o taco atrás da bola)
glm::vec3 g_PosBola = glm::vec3(0.0f, 0.025f, -3.0f); //posição atual da bola de golfe
glm::vec3 g_PosTaco = glm::vec3(0.0f, 0.0f, -1.0f); // direção apontada pelo taco
float g_DistanciaTaco = 0.2f; // distância entre o taco e a bola
float g_TacoRotacao = 0.0f; // angulo rotação do taco 
float g_TacoRotacaoVertical = 0.0f; // angulo rotação vertical do taco

// Variáveis pra controlar a animação do taco
double g_TempoRotacaoTaco = -1.0; // tempo de início da rotacao
double g_TempoDesdeEspaco = 0.0; // tempo desde que a barra de espaço foi solta
double g_TempoDesdeTacada = 0.0; // tempo desde o final da tacada
float g_DuracaoRotacaoTaco = 0.5f; // duração da rotação em segundos
float g_AnguloRotacaoTaco = M_PI / 2.5f; // ângulo máximo

// Variáveis de física da bola
glm::vec3 g_VelocidadeBola = glm::vec3(0.0f, 0.0f, 0.0f);
bool g_BolaParada = true; // Indica se a bola está parada
bool g_BolaNoBuraco = false;
glm::mat4 g_BolaRotationMatrix = glm::mat4(1.0f); // Rotação acumulativa da bola

// Variáveis para a mecânica de força da tacada
bool g_EspacoPressionado = false;
double g_InicioEspaco = 0.0;
float g_ForcaTacada = 0.0f;

//Posição da luz pontual
glm::vec3 g_PosLuz(0.0f, 10.0f, 0.0f);

// Variável que define o Field of View (FOV)
float field_of_view = M_PI / 3.0f;

// Variável que controla se a camera está rotacionando (false = segue a bola, true = rotaciona com o mouse)
bool rotacao_camera = false;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

// Número de texturas carregadas pela função LoadTextureImage()
GLuint g_NumLoadedTextures = 0;

//Variáveis que controlam o jogo no modo Multiplayer
bool g_MultiplayerAtivo = false;
bool g_JogadorAtual = false;
        //false = jogador 1
        // true = jogador 2
bool g_BolaNoBuracoTwo = false;
bool g_TerminouJogada = false;
bool g_BolaEmFocoAtual = false; // false = câmera segue bola 1, true = câmera segue bola 2

glm::vec3 g_PosBolaTwo = glm::vec3(1.0f, 0.025f, -3.0f); //posição atual da bola de golfe do SEGUNDO jogadors
glm::vec3 g_PosTacoTwo = glm::vec3(1.0f, 0.0f, -1.0f); // direção apontada pelo taco do SEGUNDO jogador
glm::vec3 g_VelocidadeBolaTwo = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 g_BolaRotationMatrixTwo = glm::mat4(1.0f); // Rotação acumulativa da bola

// ========== LOOP PHYSICS ==========
BallLoopState g_EstadoBolaLoop = BALL_ON_GROUND;
BallLoopState g_EstadoBolaLoopTwo = BALL_ON_GROUND;
float g_LoopAngulo = 0.0f;
float g_LoopAnguloTwo = 0.0f;
float g_LoopVelAngular = 0.0f;
float g_LoopVelAngularTwo = 0.0f;
float g_BolaFreeFallVelY = 0.0f;
float g_BolaFreeFallVelYTwo = 0.0f;

// Posição do buraco (atualizada por nível)
glm::vec3 g_HolePosition = glm::vec3(0.0f, 0.0f, 4.0f);

// Triângulos da pista para raycasting de heightmap
std::vector<TrackTriangle> g_PistaCurvaTriangles;
std::vector<TrackTriangle> g_PistaLoopTriangles;

// ========== SISTEMA DE MENU ==========
GameState g_CurrentState = MENU_MAIN;
float g_MenuCameraAngle = 0.0f;
float g_MasterVolume = 0.8f;
int g_TexturaPistaGrama = 0;  // 0=rocky, 1=brick, 2=solid green
int g_TexturaPistaParede = 0; // 0=rocky, 1=brick, 2=solid gray
int g_TexturaBola = 0;        // 0=white, 1=brick, 2=rocky
int g_TexturaTaco = 0;        // 0=metal, 1=textured, 2=brick
GLuint g_HudShaderProgram = 0;
GLuint g_HudVAO = 0;
GLuint g_HudVBO = 0;
GLuint g_LogoTextureID = 0;
int g_LogoWidth = 1, g_LogoHeight = 1;

// Hover states para botões do menu
bool g_HoverJogar = false, g_HoverMultiplayer = false;
bool g_HoverConfig = false, g_HoverSair = false;
bool g_HoverVoltar = false;
bool g_HoverGramaL = false, g_HoverGramaR = false;
bool g_HoverParedeL = false, g_HoverParedeR = false;
bool g_HoverBolaL = false, g_HoverBolaR = false;
bool g_HoverTacoL = false, g_HoverTacoR = false;

// Posição do cursor 
double g_LastCursorPosX, g_LastCursorPosY;

//sistema de trilha da bola
std::vector<TrailSegment> g_TrailSegmentsBola;
std::vector<TrailSegment> g_TrailSegmentsBola2;
glm::vec3 g_TrailColor = glm::vec3(0.3f, 0.7f, 1.0f);    // R - G - B
float g_Trailopacidade = 0.6f;                              // 60% de opacidade
float g_TrailSegmentLength = 0.05f;                       // Distância mínima para novo segmento (5cm)
float g_TrailtempoMax = 1.5f;                               // Duração máxima da trilha (1.5 segundos)
float g_TrailThickness = 0.008f;                          // Espessura do raio (8mm)
glm::vec3 g_LastTrailPosBola = glm::vec3(0.0f);           // Última posição registrada da trilha
glm::vec3 g_LastTrailPosBola2 = glm::vec3(0.0f);          // Última posição registrada da trilha (bola 2)
