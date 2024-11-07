#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Down, Up, Fire };
enum class State { Playing, GameOver, Win };

struct GameData {
  State m_state{State::Playing};
  std::bitset<5> m_input;  // [fire, up, down, left, right]
  float m_mass{100.0f};
  float m_force{1000.f};
  float m_gravity{-10.0f};
};

#endif