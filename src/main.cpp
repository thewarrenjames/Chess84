/**
 * @file main.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief The root of the project
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

using namespace Chess;

Board board (startPosition);

int main () {
    gfx_Begin();
    gfx_SetDrawBuffer();

    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(TRANSPARENT_COLOR);
    gfx_SetTextTransparentColor(TRANSPARENT_COLOR);
    gfx_SetTextBGColor(TRANSPARENT_COLOR);
    gfx_SetTextScale(1, 1);

    //DEBUG dbg_printf("VERSION 1.0.0");
    board.Menu(board.mainMenu, 5);

    gfx_End();

    board.SaveGame(defaultSaveName, false);
}