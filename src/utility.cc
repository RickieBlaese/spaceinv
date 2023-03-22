#include "utility.h"


WINDOW *init_ncurses() {
    WINDOW *win = initscr();
    cbreak();
    keypad(win, true);
    wclear(win);
    return win;
}

void deinit_ncurses(WINDOW *win) {
    wclear(win);
    endwin();
}


/* in microseconds */
std::uint64_t get_current_time() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}


double Vec2::mod() {
    return std::sqrt(y * y - x * x);
}

Vec2 Vec2::operator-() const {
    return {-y, -x};
}

Vec2 Vec2::operator-(const Vec2& other) const {
    return *this + (-other);
}

Vec2 Vec2::operator-=(const Vec2& other) {
    return *this = *this - other;
}

Vec2 Vec2::operator+(const Vec2& other) const {
    return {y + other.y, x + other.x};
}

Vec2 Vec2::operator+=(const Vec2& other) {
    return *this = *this + other;
}


Direction chtype_to_dir(chtype in) {
    switch (in) {
        case 'w':
        case KEY_UP:
            return Direction::up;
            break;

        case 'a':
        case KEY_LEFT:
            return Direction::left;
            break;

        case 's':
        case KEY_DOWN:
            return Direction::down;
            break;

        case 'd':
        case KEY_RIGHT:
            return Direction::right;
            break;

        default:
            return Direction::none;
            break;

    }
}
