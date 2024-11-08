#include "ship.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Ship::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset ship attributes
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  std::array positions{
      // Astronaut Body
      glm::vec2{-04.0f, +7.0f},
      glm::vec2{-07.0f, +4.0f},
      glm::vec2{-07.0f, +2.0f},
      glm::vec2{-03.0f, 0.0f},
      glm::vec2{-05.0f, -2.0f},
      glm::vec2{-07.0f, -2.0f},
      glm::vec2{-07.0f, -3.0f},
      glm::vec2{-05.0f, -3.0f},
      glm::vec2{-05.0f, -4.0f},
      glm::vec2{-03.0f, -6.0f},
      glm::vec2{-05.0f, -6.0f},
      glm::vec2{-05.0f, -7.0f},
      glm::vec2{-02.0f, -7.0f},
      glm::vec2{-02.0f, -6.0f},
      glm::vec2{+02.0f, -6.0f},
      glm::vec2{+02.0f, -7.0f},
      glm::vec2{+05.0f, -7.0f},
      glm::vec2{+05.0f, -6.0f},
      glm::vec2{+03.0f, -6.0f},
      glm::vec2{+05.0f, -4.0f},
      glm::vec2{+05.0f, -3.0f},
      glm::vec2{+07.0f, -3.0f},
      glm::vec2{+07.0f, -2.0f},
      glm::vec2{+05.0f, -2.0f},
      glm::vec2{+03.0f, 0.0f},
      glm::vec2{+07.0f, +2.0f},
      glm::vec2{+07.0f, +4.0f},
      glm::vec2{+04.0f, +7.0f},

      // Thruster trail (left)
      glm::vec2{-04.0f, -03.0f}, 
      glm::vec2{-06.0f, -06.0f}, 
      glm::vec2{-03.0f, -04.0f},

      // Thruster trail (right)
      glm::vec2{+03.0f, -04.0f}, 
      glm::vec2{+06.0f, -06.0f}, 
      glm::vec2{+04.0f, -03.0f},
      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{7.0f, 7.0f};
  }

  std::array const indices{
      0, 1, 2,
                            0, 2, 3,
                            0, 3, 24,
                            0, 24, 27,
                            24, 25, 27,
                            25, 26, 27,
                            3, 4, 23,
                            3, 23, 24,
                            4, 5, 6,
                            4, 6, 7,
                            4, 8, 19,
                            4, 19, 23,
                            20, 21, 23,
                            21, 22, 23,
                            8, 9, 18,
                            8, 18, 19,
                            9, 10, 11,
                            9, 11, 12,
                            9, 12, 13,
                            14, 15, 18,
                            15, 16, 18,
                            16, 17, 18,

                            // Thruster trails
                            28, 29, 30,
                            31, 32, 33,
  };
  // clang-format on

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Ship::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing)
    return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0)
    m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail for 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      abcg::glEnable(GL_BLEND);
      abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      abcg::glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      abcg::glDrawElements(GL_TRIANGLES, 24 * 3, GL_UNSIGNED_INT, nullptr);

      abcg::glDisable(GL_BLEND);
    }
  }

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 22 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ship::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Ship::update(GameData const &gameData, float deltaTime) {
  // Rotate
  if (gameData.m_input[gsl::narrow<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
  if (gameData.m_input[gsl::narrow<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  // Apply thrust
  if (gameData.m_input[gsl::narrow<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    auto const forward{glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation)};
    m_velocity += forward * deltaTime;
  }
}