#include "models.h"


void init_game(Game& game) {
    Level l;

    {
        Entity player;
        player.tex = {TexChar{Vec2{}, L'█'}};
        player.pos = Vec2{5, 5};
        player.health = 10.0;
        player.update = [](Entity& self, double dt, std::uint64_t tick) { /* NOLINT */
            static Vec2 lastpos = self.pos;
            if ((self.pos.x != lastpos.x) || (self.pos.y != lastpos.y)) {
                self.tex = {TexChar{Vec2{}, L'█'}, TexChar{lastpos - self.pos, L' '}}; /* clearing old pos */
            } else {
                self.tex = {TexChar{Vec2{}, L'█'}};
            }
            lastpos = self.pos;
        };
        l.entities.push_back(player);
    }

    {
        for (std::int32_t i = 0; i < 5; i++) {
            Entity thise;
            thise.layer = 1;
            thise.is_enemy = true;
            thise.tex = {TexChar{Vec2{}, L'▖'}};
            thise.pos = Vec2{1, i * 2};
            thise.health = 2.0;
            thise.update = [](Entity& self, double dt, std::uint64_t tick) {
                auto sublength = static_cast<std::int32_t>(2.0L / dt);
                if (tick % sublength == 0) {
                    self.tex[0].ch = L'▖';
                } else if (tick % sublength == sublength / 4) {
                    self.tex[0].ch = L'▘';
                } else if (tick % sublength == sublength / 2) {
                    self.tex[0].ch = L'▝';
                } else if (tick % sublength == (sublength * 3) / 4) {
                    self.tex[0].ch = L'▗';
                }
            };
            l.entities.push_back(thise);
        }
    }

    game.levels.push_back(l);
    game.clevel = game.levels.begin();
}


int main() {
    static constexpr std::int32_t max_input_size = 50;
    static constexpr double unadjusted_fps = 60.0L;

    std::setlocale(LC_ALL, "");

    WINDOW *win = init_ncurses();
    wmove(win, 0, 0);

    char in[max_input_size + 1];
    std::int32_t y = 0, x = 0;

    try {
        waddstr(win, "y size: ");
        wgetnstr(win, in, max_input_size);
        y = std::stoi(in);
        if (y == 0) {
            throw std::exception();
        }

        waddstr(win, "x size: ");
        wgetnstr(win, in, max_input_size);
        x = std::stoi(in);
        if (x == 0) {
            throw std::exception();
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        deinit_ncurses(win);
        std::cerr << "error: bad size entered, input was " << in << '\n';
        return 1;
    }
    
    wclear(win);
    curs_set(0);
    noecho();


    /* layers:
     * 0 for player
     * 1 for enemies
     * 2 for particles */
    Game game(y, x, 3);
    init_game(game);

    bool end_flag = false;
    nodelay(win, true);


    const std::uint64_t visual_wait_us = 1'000'000ULL / unadjusted_fps; /* in microseconds */

    std::uint64_t last_time = get_current_time();
    while (true) {
        while (get_current_time() - last_time < visual_wait_us && !end_flag) {;}
        last_time = get_current_time();
        if (end_flag) {
            break;
        }
        
        Entity *player = nullptr;
        for (Entity& entity : game.clevel->entities) {
            if (!entity.is_enemy) {
                player = &entity;
            }
        }


        /* handle dir */
        switch (wgetch(win)) {
            case 'w':
            case KEY_UP:
                player->pos.y--;
                break;
            
            case 's':
            case KEY_DOWN:
                player->pos.y++;
                break;

            case 'a':
            case KEY_LEFT:
                player->pos.x--;
                break;

            case 'd':
            case KEY_RIGHT:
                player->pos.x++;
                break;

            case 'q':
                end_flag = true;
                break;
                
            default:
                break;

        }
        player->pos.x = std::clamp(player->pos.x, 0, game.x - 1);
        player->pos.y = std::clamp(player->pos.y, 0, game.y - 1);

        if (end_flag) { break; }

        game.update(1.0L / unadjusted_fps);
        
        /* spawn particles */
        if (game.tick % 240 == 0) {
            for (std::int32_t i = 0; i < 5; i++) {
                Particle shot;
                shot.is_enemy = true;
                shot.damage = 1.0;
                shot.layer = 2;
                shot.pos = {1, i * 2};
                shot.tex = {TexChar{Vec2{}, L'i'}};
                shot.update = [y](Particle& self, double dt, std::uint64_t tick) { /* NOLINT */
                    if (tick % 30 == 0 && self.pos.y < y - 1) {
                        self.pos.y++;
                        self.tex = {TexChar{Vec2{}, L'i'}, TexChar{Vec2{-1, 0}, L' '}};
                    } else if (tick % 30 == 1 && self.pos.y < y) {
                        self.tex.pop_back(); /* remove */
                    }
                };
                game.clevel->particles.push_back(shot);
            }
        }
        game.render_layers();
        game.display_layers(win, 0, 0);
        move(30, 30);
        printw("y: %3i, x: %3i, health: %3.2f", player->pos.y, player->pos.x, player->health);
        wrefresh(win);

        bool over = game.is_over();
        if (over) {
            end_flag = true;
            break;
        }
    }


    deinit_ncurses(win);

    return 0;
}
