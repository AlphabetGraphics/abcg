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
      // Ship body

      // glm::vec2{-01.0f, -05.0f},
      // glm::vec2{00.0f, -04.0f},
      // glm::vec2{+01.0f, -05.0f},


      glm::vec2{-01.0f, +6.0f},
      glm::vec2{-02.0f, +5.0f},
      glm::vec2{-02.0f, +4.0f},
      glm::vec2{-01.0f, +3.0f},
      glm::vec2{-01.0f, +2.0f},
      glm::vec2{-02.0f, +1.0f},
      glm::vec2{-02.0f, -01.0f},
      glm::vec2{-01.0f, -02.0f},
      glm::vec2{-01.0f, -04.0f},
      glm::vec2{-02.0f, -04.0f},
      glm::vec2{-02.0f, -05.0f},

      glm::vec2{+02.0f,-05.0f},
      glm::vec2{+02.0f, -04.0f},
      glm::vec2{+01.0f, -04.0f},
      glm::vec2{+01.0f, -02.0f},
      glm::vec2{+02.0f, -01.0f},
      glm::vec2{+02.0f, +01.0f},
      glm::vec2{+01.0f, +02.0f},
      glm::vec2{+01.0f, +03.0f},
      glm::vec2{+02.0f, +04.0f},
      glm::vec2{+02.0f, +05.0f},
      glm::vec2{+01.0f, +06.0f}

      // // Cannon (left)
      // glm::vec2{-12.5f, +10.5f}, glm::vec2{-12.5f, +04.0f},
      // glm::vec2{-09.5f, +04.0f}, glm::vec2{-09.5f, +10.5f},

      // // Cannon (right)
      // glm::vec2{+09.5f, +10.5f}, glm::vec2{+09.5f, +04.0f},
      // glm::vec2{+12.5f, +04.0f}, glm::vec2{+12.5f, +10.5f},
      
      // // Thruster trail (left)
      // glm::vec2{-12.0f, -07.5f}, 
      // glm::vec2{-09.5f, -18.0f}, 
      // glm::vec2{-07.0f, -07.5f},

      // // Thruster trail (right)
      // glm::vec2{+07.0f, -07.5f}, 
      // glm::vec2{+09.5f, -18.0f}, 
      // glm::vec2{+12.0f, -07.5f},
      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{4.0f, 4.0f};
  }

  std::array const indices{
                            0, 1, 2,
                            0, 2, 3,
                            0, 3, 18,
                            0, 18, 21,
                            18, 19, 21,
                            19, 20, 21,
                            3, 4, 17,
                            3, 17, 18,
                            15, 16, 17,
                            4, 5, 6,
                            14, 15, 17,
                            4, 6, 7,
                            4, 7, 14,
                            4, 14, 17,
                            7, 8, 13,
                            7, 13, 14,
                            9, 10, 11,
                            9, 11, 12,
                            
                            
                            

                            
                            // 0, 24, 13,
                            // 0, 2, 1,
                            // 0, 2, 3,
                            // 21, 22, 24,
                            // 22, 24, 23,
                            // 4, 6, 5,
                            // 4, 6, 7,
                            // 17, 18, 20,
                            // 18, 20, 19,
                            // 8, 9, 11,
                            // 9, 10, 11,
                            // 15, 16, 13,
                            // 13, 15, 14,




                          //  0, 1, 23,
                          //  0, 23, 24,
                          //  1, 22, 23,
                          //  1, 2, 22,
                          //  2, 3, 22,
                          //  3, 21, 22,
                          //  3, 21, 20,
                          //  3, 4, 20,

                          //  4, 5, 6,
                          //  4, 6, 7,
                          //  4, 7, 17,
                          //  4, 17, 20,
                          //  20, 17, 18,
                          //  20, 19, 18,
                          //  7, 17, 16,
                          //  7, 8, 16,
                           
                          //  8, 9, 10,
                          //  9, 10, 11,
                          //  8, 11, 12,
                          //  12, 13, 16,
                          //  16, 13, 15,
                          //  13, 14, 15,


                          //  3, 4, 20,
                          //  6, 7, 4,
                          //  7, 17, 16,
                          //  7, 8, 16,
                          //  8, 9, 11,
                          //  9, 10, 11,
                          //  11, 12, 8,
                          //  12, 13, 16,
                          //  13, 14, 15,
                          //  13, 15, 16,
                          //  16, 7, 17,
                          //  17, 20, 18,
                          //  18, 19, 20,
                          // 
                          // 
                          //  // Cannons
                          //  10, 11, 12,
                          //  10, 12, 13,
                          //  14, 15, 16,
                          //  14, 16, 17,
                          //  // Thruster trails
                          //  18, 19, 20,
                          //  21, 22, 23
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

      abcg::glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      abcg::glDisable(GL_BLEND);
    }
  }

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ship::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Ship::update(GameData const &gameData, float deltaTime) {
  const float maxRotation = glm::radians(25.0f);  // 45 degrees in radians
  const float rotationSpeed = glm::radians(120.0f);  // Rotation speed (degrees per second)
  const float returnSpeed = glm::radians(120.0f);    // Speed to return to center

  float m_left_thrust{1.5f};
  float m_right_thrust{1.5f};
  float m_gravity_acceleration{1.0f};

  const float leftThrustAngle = glm::radians(-135.0f);    // Down-left direction
  const float rightThrustAngle = glm::radians(-45.0f);    // Down-right direction
  const float gravityAngle = glm::radians(-90.0f);        // Downward

  // Calculate directional vectors from angles
  const glm::vec2 leftThrustDirection = glm::vec2{cos(leftThrustAngle), sin(leftThrustAngle)};
  const glm::vec2 rightThrustDirection = glm::vec2{cos(rightThrustAngle), sin(rightThrustAngle)};
  const glm::vec2 gravityDirection = glm::vec2{cos(gravityAngle), sin(gravityAngle)};

  bool isLeftPressed = gameData.m_input[gsl::narrow<size_t>(Input::Left)];
  bool isRightPressed = gameData.m_input[gsl::narrow<size_t>(Input::Right)];

  // Inverted controls: pressing Left rotates right, pressing Right rotates left
  if (isLeftPressed && !isRightPressed) {
    m_rotation -= rotationSpeed * deltaTime;  // Rotate right
  } else if (isRightPressed && !isLeftPressed) {
    m_rotation += rotationSpeed * deltaTime;  // Rotate left
  }
  // No input or both keys pressed: smoothly return to center
  else {
    // Gradually move `m_rotation` towards zero
    if (m_rotation > 0) {
      m_rotation = std::max(0.0f, m_rotation - returnSpeed * deltaTime);
    } else if (m_rotation < 0) {
      m_rotation = std::min(0.0f, m_rotation + returnSpeed * deltaTime);
    }
  }

  // Clamp the rotation to the specified range
  m_rotation = std::clamp(m_rotation, -maxRotation, maxRotation);

  // Calculate rotation-based thrust directions
  glm::mat2 rotationMatrix = glm::mat2(cos(m_rotation), -sin(m_rotation),
                                       sin(m_rotation), cos(m_rotation));
  
  glm::vec2 rotatedLeftThrustDirection = rotationMatrix * leftThrustDirection;
  glm::vec2 rotatedRightThrustDirection = rotationMatrix * rightThrustDirection;
  glm::vec2 rotatedGravityDirection = rotationMatrix * gravityDirection;

  // Calculate torque generated by the left and right thrusts
  float thrustDistance = 1.0f; // Distance from center of mass to the thrust application point
  float leftTorque = thrustDistance * m_left_thrust * glm::length(rotatedLeftThrustDirection);
  float rightTorque = thrustDistance * m_right_thrust * glm::length(rotatedRightThrustDirection);

  // The total torque is the difference between the left and right thrust torques
  float totalTorque = leftTorque - rightTorque;

  // Assume a simple moment of inertia (I) for the ship
  float momentOfInertia = 1.0f;  // You can adjust this value based on the ship's mass distribution

  // Calculate angular acceleration (alpha = torque / moment of inertia)
  float angularAcceleration = totalTorque / momentOfInertia;

  // Apply the angular acceleration to the rotation
  m_rotation += angularAcceleration * deltaTime;

  // Apply left thrust
  if (isLeftPressed) {
    m_velocity -= m_left_thrust * rotatedLeftThrustDirection * deltaTime;
  }

  // Apply right thrust
  if (isRightPressed) {
    m_velocity -= m_right_thrust * rotatedRightThrustDirection * deltaTime;
  }

  // Apply gravity
  m_velocity += m_gravity_acceleration * rotatedGravityDirection * deltaTime;
}

