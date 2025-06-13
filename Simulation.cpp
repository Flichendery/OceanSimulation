#include "Simulation.h"
#include "Sand.h"
#include "Algae.h"
#include "HerbivoreFish.h"
#include "PredatorFish.h"
#include <cstdlib>
using namespace std;

Simulation::Simulation(int width_, int height_) : width(width_), height(height_) {
    entities.resize(height);
    for (auto& row : entities) {
        row.resize(width);
        for (auto& cell : row) {
            cell = nullptr;
        }
    }


    for (int y = height - 1; y > height - 3; --y)
        for (int x = 0; x < width; ++x)
            entities[y][x] = make_unique<Sand>(x, y);

    for (int i = 0; i < width / 5; ++i) {
        int x = rand() % width;
        int y = height - 3 - (rand() % 3);
        entities[y][x] = make_unique<Algae>(x, y);
    }

    for (int i = 0; i < width / 10; ++i) {
        int x = rand() % width;
        int y = rand() % (height - 5);
        entities[y][x] = make_unique<HerbivoreFish>(x, y);
    }

    for (int i = 0; i < width / 20; ++i) {
        int x = rand() % width;
        int y = rand() % (height - 5);
        entities[y][x] = make_unique<PredatorFish>(x, y);
    }
}

void Simulation::update() {
    vector<vector<Entity*>> grid(height, vector<Entity*>(width, nullptr));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            grid[y][x] = entities[y][x].get();

    vector<vector<Entity*>> new_grid(height, vector<Entity*>(width, nullptr));

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (grid[y][x] && !grid[y][x]->to_delete)
                grid[y][x]->update(grid, new_grid);

    std::vector<std::vector<std::unique_ptr<Entity>>> new_entities;
    new_entities.resize(height);

    for (int y = 0; y < height; ++y) {
        new_entities[y].reserve(width);
        for (int x = 0; x < width; ++x) {
            new_entities[y].emplace_back(nullptr);
        }
    }

    entities = move(new_entities);
}

vector<vector<Entity*>> Simulation::get_grid() const {
    vector<vector<Entity*>> result(height, vector<Entity*>(width));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            result[y][x] = entities[y][x].get();
    return result;
}
