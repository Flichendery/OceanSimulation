#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/string.hpp>
#include <thread>
#include <chrono>
#include "Simulation.h"

using namespace ftxui;

int main() {
    constexpr int width = 50;
    constexpr int height = 30;

    Simulation sim(width, height);

    while (true) {
        sim.update();
        auto grid = sim.get_grid();

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

        auto doc = vbox(std::move(rows));
        auto screen = Screen::Create(Dimension::Fixed(width), Dimension::Fixed(height));
        Render(screen, doc);
        screen.Print();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    return 0;
}
