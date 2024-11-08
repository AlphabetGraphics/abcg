#include "window.hpp"

void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  m_smallFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 20.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }


  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Create program to render the stars
  m_starsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "stars.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "stars.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;

  m_starLayers.create(m_starsProgram, 25);
  m_ship.create(m_objectsProgram);
  m_asteroids.create(m_objectsProgram, 25);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  if (m_gameData.m_state != State::Win) {
    m_gameData.m_time += deltaTime;
  }

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_ship.update(m_gameData, deltaTime);
  m_starLayers.update(m_ship, deltaTime);
  m_asteroids.update(m_ship, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_starLayers.paint();
  m_asteroids.paint();
  m_ship.paint(m_gameData);
}

// void Window::onPaintUI() {
//   abcg::OpenGLWindow::onPaintUI();

//   {
//     auto const size{ImVec2(300, 85)};
//     auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
//                                (m_viewportSize.y - size.y) / 2.0f)};
//     ImGui::SetNextWindowPos(position);
//     ImGui::SetNextWindowSize(size);
//     ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
//                                  ImGuiWindowFlags_NoTitleBar |
//                                  ImGuiWindowFlags_NoInputs};
//     ImGui::Begin(" ", nullptr, flags);
//     ImGui::PushFont(m_font);

//     if (m_gameData.m_state == State::GameOver) {
//       ImGui::Text("Game Over!");
//     } else if (m_gameData.m_state == State::Win) {
//       ImGui::Text("*You Win!*");
//     }

//     ImGui::PopFont();
//     ImGui::End();
//   }
// }

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Set window to full screen
  auto const size{ImVec2(m_viewportSize.x, m_viewportSize.y)};
  auto const position{ImVec2(0.0f, 0.0f)};
  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);
  ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                               ImGuiWindowFlags_NoTitleBar |
                               ImGuiWindowFlags_NoInputs};
  ImGui::Begin(" ", nullptr, flags);

  // Center text on the screen
  ImGui::PushFont(m_font);  // Main font for the win message
  if (m_gameData.m_state == State::Win) {
    // Display the win message at the top center
    ImGui::SetCursorPosY(m_viewportSize.y / 2.0f - 50.0f); // Adjust vertical position
    ImGui::SetCursorPosX((m_viewportSize.x - ImGui::CalcTextSize("*You Win!*").x) / 2.0f);
    ImGui::Text("*You Win!*");

    // Display the score below in a different font size
    ImGui::PushFont(m_smallFont);  // Use a smaller or larger font for the score
    ImGui::SetCursorPosY(m_viewportSize.y / 2.0f);  // Position below the win message
    ImGui::SetCursorPosX((m_viewportSize.x - ImGui::CalcTextSize("Score: %.2f").x) / 2.0f);
    ImGui::Text("Time: %.2f", m_gameData.m_time);  // Print score
    ImGui::PopFont();
  }
  ImGui::PopFont();

  ImGui::End();
}


// void Window::onPaintUI() {
//   abcg::OpenGLWindow::onPaintUI();

//   auto const size{ImVec2(300, 85)};
//   auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
//                              (m_viewportSize.y - size.y) / 2.0f)};
//   ImGui::SetNextWindowPos(position);
//   ImGui::SetNextWindowSize(size);
//   ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
//                                ImGuiWindowFlags_NoTitleBar |
//                                ImGuiWindowFlags_NoInputs};
//   ImGui::Begin(" ", nullptr, flags);
//   ImGui::PushFont(m_font);

//   if (m_gameData.m_state == State::GameOver) {
//     ImGui::Text("Game Over!");
//   } else if (m_gameData.m_state == State::Win) {
//     ImGui::Text("*You Win!*");

//     // Use a different font size for the Score
//     ImGui::PushFont(m_smallFont);  // Set to smaller or larger font
//     ImGui::Text("Score: %.2f", m_gameData.m_time);  // Display score with new font size
//     ImGui::PopFont();  // Revert to previous font
//   }

//   ImGui::PopFont();
//   ImGui::End();
// }


void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_objectsProgram);

  m_asteroids.destroy();
  m_ship.destroy();
  m_starLayers.destroy();
}

void Window::checkCollisions() {
  // Check collision between ship and asteroids
  for (auto &asteroid : m_asteroids.m_asteroids) {
    auto const asteroidTranslation{asteroid.m_translation};
    auto const distance{
        glm::distance(m_ship.m_translation, asteroidTranslation)};

    if (distance < m_ship.m_scale * 0.85f + asteroid.m_scale * 0.85f) {
      asteroid.m_hit = true;
    }
  }

  m_asteroids.m_asteroids.remove_if([](auto const &a) { return a.m_hit; });
}

void Window::checkWinCondition() {
  if (m_asteroids.m_asteroids.empty()) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}