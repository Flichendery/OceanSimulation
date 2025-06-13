#pragma once
#include "Entity.h"
#include <vector>
#include <memory>

class Simulation {
public:
    Simulation(int width_, int height_);
    void update();
    std::vector<std::vector<Entity*>> get_grid() const;

private:
    int width, height;
    std::vector<std::vector<std::unique_ptr<Entity>>> entities;
};
