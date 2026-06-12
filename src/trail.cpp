//funções que criam uma trilha colorida que segue a bola
#include "trail.h"
#include "matrices.h"
#include <glm/ext.hpp>


// Renderiza um cilindro entre dois pontos.
// p1 =  Ponto inicial
// p2 = Ponto final
//  raio = Raio do cilindro

static glm::mat4 CreateCylinderMatrix(glm::vec3 p1, glm::vec3 p2, float raio)
{
    glm::vec3 delta = p2 - p1;
    float length = glm::length(delta);
    
    if (length < 0.0001f) return glm::mat4(1.0f);
    
    glm::vec3 direction = glm::normalize(delta);
    glm::vec3 center = (p1 + p2) * 0.5f;
    
    // Calcula a orientação do cilindro
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.5f);
    if (glm::abs(glm::dot(direction, up)) > 0.99f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    up = glm::normalize(glm::cross(direction, right));
    
    glm::mat4 model = glm::mat4(1.0f);
    model[0] = glm::vec4(right * raio, 0.0f);
    model[1] = glm::vec4(direction * length * 0.5f, 0.0f);
    model[2] = glm::vec4(up * raio, 0.0f);
    model[3] = glm::vec4(center, 1.0f);
    
    return model;
}

//Atualiza a trilha da bola 1.
bool UpdateTrail(glm::vec3 current_pos, glm::vec3 &last_pos, float delta_time)
{
    float distance = glm::length(current_pos - last_pos);
    
    if (distance >= g_TrailSegmentLength) {
        TrailSegment segment;
        segment.posStart = last_pos;
        segment.posEnd = current_pos;
        segment.opacidade = g_Trailopacidade;
        segment.tempoMax = g_TrailtempoMax;
        segment.tempoAtual = 0.0f;
        
        g_TrailSegmentsBola.push_back(segment);
        last_pos = current_pos;
        return true;
    }
    
    return false;
}

// Mesma lógica para a bola 2
bool UpdateTrail2(glm::vec3 current_pos, glm::vec3 &last_pos, float delta_time)
{
    float distance = glm::length(current_pos - last_pos);
    
    if (distance >= g_TrailSegmentLength) {
        TrailSegment segment;
        segment.posStart = last_pos;
        segment.posEnd = current_pos;
        segment.opacidade = g_Trailopacidade;
        segment.tempoMax = g_TrailtempoMax;
        segment.tempoAtual = 0.0f;
        
        g_TrailSegmentsBola2.push_back(segment);
        last_pos = current_pos;
        return true;
    }
    
    return false;
}
// Atualiza a idade de todos os segmentos.

void UpdateTrailSegments(float delta_time)
{
    // Atualizar trilha da bola 1
    for (auto it = g_TrailSegmentsBola.begin(); it != g_TrailSegmentsBola.end(); ) {
        it->tempoAtual += delta_time;
        float progress = it->tempoAtual / it->tempoMax;
        it->opacidade = g_Trailopacidade * (1.0f - progress);  // Desaparece gradualmente
        
        if (it->tempoAtual >= it->tempoMax) {
            it = g_TrailSegmentsBola.erase(it);
        } else {
            ++it;
        }
    }
    
    // Atualizar trilha da bola 2
    for (auto it = g_TrailSegmentsBola2.begin(); it != g_TrailSegmentsBola2.end(); ) {
        it->tempoAtual += delta_time;
        float progress = it->tempoAtual / it->tempoMax;
        it->opacidade = g_Trailopacidade * (1.0f - progress);  // Desaparece gradualmente
        
        if (it->tempoAtual >= it->tempoMax) {
            it = g_TrailSegmentsBola2.erase(it);
        } else {
            ++it;
        }
    }
}


// Renderiza a trilha da bola 1.
void RenderTrail()
{
    if (g_TrailSegmentsBola.empty()) return;
    glUseProgram(g_GpuProgramID);
    
    for (const auto& segment : g_TrailSegmentsBola) {
        glm::mat4 model = CreateCylinderMatrix(segment.posStart, segment.posEnd, g_TrailThickness);
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, TRAJETORIA);
        
        // Passa a cor e opacidade para o shader
        GLint colorLoc = glGetUniformLocation(g_GpuProgramID, "u_TrailColor");
        GLint opacidadeLoc = glGetUniformLocation(g_GpuProgramID, "u_Trailopacidade");
        
        if (colorLoc != -1) {
            glUniform3f(colorLoc, g_TrailColor.r, g_TrailColor.g, g_TrailColor.b);
        }
        if (opacidadeLoc != -1) {
            glUniform1f(opacidadeLoc, segment.opacidade);
        }
        
        DrawVirtualObject("the_sphere");
    }
}

// Renderiza a trilha da bola 2.
void RenderTrail2()
{
    if (g_TrailSegmentsBola2.empty()) return;
    
    glUseProgram(g_GpuProgramID);
    
    for (const auto& segment : g_TrailSegmentsBola2) {
        glm::mat4 model = CreateCylinderMatrix(segment.posStart, segment.posEnd, g_TrailThickness);
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, TRAJETORIA);
        
        // Passa a cor e opacidade para o shader
        GLint colorLoc = glGetUniformLocation(g_GpuProgramID, "u_TrailColor");
        GLint opacidadeLoc = glGetUniformLocation(g_GpuProgramID, "u_Trailopacidade");
        
        if (colorLoc != -1) {
            glUniform3f(colorLoc, g_TrailColor.r, g_TrailColor.g, g_TrailColor.b);
        }
        if (opacidadeLoc != -1) {
            glUniform1f(opacidadeLoc, segment.opacidade);
        }
        
        DrawVirtualObject("the_sphere");
    }
}


//Limpa toda a trilha.
void ClearTrail()
{
    g_TrailSegmentsBola.clear();
}

//limpa a trilha da bola 2.
void ClearTrail2()
{
    g_TrailSegmentsBola2.clear();
}

// Define a cor da trilha.
void SetTrailColor(float r, float g, float b)
{
    g_TrailColor = glm::vec3(r, g, b);
}

// Define a opacidade da trilha.
void SetTrailopacidade(float opacidade)
{
    g_Trailopacidade = glm::clamp(opacidade, 0.0f, 0.8f);
}
