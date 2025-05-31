/**
 * @file bot.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of chess bot
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    struct MoveEvaluation {
        Move move;
        eval evaluation = 0;

        MoveEvaluation () {}

        MoveEvaluation (Move _move, int16_t _evaluation) {
            move = _move;
            evaluation = _evaluation;
        }
    };

    struct LegalMoveMemory {
        Move legalMoves[MAX_LEGAL_MOVES];
        eval scores[MAX_LEGAL_MOVES];
        uint8_t movesAvailable = 0;

        LegalMoveMemory () {}
    };

    struct MovePosition {
        Move move;
        hash zobristHash = 0;

        MovePosition () {}

        MovePosition (Move _move, hash _zobristHash) {
            move = _move;
            zobristHash = _zobristHash;
        }
    };

    struct Board;

    struct Bot {
        Board* board;

        const hash openingHashes[OPENING_POSITIONS] = { 0x10DA59A4, 0x10DA59A4, 0x10DA59A4, 0x12DBFE12, 0x12DBFE12, 0x13A1770C, 0x13FF676A, 0x142882, 0x14417187, 0x1546C9FD, 0x1546C9FD, 0x1546C9FD, 0x1623583A, 0x18ACD073, 0x18ACD073, 0x18ACD073, 0x19AB6809, 0x1A3E0098, 0x1EC84E5D, 0x2076A2BC, 0x20A65BC6, 0x20A8B455, 0x2427F253, 0x2427F253, 0x24414FA9, 0x251A8527, 0x266991E8, 0x29CDEBDD, 0x2B71193E, 0x2D7CABF1, 0x2D7CABF1, 0x2D7CABF1, 0x2EA10C77, 0x2F0AAF09, 0x329C1B3A, 0x33A0CD2E, 0x33A0CD2E, 0x359BDFBE, 0x35DD84D8, 0x36950E0B, 0x37118D02, 0x3B7F1785, 0x3D822159, 0x3DBAB63F, 0x4074CF3B, 0x41E4324A, 0x41F8B9BD, 0x4249A790, 0x4249A790, 0x4316FC96, 0x474B8C79, 0x4DF78205, 0x4FA79D43, 0x509C16B, 0x51C615FE, 0x5312824B, 0x5312824B, 0x561496B5, 0x57B3EA8A, 0x57B3EA8A, 0x586854F0, 0x5A9514E, 0x5BFE8F3B, 0x5D4A458C, 0x5EF2A59A, 0x609F0176, 0x615F110, 0x615F110, 0x624D31B, 0x62D77808, 0x64DCA2DC, 0x6766AFED, 0x6B303CE4, 0x6C129E3D, 0x6CCC1F28, 0x6CCC1F28, 0x6DF1685C, 0x6E14CD84, 0x6E969686, 0x6EB4337E, 0x71318E9, 0x7245D7B6, 0x734BB379, 0x74C0E939, 0x74C0E939, 0x77236AD7, 0x792AF0B7, 0x79C0DB4B, 0x79C0DB4B, 0x7A73D65, 0x7E63D6E0, 0x8061A528, 0x82F639B0, 0x83C4D2D4, 0x857B0690, 0x8B58E4DE, 0x8B58E4DE, 0x8D715504, 0x8F54E531, 0x95385D3C, 0x957E9ADD, 0x957E9ADD, 0x9670B38B, 0x9FBB93D8, 0xA1DA5F87, 0xA2D14256, 0xA852C80D, 0xAF3B5BD8, 0xB0F75F99, 0xB3CB0E84, 0xB4AA3726, 0xBA199217, 0xBA9D1E9D, 0xBA9D1E9D, 0xBA9D1E9D, 0xC17A4F5B, 0xC232A1EC, 0xC232A1EC, 0xC3695115, 0xC46FA587, 0xC6079164, 0xC6AC21B8, 0xC723E3A5, 0xC8F0723F, 0xC9144AA5, 0xC98170F8, 0xCAA8FE16, 0xCAA8FE16, 0xCAC9FA2B, 0xCE9DCE9E, 0xCFC2F738, 0xD3A480D2, 0xD5286109, 0xD69A2891, 0xD905CB90, 0xD9AF655B, 0xDB4BEB57, 0xDB769316, 0xDC52180E, 0xDF5F5444, 0xE4267B06, 0xE7FDA349, 0xE8CF979C, 0xED69FA11, 0xF6DB9799, 0xF6DB9799, 0xF8CA3A12, 0xF8CA3A12, 0xFA0CAD1E };
        const Move openingMoves[OPENING_POSITIONS] = { Move(uint16_t(0xE73)), Move(uint16_t(0x96E)), Move(uint16_t(0xB5C)), Move(uint16_t(0xA6)), Move(uint16_t(0x210)), Move(uint16_t(0xCEB)), Move(uint16_t(0x9AD)), Move(uint16_t(0x2DB)), Move(uint16_t(0xA6)), Move(uint16_t(0xE73)), Move(uint16_t(0xCAA)), Move(uint16_t(0xCAA)), Move(uint16_t(0x314)), Move(uint16_t(0xFAD)), Move(uint16_t(0xE6A)), Move(uint16_t(0x8DA)), Move(uint16_t(0x52)), Move(uint16_t(0x52)), Move(uint16_t(0x2DB)), Move(uint16_t(0x49A)), Move(uint16_t(0x83B)), Move(uint16_t(0xD2C)), Move(uint16_t(0xFAD)), Move(uint16_t(0xE6A)), Move(uint16_t(0x195)), Move(uint16_t(0x195)), Move(uint16_t(0x91C)), Move(uint16_t(0xCAA)), Move(uint16_t(0xEE0)), Move(uint16_t(0x6D8)), Move(uint16_t(0x6C3)), Move(uint16_t(0x6D3)), Move(uint16_t(0xAA3)), Move(uint16_t(0x314)), Move(uint16_t(0x70B)), Move(uint16_t(0xA6)), Move(uint16_t(0x52)), Move(uint16_t(0xF59)), Move(uint16_t(0x29A)), Move(uint16_t(0x355)), Move(uint16_t(0xF62)), Move(uint16_t(0x52)), Move(uint16_t(0xAA3)), Move(uint16_t(0xF3B)), Move(uint16_t(0xFAD)), Move(uint16_t(0x6E4)), Move(uint16_t(0xCAA)), Move(uint16_t(0x49A)), Move(uint16_t(0x9D)), Move(uint16_t(0x102)), Move(uint16_t(0x314)), Move(uint16_t(0x2DB)), Move(uint16_t(0xB5C)), Move(uint16_t(0x95C)), Move(uint16_t(0xB5C)), Move(uint16_t(0xD2C)), Move(uint16_t(0xFAD)), Move(uint16_t(0x52)), Move(uint16_t(0xC6A)), Move(uint16_t(0xCEA)), Move(uint16_t(0xEED)), Move(uint16_t(0x853)), Move(uint16_t(0xA6)), Move(uint16_t(0xCAA)), Move(uint16_t(0x91B)), Move(uint16_t(0x195)), Move(uint16_t(0x292)), Move(uint16_t(0x2DB)), Move(uint16_t(0x52)), Move(uint16_t(0xA6)), Move(uint16_t(0xF59)), Move(uint16_t(0xCEB)), Move(uint16_t(0x621)), Move(uint16_t(0xFAD)), Move(uint16_t(0x314)), Move(uint16_t(0x396)), Move(uint16_t(0xD65)), Move(uint16_t(0xCA2)), Move(uint16_t(0xD1)), Move(uint16_t(0xCE3)), Move(uint16_t(0xA6)), Move(uint16_t(0xE6A)), Move(uint16_t(0xA6)), Move(uint16_t(0x52)), Move(uint16_t(0x314)), Move(uint16_t(0x8DB)), Move(uint16_t(0xA6)), Move(uint16_t(0x9D)), Move(uint16_t(0x195)), Move(uint16_t(0x355)), Move(uint16_t(0x6A3)), Move(uint16_t(0x91B)), Move(uint16_t(0xDB)), Move(uint16_t(0x6E4)), Move(uint16_t(0xCEB)), Move(uint16_t(0xEA5)), Move(uint16_t(0xFAD)), Move(uint16_t(0x52)), Move(uint16_t(0xD65)), Move(uint16_t(0x195)), Move(uint16_t(0x52)), Move(uint16_t(0x396)), Move(uint16_t(0xA6)), Move(uint16_t(0x756)), Move(uint16_t(0x934)), Move(uint16_t(0xFAD)), Move(uint16_t(0x2DB)), Move(uint16_t(0xF59)), Move(uint16_t(0x210)), Move(uint16_t(0xEE6)), Move(uint16_t(0xA9B)), Move(uint16_t(0xCE3)), Move(uint16_t(0x52)), Move(uint16_t(0x9D)), Move(uint16_t(0x195)), Move(uint16_t(0x195)), Move(uint16_t(0xA6)), Move(uint16_t(0x9D)), Move(uint16_t(0x52)), Move(uint16_t(0x292)), Move(uint16_t(0x195)), Move(uint16_t(0x315)), Move(uint16_t(0xFAD)), Move(uint16_t(0xCEB)), Move(uint16_t(0x49B)), Move(uint16_t(0xC61)), Move(uint16_t(0xCE3)), Move(uint16_t(0xD24)), Move(uint16_t(0xCAA)), Move(uint16_t(0xFAD)), Move(uint16_t(0xE6A)), Move(uint16_t(0x102)), Move(uint16_t(0x153)), Move(uint16_t(0xA6)), Move(uint16_t(0xCE3)), Move(uint16_t(0xF6B)), Move(uint16_t(0x314)), Move(uint16_t(0xEE9)), Move(uint16_t(0xEA5)), Move(uint16_t(0xEE9)), Move(uint16_t(0xB23)), Move(uint16_t(0x195)), Move(uint16_t(0x8DA)), Move(uint16_t(0x14E)), Move(uint16_t(0xE6A)), Move(uint16_t(0xE68)), Move(uint16_t(0x396)), Move(uint16_t(0x6E4)), Move(uint16_t(0x91C)) };
        Move possibleOpeningMoves[MAX_POSSIBLE_OPENING_MOVES];
        uint8_t possibleOpeningMovesAvailable = 0;
        
        const eval pieceValues[6] = { 100, 300, 320, 500, 900, 9000 };
        const int8_t pieceSquareTables[7][32] = {
            { // Pawns
                 0,  0,  0,  0,
                50, 50, 50, 50,
                10, 10, 20, 30,
                 5,  5, 10, 25,
                 0,  0,  0, 40,
                 5, -5,-10,  0,
                 5, 10, 10,-40,
                 0,  0,  0,  0
            },
            { // Knights
                -50,-40,-30,-30,
                -40,-20,  0,  0,
                -30,  0, 10, 15,
                -30,  5, 15, 20,
                -30,  0, 15, 20,
                -30,  5, 10, 15,
                -40,-20,  0,  5,
                -50,-40,-30,-30
            },
            { // Bishops
                -20,-10,-10,-10,
                -10,  0,  0,  0,
                -10,  0,  5, 10,
                -10,  5,  5, 10,
                -10,  0, 10, 10
                -10, 10, 10, 10,
                -10,  5,  0,  0,
                -20,-10,-10,-10
            },
            { // Rooks
                 0,  0,  0,  0,
                 5, 10, 10, 10,
                -5,  0,  0,  0,
                -5,  0,  0,  0,
                -5,  0,  0,  0,
                -5,  0,  0,  0,
                -5,  0,  0,  0,
                 0,  0,  0,  5
            },
            { // Queens
                -20,-10,-10, -5,
                -10,  0,  0,  0,
                -10,  0,  5,  5,
                 -5,  0,  5,  5,
                  0,  0,  5,  5,
                -10,  5,  5,  5,
                -10,  0,  5,  0,
                -20,-10,-10, -5
            },
            { // Kings
                -30,-40,-40,-50,
                -30,-40,-40,-50,
                -30,-40,-40,-50,
                -30,-40,-40,-50,
                -20,-40,-40,-50,
                -10,-40,-40,-40,
                  0,  0,-40,-40,
                  0,  0,  0,  0
            },
            { // Endgame Kings
                -50,-40,-30,-20,
                -30,-20,-10,  0,
                -30,-10, 20, 30,
                -30,-10, 30, 40,
                -30,-10, 30, 40,
                -30,-10, 20, 30,
                -30,-30,  0,  0,
                -50,-30,-30,-30
            }
        };
        const eval aspirationWindowWidth = 50;
        const eval passedPawnBonuses[8] = { 0, 10, 15, 25, 40, 65, 100, 100 };
        const eval passedPawnEndGameBonuses[8] = { 0, 20, 30, 45, 65, 90, 130, 130 };
        const eval passedPawnBonus = 35;
        const eval protectedPassedPawnBonus = 40;
        const eval isolatedPawnPenalty = -35;
        const eval doubledPawnPenalty = -10;
        const eval rookOnOpenFileBonus = 10;
        const eval rookOnSemiOpenFileBonus = 5;
        const eval queenOnOpenFileBonus = 5;
        const eval queenOnSemiOpenFileBonus = 3;
        const eval castlingBonus = 250;
        const eval castlingAsAnOptionBonus = 50;
        const eval bishopPairBonus = 50;
        const eval kingInCornerBonus = 25;
        const eval winingLimit = 50;
        const eval losingLimit = -50;
        const eval kingsTogetherMajorPiecesBonus = 45;
        const eval kingsTogetherMinorPiecesBonus = 3;
        const eval kingsTogetherPawnsBonus = 2;
        const eval tradingWhileWinningBonus = 50;
        const eval opponentKingMoveBonus = 40;
        const eval pawnBreakBonus = 25;
        const eval pawnMoveBonus = 10;
        const eval forwardMoveBonus = 15;
        const eval randomFactor = 3;

        const int16_t victimValues[7] = { 100, 200, 300, 400, 500, 600, 0 };

        bool currentlyInEndGame = false;
        clock_t startTime;
        clock_t endTime;
        bool searchIsInterrupted = false;
        uint8_t startingPly = 0;
        uint8_t searchPly = startingPly;
        uint8_t startingDepth = 0;
        uint32_t nodes = 0;
        float failHigh = 0;
        float failHighFirst = 0;

        const clock_t maxThinkTime = 30000;
        const clock_t maxEndGameThinkTime = 30000;
        static const uint8_t minNullDepth = 2;
        static const uint8_t maximumStandardDepth = 2;
        static const uint8_t maximumDepth = 6;
        LegalMoveMemory legalMoveMemory[maximumDepth];

        static const uint16_t maxPrincipalVariationEntries = 375;
        MovePosition principalVariationTable[maxPrincipalVariationEntries];
        Move principalVariation[maximumDepth];
        uint16_t indexOfStartPrincipalMove = maxPrincipalVariationEntries;

        int16_t searchHistory[12][64];
        Move killerMoves[2][maximumDepth];

        inline void ClearPrincipalVariation ();
        inline void StoreMoveInPrincipalVariationTable (const Move& move, const hash& zobristHash);
        inline Move ProbePrincipalVariationTable (const hash& zobristHash);
        inline Move ProbeOpeningsTable (const hash& zobristHash);
        inline bool DoesMoveExist (const Move& move);
        inline uint8_t GetPrincipalVariation (const uint8_t depth);
        inline void CheckIfTimeIsUp ();
        Move Think ();
        eval Search (int8_t depth, eval alpha = LOWEST_EVALUATION, eval beta = HIGHEST_EVALUATION, bool doNullMove = true);
        eval QuiescentSearch (eval alpha, eval beta);
        inline void SetNextMove (Move* moves, eval* scores, uint8_t availableMoves, uint8_t currentIndex);
        inline void ScoreMoves (Move* moves, eval* scores, uint8_t availableMoves);
        eval Evaluate ();
        inline eval MaterialAdvantage ();
        inline bool IsEndGame ();
        void Clear ();

        Bot (Board* _board) {
            board = _board;
        }
    };
}