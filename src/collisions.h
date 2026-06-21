#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <glm/glm.hpp>
#include <vector>

// Estrutura utilizada para os testes de colisão com a pista
struct TrackTriangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

// FONTE: Código de teste de intersecção ponto-triângulo inspirado em algoritmos clássicos de detecção de colisão (Real-Time Collision Detection, Christer Ericson)
glm::vec3 ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, glm::vec3& out_normal);

// FONTE: Implementação do algoritmo de Möller–Trumbore para intersecção Raio-Triângulo (Fast, Minimum Storage Ray/Triangle Intersection, 1997)
float RaycastTrackHeight(const std::vector<TrackTriangle>& tris, float x, float current_y, float z, float fallbackY);

#endif // COLLISIONS_H
