# 🏌️ GOLFinho — Mapa da Arquitetura do Código

> **Disciplina:** INF01047 — Computação Gráfica e Visualização I (UFRGS)
> **Autores:** Gabriel & Leonardo
> **Stack:** C++11 · OpenGL 3.3 (Core Profile) · GLFW · GLM · tinyobjloader · stb_image

---

## Visão Geral

O projeto é um **mini-golf 3D** com suporte a multiplayer local, sistema de menu interativo, câmera estilo 8-Ball Pool, e iluminação Toon/Cel Shading. A estrutura segue o padrão clássico de um projeto acadêmico de CG, com um `main.cpp` monolítico apoiado por módulos auxiliares.

```
cgvis-trabalho-final/
├── src/                    ← Código-fonte (detalhado abaixo)
├── include/                ← Headers do projeto + bibliotecas externas
├── data/                   ← Modelos .obj, texturas .jpg/.png
├── bin/                    ← Executável gerado (bin/Linux/main)
├── build/                  ← Build intermediário (CMake)
├── lib-linux/              ← libglfw3.a pré-compilada para Linux
├── CMakeLists.txt          ← Configuração do build
├── Makefile                ← Wrapper para cmake + make
└── COMPILACAO.md           ← Instruções de compilação
```

---

## Arquivos em `src/` — Guia Arquivo por Arquivo

### 📄 `main.cpp` (~2.734 linhas)

O coração do projeto. Concentra a maior parte da lógica: inicialização, game loop, renderização, física, callbacks e sistema de menu.

| Trecho (linhas aprox.) | Responsabilidade |
|---|---|
| `1–102` | Includes, struct `ObjModel` (carrega `.obj` via tinyobjloader), forward declarations |
| `103–320` | Structs (`SceneObject`), variáveis globais (câmera, bola, taco, multiplayer, menu) |
| `321–971` | **`main()`** — inicialização GLFW/OpenGL, carregamento de modelos e texturas, game loop |
| `457–470` | Desvio do loop: se `g_CurrentState != PLAYING`, renderiza o menu e pula o gameplay |
| `493–573` | **Física da bola** — Euler explícito, atrito linear, colisão AABB com paredes, detecção de buraco |
| `576–935` | **Renderização da cena** — chão, paredes, pista, trajetória de mira, buraco, bandeira, taco, bola, HUD de força |
| `974–1024` | `LoadTextureImage()` — carrega imagem do disco para a GPU com mipmaps (sRGB) |
| `1026–1057` | `DrawVirtualObject()` — busca o objeto em `g_VirtualScene`, seta AABB uniforms, chama `glDrawElements` |
| `1059–1109` | `LoadShadersFromFiles()` — compila vertex/fragment shaders e obtém uniform locations |
| `1111–1129` | `PushMatrix()` / `PopMatrix()` — pilha de matrizes de modelagem |
| `1131–1245` | `ComputeNormals()` — calcula normais via método de Gouraud (média por smoothing group) |
| `1247–1387` | `BuildTrianglesAndAddToVirtualScene()` — transforma `ObjModel` em VAO/VBOs, registra em `g_VirtualScene` |
| `1389–1471` | **`CalcularTaco()`** — monta a matriz Model do taco (Translate × RotateY × RotateZ × Scale) |
| `1389–1471` | **`RotacionarTaco()`** — animação de pêndulo (sobe → bate) e aplica velocidade à bola no fim |
| `1473–1615` | Funções de shader: `LoadShader_Vertex/Fragment()`, `LoadShader()`, `CreateGpuProgram()` |
| `1617–1780` | **Callbacks GLFW** — `FramebufferSizeCallback`, `MouseButtonCallback`, `CursorPosCallback`, `ScrollCallback` |
| `1782–1884` | **`KeyCallback()`** — ESC (menu), Space (força da tacada), WASD (zoom/mira), R (reload shaders), H (toggle HUD) |
| `1892–2207` | Funções de texto on-screen (MVP debug, FPS, ângulos de Euler, mensagem de vitória) + `PrintObjModelInfo()` |
| `2209–2734` | **Sistema de Menu** — `MenuInit`, `MenuUpdate`, `MenuRenderOverlay`, `MenuRenderMainMenu`, `MenuRenderLevelSelect`, `MenuRenderSettings`, `MenuHandleClick`, HUD shader inline, botões com gradiente e hover |

#### Fluxo do Game Loop (dentro de `main()`)

```
┌─────────────────────────────────────────┐
│              glfwPollEvents()            │
│                    │                     │
│     ┌──── Estado != PLAYING? ────┐       │
│     │ SIM                    NÃO │       │
│     ▼                            ▼       │
│  MenuUpdate()               Física       │
│  SwapBuffers()              Câmera       │
│  continue                   Renderização │
│                             HUD/Texto    │
│                             SwapBuffers  │
└─────────────────────────────────────────┘
```

