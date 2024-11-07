#include "window.hpp"
#include "imgui.h"

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
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // ImGui::SetNextWindowFocus();

    auto const flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus};
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

    // Subjanela de configurações de parametros
    {
      ImGui::SetCursorPos(ImVec2(5, 50));
      ImGui::BeginChild("Propriedades", ImVec2(280, 180), false, 
                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

      ImGui::Dummy(ImVec2(0.0f, 5.0f));
      ImGui::Indent(15.0f);

      // Slider para ajustar a gravidade
      ImGui::Text("Gravidade do Planeta");
      static float g{};
      ImGui::SliderFloat("m/s2", &g, 0.0f, 200.0f);

      ImGui::Spacing();

      ImGui::Text("Massa do Astronauta");
      static float m{};
      ImGui::SliderFloat("kg", &m, 0.0f, 200.0f);

      ImGui::Spacing();

      ImGui::Text("Força do jato");
      static float jato{};
      ImGui::SliderFloat("kg", &jato, 0.0f, 200.0f);

      ImGui::Unindent(15.0f);

      ImGui::EndChild();
    }

    // {
    //   ImGui::SetNextWindowPos(ImVec2(5, 50), ImGuiCond_Always);
    //   ImGui::SetNextWindowSize(ImVec2(300, 100));
    //   auto flags2 {
    //     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    //     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove};
    //   ImGui::Begin("Propriedades" ,nullptr, flags2);

    //   // Static text
    //   ImGui::Text("Gravidade");

    //   // Slider from 0.0f to 1.0f
    //   static float f{};
    //   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    //   ImGui::End();
    // }

    ImGui::End();
  }
}