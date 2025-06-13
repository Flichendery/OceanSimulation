#include "Sand.h"
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Sand::Sand(int x_, int y_) {
    x = x_;
    y = y_;
    type = SAND;
}

void Sand::update(const std::vector<std::vector<Entity*>> &,
                  std::vector<std::vector<Entity*>> &new_grid) {
    new_grid[y][x] = this;
}

Element Sand::draw() const {
    return text("█") | color(Color::YellowLight) | bgcolor(Color::NavyBlue);
}
