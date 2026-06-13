/**
 * @file trail.h
 * @brief Sistema de trilha que segue a bolinha de golfe durante o movimento.
 * 
 * A trilha é composta por vários pequenos segmentos que são renderizados
 * como cilindros conectando posições consecutivas da bola. 
 * Suporta configuração de cor e opacidade.
 */
#ifndef TRAIL_H
#define TRAIL_H

#include "globals.h"


// =============================================
// VARIÁVEIS GLOBAIS
// =============================================

// Lista de segmentos de trilha para a bola 1
extern std::vector<TrailSegment> g_TrailSegmentsBola;

// Lista de segmentos de trilha para a bola 2
extern std::vector<TrailSegment> g_TrailSegmentsBola2;

// Cores da trilha (RGBA)
extern glm::vec3 g_TrailColor;           ///< Cor RGB da trilha (padrão: azul claro)
extern float g_Trailopacidade;             ///< Opacidade da trilha (0.0 a 1.0)

// Configurações da trilha
extern float g_TrailSegmentLength;       ///< Distância mínima para criar um novo segmento
extern float g_TrailtempoMax;              ///< Tempo máximo de vida de um segmento em segundos
extern float g_TrailThickness;           ///< Espessura do opacidade da trilha

// =============================================
// FUNÇÕES
// =============================================

/**
 * @brief Atualiza a trilha da bola 1, adicionando novo segmento se necessário.
 * @param current_pos Posição atual da bola
 * @param last_pos Última posição registrada na trilha
 * @param delta_time Tempo decorrido desde o último frame
 * @return Retorna true se um novo segmento foi adicionado
 */
bool UpdateTrail(glm::vec3 current_pos, glm::vec3 &last_pos, float delta_time);

/**
 * @brief Atualiza a trilha da bola 2, adicionando novo segmento se necessário.
 * @param current_pos Posição atual da bola
 * @param last_pos Última posição registrada na trilha
 * @param delta_time Tempo decorrido desde o último frame
 * @return Retorna true se um novo segmento foi adicionado
 */
bool UpdateTrail2(glm::vec3 current_pos, glm::vec3 &last_pos, float delta_time);

/**
 * @brief Atualiza a idade de todos os segmentos e remove os expirados.
 * @param delta_time Tempo decorrido desde o último frame
 */
void UpdateTrailSegments(float delta_time);

/**
 * @brief Renderiza todos os segmentos da trilha da bola 1.
 */
void RenderTrail();

/**
 * @brief Renderiza todos os segmentos da trilha da bola 2.
 */
void RenderTrail2();

/**
 * @brief Limpa toda a trilha.
 */
void ClearTrail();

/**
 * @brief Limpa a trilha da bola 2.
 */
void ClearTrail2();

/**
 * @brief Define a cor da trilha.
 * @param r Componente vermelho (0.0 a 1.0)
 * @param g Componente verde (0.0 a 1.0)
 * @param b Componente azul (0.0 a 1.0)
 */
void SetTrailColor(float r, float g, float b);

/**
 * @brief Define a opacidade da trilha.
 * @param opacidade Opacidade (0.0 a 1.0)
 */
void SetTrailopacidade(float opacidade);

#endif // TRAIL_H