---

### 📄 `globals.cpp` (130 linhas)

**Instanciação** de todas as variáveis globais declaradas como `extern` em `globals.h`. Cada variável é definida aqui com seu valor inicial. Organizado por categorias:

- **Cena e Matrizes:** `g_VirtualScene`, `g_MatrixStack`
- **Câmera:** `g_CameraTheta`, `g_CameraPhi`, `g_CameraDistance`, `view`
- **Bola J1:** `g_PosBola`, `g_VelocidadeBola`, `g_BolaNoBuraco`, `g_BolaRotationMatrix`
- **Taco:** `g_TacoRotacao`, `g_TacoRotacaoVertical`, `g_TempoRotacaoTaco`
- **Força:** `g_EspacoPressionado`, `g_ForcaTacada`
- **Multiplayer J2:** `g_PosBolaTwo`, `g_VelocidadeBolaTwo`, etc.
- **Menu:** `g_CurrentState`, `g_TexturaPistaGrama/Parede`, hover flags
- **GPU:** `g_GpuProgramID`, uniform locations, `g_NumLoadedTextures`

---

### 📄 `buildtriangles.cpp` (162 linhas)

Constrói manualmente dois triângulos (um quad) usando VBOs/VAO para fins didáticos. Define:
- Posições (NDC) de 4 vértices
- Cores RGBA por vértice
- Índices de topologia (`GLubyte`)

> **Nota:** Este arquivo é do código-base da disciplina. A função `BuildTriangles()` não é usada diretamente no jogo — a versão usada em runtime é `BuildTrianglesAndAddToVirtualScene()` em `main.cpp`, que processa modelos `.obj`.

---

### 📄 `textrendering.cpp` (310 linhas)

Sistema de renderização de texto 2D na janela OpenGL, baseado em um font atlas bitmap (DejaVu Sans Mono, embutido em `dejavufont.h`).

| Função | O que faz |
|---|---|
| `TextRendering_Init()` | Compila shaders inline de texto, cria textura de fonte, aloca VAO/VBO |
| `TextRendering_PrintString()` | Renderiza uma string glyph por glyph, com blending alpha |
| `TextRendering_LineHeight/CharWidth()` | Métricas de layout proporcional ao tamanho da janela |
| `TextRendering_PrintMatrix/Vector()` | Imprime matrizes e vetores formatados (uso em debug) |
| `TextRendering_PrintMatrixVectorProduct*()` | Mostra o pipeline Model→View→Projection visualmente |

> **Detalhe técnico:** Usa a texture unit 31 para evitar conflito com as texturas de cena (units 0–3).

---

### 📄 `shader_vertex.glsl` (69 linhas)

Vertex shader GLSL 3.30 Core. Pipeline:

```
model_coefficients (local)
        │
        ▼  ×model
position_world (global)
        │
        ▼  ×view
        │
        ▼  ×projection
gl_Position (NDC → clip space)
```

**Outputs interpolados** para o Fragment Shader:
- `position_world` — posição em coordenadas globais
- `position_model` — posição em coordenadas locais (usada para projeção de textura)
- `normal` — normal transformada via `inverse(transpose(model))`
- `texcoords` — coordenadas UV do `.obj`

---

### 📄 `shader_fragment.glsl` (305 linhas)

Fragment shader com iluminação **Toon/Cel Shading** + seletor de texturas por `object_id`.

| Bloco | Descrição |
|---|---|
| **Object IDs (0–14)** | `#define`s que mapeiam cada tipo de objeto a um ID numérico |
| **Mapeamento de Textura** | Esférica (`SPHERE`, `BOLA`), planar XY (`BUNNY`), UV do OBJ (`PLANE`, `TACO`, `PISTA_*`) |
| **Seletor de Texturas** | Uniforms `u_TexturaGramaPista`, `u_TexturaBola`, etc. — permitem trocar texturas pelo menu de configurações |
| **Iluminação** | Lambert discretizado em 4 steps (toon) + Blinn-Phong especular com corte binário (0 ou 1) |
| **Atenuação** | Modelo quadrático: `1 / (c + l·d + q·d²)` com a luz pontual em `g_PosLuz` |
| **HUD_BARRA** | Caso especial — sem iluminação, cor interpolada verde→vermelho pela força da tacada |
| **Gamma Correction** | `pow(color, 1/2.2)` no final (monitor sRGB) |

---

### 📄 `correcao.cpp` (12 linhas)

Callback obrigatório para **correção automatizada** dos laboratórios. Intercepta `Shift+0..9` e chama `std::exit(100+i)`. Deve ser o primeiro comando dentro de `KeyCallback()`.

