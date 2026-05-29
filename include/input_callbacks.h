/**
 * @file input_callbacks.h
 * @brief Protótipos dos callbacks GLFW para interação do usuário
 *        (teclado, mouse, scroll, redimensionamento de janela).
 */
#ifndef INPUT_CALLBACKS_H
#define INPUT_CALLBACKS_H

#include "globals.h"

/** @brief Callback de redimensionamento do framebuffer (atualiza viewport e g_ScreenRatio). */
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

/** @brief Callback de erros da GLFW — imprime no stderr. */
void ErrorCallback(int error, const char* description);

/**
 * @brief Callback de teclas do teclado.
 *
 * Processa: ESC (menu/fechar), WASD (zoom/mira), Space (força da tacada),
 * XYZ (rotação de Euler), P/O (projeção), H (toggle HUD), R (reload shaders),
 * Backspace (reset), J (multiplayer).
 */
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

/** @brief Callback de clique dos botões do mouse (esquerdo, direito, meio). */
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

/** @brief Callback de movimentação do cursor (rotação da câmera com mouse). */
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

/** @brief Callback da rodinha do mouse (zoom desabilitado intencionalmente). */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

#endif // INPUT_CALLBACKS_H
