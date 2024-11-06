#include "window.hpp"

void Window::onCreate() {
  auto const &windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void Window::onPaint() {
  // Set the clear color
  abcg::glClearColor(m_clearColor.at(0), m_clearColor.at(1), m_clearColor.at(2),
                     m_clearColor.at(3));
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::showTutorialWindow() {
  ImVec2 windowSize(300, 100);

  auto const &windowSettings{getWindowSettings()};
  float screenWidth = static_cast<float>(windowSettings.width);
  float screenHeight = static_cast<float>(windowSettings.height);

  ImVec2 windowPos((screenWidth - windowSize.x) / 2.0f,
                   (screenHeight - windowSize.y) / 2.0f);

  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

  // MENU
  ImGui::SetNextWindowSize(windowSize);
  auto flags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse};
  ImGui::Begin("Tutorial", &m_showTutorialWindow, flags);
  {
    ImGui::Text("Bem-vindo ao Planeta X");
    ImGui::Text("[Escrever aqui o tutorial]");
  }

  ImGui::End();
}

void Window::onPaintUI() {
  // Parent class will show fullscreen button and FPS meter
  // abcg::OpenGLWindow::onPaintUI();
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    auto const flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse};
    ImGui::Begin("Planeta X", nullptr, flags);

    // Menu
    {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Ajuda")) {
          if (ImGui::MenuItem("Tutorial")) {
            m_showTutorialWindow = true;
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      if (m_showTutorialWindow) {
        showTutorialWindow();
      }
    }

    ImGui::End();
  }
}