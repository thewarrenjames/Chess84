/**
 * @file statistics.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Defintion of statistics
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

namespace Chess {
    const char* Statistics::names[numberOfStatistics] = { "Name", "Elo", "Wins", "Wins as White", "Wins as Black", "Wins by Timeout", "Wins by Resignation", "Minimum Ply to Win", "Maximum Ply to Win", "Draws", "Draws as White", "Draws as Black", "Draws by Stalemate", "Draws by Insufficent Material", "Draws by Fifty Move Rule", "Draws by Threefold Repetition", "Draws Timeout & Insufficent Material", "Draws By Agreement", "Minimum Ply to Draw", "Maximum Ply to Draw", "Losses", "Losses as White", "Losses as Black", "Losses by Timeout", "Losses by Resignation", "Minimum Ply to Loss", "Maximum Ply to Loss", "Moves Played", "Pawn Moves Played", "Knight Moves Played", "Bishop Moves Played", "Rook Moves Played", "Queen Moves Played", "King Moves Played", "Checks Given", "Checks Given With A Pawn", "Checks Given With A Knight", "Checks Given With A Bishop", "Checks Given With A Rook", "Checks Given With A Queen", "Checks Given With A King", "Checkmates Given With A Pawn", "Checkmates Given With A Knight", "Checkmates Given With A Bishop", "Checkmates Given With A Rook", "Checkmates Given With A Queen", "Checkmates Given With A King", "Double Pawn Pushes Played", "Captures Played", "Pawn Captures Played", "Knight Captures Played", "Bishop Captures Played", "Rook Captures Played", "Queen Captures Played", "King Captures Played", "Castles Played", "King Side Castles Played", "Queen Side Castles Played", "En Passants Played", "Promotions Played", "Knight Promotions Played", "Bishop Promotions Played", "Rook Promotions Played", "Queen Promotions Played" };
}