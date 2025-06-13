#pragma once
#include <ftxui/dom/elements.hpp>
#include <vector>

enum EntityType { EMPTY, SAND, ALGAE, HERBIVORE, PREDATOR };

class Entity {
public:
    int x, y;
    EntityType type;
    bool to_delete = false;
    virtual ~Entity() {}
    virtual void update(const std::vector<std::vector<Entity*>> &grid,
                        std::vector<std::vector<Entity*>> &new_grid) = 0;
    virtual ftxui::Element draw() const = 0;
};
