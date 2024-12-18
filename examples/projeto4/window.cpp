#include "window.hpp"

#include "imfilebrowser.h"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Create programs
  for (auto const &name : m_shaderNames) {
    auto const path{assetsPath + "shaders/" + name};
    auto const program{abcg::createOpenGLProgram(
        {{.source = path + ".vert", .stage = abcg::ShaderStage::Vertex},
         {.source = path + ".frag", .stage = abcg::ShaderStage::Fragment}})};
    m_programs.push_back(program);
  }

  // Load ship model
  loadModel(m_model_ship, assetsPath + "ship.obj");
  m_trianglesToDraw = m_model_ship.getNumTriangles();

  // Load torus model
  loadModel(m_model, assetsPath + "torus.obj");

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

void Window::loadModel(Model& model, std::string_view path) {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  model.destroy();

  model.loadDiffuseTexture(assetsPath + "maps/pattern.png");
  model.loadNormalTexture(assetsPath + "maps/pattern_normal.png");
  model.loadCubeTexture(assetsPath + "maps/cube/");
  model.loadObj(path);
  model.setupVAO(m_programs.at(m_currentProgramIndex));
  // m_trianglesToDraw = model.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = model.getKa();
  m_Kd = model.getKd();
  m_Ks = model.getKs();
  m_shininess = model.getShininess();
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
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }

  static bool spacePressed = false; // Track if SPACE is pressed

  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_SPACE: // Track SPACE key
      spacePressed = true;
      break;

    case SDLK_UP: 
      if (spacePressed) { // Move forward (z-axis)
        m_ship.m_position.z -= 0.1f;
      } else { // Move up (y-axis)
        m_ship.m_position.y += 0.1f;
      }
      break;

    case SDLK_DOWN: 
      if (spacePressed) { // Move backward (z-axis)
        m_ship.m_position.z += 0.1f;
      } else { // Move down (y-axis)
        m_ship.m_position.y -= 0.1f;
      }
      break;

    case SDLK_LEFT: // Move left (x-axis)
      m_ship.m_position.x -= 0.1f;
      break;

    case SDLK_RIGHT: // Move right (x-axis)
      m_ship.m_position.x += 0.1f;
      break;
    }
  } else if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      spacePressed = false; // Release SPACE key
    }
  }

  // Clamp positions to defined bounds
  m_ship.m_position.x = std::clamp(m_ship.m_position.x, -0.5f, 0.5f);
  m_ship.m_position.y = std::clamp(m_ship.m_position.y, -0.5f, 0.5f);
  // m_ship.m_position.z = std::clamp(m_ship.m_position.z, 0.0f, 5.0f); // Assuming bounds for z-axis
}


void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle);

  // Define grid dimensions
  constexpr int gridCols = 10; // Number of columns in the grid
  constexpr int gridSpacing = 2; // Spacing between stars in the grid
  constexpr float bounceAmplitude = 0.5f; // Amplitude of the bouncing motion
  constexpr float bounceSpeed = 1.0f; // Speed of the bouncing motion

  static float time = 0.0f; // Time accumulator for sine wave
  time += deltaTime;

  // Update stars
  for (size_t i = 0; i < m_stars.size(); ++i) {
    auto &star = m_stars[i];

    // Compute grid position based on index
    int row = i / gridCols;  // Row index
    int col = i % gridCols;  // Column index

    // Calculate bouncing offset for the row
    float direction = (row % 2 == 0) ? 1.0f : -1.0f; // Alternate directions
    float offsetX = direction * bounceAmplitude * std::sin(time * bounceSpeed);

    // Update star position
    star.m_position.x = col * gridSpacing - (gridCols * gridSpacing) / 2 + offsetX;
    star.m_position.y = row * gridSpacing - (gridCols * gridSpacing) / 2; // Center the grid on Y

    // Move star along the Z axis
    star.m_position.z += deltaTime * 10.0f;

    // Reposition the star behind the camera if it passes the threshold on Z-axis
    if (star.m_position.z > 0.1f) {
      randomizeStar(star, i);

      // Reset position based on grid
      star.m_position.z = -100.0f; // Reset to initial Z position
    }
  }
}


