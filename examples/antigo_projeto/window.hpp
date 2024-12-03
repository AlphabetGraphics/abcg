#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include <array>
#include <glm/vec2.hpp>

class Window : public abcg::OpenGLWindow {
public:
  void onCreate() override;
  void onPaint() override;
  void onDestroy() override;
  void onResize(glm::ivec2 const &size) override;

private:
  void setupModel();

  GLuint m_program{0};        // ID do programa de shader
  GLuint m_VBO{0};            // ID do VBO (para armazenar os vértices)
  GLuint m_VAO{0};            // ID do VAO (para configurar os atributos dos vértices)

  // Armazenando as coordenadas dos vértices para os dois retângulos (plataformas)
  std::array<GLfloat, 16> vertices; // 4 vértices com 2 coordenadas cada

  glm::ivec2 m_viewportSize{};  // Tamanho da janela
};

#endif
