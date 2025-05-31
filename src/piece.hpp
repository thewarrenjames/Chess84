/**
 * @file piece.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of chess piece
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    namespace Piece {
        enum : const uint8_t {
            empty,
            pawn,
            knight,
            bishop,
            rook,
            queen,
            king,
            black = 0x08,
            blackPawn,
            blackKnight,
            blackBishop,
            blackRook,
            blackQueen,
            blackKing,
            white = 0x10,
            whitePawn,
            whiteKnight,
            whiteBishop,
            whiteRook,
            whiteQueen,
            whiteKing
        };

        const uint8_t pieceMask = 0x7;
        const uint8_t sideMask = 0x18;

        inline uint8_t GetPiece (const piece data) { return (data & pieceMask); }
        inline uint8_t GetSide (const piece data) { return data & sideMask; }
        inline bool IsPiece (const piece pieceToCompare, const piece pieceToReference) { return GetPiece(pieceToCompare) == GetPiece(pieceToReference); }
        inline bool IsSide (const piece pieceToCompare, const piece side) { return GetSide(pieceToCompare) == side; }
        inline void SetPiece (piece& pieceToSet, const piece toPiece) { pieceToSet &= sideMask; pieceToSet |= toPiece & pieceMask; }
        inline void SetSide (piece& pieceToSet, const piece side) { pieceToSet &= pieceMask; pieceToSet |= side; }

        inline piece GetOppositeSide (piece pieceToReference) { return GetSide(pieceToReference) == white ? black : white; }
        inline void SetToOppositeSide (piece& pieceToReference) { SetSide(pieceToReference, GetOppositeSide(pieceToReference)); }

        char GetPieceName (piece pieceToCompare, const bool ignoreSide = false);

        inline uint8_t GetLinearValue (const piece pieceToReference) { return pieceToReference - 9 - (IsSide(pieceToReference, white) ? 2 : 0); }
    }
}