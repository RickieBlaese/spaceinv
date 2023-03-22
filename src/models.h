#ifndef _MODELS_H
#define _MODELS_H

#include "utility.h"

/* first is position relative to center, and second is output */
struct TexChar {
    Vec2 pos;
    wchar_t ch = L'\0';
};

using Texture = std::vector<TexChar>;


struct Particle {
    bool is_enemy = false;
    Texture tex;
    Vec2 pos;
    std::int32_t layer = 0; /* top layer */
    double size = 1.0;

    double damage = 0.0;
    
    std::function<void (Particle& self, double dt, std::uint64_t tick)> update = [](Particle& self, double dt, std::uint64_t tick) {;}; /* NOLINT */

};

struct Entity {
    bool is_enemy = false;
    Texture tex;
    Vec2 pos;
    std::int32_t layer = 0; /* top layer */
    double size = 1.0;

    double health = 0.0;
    bool immortal = false;

    [[nodiscard]] bool is_alive() const;

    std::function<void (Entity& self, double dt, std::uint64_t tick)> update = [](Entity& self, double dt, std::uint64_t tick) {;}; /* NOLINT */

};


struct Level {
    std::vector<Entity> entities;
    std::vector<Particle> particles;
};


class Layer {
public:
    std::int32_t y = 0, x = 0;
    std::vector<std::wstring> out;

    Layer() = default;
    Layer(std::int32_t y, std::int32_t x);

    void clear(wchar_t ch);

};


class Game {
public:
    std::int32_t y = 0, x = 0;
    std::uint64_t tick = 0;
    std::vector<Level> levels;
    std::vector<Layer> layers;
    std::vector<std::wstring> out_buf;
    std::vector<std::wstring> last_out;
    decltype(levels)::iterator clevel;

    Game() = default;
    Game(std::int32_t y, std::int32_t x, std::int32_t layer_count);

    void update(double dt);
    void render_layers();
    void display_layers(WINDOW *win, std::int32_t offy, std::int32_t offx);
    [[nodiscard]] bool is_over() const;

};

#endif
