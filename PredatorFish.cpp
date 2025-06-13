#include "PredatorFish.h"
#include "HerbivoreFish.h"
#include <cstdlib>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

PredatorFish::PredatorFish(int x_, int y_) {
    x = x_;
    y = y_;
    type = PREDATOR;
}

void PredatorFish::update(const std::vector<std::vector<Entity*>> &grid,
                          std::vector<std::vector<Entity*>> &new_grid) {
    if (to_delete) return;

    static int dx[] = {0, 1, -1, 0};
    static int dy[] = {1, 0, 0, -1};
    int nx = x, ny = y;
    bool moved = false;

    for (int d = 0; d < 4 && !moved; ++d) {
        int cx = x + dx[d], cy = y + dy[d];
        if (cx >= 0 && cx < grid[0].size() && cy >= 0 && cy < grid.size()) {
            Entity* e = grid[cy][cx];
            if (e && e->type == HERBIVORE) {
                e->to_delete = true;
                hunger = std::min(hunger + 5, 20);
                nx = cx; ny = cy;
                moved = true;
            }
        }
    }

    if (!moved) {
        hunger--;
        int d = rand() % 4;
        int cx = x + dx[d], cy = y + dy[d];
        if (cx >= 0 && cx < grid[0].size() && cy >= 0 && cy < grid.size()) {
            Entity* e = grid[cy][cx];
            if (!e && new_grid[cy][cx] == nullptr) {
                nx = cx;
                ny = cy;
            }
        }
    }

    if (hunger <= 0) {
        to_delete = true;
        return;
    }

    new_grid[ny][nx] = this;
    x = nx;
    y = ny;
}

Element PredatorFish::draw() const {
    return text("■") | color(Color::RedLight) | bgcolor(Color::NavyBlue);
}
