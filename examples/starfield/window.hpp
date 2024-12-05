#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;

  glm::ivec2 m_viewportSize{};

  Model m_model;
  Model m_model_ship;

  struct Star {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  struct Ship {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  std::array<Star, 100> m_stars;
  Ship m_ship;

  float m_angle{90.0};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{30.0f};

  GLuint m_program{};

  void randomizeStar(Star &star, int index);
};

#endif