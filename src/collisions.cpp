#include "collisions.h"
#include <cmath>
#include <algorithm>

// FONTE: Código de teste de intersecção ponto-triângulo inspirado em algoritmos clássicos de detecção de colisão (Real-Time Collision Detection, Christer Ericson)
glm::vec3 ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, glm::vec3& out_normal) {
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

// FONTE: Implementação do algoritmo de Möller–Trumbore para intersecção Raio-Triângulo (Fast, Minimum Storage Ray/Triangle Intersection, 1997)
float RaycastTrackHeight(const std::vector<TrackTriangle>& tris,
                         float x, float current_y, float z, float fallbackY)
{
    glm::vec3 orig(x, current_y + 0.1f, z);
    glm::vec3 dir(0.0f, -1.0f, 0.0f);
    float best_y = fallbackY;
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
        if (std::fabs(a) < 1e-7f) continue; // Paralelo

        float f = 1.0f / a;
        glm::vec3 s = orig - tri.v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) continue;

        glm::vec3 q = glm::cross(s, e1);
        float v = f * glm::dot(dir, q);
        if (v < 0.0f || u + v > 1.0f) continue;

        float t = f * glm::dot(e2, q);
        if (t > 1e-5f) { // O raio cruzou o triângulo!
            float hit_y = orig.y + t * dir.y;
            if (hit_y > best_y) {
                best_y = hit_y;
                hit = true;
            }
        }
    }

    if (!hit) return fallbackY;
    return best_y;
}
