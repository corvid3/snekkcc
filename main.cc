#include <algorithm>
#include <list>
#include <raylib.h>
#include <raymath.h>
#include <unordered_map>
#include <vector>

#ifndef RL_WHITE
#define RL_WHITE WHITE
#endif

#ifndef RL_RED
#define RL_RED RED
#endif

#ifndef RL_BLACK
#define RL_BLACK BLACK
#endif

#ifndef RL_GRAY
#define RL_GRAY GRAY
#endif

constexpr static std::size_t MAP_EDGE_LEN = 16;
constexpr static std::size_t WINDOW_SIZE = 640;
constexpr static Vector2 VEC_LEFT = {-1, 0};
constexpr static Vector2 VEC_RIGHT = {1, 0};
constexpr static Vector2 VEC_UP = {0, -1};
constexpr static Vector2 VEC_DOWN = {0, 1};
struct Fruit {
  Vector2 at;

  static Fruit genRand() {
    Fruit out;
    out.at.x = GetRandomValue(0, MAP_EDGE_LEN - 1);
    out.at.y = GetRandomValue(0, MAP_EDGE_LEN - 1);
    return out;
  }
};

struct Segment {
  Vector2 pos;
  Vector2 dir;
};

enum class State {
  Alive,
  Dead,
};

class Game {
  std::vector<Fruit> m_fruits;
  std::list<Segment> m_segments;

  State state = State::Alive;
  float timer = 0.0f;
  Vector2 saved_dir = VEC_UP;

public:
  Game() { InitGame(); }

  void InitGame() {
    m_segments.clear();
    m_fruits.clear();
    // add the head
    m_segments.push_back({{static_cast<float>(MAP_EDGE_LEN) / 2,
                           static_cast<float>(MAP_EDGE_LEN) / 2},
                          VEC_UP});

    m_fruits.insert(m_fruits.begin(), Fruit::genRand());
    m_fruits.insert(m_fruits.begin(), Fruit::genRand());
    m_fruits.insert(m_fruits.begin(), Fruit::genRand());

    state = State::Alive;
  }

  void Run() {
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(RL_BLACK);

      switch (state) {
      case State::Alive:
        UpdatePlaying();
        DrawPlaying();
        break;

      case State::Dead:
        UpdateDead();
        DrawPlaying();
        DrawDead();
        break;
      }

      EndDrawing();
    }
  }

  void AdvanceSnekk() {
    // save this if we ran into a fruit,
    // instead of recomputing
    Segment const last_save = m_segments.back();

    m_segments.front().dir = saved_dir;
    Segment prev = m_segments.front();
    for (auto &segment : m_segments) {
      std::swap(segment, prev);
    }

    m_segments.front().pos =
        Vector2Add(m_segments.front().pos, m_segments.front().dir);

    // check for walls
    if (m_segments.front().pos.x < 0 || m_segments.front().pos.y < 0 ||
        m_segments.front().pos.x >= MAP_EDGE_LEN ||
        m_segments.front().pos.y >= MAP_EDGE_LEN) {
      state = State::Dead;
    }

    auto const body_collision = std::find_if(
        ++m_segments.begin(), m_segments.end(), [this](Segment seg) {
          return Vector2Equals(seg.pos, m_segments.front().pos);
        });

    if (body_collision != m_segments.end()) {
      state = State::Dead;
    }

    auto const fruit_collision =
        std::find_if(m_fruits.begin(), m_fruits.end(), [this](Fruit fruit) {
          return Vector2Equals(fruit.at, m_segments.front().pos);
        });

    if (fruit_collision != m_fruits.end()) {
      m_fruits.erase(fruit_collision);
      m_segments.push_back(last_save);

      Fruit new_fruit;

      bool breaking = true;
      while (breaking) {
        new_fruit = Fruit::genRand();

        breaking = false;

        for (auto const &segment : m_segments)
          if (Vector2Equals(new_fruit.at, segment.pos))
            breaking = true;

        for (auto const &frui : m_fruits)
          if (Vector2Equals(new_fruit.at, frui.at))
            breaking = true;
      }

      m_fruits.push_back(new_fruit);
    }
  }
  void UpdatePlaying() {
    if (IsKeyDown(KEY_LEFT) &&
        !Vector2Equals(m_segments.front().dir, VEC_RIGHT))
      saved_dir = VEC_LEFT;
    if (IsKeyDown(KEY_RIGHT) &&
        !Vector2Equals(m_segments.front().dir, VEC_LEFT))
      saved_dir = VEC_RIGHT;
    if (IsKeyDown(KEY_UP) && !Vector2Equals(m_segments.front().dir, VEC_DOWN))
      saved_dir = VEC_UP;
    if (IsKeyDown(KEY_DOWN) && !Vector2Equals(m_segments.front().dir, VEC_UP))
      saved_dir = VEC_DOWN;

    timer += GetFrameTime();
    if (timer > 0.1f) {
      timer = 0.0f;
      AdvanceSnekk();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      m_fruits.push_back(Fruit::genRand());
  }

  void DrawPlaying() {
    for (auto const fruit : m_fruits) {
      // magic size, 64 x 16
      DrawRectangle(fruit.at.x * WINDOW_SIZE / MAP_EDGE_LEN + 1,
                    fruit.at.y * WINDOW_SIZE / MAP_EDGE_LEN + 1,
                    WINDOW_SIZE / MAP_EDGE_LEN + 1,
                    WINDOW_SIZE / MAP_EDGE_LEN + 1, RL_RED);
    }

    for (auto const &segment : m_segments)
      DrawRectangle(segment.pos.x * WINDOW_SIZE / MAP_EDGE_LEN + 1,
                    segment.pos.y * WINDOW_SIZE / MAP_EDGE_LEN + 1,
                    WINDOW_SIZE / MAP_EDGE_LEN - 1,
                    WINDOW_SIZE / MAP_EDGE_LEN - 1, RL_WHITE);
  }

  void UpdateDead() {
    if (IsKeyPressed(KEY_SPACE)) {
      InitGame();
    }
  }
  void DrawDead() {
    DrawText("YOU ARE DEAD. TRY AGAIN.", 100, 100, 30, RL_GRAY);
  }
};

int main() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "snake");
  SetTargetFPS(60);

  Game().Run();

  CloseWindow();
}
