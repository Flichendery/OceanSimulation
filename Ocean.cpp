#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <random>

using namespace ftxui;

constexpr int width = 240;
constexpr int height = 40;

enum EntityType { EMPTY, SAND, ALGAE, HERBIVORE, PREDATOR };

class Entity {
public:
    int x, y;
    EntityType type;
    bool to_delete = false;
    virtual ~Entity() {}
    virtual void update(const std::vector<std::vector<Entity*>> &grid,
                        std::vector<std::vector<Entity*>> &new_grid) = 0;
    virtual Element draw() const = 0;
};

class Sand : public Entity {
public:
    Sand(int x_, int y_) {
        x = x_;
        y = y_;
        type = SAND;
    }

    void update(const std::vector<std::vector<Entity*>> & /*grid*/,
                std::vector<std::vector<Entity*>> &new_grid) override {
        new_grid[y][x] = this;
    }

    Element draw() const override {
        return text("█") | color(Color::YellowLight) | bgcolor(Color::NavyBlue);
    }
};

class Algae : public Entity {
public:
    int growth_stage = 0;
    int max_height;
    int origin_y;

    Algae(int x_, int y_) {
        x = x_;
        y = y_;
        type = ALGAE;
        origin_y = y_;
        max_height = 10 + rand() % 10;
    }

    Algae(int x_, int y_, int origin_y_, int max_height_) {
        x = x_;
        y = y_;
        type = ALGAE;
        origin_y = origin_y_;
        max_height = max_height_;
        growth_stage = origin_y_ - y_;
    }

    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override {
        new_grid[y][x] = this;

        if (growth_stage < max_height && y > 0 &&
            grid[y - 1][x] == nullptr && new_grid[y - 1][x] == nullptr) {
            new_grid[y - 1][x] = new Algae(x, y - 1, origin_y, max_height);
        }
    }

    Element draw() const override {
        return text("█") | color(Color::Green3) | bgcolor(Color::NavyBlue);
    }
};


class HerbivoreFish : public Entity {
public:
    int hunger = 15;
    int target_x = -1, target_y = -1;
    bool just_born = true;
    bool just_created = false;

    HerbivoreFish(int x_, int y_) {
        x = x_;
        y = y_;
        type = HERBIVORE;
        just_born = true;
    }

    bool find_nearest_algae(const std::vector<std::vector<Entity*>> &grid) {
        int best_dist = width + height;
        int found_x = -1, found_y = -1;
        for (int yy = 0; yy < height; ++yy) {
            for (int xx = 0; xx < width; ++xx) {
                if (grid[yy][xx] && grid[yy][xx]->type == ALGAE && !grid[yy][xx]->to_delete) {
                    int dist = abs(x - xx) + abs(y - yy);
                    if (dist < best_dist) {
                        best_dist = dist;
                        found_x = xx;
                        found_y = yy;
                    }
                }
            }
        }
        if (found_x != -1) {
            target_x = found_x;
            target_y = found_y;
            return true;
        }
        target_x = target_y = -1;
        return false;
    }

    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override {
        if (just_created) {
            just_created = false;
            new_grid[y][x] = this;
            return;
        }

        if (to_delete) return;

        static int dx[] = {0, 1, -1, 0};
        static int dy[] = {1, 0, 0, -1};
        int nx = x, ny = y;

        bool has_target = false;
        if (target_x != -1 && target_y != -1) {
            Entity* t = grid[target_y][target_x];
            if (t && !t->to_delete && t->type == ALGAE)
                has_target = true;
        }

        if (hunger < 15 && !has_target) {
            find_nearest_algae(grid);
        }

        bool moved = false;

        if (target_x != -1 && target_y != -1) {
            int dx_move = (target_x > x) - (target_x < x);
            int dy_move = (target_y > y) - (target_y < y);

            int tx = x + dx_move;
            int ty = y;

            if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
                Entity* e = grid[ty][tx];
                if (e && e->type == PREDATOR) {
                    to_delete = true;
                    return;
                }
                if (e && e->type == ALGAE) {
                    e->to_delete = true;
                    hunger = std::min(hunger + 2, 15);
                    target_x = target_y = -1;
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

                if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
                    Entity* e = grid[ty][tx];
                    if (e && e->type == PREDATOR) {
                        to_delete = true;
                        return;
                    }
                    if (e && e->type == ALGAE) {
                        e->to_delete = true;
                        hunger = std::min(hunger + 2, 15);
                        target_x = target_y = -1;
                        nx = tx; ny = ty;
                        moved = true;
                    } else if (!e && new_grid[ty][tx] == nullptr) {
                        nx = tx; ny = ty;
                        moved = true;
                    }
                }
            }

            if (!moved) {
                target_x = target_y = -1;
            }
        }

