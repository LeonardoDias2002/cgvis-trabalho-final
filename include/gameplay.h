/**
 * @file gameplay.h
 * @brief Protótipos das funções de gameplay: posicionamento do taco
 *        e animação da tacada.
 */
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "globals.h"

/**
 * @brief Calcula a matriz de modelagem (posição + orientação) do taco de golfe.
 *
 * O taco é posicionado atrás da bola, a uma distância `distancia_offset`,
 * rotacionado pelo ângulo g_TacoRotacao (mira horizontal) e
 * g_TacoRotacaoVertical (swing/pêndulo).
 *
 * A composição de matrizes segue a ordem:
 *   Translate(posição) × RotateY(mira) × RotateZ(swing) × Translate(pivô) × RotateY(90°) × Scale
 *
 * @note É obrigatório usar mat4 (4×4) pois é a única capaz de combinar
 *       translação, rotação e escala numa única transformação afim.
 *
 * @param posicao_bola    Posição da bola do jogador atual
 * @param direcao_taco    Direção em que o taco aponta (não utilizado diretamente — mira vem de g_TacoRotacao)
 * @param distancia_offset Distância entre a bola e o taco
 * @return Matriz 4×4 de modelagem para o taco
 */
glm::mat4 CalcularTaco(glm::vec3 posicao_bola, glm::vec3 direcao_taco = glm::vec3(0.0f, 0.0f, -1.0f), float distancia_offset = 0.2f);

/**
 * @brief Atualiza a animação do taco (swing de pêndulo).
 *
 * A animação é dividida em duas fases:
 *   - t ∈ [0, 0.5]: puxa o taco para trás (sin crescente)
 *   - t ∈ [0.5, 1]: desce batendo na bola (cos decrescente)
 *
 * Quando a animação termina, aplica a velocidade à bola na direção
 * oposta ao taco, com magnitude proporcional a g_ForcaTacada.
 *
 * @param window Ponteiro para a janela GLFW (usado para obter o tempo)
 */
void RotacionarTaco(GLFWwindow* window);

#endif // GAMEPLAY_H
