#pragma once
#include "Entity.h"

class HerbivoreFish : public Entity {
public:
    int hunger = 15;
    int target_x = -1, target_y = -1;
    bool just_born = true;
    bool just_created = false;

    HerbivoreFish(int x_, int y_);
    void update(const std::vector<std::vector<Entity*>> &grid,
                std::vector<std::vector<Entity*>> &new_grid) override;
    ftxui::Element draw() const override;

private:
    bool find_nearest_algae(const std::vector<std::vector<Entity*>> &grid);
};