        if (!moved) {
            hunger--;
            int dir = rand() % 4;
            int cx = x + dx[dir], cy = y + dy[dir];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height) {
                Entity* occupant = grid[cy][cx];
                if ((occupant == nullptr || occupant->type != PREDATOR) && new_grid[cy][cx] == nullptr) {
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
        x = nx; y = ny;
        just_born = false;
    }



    Element draw() const override {
        return text("■") | color(Color::Orange1) | bgcolor(Color::NavyBlue);
    }
};


class PredatorFish : public Entity {
public:
    int hunger = 25;
    bool chasing = false;
    int target_x = -1, target_y = -1;
    int wander_timer = 0;
    bool just_born = true;
    bool just_created = false;

    PredatorFish(int x_, int y_) {
        x = x_;
        y = y_;
        type = PREDATOR;
        just_born = true;
    }

    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override {
        if (just_created) {
            just_created = false;
            new_grid[y][x] = this;
            return;
        }

        if (to_delete) return;

        static int dx[] = {0, 1, -1, 0};
        static int dy[] = {1, 0, 0, -1};

        int nx = x, ny = y;
        bool ate = false;

        if (wander_timer > 0) {
            wander_timer--;
            hunger--;
            int dir = rand() % 4;
            int cx = x + dx[dir], cy = y + dy[dir];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height &&
                grid[cy][cx] == nullptr && new_grid[cy][cx] == nullptr) {
                nx = cx; ny = cy;
            }
            if (hunger <= 0) {
                to_delete = true;
                return;
            }
            new_grid[ny][nx] = this;
            x = nx; y = ny;
            just_born = false;
            return;
        }

        if (chasing) {
            Entity* target = nullptr;
            if (target_x >= 0 && target_y >= 0 && target_x < width && target_y < height)
                target = grid[target_y][target_x];
            if (!target || target->to_delete || target->type != HERBIVORE) {
                chasing = false;
                target_x = target_y = -1;
            }
        }

        if (!chasing) {
            int best_dist = width + height;
            for (int yy = 0; yy < height; ++yy) {
                for (int xx = 0; xx < width; ++xx) {
                    if (grid[yy][xx] && grid[yy][xx]->type == HERBIVORE && !grid[yy][xx]->to_delete) {
                        int dist = abs(x - xx) + abs(y - yy);
                        if (dist < best_dist) {
                            best_dist = dist;
                            target_x = xx;
                            target_y = yy;
                        }
                    }
                }
            }
            chasing = (target_x != -1);
        }

        if (chasing) {
            int dx_move = (target_x > x) - (target_x < x);
            int dy_move = (target_y > y) - (target_y < y);

            int tx = x + dx_move;
            int ty = y;

            if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
                Entity* e = grid[ty][tx];
                if (e && e->type == HERBIVORE) {
                    e->to_delete = true;
                    hunger = 25;
                    chasing = false;
                    target_x = target_y = -1;
                    wander_timer = 5 + rand() % 5;
                    nx = tx; ny = ty;
                    ate = true;
                } else if (!e && new_grid[ty][tx] == nullptr) {
                    nx = tx; ny = ty;
                }
            }