void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Use currently selected program
  auto const program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(program, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(program, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(program, "normalMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  auto const shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(program, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(program, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(program, "diffuseTex")};
  auto const normalTexLoc{abcg::glGetUniformLocation(program, "normalTex")};
  auto const cubeTexLoc{abcg::glGetUniformLocation(program, "cubeTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(program, "mappingMode")};
  auto const texMatrixLoc{abcg::glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(normalTexLoc, 1);
  abcg::glUniform1i(cubeTexLoc, 2);
  abcg::glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 const texMatrix{m_trackBallLight.getRotation()};
  abcg::glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto const lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  // // Set uniform variables for the current model
  // abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  // auto const modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  // auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  // abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

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

// void Window::onPaintUI() {
//   abcg::OpenGLWindow::onPaintUI();

//   {
//     auto const widgetSize{ImVec2(218, 62)};
//     ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
//     ImGui::SetNextWindowSize(widgetSize);
//     ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

//     {
//       ImGui::PushItemWidth(120);
//       static std::size_t currentIndex{};
//       std::vector<std::string> const comboItems{"Perspective", "Orthographic"};

//       if (ImGui::BeginCombo("Projection",
//                             comboItems.at(currentIndex).c_str())) {
//         for (auto const index : iter::range(comboItems.size())) {
//           auto const isSelected{currentIndex == index};
//           if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
//             currentIndex = index;
//           if (isSelected)
//             ImGui::SetItemDefaultFocus();
//         }
//         ImGui::EndCombo();
//       }
//       ImGui::PopItemWidth();

//       ImGui::PushItemWidth(170);
//       auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
//                         gsl::narrow<float>(m_viewportSize.y)};
//       if (currentIndex == 0) {
//         m_projMatrix =
//             glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

//         ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
//       } else {
//         m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
//                                   20.0f, 0.01f, 100.0f);
//       }
//       ImGui::PopItemWidth();
//     }

//     ImGui::End();
//   }
// }

// void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

// void Window::onDestroy() {
//   m_model.destroy();
//   abcg::glDeleteProgram(m_program);
// }

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  auto const scaledWidth{gsl::narrow_cast<int>(m_viewportSize.x * 0.8f)};
  auto const scaledHeight{gsl::narrow_cast<int>(m_viewportSize.y * 0.8f)};

  // File browser for models
  static ImGui::FileBrowser fileDialogModel;
  fileDialogModel.SetTitle("Load 3D Model");
  fileDialogModel.SetTypeFilters({".obj"});
  fileDialogModel.SetWindowSize(scaledWidth, scaledHeight);

  // File browser for textures
  static ImGui::FileBrowser fileDialogDiffuseMap;
  fileDialogDiffuseMap.SetTitle("Load Diffuse Map");
  fileDialogDiffuseMap.SetTypeFilters({".jpg", ".png"});
  fileDialogDiffuseMap.SetWindowSize(scaledWidth, scaledHeight);

  // File browser for normal maps
  static ImGui::FileBrowser fileDialogNormalMap;
  fileDialogNormalMap.SetTitle("Load Normal Map");
  fileDialogNormalMap.SetTypeFilters({".jpg", ".png"});
  fileDialogNormalMap.SetWindowSize(scaledWidth, scaledHeight);

#if defined(__EMSCRIPTEN__)
  auto const assetsPath{abcg::Application::getAssetsPath()};
  fileDialogModel.SetPwd(assetsPath);
  fileDialogDiffuseMap.SetPwd(assetsPath + "/maps");
  fileDialogNormalMap.SetPwd(assetsPath + "/maps");
#endif

  // Create main window widget
  {
    auto widgetSize{ImVec2(222, 190)};

    if (!m_model.isUVMapped()) {
      // Add extra space for static text
      widgetSize.y += 26;
    }

    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration);

    // Menu
    {
      bool loadModel{};
      bool loadDiffuseMap{};
      bool loadNormalMap{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          ImGui::MenuItem("Load 3D Model...", nullptr, &loadModel);
          ImGui::MenuItem("Load Diffuse Map...", nullptr, &loadDiffuseMap);
          ImGui::MenuItem("Load Normal Map...", nullptr, &loadNormalMap);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (loadModel)
        fileDialogModel.Open();
      if (loadDiffuseMap)
        fileDialogDiffuseMap.Open();
      if (loadNormalMap)
        fileDialogNormalMap.Open();
    }

    // Slider will be stretched horizontally
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderInt(" ", &m_trianglesToDraw, 0, m_model.getNumTriangles(),
                     "%d triangles");
    ImGui::PopItemWidth();

    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      abcg::glEnable(GL_CULL_FACE);
    } else {
      abcg::glDisable(GL_CULL_FACE);
    }

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
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

      if (currentIndex == 0) {
        abcg::glFrontFace(GL_CCW);
      } else {
        abcg::glFrontFace(GL_CW);
      }
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
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

      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix =
            glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    // Shader combo box
    {
      static std::size_t currentIndex{};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader", m_shaderNames.at(currentIndex))) {
        for (auto const index : iter::range(m_shaderNames.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(m_shaderNames.at(index), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      // Set up VAO if shader program has changed
      if (gsl::narrow<int>(currentIndex) != m_currentProgramIndex) {
        m_currentProgramIndex = gsl::narrow<int>(currentIndex);
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
      }
    }

    if (!m_model.isUVMapped()) {
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh has no UV coords.");
    }

    // UV mapping box
    {
      std::vector<std::string> comboItems{"Triplanar", "Cylindrical",
                                          "Spherical"};

      if (m_model.isUVMapped())
        comboItems.emplace_back("From mesh");

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("UV mapping",
                            comboItems.at(m_mappingMode).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{m_mappingMode == static_cast<int>(index)};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            m_mappingMode = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }

  // Create window for light sources
  if (m_currentProgramIndex >= 2 && m_currentProgramIndex <= 6) {
    auto const widgetSize{ImVec2(222, 244)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5,
                                   m_viewportSize.y - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("Light properties");

    // Slider to control light properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ia", &m_Ia.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Id", &m_Id.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Is", &m_Is.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Material properties");

    // Slider to control material properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ka", &m_Ka.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Kd", &m_Kd.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Ks", &m_Ks.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    // Slider to control the specular shininess
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderFloat(" ", &m_shininess, 0.0f, 500.0f, "shininess: %.1f");
    ImGui::PopItemWidth();

    ImGui::End();
  }

  fileDialogModel.Display();
  if (fileDialogModel.HasSelected()) {
    loadModel(m_model_ship, fileDialogModel.GetSelected().string());
    fileDialogModel.ClearSelected();

    if (m_model.isUVMapped()) {
      // Use mesh texture coordinates if available...
      m_mappingMode = 3;
    } else {
      // ...or triplanar mapping otherwise
      m_mappingMode = 0;
    }
  }

  fileDialogDiffuseMap.Display();
  if (fileDialogDiffuseMap.HasSelected()) {
    m_model.loadDiffuseTexture(fileDialogDiffuseMap.GetSelected().string());
    fileDialogDiffuseMap.ClearSelected();
  }

  fileDialogNormalMap.Display();
  if (fileDialogNormalMap.HasSelected()) {
    m_model.loadNormalTexture(fileDialogNormalMap.GetSelected().string());
    fileDialogNormalMap.ClearSelected();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBallModel.resizeViewport(size);
  m_trackBallLight.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  m_model_ship.destroy();
  for (auto const &program : m_programs) {
    abcg::glDeleteProgram(program);
  }
}