/**
 * @file menu.h
 * @brief Protótipos do sistema de menu e HUD do jogo GOLFinho.
 *
 * O menu usa um shader próprio (HUD shader) para renderizar quads 2D
 * com cantos arredondados, gradientes e hover effects. O texto é
 * renderizado usando o sistema de textrendering.cpp existente.
 */
#ifndef MENU_H
#define MENU_H

#include "globals.h"

/** @brief Inicializa o sistema de menu: compila HUD shader, carrega logo, cria VAO/VBO. */
void MenuInit();

/**
 * @brief Atualiza e renderiza o menu a cada frame.
 *
 * Anima a câmera panorâmica, renderiza a cena 3D de fundo,
 * e sobrepõe o overlay do menu atual (main, levels ou settings).
 */
void MenuUpdate(GLFWwindow* window, float delta_time);

/** @brief Renderiza o overlay semi-transparente e despacha para o sub-menu correto. */
void MenuRenderOverlay(GLFWwindow* window);

/** @brief Renderiza o menu principal (logo + botões Jogar/Níveis/Config/Sair). */
void MenuRenderMainMenu(GLFWwindow* window);

/** @brief Renderiza a tela de seleção de nível (5 botões + Voltar). */
void MenuRenderLevelSelect(GLFWwindow* window);

/** @brief Renderiza a tela de configurações (volume + seletores de textura). */
void MenuRenderSettings(GLFWwindow* window);

/** @brief Processa clique do mouse nos botões do menu atual. */
void MenuHandleClick(GLFWwindow* window);

/**
 * @brief Renderiza um quad 2D centralizado em (cx, cy) com meia-largura hw e meia-altura hh.
 *
 * Os vértices são enviados ao VBO do HUD e desenhados como dois triângulos.
 * As coordenadas estão em NDC (-1 a +1).
 */
void DrawHudQuad(float cx, float cy, float hw, float hh);

/**
 * @brief Testa se o cursor do mouse está dentro de um retângulo em NDC.
 * @return true se o cursor está sobre o retângulo.
 */
bool IsMouseOverRect(GLFWwindow* w, float cx, float cy, float hw, float hh);

/**
 * @brief Renderiza um botão com gradiente, hover effect e texto centralizado.
 * @return true se o mouse está sobre o botão (hovered).
 */
bool RenderButton(GLFWwindow* window, float cx, float cy, float hw, float hh,
                  const char* label, float r1, float g1, float b1, float r2, float g2, float b2,
                  bool enabled);

/** @brief Renderiza um slider horizontal com preenchimento proporcional a `value`. */
void RenderSlider(GLFWwindow* window, float cx, float cy, float hw, float hh, float value);

/**
 * @brief Carrega uma imagem RGBA e cria uma textura OpenGL.
 * @return ID da textura criada (0 se falhar).
 */
GLuint LoadTextureImageRGBA(const char* filename, int* outW, int* outH);

/** @brief Compila os shaders inline do HUD (vertex + fragment) e retorna o program ID. */
GLuint CompileHudShaders();

#endif // MENU_H
