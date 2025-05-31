/**
 * @file utils.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Definitions of useful variables and functions
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

namespace Chess {
    clock_t programStartTime = clock();

    const char* startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    char defaultSaveName[MAX_SAVE_NAME + 1] = "CHESNORM";
    char helpPageName[MAX_SAVE_NAME + 1] = "CHESHELP";
    char gameSaveIdentifier[IDENTIFIER_SIZE] = "GAME";
    char statisticsSaveIdentifier[IDENTIFIER_SIZE] = "STAT";
    char textSaveIdentifier[IDENTIFIER_SIZE] = "TEXT";

    gfx_sprite_t* sprites[NUMBER_OF_SPRITES] = { Neo_Black_Pawn, Neo_Black_Knight, Neo_Black_Bishop, Neo_Black_Rook, Neo_Black_Queen, Neo_Black_King, Neo_White_Pawn, Neo_White_Knight, Neo_White_Bishop, Neo_White_Rook, Neo_White_Queen, Neo_White_King, Classic_Black_Pawn, Classic_Black_Knight, Classic_Black_Bishop, Classic_Black_Rook, Classic_Black_Queen, Classic_Black_King, Classic_White_Pawn, Classic_White_Knight, Classic_White_Bishop, Classic_White_Rook, Classic_White_Queen, Classic_White_King };

    bool StringCompare (const char* string1, const char* string2) {
        for (; *string1 == *string2; string1++, string2++) {
            if (*string1 == 0 && *string1 == *string2) return true;
        }

        return false;
    }

    uint8_t Key (const bool onlySinglePresses) {
        static uint8_t last_key;
        static clock_t timeOfLastKeyPress;
        static uint8_t only_key;
        bool keyIsDown = false;

        kb_Scan();

        for (uint8_t key = 1, group = 7; group && !keyIsDown; --group) {
            for (uint8_t mask = 1; mask && !keyIsDown; mask <<= 1, ++key) {
                if (kb_Data[group] & mask) {
                    keyIsDown = true;
                    only_key = key;
                }
            }
        }

        if (!keyIsDown) return only_key = last_key = 0;

        if (only_key == last_key && (onlySinglePresses || only_key == sk_Clear || only_key == sk_Del || clock() - timeOfLastKeyPress < CLOCKS_PER_HOLD)) return 0;

        timeOfLastKeyPress = clock();

        last_key = only_key;

        return only_key;
    }

    char GetCharacterForKey (const uint8_t key) {
        switch (key) {
            case sk_Math:   return  'A';
            case sk_Apps:   return  'B';
            case sk_Prgm:   return  'C';
            case sk_Recip:  return  'D';
            case sk_Sin:    return  'E';
            case sk_Cos:    return  'F';
            case sk_Tan:    return  'G';
            case sk_Power:  return  'H';
            case sk_Square: return  'I';
            case sk_Comma:  return  'J';
            case sk_LParen: return  'K';
            case sk_RParen: return  'L';
            case sk_Div:    return  'M';
            case sk_Log:    return  'N';
            case sk_7:      return  'O';
            case sk_8:      return  'P';
            case sk_9:      return  'Q';
            case sk_Mul:    return  'R';
            case sk_Ln:     return  'S';
            case sk_4:      return  'T';
            case sk_5:      return  'U';
            case sk_6:      return  'V';
            case sk_Sub:    return  'W';
            case sk_Store:  return  'X';
            case sk_1:      return  'Y';
            case sk_2:      return  'Z';
            case sk_Del:    return  '0';
            default:        return '\0';
        }
    }

    void SquareToXY (const uint8_t square, uint16_t& x, uint16_t& y) {
        x = (square % 8 + 0.5) * SQUARE_SIZE;
        y = (square / 8 + 0.5) * SQUARE_SIZE;
    }

    void Rectangle (const int16_t x, const int16_t y, const int16_t width, int16_t height, const int16_t outlineWidth) {
        height = height < 0 ? width : height; // When height isn't defined, it is -1, so if height isn't given, it will be a square.
        if (height != -2) gfx_FillRectangle(x - width / 2, y - height / 2, width, height);
        else  for (int16_t i = 0; i < outlineWidth; i++) gfx_Rectangle(x - (width - i) / 2, y - (height - i) / 2, width - i, height - i); // If height is -2, it will draw the outline of the square.
    }

    void Sprite (gfx_sprite_t* sprite, const int16_t x, const int16_t y) {
        gfx_TransparentSprite(sprite, x - PIECE_SIZE / 2, y - PIECE_SIZE / 2);
    }

    void PrintToSide (const char* message1, const uint8_t line, const bool blitText, const char* message2) {
        if (!message1) return;

        const int16_t x = 8 * SQUARE_SIZE + 5, y = line * SQUARE_SIZE / 2 + 11;
        gfx_PrintStringXY(message1, x, y);

        const int16_t lengthInPixels = gfx_GetStringWidth(message1);

        if (blitText) gfx_BlitRectangle(gfx_buffer, x, y, lengthInPixels + 1, y + 8);

        if (!message2) return;

        gfx_PrintStringXY(message2, x + lengthInPixels, y);

        if (blitText) gfx_BlitRectangle(gfx_buffer, x + lengthInPixels, y, gfx_GetStringWidth(message2) + 1, y + 8);
    }

    void SetBackgroundOfLine (const uint8_t color, const uint8_t line, const bool blitLine) {
        const int16_t x = 8 * SQUARE_SIZE, y = line * SQUARE_SIZE / 2 + 7;

        gfx_SetColor(color);
        gfx_FillRectangle(x, y, GFX_LCD_WIDTH - x, SQUARE_SIZE / 2);

        if (blitLine) gfx_BlitRectangle(gfx_buffer, x + 5, y + 4, GFX_LCD_WIDTH - x - 5, y + 12);
    }

    uint8_t NameToSquare (char file, const char rank) {
        if (file == '-') return OffBoard;
        else if (file == 'a') file = 0;
        else if (file == 'b') file = 1;
        else if (file == 'c') file = 2;
        else if (file == 'd') file = 3;
        else if (file == 'e') file = 4;
        else if (file == 'f') file = 5;
        else if (file == 'g') file = 6;
        else if (file == 'h') file = 7;

        return file + (8 - uint8_t(rank - '0')) * 8;
    }
}