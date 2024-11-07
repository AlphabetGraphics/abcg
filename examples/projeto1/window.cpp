#include "window.hpp"
#include <glm/vec2.hpp>

void Window::onCreate() {
  // Shaders para o programa
  auto const *vertexShader{R"gl(#version 300 es
    layout(location = 0) in vec2 inPosition;
    void main() {
      gl_Position = vec4(inPosition, 0, 1);
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
    precision mediump float;
    out vec4 outColor;
    void main() { outColor = vec4(1.0, 1.0, 1.0, 1.0); }
  )gl"};

  // Cria o programa de shader
  m_program = abcg::createOpenGLProgram({
      {.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
      {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}
  });

  // Configuração inicial do tamanho das plataformas
  vertices = {
    // Plataforma esquerda (parte inferior)
    -0.8f, -0.8f, // Ponto inferior esquerdo
    -0.4f, -0.8f, // Ponto inferior direito
    -0.4f, -0.7f, // Ponto superior direito
    -0.8f, -0.7f, // Ponto superior esquerdo
    
    // Plataforma direita (parte superior)
     0.4f, -0.3f, // Ponto inferior esquerdo
     0.8f, -0.3f, // Ponto inferior direito
     0.8f, -0.2f, // Ponto superior direito
     0.4f, -0.2f  // Ponto superior esquerdo
  };

  // Configura o VBO e o VAO
  setupModel();
}

void Window::onPaint() {
  // Limpa a tela com uma cor de fundo
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Configura o viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Usa o programa de shader
  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // Desenha os dois retângulos (plataformas)
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Desenha a primeira plataforma
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 4, 4);  // Desenha a segunda plataforma

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
}

void Window::onDestroy() {
  // Libera o programa de shader, VBO e VAO
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::setupModel() {
  // Libera os VBO e VAO anteriores, se houver
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Gera e configura o VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                     vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Obtenção do atributo de posição no programa de shader
  auto const positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Gera e configura o VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindVertexArray(0);
}
