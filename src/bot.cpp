/**
 * @file bot.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Definition of chess bot
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

namespace Chess {
    void Bot::ClearPrincipalVariation () {
        for (uint16_t index = 0; index < maximumDepth; index++) principalVariation[index] = Move();
    }

    void Bot::StoreMoveInPrincipalVariationTable (const Move& move, const hash& zobristHash) {
        uint16_t index = zobristHash % maxPrincipalVariationEntries;

        if (searchPly != 0 && index == indexOfStartPrincipalMove) return; // Tried to overwrite principal move with a move in a later depth
        else if (searchPly == 0) indexOfStartPrincipalMove = index;

        principalVariationTable[index] = MovePosition(move, zobristHash);
    }

    Move Bot::ProbePrincipalVariationTable (const hash& zobristHash) {
        const uint16_t index = zobristHash % maxPrincipalVariationEntries;

        if (principalVariationTable[index].zobristHash == zobristHash) {
            return principalVariationTable[index].move;
        }

        return Move();
    }

    Move Bot::ProbeOpeningsTable (const hash& zobristHash) {
        possibleOpeningMovesAvailable = 0;

        for (uint16_t index = 0; index < OPENING_POSITIONS; ++index) {
            if (openingHashes[index] == zobristHash) {
                for (uint8_t i = 0; i < board->movesAvailable; i++) {
                    if (Move::GetStart(openingMoves[index]) == Move::GetStart(board->legalMoves[i]) && Move::GetTarget(openingMoves[index]) == Move::GetTarget(board->legalMoves[i])) {
                        if (Move::IsPromotion(openingMoves[index])) {
                            if (Move::IsThisPromotion(openingMoves[index], Move::queenPromotion) && Move::IsThisPromotion(board->legalMoves[i], Move::queenPromotion));
                            else if (Move::IsThisPromotion(openingMoves[index], Move::rookPromotion) && Move::IsThisPromotion(board->legalMoves[i], Move::rookPromotion));
                            else if (Move::IsThisPromotion(openingMoves[index], Move::bishopPromotion) && Move::IsThisPromotion(board->legalMoves[i], Move::bishopPromotion));
                            else if (Move::IsThisPromotion(openingMoves[index], Move::knightPromotion) && Move::IsThisPromotion(board->legalMoves[i], Move::knightPromotion));
                            else continue;
                        }

                        possibleOpeningMoves[possibleOpeningMovesAvailable++] = board->legalMoves[i];
                        //DEBUG dbg_printf("OPENING MOVE: ");
                        //DEBUG Move::Display(board->legalMoves[i]);
                        //DEBUG dbg_printf(";\n");
                    }
                }
            }
        }

        if (possibleOpeningMovesAvailable) return possibleOpeningMoves[uint8_t(rand() % possibleOpeningMovesAvailable)];

        return Move();
    }

    bool Bot::DoesMoveExist (const Move& move) {
        board->GenerateLegalMoves(board->legalMoves, board->movesAvailable, false);

        if (Move::IsNullMove(move)) return false;

        for (uint8_t index = 0; index < board->movesAvailable; ++index) {
            if (Move::IsEqual(move, board->legalMoves[index])) return true;
        }

        return false;
    }

    uint8_t Bot::GetPrincipalVariation (const uint8_t depth) {
        Move move = ProbePrincipalVariationTable(board->lastZobristHash);
        uint8_t count = 0;

        ClearPrincipalVariation();

        while (count < depth) {
            if (DoesMoveExist(move)) {
                board->MakeMove(move, true);
                principalVariation[count++] = move;
            } else if (count == 0) {
                // Move does not exist
                if (board->movesAvailable) {
                    // Make a random move
                    move = board->legalMoves[uint8_t(rand()) % board->movesAvailable];
                    board->MakeMove(move, true);
                    //DEBUG dbg_printf("RAND MOVE ");
                    //DEBUG Move::Display(move);
                    //DEBUG dbg_printf(";\n");
                    principalVariation[count++] = move;
                }
                break;
            } else break;

            move = ProbePrincipalVariationTable(board->lastZobristHash);
        }
        
        uint8_t movesToUndo = count;

        while (movesToUndo-- > 0) board->UnMakeMove(false, true);

        return count;
    }

    void Bot::CheckIfTimeIsUp () {
        if (nodes & 0x1F) return; // Quick way to make sure it checks somewhat often, but not all the time

        board->UpdateTimeRemaining();

        kb_Scan();
        if ((startingDepth > 1 && clock() > endTime) || kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyDel) || kb_IsDown(kb_Key0) || kb_IsDown(kb_KeyDecPnt)) searchIsInterrupted = true;
    }

    Move Bot::Think () {
        // Don't search if there is only one legal move
        board->GenerateLegalMoves(board->legalMoves, board->movesAvailable, false);
        if (board->movesAvailable == 1) return board->legalMoves[0];

        if (board->ply <= 9) {
            Move openingMove = ProbeOpeningsTable(board->lastZobristHash);
            if (!Move::IsNullMove(openingMove)) return openingMove;
        }

        Clear();

        currentlyInEndGame = IsEndGame();
        startTime = clock();
        endTime = startTime + (currentlyInEndGame ? maxEndGameThinkTime : maxThinkTime) * (CLOCKS_PER_SEC / 1000);
        searchIsInterrupted = false;

        startingPly = board->ply;

        MoveEvaluation best;
        eval alpha = LOWEST_EVALUATION, beta = HIGHEST_EVALUATION;

        for (startingDepth = 1; startingDepth <= maximumStandardDepth;) {
            //DEBUG dbg_printf("STARTING SEARCH AT DEPTH %u\n", startingDepth);

            nodes = 0;
            failHigh = 0;
            failHighFirst = 0;

            best.evaluation = Search(startingDepth, alpha, beta, true);

            if (searchIsInterrupted) {
                //DEBUG dbg_printf("SEARCH INTERRUPTED\n");
                break;
            }

            if (best.evaluation < alpha || best.evaluation > beta) {
                alpha = LOWEST_EVALUATION;
                beta = HIGHEST_EVALUATION;
                //DEBUG dbg_printf("WINDOW FAILED\n");
                continue;
            }

            alpha = best.evaluation - aspirationWindowWidth;
            beta = best.evaluation + aspirationWindowWidth;

            GetPrincipalVariation(startingDepth);
            best.move = principalVariation[0];

            // Printing an update on the best moves for the current depth
            //DEBUG dbg_printf("t: %lu, Dep: %u, Eval: %i, Node: %lu, Ord: %.2f, EG: %u, PV:", (clock() - startTime) / (CLOCKS_PER_SEC / 1000), startingDepth, best.evaluation, nodes, failHighFirst / failHigh, IsEndGame());
            //DEBUG for (uint8_t i = 0; i < startingDepth; i++) {
            //DEBUG     dbg_printf(" ");
            //DEBUG     Move::Display(principalVariation[i]);
            //DEBUG }
            //DEBUG dbg_printf(";\n");

            if (best.evaluation >= HIGHEST_EVALUATION - maximumDepth) break;

            ++startingDepth;
        }

        return best.move;
    }

    eval Bot::Search (int8_t depth, eval alpha, eval beta, bool doNullMove) {
        CheckIfTimeIsUp();

        ++nodes;

        board->UpdateInCheck();

        if (board->inCheck) depth++;
        else if (depth == 1 && !Move::IsCapture(board->lastMove)) { // Futility Pruning
            if (Evaluate() + pieceValues[Piece::knight - 1] < alpha) return QuiescentSearch(alpha, beta);
        }

        if (depth == 0) return QuiescentSearch(alpha, beta);

        board->UpdateThreefoldRepetition();
        board->UpdateFiftyMoveCount();

        if (board->repetitions >= 2 || board->fiftyMoveRule) return 0;

        Move* legalMoves = legalMoveMemory[searchPly].legalMoves;
        eval* scores = legalMoveMemory[searchPly].scores;
        uint8_t& movesAvailable = legalMoveMemory[searchPly].movesAvailable;

        board->GenerateLegalMoves(legalMoves, movesAvailable, false);

        if (searchPly >= maximumDepth - 1 || board->CheckGameState()) return Evaluate();

        eval currentEvaluation = LOWEST_EVALUATION;

        if (doNullMove && !board->inCheck && searchPly && ((board->sideToMove == Piece::white && (board->whiteQueensCount || board->whiteRooksCount)) || (board->sideToMove == Piece::black && (board->blackQueensCount || board->blackRooksCount))) && depth >= minNullDepth) {
            board->MakeNullMove();
            searchPly += minNullDepth;

            currentEvaluation = -Search(depth - minNullDepth, -beta, -beta + 1, false);

            board->UnMakeNullMove();
            searchPly -= minNullDepth;

            if (searchIsInterrupted) return 0;

            if (currentEvaluation >= beta) return beta;
        }

        Move bestMove;
        currentEvaluation = LOWEST_EVALUATION;
        eval startingAlpha = alpha;
        Move principalMove = ProbePrincipalVariationTable(board->lastZobristHash);

        ScoreMoves(legalMoves, scores, movesAvailable);

        if (!Move::IsNullMove(principalMove)) {
            for (uint8_t index = 0; index < movesAvailable; index++) {
                if (Move::IsEqual(legalMoves[index], principalMove)) scores[index] += 5000;
            }
        }

        for (uint8_t index = 0, lateMoveReductions; index < movesAvailable/*  && (currentlyInEndGame || depth < 3 || !index || scores[index]) */; index++) { // Late move pruning
            SetNextMove(legalMoves, scores, movesAvailable, index);

            board->MakeMove(legalMoves[index], true);
            ++searchPly;

            lateMoveReductions = depth >= 2 && index ? (scores[index] >= 900 ? 0 : scores[index] || depth == 2 || currentlyInEndGame ? 1 : 2) : 0;
            currentEvaluation = -Search(depth - 1 - lateMoveReductions, -beta, -alpha, true);

            board->UnMakeMove(false, true);
            --searchPly;

            if (searchIsInterrupted) return 0;

            if (currentEvaluation > alpha) {
                if (currentEvaluation >= beta) {
                    //DEBUG dbg_printf("PRUNED!\n");
                    if (index == 0) ++failHighFirst;
                    ++failHigh;

                    if (!Move::IsCapture(legalMoves[index])) { // Flags a good move that is not a capture
                        killerMoves[1][searchPly] = killerMoves[0][searchPly];
                        killerMoves[0][searchPly] = legalMoves[index];
                    }

                    return beta;
                }

                alpha = currentEvaluation;
                bestMove = legalMoves[index];

                if (!Move::IsCapture(legalMoves[index])) searchHistory[Piece::GetLinearValue(legalMoves[index].pieceMoved)][Move::GetTarget(legalMoves[index])] += depth;
            }
        }

        if (alpha != startingAlpha) StoreMoveInPrincipalVariationTable(bestMove, board->lastZobristHash);

        return alpha;
    }

    eval Bot::QuiescentSearch (eval alpha, eval beta) {
        CheckIfTimeIsUp();

        ++nodes;

        board->UpdateInCheck();

        if (board->inCheck) return Search(0, alpha, beta, true);

        Move* legalMoves = legalMoveMemory[searchPly].legalMoves;
        eval* scores = legalMoveMemory[searchPly].scores;
        uint8_t& movesAvailable = legalMoveMemory[searchPly].movesAvailable;

        if (searchPly >= maximumDepth - 1) {
            board->GenerateLegalMoves(legalMoves, movesAvailable, false);

            return Evaluate();
        }

        eval evaluation = Evaluate();

        if (evaluation >= beta) return beta;

        if (evaluation > alpha) alpha = evaluation;

        board->GenerateLegalMoves(legalMoves, movesAvailable, true);

        if (!movesAvailable) return alpha;

        Move bestMove;
        evaluation = LOWEST_EVALUATION;
        eval startingAlpha = alpha;
        Move principalMove = ProbePrincipalVariationTable(board->lastZobristHash);

        ScoreMoves(legalMoves, scores, movesAvailable);

        if (!Move::IsNullMove(principalMove)) {
            for (uint8_t index = 0; index < movesAvailable; index++) {
                if (Move::IsEqual(legalMoves[index], principalMove)) scores[index] += 5000;
            }
        }

        for (uint8_t index = 0; index < movesAvailable; index++) {
            SetNextMove(legalMoves, scores, movesAvailable, index);

            board->MakeMove(legalMoves[index], true);
            ++searchPly;

            evaluation = -QuiescentSearch(-beta, -alpha);

            board->UnMakeMove(false, true);
            --searchPly;

            if (searchIsInterrupted) return 0;

            if (evaluation > alpha) {
                if (evaluation >= beta) {
                    // dbg_printf("PRUNED!\n");
                    if (index == 0) ++failHighFirst;
                    ++failHigh;

                    return beta;
                }

                alpha = evaluation;
                bestMove = legalMoves[index];
            }
        }

        if (alpha != startingAlpha) StoreMoveInPrincipalVariationTable(bestMove, board->lastZobristHash);

        return alpha;
    }

    void Bot::SetNextMove (Move* moves, eval* scores, uint8_t availableMoves, uint8_t currentIndex) {
        if (currentIndex >= availableMoves) return;

        Move temporaryMove;
        eval temporaryScore;
        eval bestScore = scores[currentIndex];
        uint8_t bestIndex = currentIndex;

        for (uint8_t index = currentIndex + 1; index < availableMoves; ++index) {
            if (scores[index] > bestScore) {
                bestScore = scores[index];
                bestIndex = index;
            }
        }

        // Swap current move with the best move left in the list
        temporaryMove = moves[currentIndex];
        temporaryScore = scores[currentIndex];
        moves[currentIndex] = moves[bestIndex];
        scores[currentIndex] = bestScore;
        moves[bestIndex] = temporaryMove;
        scores[bestIndex] = temporaryScore;
    }

    void Bot::ScoreMoves (Move* moves, eval* scores, uint8_t availableMoves) {
        for (uint8_t index = 0; index < availableMoves; ++index) {
            if (Move::IsCapture(moves[index])) {
                scores[index] = victimValues[Piece::GetPiece(moves[index].pieceCaptured) - 1] + 6 - victimValues[Piece::GetPiece(moves[index].pieceMoved) - 1] / 100 + 2500;
            } else if (Move::IsEqual(killerMoves[0][searchPly], moves[index])) scores[index] = 1000;
            else if (Move::IsEqual(killerMoves[1][searchPly], moves[index])) scores[index] = 900;
            else scores[index] = searchHistory[Piece::GetLinearValue(moves[index].pieceMoved)][Move::GetTarget(moves[index])];
        }
    }

    eval Bot::Evaluate () {
        if (board->blackWins || board->whiteWins) return LOWEST_EVALUATION + searchPly;
        else if (board->stalemate || board->insufficentMaterial || board->fiftyMoveRule || board->threefoldRepetition || board->repetitions >= 2 || board->ply >= MAX_MOVE_HISTORY - 2) return 0;

        eval evaluation = 0;

        bool isEndGame = IsEndGame();

        for (uint8_t rank = 0, square = 0; rank < 8; ++rank) {
            for (uint8_t file = 0; file < 8; ++file, ++square) {
                if (board->matrix[square] == Piece::empty) continue;

                uint8_t index = Piece::GetPiece(board->matrix[square]) - 1;
                if (index == Piece::king - 1 && isEndGame) ++index;

                eval coefficient = Piece::IsSide(board->matrix[square], Piece::white) ? 1 : -1;

                evaluation += coefficient * pieceValues[index];
                evaluation += coefficient * pieceSquareTables[index][(coefficient == 1 ? rank : 7 - rank) * 4 + (file < 4 ? file : 7 - file)];

                if (index == Piece::pawn - 1) {
                    bool isPassedPawn = true;
                    bool isIsolatedPawn = true;
                    bool isDoubledPawn = false;

                    if (coefficient == 1 && rank > 1) { // White Pawn
                        for (uint8_t currentRank = rank - 1, currentSquare = file > 0 ? square - 9 : square - 8; currentRank > 0 && isPassedPawn; --currentRank, currentSquare -= file > 0 && file < 7 ? 11 : 10) {
                            for (uint8_t currentFile = file > 0 ? file - 1 : 0; currentFile <= file + 1 && currentFile < 8 && isPassedPawn; ++currentFile, ++currentSquare) {
                                if (board->matrix[currentSquare] == Piece::blackPawn) isPassedPawn = false;
                            }
                        }
                    } else if (coefficient == -1 && rank < 6) { // Black Pawn
                        for (uint8_t currentRank = rank + 1, currentSquare = file > 0 ? square + 7 : square + 8; currentRank < 7 && isPassedPawn; ++currentRank, currentSquare += file > 0 && file < 7 ? 5 : 6) {
                            for (uint8_t currentFile = file > 0 ? file - 1 : 0; currentFile <= file + 1 && currentFile < 8 && isPassedPawn; ++currentFile, ++currentSquare) {
                                if (board->matrix[currentSquare] == Piece::whitePawn) isPassedPawn = false;
                            }
                        }
                    }

                    if (file > 0) {
                        for (uint8_t currentSquare = (square - 1) % 8 + 8; currentSquare < 56 && isIsolatedPawn; currentSquare += 8) {
                            if (board->matrix[square] == board->matrix[currentSquare]) isIsolatedPawn = false;
                        }
                    }

                    if (isIsolatedPawn && file < 7) {
                        for (uint8_t currentSquare = (square + 1) % 8 + 8; currentSquare < 56 && isIsolatedPawn; currentSquare += 8) {
                            if (board->matrix[square] == board->matrix[currentSquare]) isIsolatedPawn = false;
                        }
                    }

                    for (uint8_t currentSquare = square % 8 + 8; currentSquare < 56 && !isDoubledPawn; currentSquare += 8) {
                        if (square != currentSquare && board->matrix[square] == board->matrix[currentSquare]) isDoubledPawn = true;
                    }

                    if (isPassedPawn) {
                        bool isProtectedPassedPawn = false;

                        if (coefficient == 1) {
                            for (uint8_t currentSquare = square % 8 + 56; currentSquare > square; currentSquare -= 8) {
                                if (board->matrix[currentSquare] == Piece::whiteRook) isProtectedPassedPawn = true;
                                else if (board->matrix[currentSquare] != Piece::empty && isProtectedPassedPawn) {
                                    isProtectedPassedPawn = false;
                                    break;
                                }
                            }
                        } else {
                            for (uint8_t currentSquare = square % 8; currentSquare < square; currentSquare += 8) {
                                if (board->matrix[currentSquare] == Piece::blackRook) isProtectedPassedPawn = true;
                                else if (board->matrix[currentSquare] != Piece::empty && isProtectedPassedPawn) {
                                    isProtectedPassedPawn = false;
                                    break;
                                }
                            }
                        }

                        evaluation += coefficient * (passedPawnBonus + (isEndGame ? passedPawnEndGameBonuses : passedPawnBonuses)[coefficient == 1 ? rank : 7 - rank]);
                        if (isProtectedPassedPawn) evaluation += coefficient * protectedPassedPawnBonus;
                    }
                    if (isIsolatedPawn) evaluation += coefficient * isolatedPawnPenalty;
                    if (isDoubledPawn) evaluation += coefficient * doubledPawnPenalty;
                } else if (index == Piece::rook - 1 || index == Piece::queen - 1) {
                    bool onOpenFile = true;
                    bool onSemiOpenFile = true;
                    piece pieceFoundOnFile;

                    for (uint8_t currentSquare = square % 8 + 8; currentSquare < 56 && onSemiOpenFile; currentSquare += 8) {
                        if (Piece::IsPiece(board->matrix[currentSquare], Piece::pawn)) {
                            if (pieceFoundOnFile == Piece::empty) {
                                pieceFoundOnFile = board->matrix[currentSquare];
                                onOpenFile = false;
                            } else if (pieceFoundOnFile != board->matrix[currentSquare]) { // Found Pawn of Opposite Color
                                onSemiOpenFile = false;
                            }
                        }
                    }

                    if (index == Piece::rook - 1) {
                        if (onOpenFile) evaluation += coefficient * rookOnOpenFileBonus;
                        else if (onSemiOpenFile) evaluation += coefficient * rookOnSemiOpenFileBonus;
                    } else {
                        if (onOpenFile) evaluation += coefficient * queenOnOpenFileBonus;
                        else if (onSemiOpenFile) evaluation += coefficient * queenOnSemiOpenFileBonus;
                    }
                }
            }
        }

        if (board->whiteBishopsCount >= 2) evaluation += bishopPairBonus;
        if (board->blackBishopsCount >= 2) evaluation -= bishopPairBonus;

        if (!isEndGame) {
            if (board->whiteCastled) evaluation += castlingBonus;
            else {
                if (board->whiteKingCastleRights) evaluation += castlingAsAnOptionBonus;
                if (board->whiteQueenCastleRights) evaluation += castlingAsAnOptionBonus;
            }
            if (board->blackCastled) evaluation -= castlingBonus;
            else {
                if (board->blackKingCastleRights) evaluation -= castlingAsAnOptionBonus;
                if (board->blackQueenCastleRights) evaluation -= castlingAsAnOptionBonus;
            }
        } else {
            // Force king to corner
            if (board->sideToMove == Piece::white || (board->sideToMove == Piece::black && evaluation > 0)) evaluation += kingInCornerBonus * ((3 > board->blackKingFile ? 3 - board->blackKingFile : board->blackKingFile - 4) + (3 > board->blackKingRank ? 3 - board->blackKingFile : board->blackKingRank - 4)); // Wants black king in corner
            if (board->sideToMove == Piece::black || (board->sideToMove == Piece::white && evaluation < 0)) evaluation -= kingInCornerBonus * ((3 > board->whiteKingFile ? 3 - board->whiteKingFile : board->whiteKingFile - 4) + (3 > board->whiteKingRank ? 3 - board->whiteKingFile : board->whiteKingRank - 4)); // Wants white king in corner
        }

        if (board->sideToMove == Piece::black) evaluation *= -1;

        // Force kings together
        if (isEndGame) {
            if (evaluation >= winingLimit || evaluation <= losingLimit) evaluation -= (evaluation > 0 ? 1 : -1) * (board->blackQueensCount || board->whiteQueensCount || board->blackRooksCount || board->whiteRooksCount ? kingsTogetherMajorPiecesBonus : board->blackKnightsCount || board->whiteKnightsCount || board->blackBishopsCount || board->whiteBishopsCount ? kingsTogetherMinorPiecesBonus : kingsTogetherPawnsBonus) * (board->whiteKingFile > board->blackKingFile ? board->whiteKingFile - board->blackKingFile : board->blackKingFile - board->whiteKingFile) + (board->whiteKingRank > board->blackKingRank ? board->whiteKingRank - board->blackKingRank : board->blackKingRank - board->whiteKingRank);

            if (evaluation >= winingLimit && Move::IsCapture(board->lastMove) && (board->ply < 2 || Move::IsCapture(board->moveHistory[board->ply - 2]))) evaluation += tradingWhileWinningBonus;
        } else {
            if (Piece::IsPiece(board->lastMove.pieceMoved, Piece::king)) evaluation += opponentKingMoveBonus;
            else if (Piece::IsPiece(board->lastMove.pieceMoved, Piece::pawn) && ((board->sideToMove == Piece::white && board->whiteCastled && !board->blackCastled) || (board->sideToMove == Piece::black && !board->whiteCastled && board->blackCastled))) evaluation += Move::IsCapture(board->lastMove) ? pawnBreakBonus : pawnMoveBonus;

            if (!Move::IsNullMove(board->lastMove) && ((board->sideToMove == Piece::black && Move::GetStart(board->lastMove) / 8 > Move::GetTarget(board->lastMove) / 8) || (board->sideToMove == Piece::white && Move::GetStart(board->lastMove) / 8 < Move::GetTarget(board->lastMove) / 8))) evaluation -= forwardMoveBonus;
        }

        evaluation += ((rand() % randomFactor) << 1) - randomFactor;

        return evaluation;
    }

    eval Bot::MaterialAdvantage () {
        return eval(board->whitePawnsCount + 3 * (board->whiteKnightsCount + board->whiteBishopsCount) + 5 * board->whiteRooksCount + 9 * board->whiteQueensCount) - eval(board->blackPawnsCount + 3 * (board->blackKnightsCount + board->blackBishopsCount) + 5 * board->blackRooksCount + 9 * board->blackQueensCount);
    }

    bool Bot::IsEndGame () {
        return ((((board->blackQueensCount || board->whiteQueensCount) && !board->blackRooksCount && !board->whiteRooksCount)
            || ((board->blackRooksCount <= 1 || board->whiteRooksCount <= 1) && !board->blackQueensCount && !board->whiteQueensCount))
            && (board->blackKnightsCount + board->blackBishopsCount <= 1 && board->whiteKnightsCount + board->whiteBishopsCount <= 1))
            || (board->blackQueensCount + board->whiteQueensCount <= 1 && board->blackRooksCount + board->whiteRooksCount <= 1)
            || ((board->blackRooksCount <= 2 || board->whiteRooksCount <= 2) && !board->blackQueensCount && !board->whiteQueensCount && !board->blackKnightsCount && !board->whiteKnightsCount && !board->blackBishopsCount && !board->whiteBishopsCount);
    }

    // Used to generate opening moves in a condensed form
    /* void Bot::PrintOpenings () {
        for (uint16_t i = 0; i < OPENING_POSITIONS; i++) {
            board->ImportFen(openings[i].fen);
            dbg_printf("[\"0x%lX\", ", board->GenerateZobristHash());
            dbg_printf("\"0x%lX\"], ", openings[i].move);
        }
        dbg_printf(";\n");
    } */

    void Bot::Clear () {
        for (uint16_t i = 0; i < maxPrincipalVariationEntries; i++) {
            principalVariationTable[i] = MovePosition();
        }

        for (uint8_t i = 0; i < 12; i++) {
            for (uint8_t j = 0; j < 64; j++) {
                searchHistory[i][j] = 0;
            }
        }

        for (uint8_t i = 0; i < 2; i++) {
            for (uint8_t j = 0; j < maximumDepth; j++) {
                killerMoves[i][j] = Move();
            }
        }
    }
}