// void Ship::update(GameData const &gameData, float deltaTime) {
//   const float maxRotation = glm::radians(25.0f);  // 45 degrees in radians
//   const float rotationSpeed = glm::radians(120.0f);  // Rotation speed (degrees per second)
//   const float returnSpeed = glm::radians(120.0f);    // Speed to return to center

//   float m_left_thrust{1.5f};
//   float m_right_thrust{1.5f};
//   float m_gravity_acceleration{1.0f};

//   const float leftThrustAngle = glm::radians(-135.0f);    // Down-left direction
//   const float rightThrustAngle = glm::radians(-45.0f);    // Down-right direction
//   const float gravityAngle = glm::radians(-90.0f);        // Downward

//   // Calculate directional vectors from angles
//   const glm::vec2 leftThrustDirection = glm::vec2{cos(leftThrustAngle), sin(leftThrustAngle)};
//   const glm::vec2 rightThrustDirection = glm::vec2{cos(rightThrustAngle), sin(rightThrustAngle)};
//   const glm::vec2 gravityDirection = glm::vec2{cos(gravityAngle), sin(gravityAngle)};

//   bool isLeftPressed = gameData.m_input[gsl::narrow<size_t>(Input::Left)];
//   bool isRightPressed = gameData.m_input[gsl::narrow<size_t>(Input::Right)];

