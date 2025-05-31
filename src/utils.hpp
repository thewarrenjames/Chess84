/**
 * @file utils.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of useful variables and functions
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    extern clock_t programStartTime;

    extern const char* startPosition; // The starting position of a standard game of chess in fen notation.

    extern char defaultSaveName[MAX_SAVE_NAME + 1]; // The name of the game file saved on exit and loaded on startup.
    extern char helpPageName[MAX_SAVE_NAME + 1]; // The name of the file that stores the help screen.
    extern char gameSaveIdentifier[IDENTIFIER_SIZE]; // The first characters in a game file.
    extern char statisticsSaveIdentifier[IDENTIFIER_SIZE]; // The first characters in a stats file.
    extern char textSaveIdentifier[IDENTIFIER_SIZE]; // The first characters in a pgn file.

    extern gfx_sprite_t* sprites[NUMBER_OF_SPRITES];

    typedef uint32_t hash;
    typedef int16_t eval;

    bool StringCompare (const char* string1, const char* string2);

    /// @brief The current key being pressed.
    /// @param onlySinglePresses Will not return the key being pressed if it was the last key returned. If off, the key being pressed will be returned after the wait defined as CLOCKS_PER_HOLD.
    /// @return The key being pressed, otherwise 0.
    uint8_t Key (const bool onlySinglePresses = false);
    /// @brief Maps the provided key to the character denoted as ALPHA+KEY.
    /// @return 'A'-'Z', '0', or '\0' if no match.
    char GetCharacterForKey (const uint8_t key);
    const char IndexToCharacter[29] = "\0ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

    /// @brief Stores the center of square in x and y.
    /// @param square The index of a square
    /// @param x The x coord of the center of the square
    /// @param y The y coord of the center of the square
    void SquareToXY (const uint8_t square, uint16_t& x, uint16_t& y);

    /// @brief A shorthand function for drawing a rectangle that's centered on x and y.
    /// @param x The x of the rectangle's center
    /// @param y The y of the rectangle's center
    /// @param width 
    /// @param height Optional, if undefined, height becomes the provided width
    /// @param outlineWidth Only used if height is -2, in which case, you can only draw the outline of a square
    void Rectangle (const int16_t x, const int16_t y, const int16_t width, int16_t height = -1, const int16_t outlineWidth = 1);
    /// @brief A shorthand for drawing a sprite centered at x and y
    void Sprite (gfx_sprite_t* sprite, const int16_t x, const int16_t y);
    inline int16_t GetStringWidthOfInt (int64_t value) {
        int16_t width = 0;
        for (; value > 0; value /= 10) width += gfx_GetCharWidth(value % 10 + '0'); // Loop through each digit
        return width;
    }
    /// @brief Prints message to side bar on from the top line 0 to the bottom line 14.
    void PrintToSide (const char* message1, const uint8_t line, const bool blitText = false, const char* message2 = nullptr);
    void SetBackgroundOfLine (const uint8_t color, const uint8_t line, const bool blitLine = false);

    const char SquareToName[65][3] = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "--"
    };
    uint8_t NameToSquare (char file, const char rank);
    enum : uint8_t {
        A8, B8, C8, D8, E8, F8, G8, H8,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A1, B1, C1, D1, E1, F1, G1, H1, OffBoard
    };

    /**
     * @brief A single byte that represents a chess piece.
     *
     * The first 3 bits are unassigned.
     * The 4th bit indicates whether the piece is white.
     * The 5th bit indicates whether the piece is black.
     * The last 3 bits tell the type of piece.
     * @see piece.hpp
     */
    typedef uint8_t piece;
}