> ⚠️ **Não modifique este arquivo** — é requisito da disciplina.

---

### 📄 `stb_image.cpp` (3 linhas)

Unidade de compilação que instancia a implementação da biblioteca [stb_image](https://github.com/nothings/stb):

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

> O `#define` antes do include faz com que o código da biblioteca (header-only) seja compilado nesta translation unit.

---

### 📄 `tiny_obj_loader.cpp` (3 linhas)

Análogo ao anterior — instancia a implementação de [tinyobjloader](https://github.com/syoyo/tinyobjloader):

```cpp
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
```

---

### 📄 `glad.c` (~93.554 bytes)

Código gerado automaticamente pelo [GLAD](https://glad.dav1d.de/). Carrega os ponteiros de função do OpenGL 3.3 Core Profile em runtime. Chamado via `gladLoadGLLoader()` no `main()`.

> **Não edite manualmente.** Se precisar mudar a versão do GL, regenere pelo site.

---

## Arquivos em `include/` — Headers do Projeto

| Header | Papel |
|---|---|
| `globals.h` | Declarações centrais: structs (`ObjModel`, `SceneObject`), enum `GameState`, `#define` dos object IDs, e `extern` de todas as variáveis globais |
| `matrices.h` | Funções matemáticas puras: matrizes de transformação (Translate, Scale, Rotate X/Y/Z, Rodrigues), projeção (Perspective, Orthographic), câmera (Camera View), norma, produto vetorial/escalar |
| `utils.h` | Macro `glCheckError()` — wrapper para `glGetError()` que imprime erros com arquivo e linha |
| `gpu_utils.h` | Protótipos de funções de GPU: shaders, texturas, VAO/VBO, `DrawVirtualObject` |
| `gameplay.h` | Protótipos de `CalcularTaco()` e `RotacionarTaco()` — lógica de posicionamento e animação do taco |
| `input_callbacks.h` | Protótipos dos callbacks GLFW: teclado, mouse, scroll, resize |
| `menu.h` | Protótipos do sistema de menu: init, update, render, click handling, HUD primitives |
| `dejavufont.h` | Font atlas bitmap (DejaVu Sans Mono) embutido como array estático — usado por `textrendering.cpp` |
| `stb_image.h` | Biblioteca header-only para decodificação de imagens (JPG, PNG, etc.) |
| `tiny_obj_loader.h` | Biblioteca header-only para parsing de arquivos Wavefront `.obj` |

---

## Conceitos-Chave de CG Usados no Projeto

| Conceito | Onde aparece |
|---|---|
| **Pipeline de Renderização** | `shader_vertex.glsl` (transformações), `shader_fragment.glsl` (shading) |
| **Coordenadas Homogêneas** | `matrices.h` — todas as operações usam `vec4`/`mat4` |
| **Projeção Perspectiva/Ortográfica** | `matrices.h` (`Matrix_Perspective`, `Matrix_Orthographic`) |
| **Camera Look-At** | `matrices.h` (`Matrix_Camera_View`) — constrói a base ortonormal {u,v,w} |
| **Mapeamento de Texturas** | `shader_fragment.glsl` — esférico, planar, UV do modelo |
| **Iluminação de Lambert + Blinn-Phong** | `shader_fragment.glsl` — com discretização para Toon Shading |
| **Normais de Gouraud** | `main.cpp` (`ComputeNormals`) — média ponderada por smoothing group |
| **AABB Collision** | `main.cpp` — colisão da bola com as paredes da pista |
| **Curvas Paramétricas** | `RotacionarTaco()` — interpolação sin/cos para animação de pêndulo |
| **Double Buffering** | `glfwSwapBuffers` no game loop |
| **Gamma Correction** | `shader_fragment.glsl` — `pow(color, 1/2.2)` |

---

## Build Rápido

```bash
# Na raiz do projeto
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Executar
cd ../bin/Linux && ./main
```

Ou simplesmente:

```bash
make          # usa o Makefile wrapper
make run      # compila e executa
```

---

## Controles no Jogo

| Tecla | Ação |
|---|---|
| `A` / `D` | Mirar o taco (rotação horizontal) |
| `W` / `S` | Zoom in / Zoom out |
| `Space` (segurar) | Carregar força da tacada |
| `Space` (soltar) | Executar tacada |
| Mouse Esquerdo | Rotacionar câmera livre |
| Mouse Direito | Resetar câmera para POV do golfista |
| `J` | Ativar modo multiplayer |
| `H` | Toggle texto de debug |
| `R` | Recarregar shaders (hot reload) |
| `P` / `O` | Projeção perspectiva / ortográfica |
| `ESC` | Voltar ao menu / Sair |
| `Backspace` | Resetar ângulos e posição do taco |