//   // Inverted controls: pressing Left rotates right, pressing Right rotates left
//   if (isLeftPressed && !isRightPressed) {
//     m_rotation -= rotationSpeed * deltaTime;  // Rotate right
//   } else if (isRightPressed && !isLeftPressed) {
//     m_rotation += rotationSpeed * deltaTime;  // Rotate left
//   }
//   // No input or both keys pressed: smoothly return to center
//   else {
//     // Gradually move `m_rotation` towards zero
//     if (m_rotation > 0) {
//       m_rotation = std::max(0.0f, m_rotation - returnSpeed * deltaTime);
//     } else if (m_rotation < 0) {
//       m_rotation = std::min(0.0f, m_rotation + returnSpeed * deltaTime);
//     }
//   }

//   // Clamp the rotation to the specified range
//   m_rotation = std::clamp(m_rotation, -maxRotation, maxRotation);

//   // Calculate rotation-based thrust directions
//   glm::mat2 rotationMatrix = glm::mat2(cos(m_rotation), -sin(m_rotation),
//                                        sin(m_rotation), cos(m_rotation));
  
//   glm::vec2 rotatedLeftThrustDirection = rotationMatrix * leftThrustDirection;
//   glm::vec2 rotatedRightThrustDirection = rotationMatrix * rightThrustDirection;
//   glm::vec2 rotatedGravityDirection = rotationMatrix * gravityDirection;

//   // Apply left thrust
//   if (isLeftPressed) {
//     m_velocity -= m_left_thrust * rotatedLeftThrustDirection * deltaTime;
//   }

//   // Apply right thrust
//   if (isRightPressed) {
//     m_velocity -= m_right_thrust * rotatedRightThrustDirection * deltaTime;
//   }

//   // Apply gravity
//   m_velocity += m_gravity_acceleration * rotatedGravityDirection * deltaTime;
// }


// void Ship::update(GameData const &gameData, float deltaTime) {
//   const float maxRotation = glm::radians(25.0f);  // 45 degrees in radians
//   const float rotationSpeed = glm::radians(120.0f);  // Rotation speed (degrees per second)
//   const float returnSpeed = glm::radians(120.0f);    // Speed to return to center

//   float m_left_thrust{1.5f};
//   float m_right_thrust{1.5f};
//   float m_gravity_acceleration{1.0f};

//   const float leftThrustAngle = glm::radians(-135.0f);    // Down-left direction
//   const float rightThrustAngle = glm::radians(-45.0f);    // Down-right direction
//   const float gravityAngle = glm::radians(-90.0f);        // Downward

//   // Calculate directional vectors from angles
//   const glm::vec2 leftThrustDirection = glm::vec2{cos(leftThrustAngle), sin(leftThrustAngle)};
//   const glm::vec2 rightThrustDirection = glm::vec2{cos(rightThrustAngle), sin(rightThrustAngle)};
//   const glm::vec2 gravityDirection = glm::vec2{cos(gravityAngle), sin(gravityAngle)};

//   bool isLeftPressed = gameData.m_input[gsl::narrow<size_t>(Input::Left)];
//   bool isRightPressed = gameData.m_input[gsl::narrow<size_t>(Input::Right)];

//   // Inverted controls: pressing Left rotates right, pressing Right rotates left
//   if (isLeftPressed && !isRightPressed) {
//     m_rotation -= rotationSpeed * deltaTime;  // Rotate right
//   } else if (isRightPressed && !isLeftPressed) {
//     m_rotation += rotationSpeed * deltaTime;  // Rotate left
//   }
//   // No input or both keys pressed: smoothly return to center
//   else {
//     // Gradually move `m_rotation` towards zero
//     if (m_rotation > 0) {
//       m_rotation = std::max(0.0f, m_rotation - returnSpeed * deltaTime);
//     } else if (m_rotation < 0) {
//       m_rotation = std::min(0.0f, m_rotation + returnSpeed * deltaTime);
//     }
//   }

//   // Clamp the rotation to the specified range
//   m_rotation = std::clamp(m_rotation, -maxRotation, maxRotation);

//   // Apply left thrust
//   if (gameData.m_input[gsl::narrow<size_t>(Input::Left)]) {
//     m_velocity -= m_left_thrust * leftThrustDirection * deltaTime;
//   }

//   // Apply right thrust
//   if (gameData.m_input[gsl::narrow<size_t>(Input::Right)]) {
//     m_velocity -= m_right_thrust * rightThrustDirection * deltaTime;
//   }

//   // Apply gravity
//   m_velocity += m_gravity_acceleration * gravityDirection * deltaTime;
// }

// void Ship::update(GameData const &gameData, float deltaTime) {
//   // Rotate
//   if (gameData.m_input[gsl::narrow<size_t>(Input::Left)])
//     m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
//   if (gameData.m_input[gsl::narrow<size_t>(Input::Right)])
//     m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

//   // Apply thrust
//   if (gameData.m_input[gsl::narrow<size_t>(Input::Up)] &&
//       gameData.m_state == State::Playing) {
//     // Thrust in the forward vector
//     auto const forward{glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation)};
//     m_velocity += forward * deltaTime;
//   }
// }