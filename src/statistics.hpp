/**
 * @file statistics.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of statistics
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    struct Statistics {
        static const uint8_t numberOfStatistics = 64;

        static const char* names[numberOfStatistics];

        enum : const uint8_t {
            name,
            elo,
            wins,
            winsAsWhite,
            winsAsBlack,
            winsByTimeout,
            winsByResignation,
            minimumPlyToWin,
            maximumPlyToWin,
            draws,
            drawsAsWhite,
            drawsAsBlack,
            drawsByStalemate,
            drawsByInsufficentMaterial,
            drawsByFiftyMoveRule,
            drawsByThreefoldRepetition,
            drawsByTimeoutWithInsufficentMaterial,
            drawsByAgreement,
            minimumPlyToDraw,
            maximumPlyToDraw,
            losses,
            lossesAsWhite,
            lossesAsBlack,
            lossesByTimeout,
            lossesByResignation,
            minimumPlyToLoss,
            maximumPlyToLoss,
            movesPlayed,
            pawnMovesPlayed,
            knightMovesPlayed,
            bishopMovesPlayed,
            rookMovesPlayed,
            queenMovesPlayed,
            kingMovesPlayed,
            checksGiven,
            checksGivenWithAPawn,
            checksGivenWithAKnight,
            checksGivenWithABishop,
            checksGivenWithARook,
            checksGivenWithAQueen,
            checksGivenWithAKing,
            checkmatesGivenWithAPawn,
            checkmatesGivenWithAKnight,
            checkmatesGivenWithABishop,
            checkmatesGivenWithARook,
            checkmatesGivenWithAQueen,
            checkmatesGivenWithAKing,
            doublePawnPushesPlayed,
            capturesPlayed,
            pawnCapturesPlayed,
            knightCapturesPlayed,
            bishopCapturesPlayed,
            rookCapturesPlayed,
            queenCapturesPlayed,
            kingCapturesPlayed,
            castlesPlayed,
            kingSideCastlesPlayed,
            queenSideCastlesPlayed,
            enPassantsPlayed,
            promotionsPlayed,
            knightPromotionsPlayed,
            bishopPromotionsPlayed,
            rookPromotionsPlayed,
            queenPromotionsPlayed
        };

        uint32_t stats[numberOfStatistics] = {};

        static const uint32_t fiveBitMask = 0x1F;

        static inline uint8_t GetName (Statistics& stats, char _name[MAX_STAT_NAME + 1]) {
            uint32_t temp = stats.stats[name];
            uint8_t length = 0, index = 0;

            for (; index < MAX_STAT_NAME + 1; _name[index++] = '\0');
            
            for (; temp; temp >>= 5, length++);

            for (temp = stats.stats[name], index = 0; temp; temp >>= 5, index++) {
                _name[length - index - 1] = IndexToCharacter[temp & fiveBitMask];
            }

            return length;
        }

        static inline uint8_t GetElo (Statistics& stats, char _elo[MAX_ELO_LENGTH + 1]) {
            uint32_t temp = stats.stats[elo];
            uint8_t length = 0, index = 0;

            for (; index < MAX_ELO_LENGTH + 1; _elo[index++] = '\0');
            
            for (; temp; temp /= 10, length++);

            for (temp = stats.stats[elo], index = 0; temp; temp /= 10, index++) {
                _elo[length - index - 1] = char(temp % 10 + '0');
            }

            return length;
        }

        Statistics (uint32_t _name) {
            stats[name] = _name;
            stats[elo] = 1000;
        }
    };
}