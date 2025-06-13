#include "HerbivoreFish.h"
#include <cstdlib>
#include <algorithm>
#include "Algae.h"
#include "PredatorFish.h"
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

HerbivoreFish::HerbivoreFish(int x_, int y_) {
    x = x_;
    y = y_;
    type = HERBIVORE;
    just_born = true;
}

bool HerbivoreFish::find_nearest_algae(const std::vector<std::vector<Entity*>> &grid) {
    int best_dist = 9999;
    for (int yy = 0; yy < grid.size(); ++yy) {
        for (int xx = 0; xx < grid[0].size(); ++xx) {
            Entity* e = grid[yy][xx];
            if (e && e->type == ALGAE && !e->to_delete) {
                int d = abs(xx - x) + abs(yy - y);
                if (d < best_dist) {
                    best_dist = d;
                    target_x = xx;
                    target_y = yy;
                }
            }
        }
    }
    return target_x != -1;
}

void HerbivoreFish::update(const std::vector<std::vector<Entity*>> &grid,
                           std::vector<std::vector<Entity*>> &new_grid) {
    if (just_created) {
        just_created = false;
        new_grid[y][x] = this;
        return;
    }

    if (to_delete) return;

    static int dx[] = {0, 1, -1, 0};
    static int dy[] = {1, 0, 0, -1};
    int nx = x, ny = y;
    bool moved = false;

    if (target_x != -1 && target_y != -1) {
        Entity* t = grid[target_y][target_x];
        if (!t || t->to_delete || t->type != ALGAE) {
            target_x = target_y = -1;
        }
    }

    if (hunger < 15 && target_x == -1) find_nearest_algae(grid);

    if (target_x != -1 && target_y != -1) {
        int dx_move = (target_x > x) - (target_x < x);
        int dy_move = (target_y > y) - (target_y < y);

        int tx = x + dx_move;
        int ty = y;
        if (tx >= 0 && tx < grid[0].size() && ty >= 0 && ty < grid.size()) {
            Entity* e = grid[ty][tx];
            if (e && e->type == PREDATOR) { to_delete = true; return; }
            if (e && e->type == ALGAE) {
                e->to_delete = true;
                hunger = std::min(hunger + 2, 15);
                nx = tx; ny = ty;
                moved = true;
            } else if (!e && new_grid[ty][tx] == nullptr) {
                nx = tx; ny = ty;
                moved = true;
            }
        }

        if (!moved) {
            tx = x;
            ty = y + dy_move;
            if (tx >= 0 && tx < grid[0].size() && ty >= 0 && ty < grid.size()) {
                Entity* e = grid[ty][tx];
                if (e && e->type == PREDATOR) { to_delete = true; return; }
                if (e && e->type == ALGAE) {
                    e->to_delete = true;
                    hunger = std::min(hunger + 2, 15);
                    nx = tx; ny = ty;
                    moved = true;
                } else if (!e && new_grid[ty][tx] == nullptr) {
                    nx = tx; ny = ty;
                    moved = true;
                }
            }
        }
    }

    if (!moved) {
        hunger--;
        int dir = rand() % 4;
        int cx = x + dx[dir], cy = y + dy[dir];
        if (cx >= 0 && cx < grid[0].size() && cy >= 0 && cy < grid.size()) {
            Entity* occupant = grid[cy][cx];
            if ((!occupant || occupant->type != PREDATOR) && new_grid[cy][cx] == nullptr) {
                nx = cx;
                ny = cy;
            }
        }
    }

    if (hunger <= 0) { to_delete = true; return; }

    new_grid[ny][nx] = this;
    x = nx; y = ny;
    just_born = false;
}

Element HerbivoreFish::draw() const {
    return text("■") | color(Color::Orange1) | bgcolor(Color::NavyBlue);
}
