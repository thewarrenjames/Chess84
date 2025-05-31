/**
 * @file move.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of chess move
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    struct Move {
        /**
         * @brief A 16 bit integer that stores the type of move, the starting square, and the target square.
         *
         * The first 4 bits are the flag, the next 6 are the starting square as defined in utils.hpp, and the last 6 are the target square.
         * @sa utils.hpp
         */
        uint16_t data = 0;
        piece pieceMoved = 0;
        piece pieceCaptured = 0;
        bool check = false;
        char algebraicNotation[MAX_ALGEBRAIC_NOTATION_LENGTH] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; // The notation without a trailing \0.

        // The first bit of a flag signals a promotion.
        // The second signals a capture.
        
        static const uint8_t quietMove              = 0b0000;
        static const uint8_t doublePawnPush         = 0b0001;
        static const uint8_t kingCastle             = 0b0010;
        static const uint8_t queenCastle            = 0b0011;
        static const uint8_t capture                = 0b0100;
        static const uint8_t enPassant              = 0b0101;
        static const uint8_t knightPromotion        = 0b1000;
        static const uint8_t bishopPromotion        = 0b1001;
        static const uint8_t rookPromotion          = 0b1010;
        static const uint8_t queenPromotion         = 0b1011;
        static const uint8_t knightPromotionCapture = 0b1100;
        static const uint8_t bishopPromotionCapture = 0b1101;
        static const uint8_t rookPromotionCapture   = 0b1110;
        static const uint8_t queenPromotionCapture  = 0b1111;

        static const uint16_t fourBitMask = 0xF;
        static const uint16_t sixBitMask = 0x3F;
        static const uint16_t inverseFlagMask = 0xFFF;
        static const uint16_t inverseStartMask = 0xF03F;
        static const uint16_t inverseTargetMask = 0xFFC0;
        static const uint16_t captureMask = 0x4000;
        static const uint16_t promotionMask = 0x8000;
        static const uint16_t inverseCaptureMask = 0xB000;

        static inline uint16_t GetFlag (const Move& move) { return (move.data >> 12) & fourBitMask; }
        static inline uint16_t GetStart (const Move& move) { return (move.data >> 6) & sixBitMask; }
        static inline uint16_t GetTarget (const Move& move) { return move.data & sixBitMask; }
        static inline bool IsFlag (const Move& move, const uint8_t flag) { return GetFlag(move) == flag; }
        static inline void SetFlag (Move& move, const uint16_t flag) { move.data &= inverseFlagMask; move.data |= (flag & fourBitMask) << 12; }
        static inline void SetStart (Move& move, const uint16_t start) { move.data &= inverseStartMask; move.data |= (start & sixBitMask) << 6; }
        static inline void SetTarget (Move& move, const uint16_t target) { move.data &= inverseTargetMask; move.data |= target & sixBitMask; }
        static inline bool IsCapture (const Move& move) { return move.data & captureMask; }
        static inline bool IsPromotion (const Move& move) { return move.data & promotionMask; }
        static inline bool IsThisPromotion (const Move& move, const uint8_t promotion) { return (move.data & inverseCaptureMask) >> 12 == promotion; }
        static inline bool IsNullMove (const Move& move) { return move.data == 0 || move.pieceMoved == 0; }

        static inline bool IsEqual (const Move& moveToCompare, const Move& moveToReference) { return moveToCompare.data == moveToReference.data && moveToCompare.pieceMoved == moveToReference.pieceMoved && moveToCompare.pieceCaptured == moveToReference.pieceCaptured && moveToCompare.check == moveToReference.check; };

        Move () {}
        Move (const uint16_t _data) : data(_data) {}
        Move (const uint16_t _data, const piece _pieceMoved, const piece _pieceCaptured) : data(_data), pieceMoved(_pieceMoved), pieceCaptured(_pieceCaptured) {}
        Move (const uint8_t flag, const uint8_t start, const uint8_t target, const piece _pieceMoved, const piece _pieceCaptured, const bool _check = false) : data(((flag & fourBitMask) << 12) | ((start & sixBitMask) << 6) | (target & sixBitMask)), pieceMoved(_pieceMoved), pieceCaptured(_pieceCaptured), check(_check) {}
        
        //DEBUG
        /* static inline void Display (Move& move) {
            dbg_printf("MOVE: ");
            dbg_printf("DATA: %X ", move.data);
            dbg_printf("PIECE MOVED: %X ", move.pieceMoved);
            dbg_printf("PIECE CAPTURED: %X ", move.pieceCaptured);
            dbg_printf("CHECK: %u ", move.check);
            dbg_printf("NOTATION: ");
            for (uint8_t i = 0; i < MAX_ALGEBRAIC_NOTATION_LENGTH && move.algebraicNotation[i] != '\0'; dbg_printf("%c", move.algebraicNotation[i++]));
            dbg_printf(" END;\n");
        } */
    };
}