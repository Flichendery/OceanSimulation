#include "Algae.h"
#include <cstdlib>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Algae::Algae(int x_, int y_) {
    x = x_;
    y = y_;
    type = ALGAE;
    origin_y = y_;
    max_height = 10 + rand() % 10;
}

Algae::Algae(int x_, int y_, int origin_y_, int max_height_) {
    x = x_;
    y = y_;
    type = ALGAE;
    origin_y = origin_y_;
    max_height = max_height_;
    growth_stage = origin_y_ - y_;
}

void Algae::update(const std::vector<std::vector<Entity*>> &grid,
                   std::vector<std::vector<Entity*>> &new_grid) {
    new_grid[y][x] = this;
    if (growth_stage < max_height && y > 0 &&
        grid[y - 1][x] == nullptr && new_grid[y - 1][x] == nullptr) {
        new_grid[y - 1][x] = new Algae(x, y - 1, origin_y, max_height);
    }
}

Element Algae::draw() const {
    return text("█") | color(Color::Green3) | bgcolor(Color::NavyBlue);
}
