#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "torus.obj");
  m_model.setupVAO(m_program);

  m_model_ship.loadObj(assetsPath + "ship.obj");
  m_model_ship.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z
  glm::vec3 const eye{0.0f, 0.0f, 0.0f};
  glm::vec3 const at{0.0f, 0.0f, -1.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  // Setup stars
  for (size_t i = 0; i < m_stars.size(); ++i) {
    randomizeStar(m_stars[i], i);
  }

  float x = 0.0f;
  float y = -0.3f; // Mantém Y fixo
  float z = -2.0f; // Mantém Z fixo

  m_ship.m_position = glm::vec3(x, y, z);

  // Define um eixo de rotação aleatório
  // star.m_rotationAxis = glm::sphericalRand(1.0f);
  m_ship.m_rotationAxis = glm::vec3(0.1f, 0.0f, 0.0f);
}

void Window::randomizeStar(Star &star, int index) {
  // Define a linha no eixo X, com espaçamento fixo
  float spacing = 2.0f; // Espaçamento entre as estrelas
  float startX = -50.0f; // Posição inicial no eixo X

  // Calcula a posição baseada no índice
  float x = startX + index * spacing;
  float y = 0.0f; // Mantém Y fixo
  float z = -100.0f; // Mantém Z fixo

  star.m_position = glm::vec3(x, y, z);

  // Define um eixo de rotação aleatório
  // star.m_rotationAxis = glm::sphericalRand(1.0f);
  star.m_rotationAxis = glm::vec3(1.0, 1.0, 1.0);
}

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP: // Mover para cima
      m_ship.m_position.y += 0.1f;
      break;
    case SDLK_DOWN: // Mover para baixo
      m_ship.m_position.y -= 0.1f;
      break;
    case SDLK_LEFT: // Mover para a esquerda
      m_ship.m_position.x -= 0.1f;
      break;
    case SDLK_RIGHT: // Mover para a direita
      m_ship.m_position.x += 0.1f;
      break;
    }
  }

  // Limites para evitar que o astronauta saia da tela
  m_ship.m_position.x = std::clamp(m_ship.m_position.x, -0.6f, 0.6f);
  m_ship.m_position.y = std::clamp(m_ship.m_position.y, -0.6f, 0.6f);
}

void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  // m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);
  m_angle = glm::wrapAngle(m_angle);

  // Update stars
  for (size_t i = 0; i < m_stars.size(); ++i) {
    auto &star = m_stars[i];

    // Move estrela ao longo do eixo Z
    star.m_position.z += deltaTime * 10.0f;

    // Reposicionar estrela atrás da câmera
    if (star.m_position.z > 0.1f) {
      randomizeStar(star, i);
      star.m_position.z = -100.0f; // Volta para Z inicial
    }
  }
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White

  // Render each star
  for (auto &star : m_stars) {
    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, star.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, star.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  // Desenhar o astronauta fixo
  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, m_ship.m_position); // Parte inferior central
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f)); // Ajusta o tamanho
  modelMatrix = glm::rotate(modelMatrix, 11.0f, m_ship.m_rotationAxis);

  // Usamos uma matriz de identidade para manter o astronauta fixo
  // glm::mat4 identityMatrix{1.0f};

  // Enviar as matrizes para o shader
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  // abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &identityMatrix[0][0]);
  // abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  // abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  // abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

  // Renderizar o astronauta
  m_model_ship.render();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const widgetSize{ImVec2(218, 62)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      ImGui::PushItemWidth(120);
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"Perspective", "Orthographic"};

      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      ImGui::PushItemWidth(170);
      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

        ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
      } else {
        m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                                  20.0f, 0.01f, 100.0f);
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}