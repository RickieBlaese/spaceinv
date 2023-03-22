#include "models.h"


bool Entity::is_alive() const {
    return health > 0 || immortal;
}

Layer::Layer(std::int32_t y, std::int32_t x) : y(y), x(x) {
    out.resize(y);
    for (std::int32_t j = 0; j < y; j++) {
        out[j] = std::wstring(x, L'\0');
    }
}

void Layer::clear(wchar_t ch = L'\0') {
    for (std::int32_t j = 0; j < y; j++) {
        for (std::int32_t i = 0; i < x; i++) {
            out[j][i] = ch;
        }
    }
}


Game::Game(std::int32_t y, std::int32_t x, std::int32_t layer_count) : y(y), x(x) {
    for (std::int32_t i = 0; i < layer_count; i++) {
        layers.emplace_back(y, x);
    }
    out_buf.resize(y);
    for (std::int32_t j = 0; j < y; j++) {
        out_buf[j] = std::wstring(x, L' ');
    }
    last_out = out_buf;
}


void Game::update(double dt) {
    std::vector<Particle>& particles = clevel->particles;
    std::vector<Entity>& entities = clevel->entities;

    tick++;
    for (Particle& particle : particles) {
        particle.update(particle, dt, tick);
    }

    for (std::int32_t i = 0; i < entities.size(); i++) {
        Entity& entity = entities[i];
        entity.update(entity, dt, tick);
        for (std::int32_t j = 0; j < particles.size(); j++) {
            Particle& particle = particles[j];
            if (entity.is_enemy == !particle.is_enemy && !entity.immortal && (particle.pos - entity.pos).mod() < particle.size + entity.size) {
                entity.health -= particle.damage;
                particles.erase(particles.begin() + j);
                j--;
            }
        }

        if (!entity.is_alive()) {
            entities.erase(entities.begin() + i);
            i--;
        }
    }

}

void Game::render_layers() {
    std::vector<Entity>& entities = clevel->entities;
    std::vector<Particle>& particles = clevel->particles;
    

    auto rlayersl = [&]<typename T>(const T& items) {
        for (const auto& item : items) {
            for (const TexChar& tch : item.tex) {
                layers[std::clamp<std::size_t>(item.layer, 0, layers.size() - 1)].out
                    [item.pos.y + tch.pos.y]
                    [item.pos.x + tch.pos.x] = tch.ch;
            }
        }
    };
    
    rlayersl(entities);
    rlayersl(particles);

    for (std::int64_t li = layers.size() - 1; li >= 0; li--) {
        for (std::int32_t j = 0; j < layers[li].y; j++) {
            for (std::int32_t i = 0; i < layers[li].x; i++) {
                if (layers[li].out[j][i] == L'\0') {
                    continue;
                }
                out_buf[j][i] = layers[li].out[j][i];
            }
        }
        layers[li].clear();
    }

}


void Game::display_layers(WINDOW *win, std::int32_t offy, std::int32_t offx) {
    for (std::int32_t j = 0; j < y; j++) { 
        for (std::int32_t i = 0; i < x; i++) {
            if (out_buf[j][i] == last_out[j][i]) {
                continue;
            }
            mvwaddnwstr(win, j + offy, i + offx, &out_buf[j][i], 1);
            last_out[j][i] = out_buf[j][i];
        }
    }

}


bool Game::is_over() const {
    std::vector<Entity>& entities = clevel->entities;
    for (const Entity& entity : entities) {
        if (!entity.is_enemy && entity.is_alive()) {
            return false;
        }
    }

    return true;
}

