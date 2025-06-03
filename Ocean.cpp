#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <atomic>
#include <chrono>

using namespace ftxui;

constexpr int width = 120;   // уменьшил для удобства и нормального отображения
constexpr int height = 20;

enum EntityType { EMPTY, SAND, ALGAE, HERBIVORE, PREDATOR };

struct Entity {
    int x, y;
    EntityType type;
    bool to_delete = false;
    virtual ~Entity() {}
    virtual void update(const std::vector<std::vector<Entity*>> &grid, std::vector<std::vector<Entity*>> &new_grid) = 0;
    virtual Element draw() const = 0;
};

struct Sand : public Entity {
    Sand(int x_, int y_) { x = x_; y = y_; type = SAND; }
    void update(const std::vector<std::vector<Entity*>> & /*grid*/, std::vector<std::vector<Entity*>> &new_grid) override {
        new_grid[y][x] = this;
    }
    Element draw() const override {
        return text("█") | color(Color::YellowLight) | bgcolor(Color::NavyBlue);
    }
};

struct Algae : public Entity {
    int growth_stage = 0;
    Algae(int x_, int y_) { x = x_; y = y_; type = ALGAE; }
    void update(const std::vector<std::vector<Entity*>> &grid, std::vector<std::vector<Entity*>> &new_grid) override {
        new_grid[y][x] = this;
        if (growth_stage < 3 && y > 0 && grid[y-1][x] == nullptr && new_grid[y-1][x] == nullptr) {
            new_grid[y-1][x] = new Algae(x, y-1);
            growth_stage++;
        }
    }
    Element draw() const override {
        return text("|") | color(Color::Green3) | bgcolor(Color::NavyBlue);
    }
};

struct HerbivoreFish : public Entity {
    int hunger = 5;
    int reproduce_timer = 0;

    HerbivoreFish(int x_, int y_) { x = x_; y = y_; type = HERBIVORE; }

    void update(const std::vector<std::vector<Entity*>> &grid, std::vector<std::vector<Entity*>> &new_grid) override {
        static int dx[] = {0, 1, -1, 0};
        static int dy[] = {1, 0, 0, -1};

        int nx = x, ny = y;
        bool ate = false;

        for (int i = 0; i < 4; ++i) {
            int cx = x + dx[i], cy = y + dy[i];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height) {
                if (grid[cy][cx] && grid[cy][cx]->type == ALGAE) {
                    ate = true;
                    nx = cx; ny = cy;
                    hunger = 5;
                    break;
                }
            }
        }

        if (!ate) {
            hunger--;
            int dir = rand() % 4;
            int cx = x + dx[dir], cy = y + dy[dir];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height && grid[cy][cx] == nullptr && new_grid[cy][cx] == nullptr) {
                nx = cx; ny = cy;
            }
        }

        if (hunger <= 0) {
            to_delete = true;
            return;
        }

        reproduce_timer++;

        if (reproduce_timer > 10) {
            reproduce_timer = 0;
            for (int i = 0; i < 4; ++i) {
                int cx = nx + dx[i], cy = ny + dy[i];
                if (cx >= 0 && cx < width && cy >= 0 && cy < height && grid[cy][cx] == nullptr && new_grid[cy][cx] == nullptr) {
                    new_grid[cy][cx] = new HerbivoreFish(cx, cy);
                    break;
                }
            }
        }

        new_grid[ny][nx] = this;
        x = nx; y = ny;
    }

    Element draw() const override {
        return text("■") | color(Color::Orange1) | bgcolor(Color::NavyBlue);
    }
};

struct PredatorFish : public Entity {
    int hunger = 7;
    int reproduce_timer = 0;

    PredatorFish(int x_, int y_) { x = x_; y = y_; type = PREDATOR; }

    void update(const std::vector<std::vector<Entity*>> &grid, std::vector<std::vector<Entity*>> &new_grid) override {
        static int dx[] = {0, 1, -1, 0};
        static int dy[] = {1, 0, 0, -1};
        int nx = x, ny = y;
        bool ate = false;

        for (int i = 0; i < 4; ++i) {
            int cx = x + dx[i], cy = y + dy[i];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height) {
                if (grid[cy][cx] && grid[cy][cx]->type == HERBIVORE) {
                    ate = true;
                    nx = cx; ny = cy;
                    hunger = 7;
                    break;
                }
            }
        }

        if (!ate) {
            hunger--;
            int dir = rand() % 4;
            int cx = x + dx[dir], cy = y + dy[dir];
            if (cx >= 0 && cx < width && cy >= 0 && cy < height && grid[cy][cx] == nullptr && new_grid[cy][cx] == nullptr) {
                nx = cx; ny = cy;
            }
        }

        if (hunger <= 0) {
            to_delete = true;
            return;
        }

        reproduce_timer++;

        if (reproduce_timer > 15) {
            reproduce_timer = 0;
            for (int i = 0; i < 4; ++i) {
                int cx = nx + dx[i], cy = ny + dy[i];
                if (cx >= 0 && cx < width && cy >= 0 && cy < height && grid[cy][cx] == nullptr && new_grid[cy][cx] == nullptr) {
                    new_grid[cy][cx] = new PredatorFish(cx, cy);
                    break;
                }
            }
        }

        new_grid[ny][nx] = this;
        x = nx; y = ny;
    }

    Element draw() const override {
        return text("■") | color(Color::Red) | bgcolor(Color::NavyBlue);
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

void update_simulation() {
    std::vector<std::vector<Entity*>> new_grid(height, std::vector<Entity*>(width, nullptr));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] && !grid[y][x]->to_delete) {
                grid[y][x]->update(grid, new_grid);
            }
        }
    }

    // Удаляем объекты, помеченные на удаление
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] && grid[y][x]->to_delete) {
                delete grid[y][x];
                grid[y][x] = nullptr;
            }
        }
    }

    grid = std::move(new_grid);

    // Появление новых водорослей с небольшой вероятностью
    if (rand() % 100 < 5) {
        int x = rand() % width;
        int y = height - 4;
        if (!grid[y][x] && grid[y + 1][x] && grid[y + 1][x]->type == SAND) {
            grid[y][x] = new Algae(x, y);
        }
    }

    // Появление травоядных с небольшой вероятностью
    if (rand() % 100 < 3) {
        int x = rand() % width;
        int y = rand() % (height - 4);
        if (!grid[y][x]) {
            grid[y][x] = new HerbivoreFish(x, y);
        }
    }

    // Появление хищников ещё реже
    if (rand() % 200 < 1) {
        int x = rand() % width;
        int y = rand() % (height - 4);
        if (!grid[y][x]) {
            grid[y][x] = new PredatorFish(x, y);
        }
    }
}

int main() {
    srand(time(nullptr));
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

    // Поток обновления симуляции
    std::thread update_thread([&]() {
        while (running) {
            update_simulation();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            screen.PostEvent(Event::Custom); // чтобы перерисовать
        }
    });

    screen.Loop(main_loop);

    running = false;
    update_thread.join();

    cleanup_grid();

    return 0;
}