            if (!ate) {
                ty = y + dy_move;
                tx = x;
                if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
                    Entity* e = grid[ty][tx];
                    if (e && e->type == HERBIVORE) {
                        e->to_delete = true;
                        hunger = 25;
                        chasing = false;
                        target_x = target_y = -1;
                        wander_timer = 5 + rand() % 5;
                        nx = tx; ny = ty;
                        ate = true;
                    } else if (!e && new_grid[ty][tx] == nullptr) {
                        nx = tx; ny = ty;
                    }
                }
            }
        }

        if (!ate) hunger--;
        if (hunger <= 0) {
            to_delete = true;
            return;
        }

        new_grid[ny][nx] = this;
        x = nx; y = ny;
        just_born = false;
    }

    Element draw() const override {
        return text("■") | color(Color::Red3) | bgcolor(Color::NavyBlue);
    }
};


std::vector<std::vector<Entity*>> grid(height, std::vector<Entity*>(width, nullptr));

Element render_grid() {
    Elements rows;
    for (int y = 0; y < height; ++y) {
        Elements row;
        for (int x = 0; x < width; ++x) {
            if (grid[y][x])
                row.push_back(grid[y][x]->draw());
            else
                row.push_back(text(" ") | bgcolor(Color::NavyBlue));
        }
        rows.push_back(hbox(std::move(row)));
    }
    return vbox(std::move(rows)) | bgcolor(Color::NavyBlue);
}

void initialize_sand() {
    for (int y = height - 3; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!grid[y][x])
                grid[y][x] = new Sand(x, y);
        }
    }
}

void cleanup_grid() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            delete grid[y][x];
            grid[y][x] = nullptr;
        }
    }
}

int tick_count = 0;

void update_simulation() {
    tick_count++;
    std::vector<std::vector<Entity*>> new_grid(height, std::vector<Entity*>(width, nullptr));

    // Появление
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] && !grid[y][x]->to_delete) {
                grid[y][x]->update(grid, new_grid);
            }
        }
    }

    // Удаление объектов
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] && grid[y][x]->to_delete) {
                delete grid[y][x];
                grid[y][x] = nullptr;
            }
        }
    }

    grid = std::move(new_grid);

    // Появление водорослей
    if (tick_count < 100 && rand() % 100 < 55) {
        int x = rand() % width;
        int y = height - 4;

        bool nearby_algae = false;
        int min_dist = 1 + rand() % 3;  // расстояние 1–3 клетки

        for (int dx = -min_dist; dx <= min_dist; ++dx) {
            int cx = x + dx;
            if (cx >= 0 && cx < width && grid[y][cx] && grid[y][cx]->type == ALGAE) {
                nearby_algae = true;
                break;
            }
        }

        if (!nearby_algae && !grid[y][x] && grid[y + 1][x] && grid[y + 1][x]->type == SAND) {
            grid[y][x] = new Algae(x, y);
        }
    }

    // Появление травоядных
    if (tick_count < 150 && rand() % 100 < 40) {
        int x = rand() % width;
        int y = rand() % (height - 4);
        if (!grid[y][x]) {
            grid[y][x] = new HerbivoreFish(x, y);
        }
    }

    // Появление хищников
    if (tick_count < 150 && rand() % 100 < 10) {
        int x = rand() % width;
        int y = rand() % (height - 4);
        if (!grid[y][x]) {
            grid[y][x] = new PredatorFish(x, y);
        }
    }
}


int main() {
    srand(time(NULL));
    initialize_sand();

    auto screen = ScreenInteractive::TerminalOutput();

    std::atomic<bool> running = true;

    auto simulation = Renderer([] {
        return render_grid();
    });

    auto main_loop = CatchEvent(simulation, [&](Event event) {
        if (event == Event::Character('q')) {
            running = false;
            screen.Exit();
        }
        return true;
    });

    std::thread update_thread([&]() {
        while (running) {
            update_simulation();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            screen.PostEvent(Event::Custom);
        }
    });

    screen.Loop(main_loop);

    running = false;
    update_thread.join();

    cleanup_grid();

    return 0;
}
