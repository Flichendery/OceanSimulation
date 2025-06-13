#pragma once
#include "Entity.h"

class Algae : public Entity {
public:
    int growth_stage = 0;
    int max_height;
    int origin_y;

    Algae(int x_, int y_);
    Algae(int x_, int y_, int origin_y_, int max_height_);
    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override;
    ftxui::Element draw() const override;
};
