/**
 * @file piece.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Definition of chess piece
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

namespace Chess {
    namespace Piece {
        char GetPieceName (piece pieceToCompare, const bool ignoreSide) {
            if (ignoreSide) SetSide(pieceToCompare, white);

            if (pieceToCompare == empty) return '-';
            else if (pieceToCompare == blackPawn) return 'p';
            else if (pieceToCompare == blackKnight) return 'n';
            else if (pieceToCompare == blackBishop) return 'b';
            else if (pieceToCompare == blackRook) return 'r';
            else if (pieceToCompare == blackQueen) return 'q';
            else if (pieceToCompare == blackKing) return 'k';
            else if (pieceToCompare == whitePawn) return 'P';
            else if (pieceToCompare == whiteKnight) return 'N';
            else if (pieceToCompare == whiteBishop) return 'B';
            else if (pieceToCompare == whiteRook) return 'R';
            else if (pieceToCompare == whiteQueen) return 'Q';
            else if (pieceToCompare == whiteKing) return 'K';
            else return '\0';
        }
    }
}