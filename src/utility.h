#ifndef _UTILITY_H
#define _UTILITY_H

#include <iostream>
#include <vector>
#include <chrono>
#include <exception>
#include <functional>
#include <mutex>
#include <atomic>
#include <string>
#include <queue>
#include <thread>

#include <cinttypes>
#include <cmath>

#include <ncurses.h>


WINDOW *init_ncurses();

void deinit_ncurses(WINDOW *win);

std::uint64_t get_current_time();


struct Vec2 { /* NOLINT */
    std::int32_t y = 0.0, x = 0.0;

    double mod();

    Vec2& operator=(const Vec2& other) = default;

    Vec2 operator-() const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator-=(const Vec2& other);
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator+=(const Vec2& other);

};

enum Direction : std::uint32_t {
    up, down, left, right, none
};

Direction chtype_to_dir(chtype in);


#endif
