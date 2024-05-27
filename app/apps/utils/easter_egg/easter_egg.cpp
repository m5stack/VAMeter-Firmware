/**
 * @file easter_egg.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "easter_egg.h"
#include "raylib/raylib.h"
#include "../system/system.h"
#include "../../../assets/assets.h"
#include <string>
#include <vector>

using namespace SYSTEM::UI;

void EasterEgg::CreateRandomEasterEgg()
{
    // Roll the dice
    auto the_one = GetRandomValue(0, 3);
    // the_one = 3;

    switch (the_one)
    {
    case 0:
        start_arkanoid();
        break;
    case 1:
        start_tetris();
        break;
    case 2:
        start_pang();
        break;
    case 3:
        start_floppy();
        break;
    default:
        break;
    }
}

void EasterEgg::CreateEasterEggMenu()
{
    SelectMenuPage::Theme_t theme;
    theme.background = AssetPool::GetColor().AppSettings.background;
    theme.optionText = AssetPool::GetColor().AppSettings.optionText;
    theme.selector = AssetPool::GetColor().AppSettings.selector;

    std::vector<std::string> option_list = {"Arkanoid", "Tetris", "Pang", "Floppy"};
    int the_one = 0;
    while (1)
    {
        the_one = SelectMenuPage::CreateAndWaitResult("o.O??", option_list, the_one, &theme);
        if (the_one < 0)
            break;

        switch (the_one)
        {
        case 0:
            start_arkanoid();
            break;
        case 1:
            start_tetris();
            break;
        case 2:
            start_pang();
            break;
        case 3:
            start_floppy();
            break;
        default:
            break;
        }
    }
}
