#pragma once
#include "Entity.h"

class Sand : public Entity {
public:
    Sand(int x_, int y_);
    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override;
    ftxui::Element draw() const override;
};
