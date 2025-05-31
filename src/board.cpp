/**
 * @file board.cpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Definitions of chess board
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "chess.hpp"

namespace Chess {
    BoardDefinitions newBoard {};

    char* Board::CheckGameState () {
        if      (blackWins)                      { gameStateReasoning = (char*)"checkmate";   return gameState = (char*)"black wins"; }
        else if (whiteWins)                      { gameStateReasoning = (char*)"checkmate";   return gameState = (char*)"white wins"; }
        else if (blackWinsByTimeout)             { gameStateReasoning = (char*)"on time";     return gameState = (char*)"black wins"; }
        else if (whiteWinsByTimeout)             { gameStateReasoning = (char*)"on time";     return gameState = (char*)"white wins"; }
        else if (blackWinsByResignation)         { gameStateReasoning = (char*)"resignation"; return gameState = (char*)"black wins"; }
        else if (whiteWinsByResignation)         { gameStateReasoning = (char*)"resignation"; return gameState = (char*)"white wins"; }
        else if (stalemate)                      { gameStateReasoning = (char*)"stalemate";   return gameState = (char*)"draw";       }
        else if (insufficentMaterial)            { gameStateReasoning = (char*)"unwinnable";  return gameState = (char*)"draw";       }
        else if (fiftyMoveRule)                  { gameStateReasoning = (char*)"fifty moves"; return gameState = (char*)"draw";       }
        else if (threefoldRepetition)            { gameStateReasoning = (char*)"threefold";   return gameState = (char*)"draw";       }
        else if (timeoutWithInsufficentMaterial) { gameStateReasoning = (char*)"unwinnable";  return gameState = (char*)"draw";       }
        else if (drawByAgreement)                { gameStateReasoning = (char*)"agreement";   return gameState = (char*)"draw";       }
        else if (ply >= MAX_MOVE_HISTORY - 2)    { gameStateReasoning = (char*)"max history"; return gameState = (char*)"draw";       }
        else                                     { gameStateReasoning = nullptr;              return gameState = nullptr;             }
    }

    void Board::MakeMove (const Move& move, const bool ignoreTime) {
        if (matrix[Move::GetStart(move)] == Piece::empty || Move::IsNullMove(move) || CheckGameState()) return;

        if (Piece::IsSide(move.pieceMoved, Piece::white)) {
            if (Piece::IsPiece(move.pieceMoved, Piece::king)) {
                if (Move::IsFlag(move, Move::kingCastle)) {
                    PlacePieceOnSquare(matrix[H1], F1);
                    PlacePieceOnSquare(Piece::empty, H1);
                    whiteCastled = true;
                } else if (Move::IsFlag(move, Move::queenCastle)) {
                    PlacePieceOnSquare(matrix[A1], D1);
                    PlacePieceOnSquare(Piece::empty, A1);
                    whiteCastled = true;
                }

                whiteKingCastleRights = false;
                whiteQueenCastleRights = false;
            } else if (Piece::IsPiece(move.pieceMoved, Piece::rook)) {
                if (Move::GetStart(move) == H1) whiteKingCastleRights = false;
                else if (Move::GetStart(move) == A1) whiteQueenCastleRights = false;
            }
            if (Piece::IsPiece(move.pieceCaptured, Piece::rook)) {
                if (Move::GetTarget(move) == H8) blackKingCastleRights = false;
                else if (Move::GetTarget(move) == A8) blackQueenCastleRights = false;
            }
            if (Move::IsFlag(move, Move::enPassant)) {
                PlacePieceOnSquare(Piece::empty, Move::GetTarget(move) + 8);
            }
        } else {
            if (Piece::IsPiece(move.pieceMoved, Piece::king)) {
                if (Move::IsFlag(move, Move::kingCastle)) {
                    PlacePieceOnSquare(matrix[H8], F8);
                    PlacePieceOnSquare(Piece::empty, H8);
                    blackCastled = true;
                } else if (Move::IsFlag(move, Move::queenCastle)) {
                    PlacePieceOnSquare(matrix[A8], D8);
                    PlacePieceOnSquare(Piece::empty, A8);
                    blackCastled = true;
                }
                
                blackKingCastleRights = false;
                blackQueenCastleRights = false;
            } else if (Piece::IsPiece(move.pieceMoved, Piece::rook)) {
                if (Move::GetStart(move) == H8) blackKingCastleRights = false;
                else if (Move::GetStart(move) == A8) blackQueenCastleRights = false;
            }
            if (Piece::IsPiece(move.pieceCaptured, Piece::rook)) {
                if (Move::GetTarget(move) == H1) whiteKingCastleRights = false;
                else if (Move::GetTarget(move) == A1) whiteQueenCastleRights = false;
            }
            if (Move::IsFlag(move, Move::enPassant)) {
                PlacePieceOnSquare(Piece::empty, Move::GetTarget(move) - 8);
            }
        }

        piece pieceMoved = move.pieceMoved;
        if (Move::IsThisPromotion(move, Move::queenPromotion)) {
            pieceMoved = (sideToMove | Piece::queen);
        } else if (Move::IsThisPromotion(move, Move::rookPromotion)) {
            pieceMoved = (sideToMove | Piece::rook);
        } else if (Move::IsThisPromotion(move, Move::bishopPromotion)) {
            pieceMoved = (sideToMove | Piece::bishop);
        } else if (Move::IsThisPromotion(move, Move::knightPromotion)) {
            pieceMoved = (sideToMove | Piece::knight);
        }

        PlacePieceOnSquare(pieceMoved, Move::GetTarget(move));
        PlacePieceOnSquare(Piece::empty, Move::GetStart(move));
        moveHistory[ply++] = move;

        if (Move::IsFlag(move, Move::doublePawnPush)) enPassantSquare = Move::GetStart(move) + (Move::GetTarget(move) - Move::GetStart(move)) / 2;
        else enPassantSquare = OFFBOARD;
        // The fifty move rule states that the game is a draw if no pawn has been moved or piece captured in the last fifty complete cycles of the game.
        if (!Piece::IsPiece(move.pieceMoved, Piece::pawn) && !Move::IsCapture(move)) fiftyMoveCount++;
        else fiftyMoveCount = 0;

        inCheck = move.check;

        Piece::SetToOppositeSide(sideToMove);

        if (!ignoreTime && isTimedGame) {
            if (!timeRemainingAtPly[ply - 1]) {
                if (sideToMove == Piece::black) whiteTimeRemaining += bonusTime;
                else blackTimeRemaining += bonusTime;
                timeRemainingAtPly[ply - 1] = sideToMove == Piece::black ? whiteTimeRemaining : blackTimeRemaining;
            } else UpdateTimeRemainingForMove();
        }

        lastZobristHash = zobristHashHistory[ply - 1] = GenerateZobristHash();

        lastMove = move;
    }

    void Board::UnMakeMove (const bool temporary, const bool ignoreTime) {
        if (ply <= 0) return;

        blackWins = whiteWins = blackWinsByTimeout = whiteWinsByTimeout = blackWinsByResignation = whiteWinsByResignation = false;
        stalemate = fiftyMoveRule = insufficentMaterial = threefoldRepetition = timeoutWithInsufficentMaterial = drawByAgreement = false;
        gameStateReasoning = gameState = nullptr;

        lastMove = moveHistory[--ply];

        if (!temporary) {
            moveHistory[ply] = Move();

            zobristHashHistory[ply] = 0;
        }

        Piece::SetToOppositeSide(sideToMove);

        if (!ignoreTime && isTimedGame) {
            if (!temporary) timeRemainingAtPly[ply] = 0;
            UpdateTimeRemainingForMove();
        }

        if (Piece::IsSide(lastMove.pieceMoved, Piece::white)) {
            if (lastMove.pieceMoved == Piece::whiteKing || lastMove.pieceMoved == Piece::whiteRook) {
                UpdateWhitesCastleRights();
            }
            if (lastMove.pieceCaptured == Piece::blackRook) {
                UpdateBlacksCastleRights();
            }
            if (Move::IsFlag(lastMove, Move::kingCastle)) {
                PlacePieceOnSquare(matrix[F1], H1);
                PlacePieceOnSquare(Piece::empty, F1);
                whiteCastled = true;
            } else if (Move::IsFlag(lastMove, Move::queenCastle)) {
                PlacePieceOnSquare(matrix[D1], A1);
                PlacePieceOnSquare(Piece::empty, D1);
                whiteCastled = true;
            } else if (Move::IsFlag(lastMove, Move::enPassant)) {
                PlacePieceOnSquare(Piece::blackPawn, Move::GetTarget(lastMove) + 8);
                enPassantSquare = Move::GetTarget(lastMove);
            }
        } else {
            if (lastMove.pieceMoved == Piece::blackKing || lastMove.pieceMoved == Piece::blackRook) {
                UpdateBlacksCastleRights();
            }
            if (lastMove.pieceCaptured == Piece::whiteRook) {
                UpdateWhitesCastleRights();
            }
            if (Move::IsFlag(lastMove, Move::kingCastle)) {
                PlacePieceOnSquare(matrix[F8], H8);
                PlacePieceOnSquare(Piece::empty, F8);
                blackCastled = true;
            } else if (Move::IsFlag(lastMove, Move::queenCastle)) {
                PlacePieceOnSquare(matrix[D8], A8);
                PlacePieceOnSquare(Piece::empty, D8);
                blackCastled = true;
            } else if (Move::IsFlag(lastMove, Move::enPassant)) {
                PlacePieceOnSquare(Piece::whitePawn, Move::GetTarget(lastMove) - 8);
                enPassantSquare = Move::GetTarget(lastMove);
            }
        }

        PlacePieceOnSquare(lastMove.pieceMoved, Move::GetStart(lastMove));
        PlacePieceOnSquare(lastMove.pieceCaptured, Move::GetTarget(lastMove));

        Move previousLastMove = ply != 0 ? moveHistory[ply - 1] : Move();
        if (Move::IsFlag(previousLastMove, Move::doublePawnPush)) enPassantSquare = Move::GetStart(previousLastMove) + (Move::GetTarget(previousLastMove) - Move::GetStart(previousLastMove)) / 2;
        else enPassantSquare = OFFBOARD;

        inCheck = ply != 0 ? previousLastMove.check : initalInCheck;

        lastZobristHash = ply != 0 ? zobristHashHistory[ply - 1] : initialZobristHash;
        
        lastMove = previousLastMove;

        UpdateInsufficentMaterial();
        UpdateFiftyMoveCount();
        UpdateThreefoldRepetition();
    }

    void Board::RedoMove () { if (!IsNextMoveNull()) MakeMove(moveHistory[ply]); };

    bool Board::IsNextMoveNull () { return Move::IsNullMove(moveHistory[ply]); };

    void Board::UpdateInCheck () {
        inCheck = false;
        piece playingSide = sideToMove;
        piece opposingSide = Piece::GetOppositeSide(sideToMove);

        sideToMove = opposingSide;

        GeneratePseudoLegalMoves(pseudoLegalFirstStrikes, pseudoLegalFirstStrikesAvailable, true);

        uint8_t playingKingSquare = playingSide == Piece::white ? whiteKingSquare : blackKingSquare;

        for (uint8_t i = 0; i < pseudoLegalFirstStrikesAvailable && !inCheck; inCheck = Move::GetTarget(pseudoLegalFirstStrikes[i++]) == playingKingSquare);

        if (inCheck && ply) {
            lastMove.check = moveHistory[ply - 1].check = true;
            uint8_t lastCharacterIndex = 0;
            for (; lastCharacterIndex < MAX_ALGEBRAIC_NOTATION_LENGTH - 1 && lastMove.algebraicNotation[lastCharacterIndex] != '\0' && lastMove.algebraicNotation[lastCharacterIndex] != '+'; lastCharacterIndex++);
            lastMove.algebraicNotation[lastCharacterIndex] = moveHistory[ply - 1].algebraicNotation[lastCharacterIndex] = '+';
        }

        sideToMove = playingSide;
    }

    bool Board::UpdateInsufficentMaterial () {
        /*
        A game has insufficent material in the following cases:
        1 king             v. 1 king
        1 king + 1 knight  v. 1 king
        1 king + 2 knights v. 1 king
        1 king + 1 knight  v. 1 king + 1 knight
        1 king + 1 bishop  v. 1 king
        1 king + 1 bishop  v. 1 king + 1 bishop
        */

        if (blackPawnsCount || blackRooksCount || blackQueensCount || whitePawnsCount || whiteRooksCount || whiteQueensCount) return insufficentMaterial = false;
        // There are no pawns, rooks, or queens on the board.
        if (!blackKnightsCount && !whiteKnightsCount && !blackBishopsCount && !whiteBishopsCount) return insufficentMaterial = true; // There are only kings on the board
        
        if (!blackBishopsCount && !whiteBishopsCount) {
            // There are no bishops on the board, only knights and kings
            if (blackKnightsCount == 1 && !whiteKnightsCount) return insufficentMaterial = true;
            if (whiteKnightsCount == 1 && !blackKnightsCount) return insufficentMaterial = true;
            if (blackKnightsCount == 1 && whiteKnightsCount == 1) return insufficentMaterial = true;
            if (blackKnightsCount == 2 && !whiteKnightsCount) return insufficentMaterial = true;
            if (whiteKnightsCount == 2 && !blackKnightsCount) return insufficentMaterial = true;
        } else if (!blackKnightsCount && !whiteKnightsCount) {
            // There are no knights on the board, only bishops and kings
            if (blackBishopsCount == 1 && !whiteBishopsCount) return insufficentMaterial = true;
            if (whiteBishopsCount == 1 && !blackBishopsCount) return insufficentMaterial = true;
            if (blackBishopsCount == 1 && whiteBishopsCount == 1) return insufficentMaterial = true;
        }

        return insufficentMaterial = false;
    }

    bool Board::DoesBlackHaveInsufficentMaterial () {
        if (blackPawnsCount || blackRooksCount || blackQueensCount) return false;
        // There are no pawns, rooks, or queens on the board
        if (!blackKnightsCount && !blackBishopsCount) return true; // There are only kings on the board

        if (!blackBishopsCount) return blackKnightsCount == 1; // There are no bishops on the board, only knights and kings
        else if (!blackKnightsCount) return blackBishopsCount == 1; // There are no knights on the board, only bishops and kings
        
        return false;
    }

    bool Board::DoesWhiteHaveInsufficentMaterial () {
        if (whitePawnsCount || whiteRooksCount || whiteQueensCount) return false;
        // There are no pawns, rooks, or queens on the board
        if (!whiteKnightsCount && !whiteBishopsCount) return true; // There are only kings on the board

        if (!blackBishopsCount) return whiteKnightsCount == 1; // There are no bishops on the board, only knights and kings
        else if (!whiteKnightsCount) return whiteBishopsCount == 1; // There are no knights on the board, only bishops and kings
        
        return false;
    }

    void Board::UpdateFiftyMoveCount () {
        fiftyMoveCount = initialFiftyMoveCount;

        for (uint8_t index = 0; index < ply; index++) {
            if (!Piece::IsPiece(moveHistory[index].pieceMoved, Piece::pawn) && !Move::IsCapture(moveHistory[index])) fiftyMoveCount++;
            else fiftyMoveCount = 0;
        }
    }

    void Board::UpdateThreefoldRepetition () {
        /*
        A position is considered equal if the following are true:
        the board has the same types of pieces in the same squares
        the side to move is the same
        the castling rights are the same
        there were no enpassant captures possible in the first occurrence
        */
        threefoldRepetition = false;
        
        if (ply == 0) return; // It would be imposable to have a three move repetition before playing a single move.

        // Because moves that reset the fifty move count are irreversible, the hashes before when the fifty move count was set to zero can be ignored.
        repetitions = 0;
        for (uint8_t index = ply > fiftyMoveCount ? ply - fiftyMoveCount - 1 : 0; index < ply - 1 && !threefoldRepetition; index++) {
            if (zobristHashHistory[index] == lastZobristHash) threefoldRepetition = ++repetitions >= 3; // There were two previous board positions that were the same as the current board position.
        }
    }

    void Board::UpdateWhitesCastleRights () {
        whiteKingCastleRights = initialWhiteKingCastleRights;
        whiteQueenCastleRights = initialWhiteQueenCastleRights;
        if (!initialWhiteKingCastleRights && !initialWhiteQueenCastleRights) return;
        // This loop checks to see whether white has every moved their king side or queen side rook, moved their king, or castled before, and if so, removes their right to castle on whichever side.
        for (uint8_t index = 0; index < ply; index++) {
            if (moveHistory[index].pieceMoved == Piece::whiteRook) {
                if (Move::GetStart(moveHistory[index]) == H1) whiteKingCastleRights = false;
                else if (Move::GetStart(moveHistory[index]) == A1) whiteQueenCastleRights = false;
            } else if (moveHistory[index].pieceMoved == Piece::whiteKing) { // Castling before is included here because it is internally classified as moving the king.
                whiteQueenCastleRights = false;
                whiteKingCastleRights = false;
                break;
            } else if (moveHistory[index].pieceCaptured == Piece::whiteRook) {
                if (Move::GetTarget(moveHistory[index]) == H1) whiteKingCastleRights = false;
                else if (Move::GetTarget(moveHistory[index]) == A1) whiteQueenCastleRights = false;
            }
        }
    }

    void Board::UpdateBlacksCastleRights () {
        blackKingCastleRights = initialBlackKingCastleRights;
        blackQueenCastleRights = initialBlackQueenCastleRights;
        if (!initialBlackKingCastleRights && !initialBlackQueenCastleRights) return;
        // This loop checks to see whether black has every moved their king side or queen side rook, moved their king, or castled before, and if so, removes their right to castle on whichever side.
        for (uint8_t index = 0; index < ply; index++) {
            if (moveHistory[index].pieceMoved == Piece::blackRook) {
                if (Move::GetStart(moveHistory[index]) == H8) blackKingCastleRights = false;
                else if (Move::GetStart(moveHistory[index]) == A8) blackQueenCastleRights = false;
            } else if (moveHistory[index].pieceMoved == Piece::blackKing) { // Castling before is included here because it is internally classified as moving the king.
                blackQueenCastleRights = false;
                blackKingCastleRights = false;
                break;
            } else if (moveHistory[index].pieceCaptured == Piece::blackRook) {
                if (Move::GetTarget(moveHistory[index]) == H8) blackKingCastleRights = false;
                else if (Move::GetTarget(moveHistory[index]) == A8) blackQueenCastleRights = false;
            }
        }
    }

    uint8_t Board::GenerateLegalMoves (Move* moves, uint8_t& available, const bool onlyCaptures) {
        available = 0;

        UpdateInsufficentMaterial();
        UpdateThreefoldRepetition();
        if (CheckGameState()) return 1;

        UpdateInCheck();
        const bool currentlyInCheck = inCheck;

        const uint8_t playingSide = sideToMove;
        const uint8_t& playingKingSquare = playingSide == Piece::white ? whiteKingSquare : blackKingSquare;

        GeneratePseudoLegalMoves(pseudoLegalMoves, pseudoLegalMovesAvailable, onlyCaptures);

        for (uint8_t i = 0; i < pseudoLegalMovesAvailable; ++i) {
            MakeMove(pseudoLegalMoves[i], true);

            bool isLegalMove = true;
            const bool moveIsKingCastle = Move::IsFlag(pseudoLegalMoves[i], Move::kingCastle);
            const bool moveIsQueenCastle = Move::IsFlag(pseudoLegalMoves[i], Move::queenCastle);

            if (moveIsKingCastle || moveIsQueenCastle) isLegalMove = !currentlyInCheck;
            
            if (isLegalMove) {
                GeneratePseudoLegalMoves(pseudoLegalResponses, pseudoLegalResponsesAvailable, !moveIsKingCastle && !moveIsQueenCastle);

                for (uint8_t j = 0; j < pseudoLegalResponsesAvailable && isLegalMove; ++j) {
                    isLegalMove = Move::GetTarget(pseudoLegalResponses[j]) != playingKingSquare;

                    if (isLegalMove) {
                        if (moveIsKingCastle) {
                            if (playingSide == Piece::white) isLegalMove = Move::GetTarget(pseudoLegalResponses[j]) != F1;
                            else isLegalMove = Move::GetTarget(pseudoLegalResponses[j]) != F8;
                        } else if (moveIsQueenCastle) {
                            if (playingSide == Piece::white) isLegalMove = Move::GetTarget(pseudoLegalResponses[j]) != D1;
                            else isLegalMove = Move::GetTarget(pseudoLegalResponses[j]) != D8;
                        }
                    }
                }
            }

            if (isLegalMove) moves[available++] = pseudoLegalMoves[i];
            
            UnMakeMove(false, true);
        }

        inCheck = currentlyInCheck;

        if (!onlyCaptures && available == 0) {
            if (!inCheck) stalemate = true;
            else if (playingSide == Piece::black) whiteWins = true;
            else blackWins = true;
            SetCheckmateAlgebraicNotation();
            CheckGameState();
            return 1;
        } else SetAlgebraicNotations(moves, available);

        return 0;
    }

    void Board::GeneratePseudoLegalMoves (Move* moves, uint8_t& offset, const bool onlyCaptures) {
        offset = 0;

        for (uint8_t square = 0; square < 64; square++) {
            if (!Piece::IsSide(matrix[square], sideToMove)) continue;

            switch (Piece::GetPiece(matrix[square])) {
                case Piece::pawn: GeneratePawnMoves(moves, offset, square, onlyCaptures); break;
                case Piece::knight: GenerateKnightMoves(moves, offset, square, onlyCaptures); break;
                case Piece::bishop: GenerateBishopMoves(moves, offset, square, onlyCaptures); break;
                case Piece::rook: GenerateRookMoves(moves, offset, square, onlyCaptures); break;
                case Piece::queen: GenerateQueenMoves(moves, offset, square, onlyCaptures); break;
                case Piece::king: GenerateKingMoves(moves, offset, square, onlyCaptures); break;
            }
        }
    }

    void Board::GeneratePawnMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        if (matrix[square] == Piece::whitePawn) {
            if (!onlyCaptures && square >= 16 && matrix[square - 8] == Piece::empty) moves[offset++] = Move(Move::quietMove, square, square - 8, Piece::whitePawn, Piece::empty);
            if (!onlyCaptures && square >= 48 && square < 56 && matrix[square - 8] == Piece::empty && matrix[square - 16] == Piece::empty) moves[offset++] = Move(Move::doublePawnPush, square, square - 16, Piece::whitePawn, Piece::empty);
            if (square % 8 != 0 && Piece::IsSide(matrix[square - 9], Piece::black)) {
                if (square >= 16) moves[offset++] = Move(Move::capture, square, square - 9, Piece::whitePawn, matrix[square - 9]);
                else {
                    moves[offset++] = Move(Move::knightPromotionCapture, square, square - 9, Piece::whitePawn, matrix[square - 9]);
                    moves[offset++] = Move(Move::bishopPromotionCapture, square, square - 9, Piece::whitePawn, matrix[square - 9]);
                    moves[offset++] = Move(Move::rookPromotionCapture, square, square - 9, Piece::whitePawn, matrix[square - 9]);
                    moves[offset++] = Move(Move::queenPromotionCapture, square, square - 9, Piece::whitePawn, matrix[square - 9]);
                }
            }
            if (square % 8 != 7 && Piece::IsSide(matrix[square - 7], Piece::black)) {
                if (square >= 16) moves[offset++] = Move(Move::capture, square, square - 7, Piece::whitePawn, matrix[square - 7]);
                else {
                    moves[offset++] = Move(Move::knightPromotionCapture, square, square - 7, Piece::whitePawn, matrix[square - 7]);
                    moves[offset++] = Move(Move::bishopPromotionCapture, square, square - 7, Piece::whitePawn, matrix[square - 7]);
                    moves[offset++] = Move(Move::rookPromotionCapture, square, square - 7, Piece::whitePawn, matrix[square - 7]);
                    moves[offset++] = Move(Move::queenPromotionCapture, square, square - 7, Piece::whitePawn, matrix[square - 7]);
                }
            }
            if (square % 8 != 0 && enPassantSquare == square - 9 && matrix[square - 1] == Piece::blackPawn) moves[offset++] = Move(Move::enPassant, square, square - 9, Piece::whitePawn, matrix[square - 9]);
            if (square % 8 != 7 && enPassantSquare == square - 7 && matrix[square + 1] == Piece::blackPawn) moves[offset++] = Move(Move::enPassant, square, square - 7, Piece::whitePawn, matrix[square - 7]);
            if (!onlyCaptures && square >= 8 && square < 16 && matrix[square - 8] == Piece::empty) {
                moves[offset++] = Move(Move::queenPromotion, square, square - 8, Piece::whitePawn, Piece::empty);
                moves[offset++] = Move(Move::rookPromotion, square, square - 8, Piece::whitePawn, Piece::empty);
                moves[offset++] = Move(Move::bishopPromotion, square, square - 8, Piece::whitePawn, Piece::empty);
                moves[offset++] = Move(Move::knightPromotion, square, square - 8, Piece::whitePawn, Piece::empty);
            }
        } else {
            if (!onlyCaptures && square < 48 && matrix[square + 8] == Piece::empty) moves[offset++] = Move(Move::quietMove, square, square + 8, Piece::blackPawn, Piece::empty);
            if (!onlyCaptures && square >= 8 && square < 16 && matrix[square + 8] == Piece::empty && matrix[square + 16] == Piece::empty) moves[offset++] = Move(Move::doublePawnPush, square, square + 16, Piece::blackPawn, Piece::empty);
            if (square % 8 != 0 && Piece::IsSide(matrix[square + 7], Piece::white)) {
                if (square < 48) moves[offset++] = Move(Move::capture, square, square + 7, Piece::blackPawn, matrix[square + 7]);
                else {
                    moves[offset++] = Move(Move::queenPromotionCapture, square, square + 7, Piece::blackPawn, matrix[square + 7]);
                    moves[offset++] = Move(Move::rookPromotionCapture, square, square + 7, Piece::blackPawn, matrix[square + 7]);
                    moves[offset++] = Move(Move::bishopPromotionCapture, square, square + 7, Piece::blackPawn, matrix[square + 7]);
                    moves[offset++] = Move(Move::knightPromotionCapture, square, square + 7, Piece::blackPawn, matrix[square + 7]);
                }
            }
            if (square % 8 != 7 && Piece::IsSide(matrix[square + 9], Piece::white)) {
                if (square < 48) moves[offset++] = Move(Move::capture, square, square + 9, Piece::blackPawn, matrix[square + 9]);
                else {
                    moves[offset++] = Move(Move::queenPromotionCapture, square, square + 9, Piece::blackPawn, matrix[square + 9]);
                    moves[offset++] = Move(Move::rookPromotionCapture, square, square + 9, Piece::blackPawn, matrix[square + 9]);
                    moves[offset++] = Move(Move::bishopPromotionCapture, square, square + 9, Piece::blackPawn, matrix[square + 9]);
                    moves[offset++] = Move(Move::knightPromotionCapture, square, square + 9, Piece::blackPawn, matrix[square + 9]);
                }
            }
            if (square % 8 != 0 && enPassantSquare == square + 7 && matrix[square - 1] == Piece::whitePawn) moves[offset++] = Move(Move::enPassant, square, square + 7, Piece::blackPawn, matrix[square + 7]);
            if (square % 8 != 7 && enPassantSquare == square + 9 && matrix[square + 1] == Piece::whitePawn) moves[offset++] = Move(Move::enPassant, square, square + 9, Piece::blackPawn, matrix[square + 9]);
            if (!onlyCaptures && square >= 48 && square < 56 && matrix[square + 8] == Piece::empty) {
                moves[offset++] = Move(Move::queenPromotion, square, square + 8, Piece::blackPawn, Piece::empty);
                moves[offset++] = Move(Move::rookPromotion, square, square + 8, Piece::blackPawn, Piece::empty);
                moves[offset++] = Move(Move::bishopPromotion, square, square + 8, Piece::blackPawn, Piece::empty);
                moves[offset++] = Move(Move::knightPromotion, square, square + 8, Piece::blackPawn, Piece::empty);
            }
        }
    }

    void Board::GenerateKnightMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        AddMoveToSquare(moves, offset, square, 1, -2, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 2, -1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 2, 1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 1, 2, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -1, 2, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -2, 1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -2, -1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -1, -2, onlyCaptures);
    }

    void Board::GenerateBishopMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, i, i, onlyCaptures); i++);

        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, -i, i, onlyCaptures); i++);

        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, i, -i, onlyCaptures); i++);

        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, -i, -i, onlyCaptures); i++);
    }

    void Board::GenerateRookMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, i, 0, onlyCaptures); i++);

        for (int8_t i = -1; i >= -7 && !AddMoveToSquare(moves, offset, square, i, 0, onlyCaptures); i--);

        for (int8_t i = 1; i <= 7 && !AddMoveToSquare(moves, offset, square, 0, i, onlyCaptures); i++);

        for (int8_t i = -1; i >= -7 && !AddMoveToSquare(moves, offset, square, 0, i, onlyCaptures); i--);
    }

    void Board::GenerateQueenMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        GenerateBishopMoves(moves, offset, square, onlyCaptures);
        GenerateRookMoves(moves, offset, square, onlyCaptures);
    }

    void Board::GenerateKingMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures) {
        AddMoveToSquare(moves, offset, square, 0, -1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 1, -1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 1, 0, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 1, 1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, 0, 1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -1, 1, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -1, 0, onlyCaptures);
        AddMoveToSquare(moves, offset, square, -1, -1, onlyCaptures);

        if (!onlyCaptures && matrix[square] == Piece::whiteKing) {
            if (whiteKingCastleRights && matrix[square + 1] == Piece::empty && matrix[square + 2] == Piece::empty) moves[offset++] = Move(Move::kingCastle, square, square + 2, Piece::whiteKing, Piece::empty);
            if (whiteQueenCastleRights && matrix[square - 1] == Piece::empty && matrix[square - 2] == Piece::empty && matrix[square - 3] == Piece::empty) moves[offset++] = Move(Move::queenCastle, square, square - 2, Piece::whiteKing, Piece::empty);
        } else if (!onlyCaptures) {
            if (blackKingCastleRights && matrix[square + 1] == Piece::empty && matrix[square + 2] == Piece::empty) moves[offset++] = Move(Move::kingCastle, square, square + 2, Piece::blackKing, Piece::empty);
            if (blackQueenCastleRights && matrix[square - 1] == Piece::empty && matrix[square - 2] == Piece::empty && matrix[square - 3] == Piece::empty) moves[offset++] = Move(Move::queenCastle, square, square - 2, Piece::blackKing, Piece::empty);
        }
    }

    int8_t Board::AddMoveToSquare (Move* moves, uint8_t& offset, const uint8_t square, int8_t file, int8_t rank, const bool onlyCaptures) {
        file += square % 8;
        rank += square / 8;

        if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;

        uint8_t target = file + 8 * rank;

        if (matrix[target] == Piece::empty) {
            if (!onlyCaptures) moves[offset++] = Move(Move::quietMove, square, target, matrix[square], matrix[target]);
            return 0;
        } else if (!Piece::IsSide(matrix[square], Piece::GetSide(matrix[target]))) moves[offset++] = Move(Move::capture, square, target, matrix[square], matrix[target]);

        return 1;
    }

    uint8_t Board::Menu (Option* currentOptions, const uint8_t numberOfOptions) {
        uint8_t key = 0;

        uint8_t selectedOption = 0;
        Option* option = currentOptions + selectedOption;
        bool cannotAccessOption = (ply && option->names[1]) || (option->callBack == Option::UpdateTimedGame && (whitePlayer != HUMAN || blackPlayer != HUMAN)) || ((option->callBack == Option::UpdateWhite || option->callBack == Option::UpdateBlack) && isTimedGame);

        DisplayOptions(currentOptions, numberOfOptions, selectedOption);

        while (key != sk_Clear && key != sk_Del && key != sk_0 && !(key == sk_Chs && currentOptions != mainMenu) && key != sk_DecPnt) {
            key = Key();

            if (option->type == Option::text) DisplayOptions(currentOptions, numberOfOptions, selectedOption);
            if (!key) continue;

            if (option->type != Option::text) {
                uint8_t digit = 0;

                switch (key) {
                    case sk_1: digit = 1; break;
                    case sk_2: digit = 2; break;
                    case sk_3: digit = 3; break;
                    case sk_4: digit = 4; break;
                    case sk_5: digit = 5; break;
                    case sk_6: digit = 6; break;
                    case sk_7: digit = 7; break;
                    case sk_8: digit = 8; break;
                    case sk_9: digit = 9; break;
                }

                if (digit && --digit < numberOfOptions) {
                    option->nameToUse = 0;
                    selectedOption = digit;
                    option = currentOptions + selectedOption;
                    cannotAccessOption = (ply && option->names[1]) || (option->callBack == Option::UpdateTimedGame && (whitePlayer != HUMAN || blackPlayer != HUMAN)) || ((option->callBack == Option::UpdateWhite || option->callBack == Option::UpdateBlack) && isTimedGame);
                    if (option->type == Option::menu || option->type == Option::button) key = sk_2nd;
                    else key = 0;
                }
            }

            if (key == sk_2nd || key == sk_Enter) {
                if (option->type == Option::menu) {
                    uint8_t _key = Menu(option->options, option->numberOfOptions);
                    if (_key == sk_Clear || ((_key == sk_0 || _key == sk_Chs) && currentOptions != mainMenu)) return _key;
                    else if (_key == sk_Chs) GameLoop();
                    selectedOption = 0;
                } else if (option->type == Option::button) {
                    option->nameToUse = 1;
                    uint8_t _key = OptionCallBack(option);
                    if (_key == sk_Clear || ((_key == sk_0 || _key == sk_Chs) && currentOptions != mainMenu)) return _key;
                    else if (_key == sk_Chs) GameLoop();
                } else key = sk_Down;
            }
            if (key == sk_Up || ((key == sk_Left || key == sk_Sub) && option->type != Option::cycle && option->type != Option::number)) {
                option->nameToUse = 0;
                selectedOption = !selectedOption ? numberOfOptions - 1 : selectedOption - 1;
            } else if (key == sk_Down || ((key == sk_Right || key == sk_Add) && option->type != Option::cycle && option->type != Option::number)) {
                option->nameToUse = 0;
                selectedOption = selectedOption >= numberOfOptions - 1 ? 0 : selectedOption + 1;
            } else if (key == sk_Left || key == sk_Sub) {
                if (option->type == Option::cycle) {
                    if (cannotAccessOption) option->nameToUse = 1;
                    else {
                        option->value = !option->value ? option->numberOfCycleValues - 1 : option->value - 1;
                        OptionCallBack(option);
                    }
                } else if (option->type == Option::number) {
                    if (cannotAccessOption) option->nameToUse = 1;
                    else {
                        option->value = option->value <= option->lowerBound ? option->upperBound : option->value - option->step;
                        OptionCallBack(option);
                    }
                }
            } else if (key == sk_Right || key == sk_Add) {
                if (option->type == Option::cycle) {
                    if (cannotAccessOption) option->nameToUse = 1;
                    else {
                        option->value = option->value >= option->numberOfCycleValues - 1 ? 0 : option->value + 1;
                        OptionCallBack(option);
                    }
                } else if (option->type == Option::number) {
                    if (cannotAccessOption) option->nameToUse = 1;
                    else {
                        option->value = option->value >= option->upperBound ? option->lowerBound : option->value + option->step;
                        OptionCallBack(option);
                    }
                } else if (option->type == Option::menu) Menu(option->options, option->numberOfOptions);
            } else if (key && option->type == Option::text) {
                const char character = GetCharacterForKey(key);

                if (character) {
                    if (character != '0') {
                        uint8_t length = 0;
                        for (int64_t temp = option->value; temp; temp >>= 5, length++);

                        if (length < option->upperBound) {
                            option->value <<= 5;
                            option->value |= character - 'A' + 1;
                        }
                    } else if (option->value) {
                        option->value >>= 5;
                        key = 0;
                    }

                    OptionCallBack(option);
                }
            } else if (key == sk_Clear || key == sk_Del || key == sk_0 || key == sk_DecPnt || key == sk_Chs) {
                option->nameToUse = 0;
                for (uint8_t index = 0; index < numberOfOptions; index++) if (currentOptions[index].type == Option::text) currentOptions[index].value = 0;
                if (key == sk_Chs && currentOptions == mainMenu) GameLoop();
            }

            if (key != sk_Clear && key != sk_Del && key != sk_0 && !(key == sk_Chs && currentOptions != mainMenu) && key != sk_DecPnt) DisplayOptions(currentOptions, numberOfOptions, selectedOption);
            option = currentOptions + selectedOption;
            cannotAccessOption = (ply && option->names[1]) || (option->callBack == Option::UpdateTimedGame && (whitePlayer != HUMAN || blackPlayer != HUMAN)) || ((option->callBack == Option::UpdateWhite || option->callBack == Option::UpdateBlack) && isTimedGame);
        }

        return key;
    }

    uint8_t Board::OptionCallBack (Option* option) {
        if (!(option->callBack)) return 0;

        switch (option->callBack) {
            case Option::GameLoop: return GameLoop();
            case Option::UpdateAlwaysPromoteToPiece: promotePiece = AdjustedLinearPromotionValue(option->value); return 0;
            case Option::UpdateWhite: whitePlayer = option->value; return 0;
            case Option::UpdateBlack: blackPlayer = option->value; return 0;
            case Option::UpdateTimedGame: isTimedGame = option->value; paused = isTimedGame && paused; return 0;
            case Option::UpdateLowTimeWarning: lowTimeWarning = option->value; return 0;
            case Option::UpdateWhiteStartingTime: whiteTimeRemaining = blackTimeRemaining = whiteStartingTime = blackStartingTime = timingOptions[3].value = option->value; return 0;
            case Option::UpdateBlackStartingTime: blackTimeRemaining = blackStartingTime = option->value; return 0;
            case Option::UpdateBonusTime: bonusTime = option->value; return 0;
            case Option::UpdateBoardView: boardView = option->value; return 0;
            case Option::UpdateTilePalette: tilePalette = option->value * TILE_PALETTE_OFFSET; return 0;
            case Option::UpdatePieceStyle: pieceStyle = option->value * PIECE_STYLE_OFFSET; return 0;
            case Option::UpdateCheckHighlighting: checkHighlighting = option->value; return 0;
            case Option::UpdateLastMoveHighlighting: lastMoveHighlighting = option->value; return 0;
            case Option::UpdateBackground: background = option->value; return 0;
            case Option::UpdateHideLegalMoveDots: legalMoveDots = option->value; return 0;
            case Option::SaveGame: option->nameToUse = SaveGame(); return 0;
            case Option::SaveAsDefault: option->nameToUse = SaveGame(defaultSaveName, false); return 0;
            case Option::SavePGN: option->nameToUse = ExportPGN(); return 0;
            case Option::SaveWhiteStatistics: option->nameToUse = SaveStatistics(whiteStatistics); return 0;
            case Option::SaveBlackStatistics: option->nameToUse = SaveStatistics(blackStatistics); return 0;
            case Option::UpdateSavePGNOnGameOver: savePGNOnGameOver = option->value; return 0;
            case Option::LoadSave: option->nameToUse = LoadSave(); return 0;
            case Option::LoadWhiteStatistics: option->nameToUse = LoadStatistics(whiteStatistics, true); return 0;
            case Option::LoadBlackStatistics: option->nameToUse = LoadStatistics(blackStatistics, false); return 0;
            case Option::ViewPGN: { uint8_t key = ViewText("PGN VIEWER"); option->nameToUse = !key ? FILE_FAILURE : 0; return key; }
            case Option::ViewWhiteStatistics: return ViewStatistics(whiteStatistics);
            case Option::ViewBlackStatistics: return ViewStatistics(blackStatistics);
            case Option::DeleteSave: option->nameToUse = DeleteSave(); return 0;
            case Option::DeleteDefaultSave: option->nameToUse = DeleteSave(defaultSaveName, false); return 0;
            case Option::ResetBoard: ImportFen(startPosition); return 0;
            case Option::ResetSettings: RestoreSettings(); return 0;
            case Option::ResetToDefault: RestoreSettings(); ImportFen(startPosition); return 0;
            case Option::ResetWhiteStatistics: whiteStatistics = Statistics(WHITE_ENCODED); return 0;
            case Option::ResetBlackStatistics: blackStatistics = Statistics(BLACK_ENCODED); return 0;
            case Option::ResetAll: RestoreSettings(); ImportFen(startPosition); whiteStatistics = Statistics(WHITE_ENCODED); blackStatistics = Statistics(BLACK_ENCODED); return 0;
            case Option::UpdateResetBoardOn: resetBoardOn = option->value; return 0;
            case Option::ViewHelp: { uint8_t key = ViewText("HELP", helpPageName, false); option->nameToUse = !key ? FILE_FAILURE : 0; return key; }
            default: return 0;
        }
    }

    bool Board::IsMoveKey (const uint8_t key, const uint8_t comparisonKey) {
        if (key == comparisonKey && IsBoardUpright()) return true;

        if (boardView == ONLY_BLACK ||
            (boardView == DYNAMIC && sideToMove == Piece::black) ||
            (boardView == FOLLOW_PLAYER && whitePlayer != HUMAN && blackPlayer == HUMAN) ||
            ((boardView == FOLLOW_PLAYER && (whitePlayer != HUMAN) == (blackPlayer != HUMAN)) && sideToMove == Piece::black))
            return (comparisonKey == sk_Up && key == sk_Down) || (comparisonKey == sk_Down && key == sk_Up) || (comparisonKey == sk_Left && key == sk_Right) || (comparisonKey == sk_Right && key == sk_Left);

        return false;
    }

    bool Board::IsBoardUpright () {
        return boardView == ONLY_WHITE ||
            (boardView == DYNAMIC && sideToMove == Piece::white) ||
            (boardView == FOLLOW_PLAYER && whitePlayer == HUMAN && blackPlayer != HUMAN) ||
            ((boardView == FOLLOW_PLAYER && (whitePlayer != HUMAN) == (blackPlayer != HUMAN)) && sideToMove == Piece::white);
    };
    uint8_t Board::AdjustedLinearPromotionValue (uint8_t value) {
        switch (value) {
            case 0: return Piece::queen;
            case 1: return Piece::rook;
            case 2: return Piece::bishop;
            case 3: return Piece::knight;
            default: return Piece::empty;
        }
    };
    uint8_t Board::DefaultLinearPromotionValue (uint8_t value) {
        switch (value) {
            case Piece::queen: return 0;
            case Piece::rook: return 1;
            case Piece::bishop: return 2;
            case Piece::knight: return 3;
            default: return 4;
        }
    };

    uint8_t Board::GameLoop () {
        Move move;
        uint8_t key = 0;

        previousClockTime = clock();

        if (resetBoardOn == RESET_BOARD_ON_ENTER) ImportFen(startPosition);
        else if (resetBoardOn == RESET_BOARD_ON_ENTER_WITH_GAME_OVER && IsNextMoveNull()) {
            GenerateLegalMoves(legalMoves, movesAvailable);
            if (CheckGameState()) ImportFen(startPosition);
        }

        if (IsNextMoveNull()) GenerateLegalMoves(legalMoves, movesAvailable);

        DisplayGame();

        while (key != sk_Clear && key != sk_Del && key != sk_0 && key != sk_DecPnt) {
            key = Key(true);

            if (((whitePlayer != HUMAN && sideToMove == Piece::white) || (blackPlayer != HUMAN && sideToMove == Piece::black)) && IsNextMoveNull() && !CheckGameState()) {
                uint8_t previousSelection = selection;
                selection = OFFBOARD;
                selected = OFFBOARD;

                thinking = true;
                DisplayGame();
                thinking = false;
                selection = previousSelection;

                Move botMove = bot.Think();

                if (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyDel)) {
                    MakeMove(botMove);

                    GenerateLegalMoves(legalMoves, movesAvailable);

                    AddMoveToStatistics();

                    previousClockTime = clock();

                    if (!(((whitePlayer != HUMAN && sideToMove == Piece::white) || (blackPlayer != HUMAN && sideToMove == Piece::black)) && !CheckGameState())) DisplayGame();
                } else if (kb_IsDown(kb_KeyClear)) key = sk_Clear;
                else key = sk_Del;
            }

            if (IsMoveKey(key, sk_Up) && selection != OFFBOARD) { selection += selection < 8 ? 56 : -8; }
            else if (IsMoveKey(key, sk_Down) && selection != OFFBOARD) { selection += 8; selection %= 64; }
            else if (IsMoveKey(key, sk_Left) && selection != OFFBOARD) { selection += selection % 8 ? -1 : 7; }
            else if (IsMoveKey(key, sk_Right) && selection != OFFBOARD) { selection += selection % 8 == 7 ? -7 : 1; }
            else if ((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && IsNextMoveNull()) {
                bool keyIsSelect = key == sk_2nd || key == sk_Enter;
                
                if (selected == OFFBOARD && keyIsSelect) selected = selection;
                else if (selection == selected && keyIsSelect) selected = OFFBOARD;
                else if (selected != OFFBOARD && (keyIsSelect || kb_IsDown(kb_KeyDiv) || kb_IsDown(kb_KeyMul) || kb_IsDown(kb_KeySub) || kb_IsDown(kb_KeyAdd))) {
                    for (uint8_t index = 0; index < movesAvailable; index++) {
                        if (Move::GetStart(legalMoves[index]) == selected && Move::GetTarget(legalMoves[index]) == selection) {
                            bool isPromotion = Move::IsPromotion(legalMoves[index]);

                            if (isPromotion) {
                                if ((kb_IsDown(kb_KeyDiv) || promotePiece == Piece::queen) && Move::IsThisPromotion(legalMoves[index], Move::queenPromotion));
                                else if ((kb_IsDown(kb_KeyMul) || promotePiece == Piece::rook) && Move::IsThisPromotion(legalMoves[index], Move::rookPromotion));
                                else if ((kb_IsDown(kb_KeySub) || promotePiece == Piece::bishop) && Move::IsThisPromotion(legalMoves[index], Move::bishopPromotion));
                                else if ((kb_IsDown(kb_KeyAdd) || promotePiece == Piece::knight) && Move::IsThisPromotion(legalMoves[index], Move::knightPromotion));
                                else continue;
                            }

                            move = legalMoves[index];

                            break;
                        }
                    }

                    if (!Move::IsNullMove(move)) {
                        MakeMove(move);

                        GenerateLegalMoves(legalMoves, movesAvailable);

                        AddMoveToStatistics();

                        move = Move();

                        previousClockTime = clock();
                    }

                    selected = OFFBOARD;
                }
            } else if (key == sk_Yequ || key == sk_Window) {
                do UnMakeMove(true);
                while (key == sk_Yequ && ply > 0);

                UpdateInCheck();

                selected = OFFBOARD;
            } else if (key == sk_Graph || key == sk_Trace) {
                do RedoMove();
                while (key == sk_Graph && !IsNextMoveNull());

                if (!IsNextMoveNull()) UpdateInCheck();
                else GenerateLegalMoves(legalMoves, movesAvailable);

                selected = OFFBOARD;
            } else if (key == sk_Zoom && ply && isTimedGame && IsNextMoveNull() && !gameState) {
                paused = !paused;
                if (!paused) {
                    previousClockTime = clock();
                    UpdateTimeRemaining();
                    DisplayTimeRemaining(true);
                }
            }

            if (key == sk_Up || key == sk_Down || key == sk_Left || key == sk_Right || key == sk_2nd || key == sk_Enter || key == sk_Yequ || key == sk_Window || key == sk_Graph || key == sk_Trace) DisplayGame();

            UpdateTimeRemaining();
        }

        return key;
    }

    void Board::DisplayGame () {
        int8_t targets[64];
        for (uint8_t i = 0; i < 64; i++) targets[i] = -1;
        if (IsNextMoveNull() && legalMoveDots) {
            for (uint8_t index = 0; index < movesAvailable; index++) if (Move::GetStart(legalMoves[index]) == (selected == OFFBOARD ? selection : selected)) targets[Move::GetTarget(legalMoves[index])] = Move::IsCapture(legalMoves[index]) ? Move::capture : Move::quietMove;
        }

        for (uint8_t file = 0; file < 8; file++) {
            for (uint8_t rank = 0; rank < 8; rank++) {
                uint8_t square = file + rank * 8;
                uint16_t x = 0, y = 0; SquareToXY(square, x, y);
                square = boardView == ONLY_BLACK ||
                    (boardView == DYNAMIC && sideToMove == Piece::black) ||
                    (boardView == FOLLOW_PLAYER && whitePlayer != HUMAN && blackPlayer == HUMAN) ||
                    ((boardView == FOLLOW_PLAYER && (whitePlayer != HUMAN) == (blackPlayer != HUMAN)) && sideToMove == Piece::black) ?
                    (7 - file) + (7 - rank) * 8 : square;
                bool isLightSquare = (file + rank) % 2 == 0;
                uint8_t squareColor;

                if (square == selected) squareColor = DARK_LAST_MOVE_COLOR + tilePalette;
                else if (square == selection && IsNextMoveNull()) squareColor = LIGHT_LAST_MOVE_COLOR + tilePalette;
                else if (checkHighlighting && inCheck && matrix[square] == (sideToMove | Piece::king)) squareColor = background == DARK ? LIGHT_IMPORTANT_COLOR : DARK_IMPORTANT_COLOR;
                else if (lastMoveHighlighting && !Move::IsNullMove(lastMove) && (Move::GetStart(lastMove) == square || Move::GetTarget(lastMove) == square)) squareColor = isLightSquare ? LIGHT_LAST_MOVE_COLOR + tilePalette : DARK_LAST_MOVE_COLOR + tilePalette;
                else squareColor = isLightSquare ? LIGHT_COLOR + tilePalette : DARK_COLOR + tilePalette;

                gfx_SetColor(squareColor);

                Rectangle(x, y, SQUARE_SIZE);

                if (targets[square] != -1) {
                    if (selected != OFFBOARD && square == selection) gfx_SetColor(DARK_LAST_MOVE_COLOR + tilePalette);
                    else if (!Move::IsNullMove(lastMove) && (Move::GetStart(lastMove) == square || Move::GetTarget(lastMove) == square)) gfx_SetColor(isLightSquare ? DARK_LAST_MOVE_TARGET_COLOR + tilePalette : LIGHT_LAST_MOVE_TARGET_COLOR + tilePalette);
                    else gfx_SetColor(isLightSquare ? LIGHT_TARGET_COLOR + tilePalette : DARK_TARGET_COLOR + tilePalette);

                    if (targets[square] == Move::capture) {
                        gfx_FillCircle(x, y, CAPTURE_OUTER_SIZE);
                        gfx_SetColor(squareColor);
                        gfx_FillCircle(x, y, CAPTURE_INNER_SIZE);
                    } else gfx_FillCircle(x, y, CENTER_SIZE);
                }

                if (matrix[square] != Piece::empty) {
                    Sprite(GetSprite(matrix[square]), x, y);

                    if (gameState && Piece::IsPiece(matrix[square], Piece::king)) {
                        uint16_t markerX = x + 6 - SQUARE_SIZE / 2, markerY = y + 6 - SQUARE_SIZE / 2;
                        
                        gfx_SetColor(BLACK_COLOR);
                        gfx_FillCircle(markerX, markerY, 5);

                        if (((whiteWins || whiteWinsByTimeout) && matrix[square] == Piece::whiteKing) || ((blackWins || blackWinsByTimeout) && matrix[square] == Piece::blackKing)) gfx_SetColor(WIN_COLOR);
                        else gfx_SetColor(LOSE_COLOR);

                        gfx_FillCircle(markerX, markerY, 4);
                    }
                }
            }
        }

        gfx_SetTextBGColor(TRANSPARENT_COLOR);

        const bool isBoardUpright = IsBoardUpright();
        uint8_t backgroundColor = background == LIGHT ? LIGHT_BACKGROUND_COLOR : DARK_BACKGROUND_COLOR;
        uint8_t offset = isBoardUpright ? 0 : 6;
        uint8_t grayColor = background == MATCHING ? GRAY_COLOR : background == LIGHT ? LIGHT_GRAY_COLOR : DARK_GRAY_COLOR;
        gfx_SetTextFGColor(background == LIGHT ? BLACK_COLOR : WHITE_COLOR);
        for (uint8_t index = 0; index <= 4; index++) SetBackgroundOfLine(backgroundColor, offset + index);
        char name[MAX_STAT_NAME + 1];
        Statistics::GetName(blackStatistics, name);
        char elo[MAX_ELO_LENGTH + 1];
        Statistics::GetElo(blackStatistics, elo);
        PrintToSide(name, isBoardUpright ? 0 : 10);
        PrintToSide(elo, isBoardUpright ? 1 : 9);

        backgroundColor = background == DARK ? DARK_BACKGROUND_COLOR : LIGHT_BACKGROUND_COLOR;
        offset = isBoardUpright ? 6 : 0;
        gfx_SetTextFGColor(background == DARK ? WHITE_COLOR : BLACK_COLOR);
        for (uint8_t index = 0; index <= 4; index++) SetBackgroundOfLine(backgroundColor, offset + index);
        Statistics::GetName(whiteStatistics, name);
        Statistics::GetElo(whiteStatistics, elo);
        PrintToSide(name, isBoardUpright ? 10 : 0);
        PrintToSide(elo, isBoardUpright ? 9 : 1);

        gfx_SetColor(background == LIGHT ? LIGHT_BACKGROUND_COLOR : background == DARK ? DARK_BACKGROUND_COLOR : isBoardUpright ? DARK_BACKGROUND_COLOR : LIGHT_BACKGROUND_COLOR);
        gfx_FillRectangle(8 * SQUARE_SIZE, 0, GFX_LCD_WIDTH - 8 * SQUARE_SIZE, 7);
        SetBackgroundOfLine(background == LIGHT ? LIGHT_BACKGROUND_COLOR : background == DARK ? DARK_BACKGROUND_COLOR : isBoardUpright ? LIGHT_BACKGROUND_COLOR : DARK_BACKGROUND_COLOR, 11);

        DisplayTimeRemaining(false);

        gfx_SetTextFGColor(background != LIGHT ? WHITE_COLOR : BLACK_COLOR);
        SetBackgroundOfLine(grayColor, 5);
        if (selected == OFFBOARD) PrintToSide(SquareToName[selection], 5);
        else PrintToSide(SquareToName[selected], 5, false, SquareToName[selection]);

        uint8_t historicPly = ply;
        while (!Move::IsNullMove(moveHistory[historicPly])) historicPly++;
        const char plyAsChar[8] = { char(ply / 100 + '0'), char(ply / 10 % 10 + '0'), char(ply % 10 + '0'), ply != historicPly ? '/' : '\0', char(historicPly / 100 + '0'), char(historicPly / 10 % 10 + '0'), char(historicPly % 10 + '0'), '\0' };
        SetBackgroundOfLine(grayColor, 12);
        SetBackgroundOfLine(grayColor, 13);
        SetBackgroundOfLine(grayColor, 14);
        SetBackgroundOfLine(grayColor, 15);
        PrintToSide(plyAsChar, 12);

        gfx_SetTextFGColor(background == DARK ? LIGHT_IMPORTANT_COLOR : DARK_IMPORTANT_COLOR);

        if (gameState) {
            PrintToSide(gameState, 13);
            PrintToSide(gameStateReasoning, 14);
        } else if (thinking) PrintToSide("thinking", 13);
        if(!gameState && inCheck) PrintToSide("check", thinking ? 14 : 13);

        gfx_BlitBuffer();
    }

    //DEBUG
    /* void Board::PrintGame () {
        const char* spacer = "+---+---+---+---+---+---+---+---+\n";

        dbg_printf("%s", spacer);

        for (uint8_t rank = 0; rank < 8; rank++) {
            dbg_printf("| ");

            for (uint8_t file = 0; file < 8; file++) {
                uint8_t square = file + 8 * rank;

                dbg_printf("%c | ", Piece::GetPieceName(matrix[square]));
            }

            dbg_printf("%u\n", 8 - rank);
            dbg_printf("%s", spacer);
        }

        dbg_printf("  a   b   c   d   e   f   g   h    \n");
    } */

    gfx_sprite_t* Board::GetSprite (piece pieceToReference) { return sprites[Piece::GetLinearValue(pieceToReference) + pieceStyle]; };

    void Board::DisplayTimeRemaining (const bool blitText) {
        if (!isTimedGame) return;

        const bool isBoardUpright = IsBoardUpright();

        if (paused && (clock() - programStartTime) / CLOCKS_PER_HOLD % 5 <= 1) {
            SetBackgroundOfLine(background == LIGHT ? LIGHT_BACKGROUND_COLOR : DARK_BACKGROUND_COLOR, isBoardUpright ? 3 : 7, true);
            SetBackgroundOfLine(background == DARK ? DARK_BACKGROUND_COLOR : LIGHT_BACKGROUND_COLOR, isBoardUpright ? 7 : 3, true);
            return;
        }

        uint8_t whiteHoursRemaining = whiteTimeRemaining / 3600000ull % 10, blackHoursRemaining = blackTimeRemaining / 3600000ull % 10;
        uint8_t whiteMinutesRemaining = whiteTimeRemaining / 60000 % 60, blackMinutesRemaining = blackTimeRemaining / 60000 % 60;
        uint8_t whiteSecondsRemaining = whiteTimeRemaining / 1000 % 60, blackSecondsRemaining = blackTimeRemaining / 1000 % 60;

        gfx_SetTextBGColor(TRANSPARENT_COLOR);

        const char whiteTimeRemainingAsChar[8] = { char(whiteHoursRemaining % 10 + '0'), ':', char(whiteMinutesRemaining / 10 % 10 + '0'), char(whiteMinutesRemaining % 10 + '0'), ':', char(whiteSecondsRemaining / 10 % 10 + '0'), char(whiteSecondsRemaining % 10 + '0'), '\0' };
        const char blackTimeRemainingAsChar[8] = { char(blackHoursRemaining % 10 + '0'), ':', char(blackMinutesRemaining / 10 % 10 + '0'), char(blackMinutesRemaining % 10 + '0'), ':', char(blackSecondsRemaining / 10 % 10 + '0'), char(blackSecondsRemaining % 10 + '0'), '\0' };

        gfx_SetTextFGColor(!lowTimeWarning || blackMinutesRemaining ? (background == LIGHT ? BLACK_COLOR : WHITE_COLOR) : blackSecondsRemaining > 10 || blackSecondsRemaining % 2 ? (background == LIGHT ? DARK_LOW_ON_TIME_COLOR: LIGHT_LOW_ON_TIME_COLOR) : (background == LIGHT ? DARK_OUT_OF_TIME_COLOR : LIGHT_OUT_OF_TIME_COLOR));
        SetBackgroundOfLine(background == LIGHT ? LIGHT_BACKGROUND_COLOR : DARK_BACKGROUND_COLOR, isBoardUpright ? 3 : 7);
        PrintToSide(blackTimeRemainingAsChar, isBoardUpright ? 3 : 7, blitText);

        gfx_SetTextFGColor(!lowTimeWarning || whiteMinutesRemaining ? (background == DARK ? WHITE_COLOR : BLACK_COLOR) : whiteSecondsRemaining > 10 || whiteSecondsRemaining % 2 ? (background == DARK ? LIGHT_LOW_ON_TIME_COLOR : DARK_LOW_ON_TIME_COLOR) : (background == DARK ? LIGHT_OUT_OF_TIME_COLOR : DARK_OUT_OF_TIME_COLOR));
        SetBackgroundOfLine(background == DARK ? DARK_BACKGROUND_COLOR : LIGHT_BACKGROUND_COLOR, isBoardUpright ? 7 : 3);
        PrintToSide(whiteTimeRemainingAsChar, isBoardUpright ? 7 : 3, blitText);
    }

    void Board::DisplayOptions (const Option* options, const uint8_t numberOfOptions, const uint8_t selectedOption) {
        zx7_Decompress(gfx_vbuffer, Background_compressed);
        gfx_SetTextBGColor(TRANSPARENT_COLOR);
        gfx_SetTextFGColor(WHITE_COLOR);

        bool oneColumn = true;
        uint32_t longestStringWidth = 0;
        for (int8_t index = 0; index < numberOfOptions && oneColumn; oneColumn = options[index].type != Option::cycle && options[index].type != Option::number && options[index].type != Option::text, longestStringWidth = longestStringWidth < gfx_GetStringWidth(options[index].names[0]) ? gfx_GetStringWidth(options[index].names[0]) : longestStringWidth, index++);
        const int16_t divisor = 10;

        gfx_SetTextScale(2, 2);
        gfx_PrintStringXY("Chess84", GFX_LCD_WIDTH / 2 - gfx_GetStringWidth("Chess84") / 2, (5 * GFX_LCD_HEIGHT / 12 - 4) / 2);
        gfx_SetTextScale(1, 1);

        for (int8_t index = 0; index < numberOfOptions; index++) {
            gfx_SetTextFGColor(WHITE_COLOR);
            
            const char* name = options[index].names[options[index].nameToUse];
            if (name == nullptr) name = options[index].names[0];
            int16_t x = oneColumn ? GFX_LCD_WIDTH / 2 - longestStringWidth / 2 : GFX_LCD_WIDTH / divisor;
            const int16_t y = 5 * GFX_LCD_HEIGHT / 12 - 4 + index * 12;

            gfx_SetTextXY(x - gfx_GetCharWidth(RIGHT_ARROW_TEXT) - 4, y);
            if (index == selectedOption) gfx_PrintChar(RIGHT_ARROW_TEXT);
            else {
                gfx_SetTextFGColor(LIGHT_BACKGROUND_COLOR);
                gfx_PrintInt(index + 1, 1);
                gfx_SetTextFGColor(WHITE_COLOR);
            }

            gfx_PrintStringXY(name, x, y);

            if (options[index].callBack == Option::UpdateTilePalette) gfx_SetTextFGColor((tilePalette == 16 || tilePalette == 24 ? LIGHT_COLOR : DARK_COLOR) + tilePalette);

            if ((options[index].type == Option::cycle || options[index].type == Option::number || options[index].type == Option::text) && !(options[index].nameToUse)) {
                x = (divisor - 1) * GFX_LCD_WIDTH / divisor;
                int16_t leftBound = 0;

                if (options[index].type == Option::cycle) {
                    const char* cycleValue = options[index].cycleValues[options[index].value];
                    
                    if (!(cycleValue == nullptr && options[index].value == 0 && options == playerOptions)) goto print; // Exception to allow the statistics name to be the same in the player selection
                    char playerName[MAX_STAT_NAME + 1];
                    Statistics::GetName(index ? blackStatistics : whiteStatistics, playerName);
                    cycleValue = playerName;

                    print:
                        gfx_PrintStringXY(cycleValue, leftBound = x - gfx_GetStringWidth(cycleValue), y);
                } else if (options[index].type == Option::number) {
                    if (!options[index].format) {
                        gfx_SetTextXY(leftBound = x - GetStringWidthOfInt(options[index].value), y);

                        gfx_PrintInt(options[index].value, 1);
                    } else if (options[index].format == Option::timeInHoursMinutesSeconds) {
                        uint8_t hours = options[index].value / 3600000ull % 10;
                        uint8_t minutes = options[index].value / 60000 % 60;
                        uint8_t seconds = options[index].value / 1000 % 60;

                        const char time[8] = { char(hours % 10 + '0'), ':', char(minutes / 10 % 10 + '0'), char(minutes % 10 + '0'), ':', char(seconds / 10 % 10 + '0'), char(seconds % 10 + '0'), '\0' };

                        gfx_PrintStringXY(time, leftBound = x - gfx_GetStringWidth(time), y);
                    } else if (options[index].format == Option::timeInSeconds) {
                        uint8_t seconds = options[index].value / 1000 % 60;

                        const char time[3] = { char(seconds / 10 % 10 + '0'), char(seconds % 10 + '0'), '\0' };

                        gfx_PrintStringXY(time, leftBound = x - gfx_GetStringWidth(time), y);
                    }
                } else {
                    int16_t length = 0;
                    
                    for (int64_t value = options[index].value; value; value >>= 5) {
                        const char character = IndexToCharacter[value & 0x1Fll];
                        length += gfx_GetCharWidth(character);

                        gfx_SetTextXY(x - length, y);
                        gfx_PrintChar(character);
                    }

                    leftBound = x - length;
                }

                if (index == selectedOption && options[index].type != Option::text) {
                    gfx_SetTextXY(leftBound - gfx_GetCharWidth(LEFT_ARROW_TEXT) - 2, y);
                    gfx_PrintChar(LEFT_ARROW_TEXT);

                    gfx_SetTextXY(x + 2, y);
                    gfx_PrintChar(RIGHT_ARROW_TEXT);
                } else if (index == selectedOption && (clock() - programStartTime) / CLOCKS_PER_TEXT % 2) {
                    gfx_SetTextXY(x + 2, y);
                    gfx_PrintChar('|');
                }
            }
        }

        gfx_BlitBuffer();
    }

    //DEBUG
    /* void Board::DisplayBoardInformation () {
        dbg_printf("Ply: %u\n", ply);
        dbg_printf("\n");
        dbg_printf("SideToMove: %u\n", sideToMove);
        dbg_printf("InCheck: %u\n", inCheck);
        dbg_printf("EnPassantSquare: %s\n", SquareToName[enPassantSquare]);
        dbg_printf("FiftyMoveCount: %u\n", fiftyMoveCount);
        dbg_printf("\n");
        dbg_printf("BlackKingCastle: %u\n", blackKingCastleRights);
        dbg_printf("BlackQueenCastle: %u\n", blackQueenCastleRights);
        dbg_printf("WhiteKingCastle: %u\n", whiteKingCastleRights);
        dbg_printf("WhiteQueenCastle: %u\n", whiteQueenCastleRights);
        dbg_printf("\n");
        dbg_printf("BlackWins: %u\n", blackWins);
        dbg_printf("WhiteWins: %u\n", whiteWins);
        dbg_printf("BlackWinsByTimeout: %u\n", blackWinsByTimeout);
        dbg_printf("WhiteWinsByTimeout: %u\n", whiteWinsByTimeout);
        dbg_printf("\n");
        dbg_printf("Stalemate: %u\n", stalemate);
        dbg_printf("InsufficentMaterial: %u\n", insufficentMaterial);
        dbg_printf("FiftyMoveRule: %u\n", fiftyMoveRule);
        dbg_printf("ThreefoldRepetition: %u\n", threefoldRepetition);
        dbg_printf("TimeoutWithInsufficentMaterial: %u\n", timeoutWithInsufficentMaterial);
        dbg_printf("\n\n");
    } */

    void Board::PlacePieceOnSquare (const piece pieceToPlace, const uint8_t square) {
        switch (Piece::GetSide(matrix[square])) { // If target square has a piece on it, the piece is either being removed or captured, both lowering the count
            case Piece::black:
                switch (matrix[square]) {
                    case Piece::blackPawn: --blackPawnsCount; break;
                    case Piece::blackKnight: --blackKnightsCount; break;
                    case Piece::blackBishop: --blackBishopsCount; break;
                    case Piece::blackRook: --blackRooksCount; break;
                    case Piece::blackQueen: --blackQueensCount; break;
                }
                break;
            case Piece::white:
                switch (matrix[square]) {
                    case Piece::whitePawn: --whitePawnsCount; break;
                    case Piece::whiteKnight: --whiteKnightsCount; break;
                    case Piece::whiteBishop: --whiteBishopsCount; break;
                    case Piece::whiteRook: --whiteRooksCount; break;
                    case Piece::whiteQueen: --whiteQueensCount; break;
                }
                break;
        }

        switch (Piece::GetSide(pieceToPlace)) {
            case Piece::black:
                switch (pieceToPlace) {
                    case Piece::blackPawn: ++blackPawnsCount; break;
                    case Piece::blackKnight: ++blackKnightsCount; break;
                    case Piece::blackBishop: ++blackBishopsCount; break;
                    case Piece::blackRook: ++blackRooksCount; break;
                    case Piece::blackQueen: ++blackQueensCount; break;
                }
                break;
            case Piece::white:
                switch (pieceToPlace) {
                    case Piece::whitePawn: ++whitePawnsCount; break;
                    case Piece::whiteKnight: ++whiteKnightsCount; break;
                    case Piece::whiteBishop: ++whiteBishopsCount; break;
                    case Piece::whiteRook: ++whiteRooksCount; break;
                    case Piece::whiteQueen: ++whiteQueensCount; break;
                }
                break;
        }

        switch (pieceToPlace) {
            case Piece::blackKing: blackKingSquare = square; blackKingFile = square % 8; blackKingRank = square / 8; break;
            case Piece::whiteKing: whiteKingSquare = square; whiteKingFile = square % 8; whiteKingRank = square / 8; break;
        }

        matrix[square] = pieceToPlace;
    }

    hash Board::GenerateZobristHash () {
        hash hash = 0;

        for (uint8_t square = 0; square < 64; square++) if (matrix[square] != Piece::empty) hash ^= zobristPieceDataKeys[square][Piece::GetLinearValue(matrix[square])];
        if (enPassantSquare != OFFBOARD) {
            bool blackIsAbleToCaptureEnPassant = sideToMove == Piece::black && ((enPassantSquare % 8 != 0 && matrix[enPassantSquare - 9] == Piece::blackPawn) || (enPassantSquare % 8 != 7 && matrix[enPassantSquare - 7] == Piece::blackPawn));
            bool whiteIsAbleToCaptureEnPassant = sideToMove == Piece::white && ((enPassantSquare % 8 != 7 && matrix[enPassantSquare + 9] == Piece::whitePawn) || (enPassantSquare % 8 != 0 && matrix[enPassantSquare + 7] == Piece::whitePawn));
            if (blackIsAbleToCaptureEnPassant || whiteIsAbleToCaptureEnPassant) hash ^= zobristPieceDataKeys[enPassantSquare][12];
        }
        if (sideToMove == Piece::white) hash ^= zobristSideToMoveKey;
        hash ^= zobristCastleRightsKeys[(blackKingCastleRights << 0) | (blackQueenCastleRights << 1) | (whiteKingCastleRights << 2) | (whiteQueenCastleRights << 3)];

        return hash;
    }

    void Board::UpdateTimeRemaining () {
        if ((!ply && IsNextMoveNull()) || !isTimedGame || gameState || gameOver) return;
        if (paused) {
            DisplayTimeRemaining(true);
            return;
        }

        deltaTime += (clock() - previousClockTime) / CLOCKS_PER_SEC;

        if (deltaTime >= 1 && whiteTimeRemaining && blackTimeRemaining) {
            uint8_t currentSideToMove = sideToMove;
            for (uint8_t _ply = ply; !Move::IsNullMove(moveHistory[_ply++]); Piece::SetToOppositeSide(currentSideToMove));

            int64_t& currentSideToMovesTimeRemaining = currentSideToMove == Piece::white ? whiteTimeRemaining : blackTimeRemaining;

            currentSideToMovesTimeRemaining -= deltaTime * 1000;

            if (currentSideToMovesTimeRemaining <= 0) {
                if (currentSideToMove == Piece::white) {
                    if (DoesBlackHaveInsufficentMaterial()) timeoutWithInsufficentMaterial = true;
                    else blackWinsByTimeout = true;
                    whiteTimeRemaining = 0;
                } else {
                    if (DoesWhiteHaveInsufficentMaterial()) timeoutWithInsufficentMaterial = true;
                    else whiteWinsByTimeout = true;
                    blackTimeRemaining = 0;
                }

                if (CheckGameState()) DisplayGame();
            }

            DisplayTimeRemaining(true);
            
            deltaTime = 0;
            previousClockTime = clock();
        }
    }

    void Board::UpdateTimeRemainingForMove () {
        if (!gameOver) return;
        whiteTimeRemaining = !ply ? whiteStartingTime : timeRemainingAtPly[ply - (sideToMove == Piece::white ? 2 : 1)];
        blackTimeRemaining = ply <= 1 ? blackStartingTime : timeRemainingAtPly[ply - (sideToMove == Piece::black ? 2 : 1)];
    }

    void Board::ImportFen (const char* fen) {
        srandom(rtc_Time());

        uint8_t startingFenLength = 0;
        for (; fen[startingFenLength] != '\0'; startingFen[startingFenLength] = fen[startingFenLength], startingFenLength++);
        for (; startingFenLength < MAX_FEN_LENGTH; startingFen[startingFenLength++] = '\0');

        for (uint8_t i = 0; i < ply; i++) { moveHistory[i] = Move(); zobristHashHistory[i] = 0; timeRemainingAtPly[i] = 0; }
        lastMove = Move();
        lastZobristHash = 0;
        ply = 0;
        gameOver = false;
        inCheck = false;
        blackWins = whiteWins = blackWinsByTimeout = whiteWinsByTimeout = blackWinsByResignation = whiteWinsByResignation = false;
        stalemate = fiftyMoveRule = insufficentMaterial = threefoldRepetition = timeoutWithInsufficentMaterial = drawByAgreement = false;
        gameStateReasoning = gameState = nullptr;
        blackTimeRemaining = blackStartingTime;
        whiteTimeRemaining = whiteStartingTime;

        uint8_t index = 0;
        uint8_t square = 0;
        uint8_t length = 0; while (fen[length] != '\0') length++;

        for (uint8_t i = 0; i < 64; i++) PlacePieceOnSquare(Piece::empty, i);

        for (; index < length && fen[index] != ' ' && square < 64; index++, square++) {
            int8_t digit = fen[index] - '0';

            if (fen[index] == '/') square--;
            else if (digit >= 0 && digit < 10) square += digit - 1;
            else if (fen[index] == 'p') PlacePieceOnSquare(Piece::blackPawn, square);
            else if (fen[index] == 'n') PlacePieceOnSquare(Piece::blackKnight, square);
            else if (fen[index] == 'b') PlacePieceOnSquare(Piece::blackBishop, square);
            else if (fen[index] == 'r') PlacePieceOnSquare(Piece::blackRook, square);
            else if (fen[index] == 'q') PlacePieceOnSquare(Piece::blackQueen, square);
            else if (fen[index] == 'k') PlacePieceOnSquare(Piece::blackKing, square);
            else if (fen[index] == 'P') PlacePieceOnSquare(Piece::whitePawn, square);
            else if (fen[index] == 'N') PlacePieceOnSquare(Piece::whiteKnight, square);
            else if (fen[index] == 'B') PlacePieceOnSquare(Piece::whiteBishop, square);
            else if (fen[index] == 'R') PlacePieceOnSquare(Piece::whiteRook, square);
            else if (fen[index] == 'Q') PlacePieceOnSquare(Piece::whiteQueen, square);
            else if (fen[index] == 'K') PlacePieceOnSquare(Piece::whiteKing, square);
        }

        if (fen[++index] == 'w') sideToMove = Piece::white;
        else sideToMove = Piece::black;

        blackKingCastleRights = blackQueenCastleRights = whiteKingCastleRights = whiteQueenCastleRights = false;
        initialBlackKingCastleRights = initialBlackQueenCastleRights = initialWhiteKingCastleRights = initialWhiteQueenCastleRights = false;
        
        for (index += 2; index < length && fen[index] != ' '; index++) {
            if (fen[index] == 'k') blackKingCastleRights = initialBlackKingCastleRights = true;
            if (fen[index] == 'q') blackQueenCastleRights = initialBlackQueenCastleRights = true;
            if (fen[index] == 'K') whiteKingCastleRights = initialWhiteKingCastleRights = true;
            if (fen[index] == 'Q') whiteQueenCastleRights = initialWhiteQueenCastleRights = true;
        }

        index++;

        enPassantSquare = NameToSquare(fen[index], fen[index + 1]);

        if (fen[index] != '-') index++; index += 2;

        uint8_t magnitude = 1, change = 0;
        for (; index < length && fen[index] != ' '; index++, magnitude *= 10, change++) {};
        for (index -= change, magnitude /= 10; index < length && fen[index] != ' '; index++, magnitude /= 10) {
            fiftyMoveCount += uint8_t(fen[index] - '0') * magnitude;
        }
        initialFiftyMoveCount = fiftyMoveCount;

        index++;

        for (magnitude = 1, change = 0; index < length; index++, magnitude *= 10, change++) {};
        for (index -= change, magnitude /= 10; index < length; index++, magnitude /= 10) {
            ply += uint8_t(fen[index] - '0') * magnitude;
        }
        ply--; ply *= 2; ply += sideToMove == Piece::black ? 1 : 0;

        selection = A1;
        selected = OFFBOARD;

        UpdateInCheck();
        initalInCheck = inCheck;
        UpdateInsufficentMaterial();
        lastZobristHash = initialZobristHash = GenerateZobristHash();
    }

    void Board::ExportFen (char* fen) {
        uint8_t index = 0;

        for (uint8_t rank = 0; rank < 8; rank++) {
            uint8_t emptyCounter = 0;

            for (uint8_t file = 0; file < 8; file++) {
                uint8_t square = file + 8 * rank;

                if (matrix[square] == Piece::empty) {
                    emptyCounter++;
                    continue;
                }

                if (emptyCounter != 0) {
                    fen[index++] = emptyCounter + '0';
                    emptyCounter = 0;
                }

                fen[index++] = Piece::GetPieceName(matrix[square]);
            }

            if (emptyCounter != 0) fen[index++] = emptyCounter + '0';

            if (rank != 7) fen[index++] = '/';
        }

        fen[index++] = ' ';

        fen[index++] = sideToMove == Piece::white ? 'w' : 'b';

        fen[index++] = ' ';

        if (whiteKingCastleRights) fen[index++] = 'K';
        if (whiteQueenCastleRights) fen[index++] = 'Q';
        if (blackKingCastleRights) fen[index++] = 'k';
        if (blackQueenCastleRights) fen[index++] = 'q';
        if (!whiteKingCastleRights && !whiteQueenCastleRights && !blackKingCastleRights && !blackQueenCastleRights) fen[index++] = '-';

        fen[index++] = ' ';

        if (enPassantSquare != OFFBOARD) {
            const char* squareName = SquareToName[enPassantSquare];
            fen[index++] = squareName[0];
            fen[index++] = squareName[1];
        } else fen[index++] = '-';

        fen[index++] = ' ';

        if (fiftyMoveCount >= 10) fen[index++] = fiftyMoveCount / 10 + '0';
        fen[index++] = fiftyMoveCount % 10 + '0';

        fen[index++] = ' ';

        if (ply / 2 + 1 >= 100) fen[index++] = (ply / 2 + 1) / 100 + '0';
        if (ply / 2 + 1 >= 10) fen[index++] = (ply / 2 + 1) / 10 % 10 + '0';
        fen[index++] = (ply / 2 + 1) % 10 + '0';
    }

    void Board::SetAlgebraicNotations (Move* moves, const uint8_t available) {
        for (uint8_t i = 0; i < available; i++) {
            uint8_t index = 0;

            if (Move::IsFlag(moves[i], Move::kingCastle)) {
                moves[i].algebraicNotation[index++] = 'O';
                moves[i].algebraicNotation[index++] = '-';
                moves[i].algebraicNotation[index++] = 'O';
                continue;
            } else if (Move::IsFlag(moves[i], Move::queenCastle)) {
                moves[i].algebraicNotation[index++] = 'O';
                moves[i].algebraicNotation[index++] = '-';
                moves[i].algebraicNotation[index++] = 'O';
                moves[i].algebraicNotation[index++] = '-';
                moves[i].algebraicNotation[index++] = 'O';
                continue;
            }

            uint8_t start = Move::GetStart(moves[i]), target = Move::GetTarget(moves[i]);
            uint8_t startingFile = start % 8;
            uint8_t startingRank = start / 8;

            piece* pieceMoved = &(moves + i)->pieceMoved;

            bool moveIsUnique = true;
            bool moveIsUniqueToFile = true, moveIsUniqueToRank = true;

            bool pieceIsPawn = Piece::IsPiece(matrix[start], Piece::pawn);

            if (!pieceIsPawn) {
                for (uint8_t j = 0; j < available && moveIsUniqueToFile; j++) {
                    if (i == j) continue;

                    if (target == Move::GetTarget(moves[j]) && matrix[Move::GetStart(moves[j])] == (*pieceMoved)) {
                        moveIsUnique = false;
                        if (startingFile == Move::GetStart(moves[j]) % 8) moveIsUniqueToFile = false;
                    }
                }

                for (uint8_t j = 0; j < available && moveIsUniqueToRank; j++) {
                    if (i == j) continue;

                    if (target == Move::GetTarget(moves[j]) && matrix[Move::GetStart(moves[j])] == (*pieceMoved)) {
                        moveIsUnique = false;
                        if (startingRank == Move::GetStart(moves[j]) / 8) moveIsUniqueToRank = false;
                    }
                }
            }

            bool isCapture = Move::IsCapture(moves[i]);

            //! For some unknown reason having only the second condition in the if statement causes a compilation error. Fortunately, including both cases of the first statement bypasses this issue.
            if (isCapture && !pieceIsPawn) moves[i].algebraicNotation[index++] = Piece::GetPieceName(matrix[start], true);
            else if (!isCapture && !pieceIsPawn) moves[i].algebraicNotation[index++] = Piece::GetPieceName(matrix[start], true);
            //! If the first and condition is put in reverse, it causes a compilation error.
            if ((isCapture && (matrix[start] & 0x7) == Piece::pawn) || (!moveIsUnique && !(!moveIsUniqueToFile && moveIsUniqueToRank))) moves[i].algebraicNotation[index++] = SquareToName[start][0];
            if (!pieceIsPawn && !moveIsUniqueToFile) moves[i].algebraicNotation[index++] = SquareToName[start][1];
            if (isCapture) moves[i].algebraicNotation[index++] = 'x';
            moves[i].algebraicNotation[index++] = SquareToName[target][0];
            moves[i].algebraicNotation[index++] = SquareToName[target][1];
            if (pieceIsPawn) {
                if (Move::IsThisPromotion(moves[i], Move::queenPromotion)) {
                    moves[i].algebraicNotation[index++] = '=';
                    moves[i].algebraicNotation[index++] = 'Q';
                } else if (Move::IsThisPromotion(moves[i], Move::rookPromotion)) {
                    moves[i].algebraicNotation[index++] = '=';
                    moves[i].algebraicNotation[index++] = 'R';
                } else if (Move::IsThisPromotion(moves[i], Move::bishopPromotion)) {
                    moves[i].algebraicNotation[index++] = '=';
                    moves[i].algebraicNotation[index++] = 'B';
                } else if (Move::IsThisPromotion(moves[i], Move::knightPromotion)) {
                    moves[i].algebraicNotation[index++] = '=';
                    moves[i].algebraicNotation[index++] = 'K';
                }
            }
        }
    }

    void Board::SetCheckmateAlgebraicNotation () {
        if (blackWins || whiteWins) {
            char* algebraicNotation = moveHistory[ply ? ply - 1 : 0].algebraicNotation;

            for (uint8_t index = 0; index < MAX_ALGEBRAIC_NOTATION_LENGTH && algebraicNotation[index] != '\0' && algebraicNotation[index] != '#'; index++) {
                if (algebraicNotation[index] == '+') {
                    algebraicNotation[index] = '#';
                    break;
                }
            }

            lastMove = moveHistory[ply ? ply - 1 : 0];
        }
    }

    uint16_t Board::GeneratePGN (char* pgn) {
        uint8_t _ply = ply;
        while (!IsNextMoveNull()) RedoMove();
        GenerateLegalMoves(legalMoves, movesAvailable);

        uint16_t appendIndex = 0;
        const char* result = blackWins || blackWinsByTimeout ? "0-1" : whiteWins || whiteWinsByTimeout ? "1-0" : stalemate || insufficentMaterial || fiftyMoveRule || threefoldRepetition || timeoutWithInsufficentMaterial ? "1/2-1/2" : "*";

        //? Required Seven Tag Roster
        AddTagToPGN(pgn, appendIndex, "Event", "Live Calculator Game");
        AddTagToPGN(pgn, appendIndex, "Site", "Calculator");
        AddDateToPGN(pgn, appendIndex);
        AddTagToPGN(pgn, appendIndex, "Round", "-");

        char name[MAX_STAT_NAME + 1];
        Statistics::GetName(whiteStatistics, name);
        AddTagToPGN(pgn, appendIndex, "White", name);
        Statistics::GetName(blackStatistics, name);
        AddTagToPGN(pgn, appendIndex, "Black", name);

        AddTagToPGN(pgn, appendIndex, "Result", result);

        pgn[appendIndex++] = '\n';

        for (uint8_t index = 0; index < ply; index++) {
            if (!(index % 2)) {
                AddNumberToPGN(pgn, appendIndex, index / 2 + 1, 0);
                AddTextToPGN(pgn, appendIndex, ". ", 2);
            }
            AddTextToPGN(pgn, appendIndex, moveHistory[index].algebraicNotation, 7);
            AddTextToPGN(pgn, appendIndex, " ", 1);
        }

        AddTextToPGN(pgn, appendIndex, result, -1);

        pgn[appendIndex++] = '\0';

        while (ply > _ply) UnMakeMove(true);
        UpdateInCheck();
        selected = OFFBOARD;

        return appendIndex;
    }

    void Board::AddTagToPGN (char* pgn, uint16_t& appendIndex, const char* name, const char* value) {
        AddTextToPGN(pgn, appendIndex, "[", 1);
        AddTextToPGN(pgn, appendIndex, name, -1);
        AddTextToPGN(pgn, appendIndex, " \"", 2);
        AddTextToPGN(pgn, appendIndex, value, -1);
        AddTextToPGN(pgn, appendIndex, "\"]\n", 3);
    }

    void Board::AddTextToPGN (char* pgn, uint16_t& appendIndex, const char* text, const int16_t length) { for (uint8_t index = 0; (length <= 0 || index < length) && text[index] != '\0'; pgn[appendIndex++] = text[index++]); };

    void Board::AddNumberToPGN (char* pgn, uint16_t& appendIndex, uint16_t value, const int8_t minimumCharacters) {
        char text[5] = "\0\0\0\0";
        int8_t length = 0;
        for (uint16_t temp = value; temp; temp /= 10, length++);
        for (int8_t index = length > minimumCharacters ? length : minimumCharacters; index > 0; text[--index] = value % 10 + '0', value /= 10);
        AddTextToPGN(pgn, appendIndex, text);
    }

    void Board::AddDateToPGN (char* pgn, uint16_t& appendIndex) {
        uint16_t year = 0;
        uint8_t day = 0, month = 0; boot_GetDate(&day, &month, &year);
        
        AddTextToPGN(pgn, appendIndex, "[Date \"");
        AddNumberToPGN(pgn, appendIndex, year, 4);
        pgn[appendIndex++] = '.';
        AddNumberToPGN(pgn, appendIndex, month, 2);
        pgn[appendIndex++] = '.';
        AddNumberToPGN(pgn, appendIndex, day, 2);
        AddTextToPGN(pgn, appendIndex, "\"]\n");
    }

    void Board::NameToString (int64_t name, char* nameToString) {
        for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameToString[i++] = '\0');
        uint8_t length = 0;
        for (int64_t temp = name; temp; temp >>= 5, length++);
        for (int64_t temp = name; temp; nameToString[--length] = IndexToCharacter[temp & 0x1F], temp >>= 5);
    }

    void Board::SaveBoard (BoardDefinitions& newBoard, const bool onlyStatistics) {
        if (!onlyStatistics) {
            for (uint8_t index = 0; index < MAX_FEN_LENGTH; newBoard.startingFen[index] = startingFen[index], index++);

            for (uint8_t index = 0; index < MAX_MOVE_HISTORY; newBoard.moveHistory[index] = moveHistory[index], newBoard.timeRemainingAtPly[index] = timeRemainingAtPly[index], index++);
            newBoard.ply = ply;

            newBoard.gameOver = gameOver;

            newBoard.promotePiece = promotePiece;

            newBoard.whitePlayer = whitePlayer;
            newBoard.blackPlayer = blackPlayer;

            newBoard.isTimedGame = isTimedGame;
            newBoard.lowTimeWarning = lowTimeWarning;
            newBoard.blackStartingTime = blackStartingTime;
            newBoard.whiteStartingTime = whiteStartingTime;
            newBoard.blackTimeRemaining = blackTimeRemaining;
            newBoard.whiteTimeRemaining = whiteTimeRemaining;
            newBoard.bonusTime = bonusTime;

            newBoard.boardView = boardView;
            newBoard.tilePalette = tilePalette;
            newBoard.pieceStyle = pieceStyle;
            newBoard.checkHighlighting = checkHighlighting;
            newBoard.lastMoveHighlighting = lastMoveHighlighting;
            newBoard.background = background;
            newBoard.legalMoveDots = legalMoveDots;

            newBoard.savePGNOnGameOver = savePGNOnGameOver;

            newBoard.resetBoardOn = resetBoardOn;
        }

        newBoard.whiteStatistics = whiteStatistics;
        newBoard.blackStatistics = blackStatistics;
    }

    void Board::LoadBoard (BoardDefinitions& newBoard) {
        ImportFen(newBoard.startingFen);
        for (uint8_t index = 0; index < MAX_MOVE_HISTORY; index++) {
            if (Move::IsNullMove(newBoard.moveHistory[index])) break;
            MakeMove(newBoard.moveHistory[index]);
            timeRemainingAtPly[index] = newBoard.timeRemainingAtPly[index];
        }
        for (; ply > newBoard.ply; UnMakeMove(true));
        UpdateInCheck();

        gameOver = newBoard.gameOver;
        gameState = nullptr;
        gameStateReasoning = nullptr;

        settingOptions[0].value = DefaultLinearPromotionValue(promotePiece = newBoard.promotePiece);

        playerOptions[0].value = whitePlayer = newBoard.whitePlayer;
        playerOptions[1].value = blackPlayer = newBoard.blackPlayer;

        timingOptions[0].value = isTimedGame = newBoard.isTimedGame;
        timingOptions[1].value = lowTimeWarning = newBoard.lowTimeWarning;
        deltaTime = 0;
        previousClockTime = clock();
        timingOptions[2].value = blackStartingTime = newBoard.blackStartingTime;
        timingOptions[3].value = whiteStartingTime = newBoard.whiteStartingTime;
        blackTimeRemaining = newBoard.blackTimeRemaining;
        whiteTimeRemaining = newBoard.whiteTimeRemaining;
        timingOptions[4].value = bonusTime = newBoard.bonusTime;
        paused = newBoard.paused;

        boardOptions[0].value = boardView = newBoard.boardView;
        boardOptions[1].value = (tilePalette = newBoard.tilePalette) / TILE_PALETTE_OFFSET;
        boardOptions[2].value = (pieceStyle = newBoard.pieceStyle) / PIECE_STYLE_OFFSET;
        boardOptions[3].value = background = newBoard.background;
        boardOptions[4].value = checkHighlighting = newBoard.checkHighlighting;
        boardOptions[5].value = lastMoveHighlighting = newBoard.lastMoveHighlighting;
        boardOptions[6].value = legalMoveDots = newBoard.legalMoveDots;

        saveOptions[6].value = savePGNOnGameOver = newBoard.savePGNOnGameOver;

        resetOptions[6].value = resetBoardOn = newBoard.resetBoardOn;

        whiteStatistics = newBoard.whiteStatistics;
        blackStatistics = newBoard.blackStatistics;
    }

    uint8_t Board::SaveGame (char* saveName, const bool overwrite) {
        if (overwrite) NameToString(saveOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t savedGame = ti_Open(saveName, "w");

        if (savedGame) {
            SaveBoard(newBoard);

            if (ti_Write(gameSaveIdentifier, 1, IDENTIFIER_SIZE, savedGame) == IDENTIFIER_SIZE && ti_Write(&newBoard, sizeof(BoardDefinitions), 1, savedGame) == 1) {
                ti_Close(savedGame);

                return FILE_SUCCESS;
            }

            ti_Delete(saveName);
        }

        return FILE_FAILURE;
    }

    uint8_t Board::SaveGame () { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return SaveGame(nameAddress, true); };

    uint8_t Board::LoadSave (char* saveName, const bool overwrite) {
        if (overwrite) NameToString(loadOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t savedGame = ti_Open(saveName, "r");

        char identifier[IDENTIFIER_SIZE];

        if (savedGame && ti_Read(identifier, 1, IDENTIFIER_SIZE, savedGame) == IDENTIFIER_SIZE && StringCompare(identifier, gameSaveIdentifier) && ti_Read(&newBoard, sizeof(BoardDefinitions), 1, savedGame) == 1) {
            LoadBoard(newBoard);

            ti_Close(savedGame);

            return FILE_SUCCESS;
        }

        if (savedGame) ti_Close(savedGame);

        return FILE_FAILURE;
    }

    uint8_t Board::LoadSave () { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return LoadSave(nameAddress, true); };

    uint8_t Board::DeleteSave (char* saveName, const bool overwrite) {
        if (overwrite) NameToString(deleteOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t saveToDelete = ti_Open(saveName, "r");

        char identifier[IDENTIFIER_SIZE];

        if (saveToDelete && ti_Read(identifier, 1, IDENTIFIER_SIZE, saveToDelete) == IDENTIFIER_SIZE && (StringCompare(identifier, gameSaveIdentifier) || StringCompare(identifier, textSaveIdentifier) || StringCompare(identifier, statisticsSaveIdentifier))) {

            return ti_Delete(saveName) != 0 ? FILE_SUCCESS : FILE_FAILURE;
        }

        if (saveToDelete) ti_Close(saveToDelete);

        return FILE_FAILURE;
    }

    uint8_t Board::DeleteSave () { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return DeleteSave(nameAddress, true); };

    uint8_t Board::ExportPGN (char* saveName, const bool overwrite) {
        if (overwrite) NameToString(saveOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t savedPGN = ti_Open(saveName, "w");

        char pgn[MAX_PGN_LENGTH + 2]; uint16_t pgnSize = GeneratePGN(pgn + 2);
        // Help page generation
        // char pgn[] = "  MENU MOVEMENT\n--------------\n2ND or ENTER : select option\nUP : up 1\nDOWN : down 1\nNUMBER : select nth option\nLEFT or MINUS : cycle left or dec number\nRIGHT or PLUS : cycle right or inc number\nDECIMAL or DEL : go back or exit\nDEL : delete text\nCLEAR or ZERO : exit\nZERO : home\nNEGATE : play\n\nCHESS\n------\nUP : up 1 square\nDOWN : down 1 square\nLEFT : left 1 square\nRIGHT : right 1 square\n2ND or ENTER : select current square\nY= : rewind to start\nWINDOW : rewind 1 move\nZOOM : pause\nTRACE : fast-forward 1 move\nGRAPH : fast-forward to end\n\nMoving a chess piece\n1. Select square of piece to move\n2. Select square you want to go\n\nIf always promote is disabled\nDIVIDE + SELECT : promote to queen\nMULTIPLY + SELECT : promote to rook\nMINUS + SELECT : promote to bishop\nPLUS + SELECT : promote to knight"; uint16_t pgnSize = 795;

        pgn[0] = pgnSize >> 8;
        pgn[1] = pgnSize & 0xFF;

        if (savedPGN && ti_Write(textSaveIdentifier, 1, IDENTIFIER_SIZE, savedPGN) == IDENTIFIER_SIZE && ti_Write(pgn, 1, pgnSize + 2, savedPGN) == pgnSize + 2) {
            ti_Close(savedPGN);

            return FILE_SUCCESS;
        }

        ti_Delete(saveName);

        return FILE_FAILURE;
    }

    uint8_t Board::ExportPGN () { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return ExportPGN(nameAddress, true); }

    uint8_t Board::ExportDefaultPGN () {
        //? CHES_ZZZ - CHES_ZZY - CHES_ZZX ...
        char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 2; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {};
        nameAddress[0] = 'C';
        nameAddress[1] = 'H';
        nameAddress[2] = 'E';
        nameAddress[3] = 'S';
        nameAddress[4] = 'S';
        nameAddress[5] = 'Z';
        nameAddress[6] = 'Z';
        nameAddress[7] = 'Z';

        ti_var_t savedPGN;
        uint8_t index;

        for (uint32_t limit = 0, temp = 0; limit < 17576; limit++) {
            index = 7;
            for (temp = limit; temp; nameAddress[index--] = char('Z' - temp % 26), temp /= 26);
            savedPGN = ti_Open(nameAddress, "r+");
            ti_Close(savedPGN);
            if (!savedPGN) break;
        }

        return ExportPGN(nameAddress, false);
    }

    uint8_t Board::SaveStatistics (Statistics& stats, char* saveName, const bool overwrite) {
        if (overwrite) NameToString(saveOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t savedStatistics = ti_Open(saveName, "w");

        if (savedStatistics) {
            if (ti_Write(statisticsSaveIdentifier, 1, IDENTIFIER_SIZE, savedStatistics) == IDENTIFIER_SIZE && ti_Write(&stats, sizeof(Statistics), 1, savedStatistics) == 1) {
                ti_Close(savedStatistics);

                return FILE_SUCCESS;
            }

            ti_Delete(saveName);
        }

        return FILE_FAILURE;
    }

    uint8_t Board::SaveStatistics (Statistics& stats) { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return SaveStatistics(stats, nameAddress, true); };

    uint8_t Board::LoadStatistics (Statistics& stats, const bool forWhite, char* saveName, const bool overwrite) {
        if (overwrite) NameToString(loadOptions[0].value, saveName);
        if (!saveName[0]) return FILE_FAILURE;
        ti_var_t savedStatistics = ti_Open(saveName, "r");

        char identifier[IDENTIFIER_SIZE];

        if (savedStatistics && ti_Read(identifier, 1, IDENTIFIER_SIZE, savedStatistics) == IDENTIFIER_SIZE && StringCompare(identifier, statisticsSaveIdentifier) && ti_Read(&stats, sizeof(Statistics), 1, savedStatistics) == 1) {
            if (forWhite) whiteStatistics = stats;
            else blackStatistics = stats;

            ti_Close(savedStatistics);

            return FILE_SUCCESS;
        }

        if (savedStatistics) ti_Close(savedStatistics);

        return FILE_FAILURE;
    }

    uint8_t Board::LoadStatistics (Statistics& stats, const bool forWhite) { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return LoadStatistics(stats, forWhite, nameAddress, true); };

    uint8_t Board::ViewText (const char* title, char* saveName, const bool overwrite) {
        if (overwrite) NameToString(pgnOptions[0].value, saveName);
        if (!saveName[0]) return 0;
        ti_var_t savedText = ti_Open(saveName, "r");

        char identifier[IDENTIFIER_SIZE];

        if (savedText && ti_Read(identifier, 1, IDENTIFIER_SIZE, savedText) == IDENTIFIER_SIZE && StringCompare(identifier, textSaveIdentifier)) {
            const uint16_t textSize = (uint16_t(ti_GetC(savedText)) << 8) | ti_GetC(savedText);

            char text[textSize];

            uint16_t read = 0;
            if ((read = ti_Read(text, 1, textSize, savedText)) != textSize) {
                ti_Close(savedText);

                return 0;
            }

            if (ti_Close(savedText) == 0) return 0;

            uint8_t key = 0, line = 0;

            uint8_t maxLines = DisplayText(title, text, textSize, line, 0, true);
            if (maxLines <= 17) DisplayText(title, text, textSize, line, maxLines); // Overrides the down arrow when there are too few lines.

            while (key != sk_Clear && key != sk_Del && key != sk_0 && key != sk_DecPnt && key != sk_Chs) {
                key = Key();

                if (key == sk_Up && line > 0) {
                    line--;
                    DisplayText(title, text, textSize, line, maxLines);
                } else if (key == sk_Down && line + 17 < maxLines) {
                    line++;
                    DisplayText(title, text, textSize, line, maxLines);
                }
            }

            return key;
        }

        ti_Close(savedText);

        return 0;
    }

    uint8_t Board::ViewText (const char* title) { char nameAddress[MAX_SAVE_NAME + 1]; for (uint8_t i = 0; i < MAX_SAVE_NAME + 1; nameAddress[i++] = '\0') {}; return ViewText(title, nameAddress, true); };

    uint8_t Board::DisplayText (const char* title, char* text, const uint16_t textSize, const uint8_t offset, const uint8_t maxLines, const bool getFullLineCount) {
        gfx_ZeroScreen();
        gfx_SetTextBGColor(TRANSPARENT_COLOR);

        gfx_SetColor(LIGHT_BACKGROUND_COLOR);
        gfx_FillRectangle(0, 0, GFX_LCD_WIDTH, 18);

        gfx_SetTextFGColor(BLACK_COLOR);
        gfx_PrintStringXY(title, GFX_LCD_WIDTH / 2 - gfx_GetStringWidth(title) / 2, 5);

        if (!offset) {
            gfx_SetTextFGColor(LIGHT_BACKGROUND_COLOR);
            gfx_SetTextXY(5, 24);
            gfx_PrintInt(1, 2);
            gfx_SetTextFGColor(WHITE_COLOR);
        }

        uint16_t lineCounter = 0;
        bool previousIndexCausedLineBreak = false;
        for (uint16_t index = 0, currentLineWidth = 32; index < textSize && (getFullLineCount || lineCounter < offset + 17); index++) {
            if (text[index] == '\n' || (index && !previousIndexCausedLineBreak && (currentLineWidth += gfx_GetCharWidth(text[index - 1])) >= GFX_LCD_WIDTH - 12)) {
                currentLineWidth = 32;
                lineCounter++;

                previousIndexCausedLineBreak = true;

                if (text[index] != '\n') index--;

                if (lineCounter < offset || lineCounter >= offset + 17) continue;

                gfx_SetTextFGColor(LIGHT_BACKGROUND_COLOR);
                gfx_SetTextXY(5, (lineCounter - offset + 2) * 12);
                gfx_PrintInt(lineCounter + 1, 2);
                if (lineCounter == offset + 16 && lineCounter != maxLines - 1) {
                    gfx_SetTextXY(8, 228);
                    gfx_PrintChar(DOWN_ARROW_TEXT);
                }
                gfx_SetTextFGColor(WHITE_COLOR);

                continue;
            } else previousIndexCausedLineBreak = false;

            if (lineCounter < offset || lineCounter >= offset + 17) continue;

            gfx_SetTextXY(currentLineWidth, (lineCounter - offset + 2) * 12);
            gfx_PrintChar(text[index]);
        }

        gfx_BlitBuffer();

        return lineCounter + (lineCounter > 17);
    }

    void Board::AddMoveToStatistics () {
        uint32_t* stats = sideToMove == Piece::black ? whiteStatistics.stats : blackStatistics.stats;

        stats[Statistics::movesPlayed]++;
        if (lastMove.check) stats[Statistics::checksGiven]++;
        switch (Piece::GetPiece(lastMove.pieceMoved)) {
            case Piece::pawn:
                stats[Statistics::pawnMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::pawnCapturesPlayed]++;
                    if (Move::IsFlag(lastMove, Move::enPassant)) stats[Statistics::enPassantsPlayed]++;
                } else if (Move::IsFlag(lastMove, Move::doublePawnPush)) stats[Statistics::doublePawnPushesPlayed]++;
                if (Move::IsPromotion(lastMove)) {
                    stats[Statistics::promotionsPlayed]++;
                    if (Move::IsThisPromotion(lastMove, Move::knightPromotion)) stats[Statistics::knightPromotionsPlayed]++;
                    else if (Move::IsThisPromotion(lastMove, Move::bishopPromotion)) stats[Statistics::bishopPromotionsPlayed]++;
                    else if (Move::IsThisPromotion(lastMove, Move::rookPromotion)) stats[Statistics::rookPromotionsPlayed]++;
                    else if (Move::IsThisPromotion(lastMove, Move::queenPromotion)) stats[Statistics::queenPromotionsPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithAPawn]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithAPawn]++;
                }
                break;
            case Piece::knight:
                stats[Statistics::knightMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::knightCapturesPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithAKnight]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithAKnight]++;
                }
                break;
            case Piece::bishop:
                stats[Statistics::bishopMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::bishopCapturesPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithABishop]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithABishop]++;
                }
                break;
            case Piece::rook:
                stats[Statistics::rookMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::rookCapturesPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithARook]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithARook]++;
                }
                break;
            case Piece::queen:
                stats[Statistics::queenMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::queenCapturesPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithAQueen]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithAQueen]++;
                }
                break;
            case Piece::king:
                stats[Statistics::kingMovesPlayed]++;
                if (Move::IsCapture(lastMove)) {
                    stats[Statistics::capturesPlayed]++;
                    stats[Statistics::kingCapturesPlayed]++;
                }
                if (Move::IsFlag(lastMove, Move::kingCastle) || Move::IsFlag(lastMove, Move::queenCastle)) {
                    stats[Statistics::castlesPlayed]++;
                    if (Move::IsFlag(lastMove, Move::kingCastle)) stats[Statistics::kingSideCastlesPlayed]++;
                    else stats[Statistics::queenSideCastlesPlayed]++;
                }
                if (lastMove.check) {
                    stats[Statistics::checksGivenWithAKing]++;
                    if (!movesAvailable) stats[Statistics::checkmatesGivenWithAKing]++;
                }
                break;
        }

        bool whiteWon = whiteWins || whiteWinsByTimeout || whiteWinsByResignation;
        bool blackWon = blackWins || blackWinsByTimeout || blackWinsByResignation;
        bool drew = stalemate || insufficentMaterial || fiftyMoveRule || threefoldRepetition || drawByAgreement;

        if (whiteWon) {
            whiteStatistics.stats[Statistics::wins]++;
            whiteStatistics.stats[Statistics::winsAsWhite]++;
            blackStatistics.stats[Statistics::losses]++;
            blackStatistics.stats[Statistics::lossesAsBlack]++;
            if (whiteWinsByTimeout) {
                whiteStatistics.stats[Statistics::winsByTimeout]++;
                blackStatistics.stats[Statistics::lossesByTimeout]++;
            } else if (whiteWinsByResignation) {
                whiteStatistics.stats[Statistics::winsByResignation]++;
                blackStatistics.stats[Statistics::lossesByResignation]++;
            }
            if (ply < whiteStatistics.stats[Statistics::minimumPlyToWin] || !whiteStatistics.stats[Statistics::minimumPlyToWin]) whiteStatistics.stats[Statistics::minimumPlyToWin] = ply;
            if (ply < blackStatistics.stats[Statistics::minimumPlyToLoss] || !blackStatistics.stats[Statistics::minimumPlyToLoss]) blackStatistics.stats[Statistics::minimumPlyToLoss] = ply;
            if (ply > whiteStatistics.stats[Statistics::maximumPlyToWin]) whiteStatistics.stats[Statistics::maximumPlyToWin] = ply;
            if (ply > blackStatistics.stats[Statistics::maximumPlyToLoss]) blackStatistics.stats[Statistics::maximumPlyToLoss] = ply;
        } else if (blackWon) {
            blackStatistics.stats[Statistics::wins]++;
            blackStatistics.stats[Statistics::winsAsBlack]++;
            whiteStatistics.stats[Statistics::losses]++;
            whiteStatistics.stats[Statistics::lossesAsWhite]++;
            if (whiteWinsByTimeout) {
                blackStatistics.stats[Statistics::winsByTimeout]++;
                whiteStatistics.stats[Statistics::lossesByTimeout]++;
            } else if (blackWinsByResignation) {
                blackStatistics.stats[Statistics::winsByResignation]++;
                whiteStatistics.stats[Statistics::lossesByResignation]++;
            }
            if (ply < blackStatistics.stats[Statistics::minimumPlyToWin] || !blackStatistics.stats[Statistics::minimumPlyToWin]) blackStatistics.stats[Statistics::minimumPlyToWin] = ply;
            if (ply < whiteStatistics.stats[Statistics::minimumPlyToLoss] || !whiteStatistics.stats[Statistics::minimumPlyToLoss]) whiteStatistics.stats[Statistics::minimumPlyToLoss] = ply;
            if (ply > blackStatistics.stats[Statistics::maximumPlyToWin]) blackStatistics.stats[Statistics::maximumPlyToWin] = ply;
            if (ply > whiteStatistics.stats[Statistics::maximumPlyToLoss]) whiteStatistics.stats[Statistics::maximumPlyToLoss] = ply;
        } else if (drew) {
            whiteStatistics.stats[Statistics::draws]++;
            blackStatistics.stats[Statistics::draws]++;
            if (stalemate) {
                whiteStatistics.stats[Statistics::drawsByStalemate]++;
                blackStatistics.stats[Statistics::drawsByStalemate]++;
            } else if (insufficentMaterial) {
                whiteStatistics.stats[Statistics::drawsByInsufficentMaterial]++;
                blackStatistics.stats[Statistics::drawsByInsufficentMaterial]++;
            } else if (fiftyMoveRule) {
                whiteStatistics.stats[Statistics::drawsByFiftyMoveRule]++;
                blackStatistics.stats[Statistics::drawsByFiftyMoveRule]++;
            } else if (threefoldRepetition) {
                whiteStatistics.stats[Statistics::drawsByThreefoldRepetition]++;
                blackStatistics.stats[Statistics::drawsByThreefoldRepetition]++;
            } else if (timeoutWithInsufficentMaterial) {
                whiteStatistics.stats[Statistics::drawsByTimeoutWithInsufficentMaterial]++;
                blackStatistics.stats[Statistics::drawsByTimeoutWithInsufficentMaterial]++;
            } else if (drawByAgreement) {
                whiteStatistics.stats[Statistics::drawsByAgreement]++;
                blackStatistics.stats[Statistics::drawsByAgreement]++;
            }
            if (ply < whiteStatistics.stats[Statistics::minimumPlyToDraw] || !whiteStatistics.stats[Statistics::minimumPlyToDraw]) whiteStatistics.stats[Statistics::minimumPlyToDraw] = ply;
            if (ply < blackStatistics.stats[Statistics::minimumPlyToDraw] || !blackStatistics.stats[Statistics::minimumPlyToDraw]) blackStatistics.stats[Statistics::minimumPlyToDraw] = ply;
            if (ply > whiteStatistics.stats[Statistics::maximumPlyToDraw]) whiteStatistics.stats[Statistics::maximumPlyToDraw] = ply;
            if (ply > blackStatistics.stats[Statistics::maximumPlyToDraw]) blackStatistics.stats[Statistics::maximumPlyToDraw] = ply;
        }

        if (whiteWon || blackWon || drew) {
            int32_t deltaElo = round(32.0 * ((whiteWon ? 1.0 : blackWon ? 0.0 : 0.5) - (1.0 / (1.0 + pow(10.0, (double(int32_t(blackStatistics.stats[Statistics::elo]) - int32_t(whiteStatistics.stats[Statistics::elo])) / 400.0))))));
            whiteStatistics.stats[Statistics::elo] += deltaElo;
            blackStatistics.stats[Statistics::elo] -= deltaElo;

            selection = OFFBOARD;

            gameOver = true;

            if (savePGNOnGameOver) ExportDefaultPGN();
        }
    }

    uint8_t Board::ViewStatistics (Statistics& stats) {
        const uint8_t numberOfStats = 8;
        uint8_t offset = 0;

        uint8_t key = 0;

        DisplayStatistics(stats, numberOfStats, offset);

        while (key != sk_Clear && key != sk_Del && key != sk_0 && key != sk_DecPnt && key != sk_Chs) {
            key = Key();

            if (key == sk_Left) offset = offset < numberOfStats ? Statistics::numberOfStatistics / numberOfStats * numberOfStats : offset - numberOfStats;
            else if (key == sk_Right) offset = offset >= Statistics::numberOfStatistics - numberOfStats ? 0 : offset + numberOfStats;
            else if (key && offset == Statistics::name) {
                const char character = GetCharacterForKey(key);

                if (character) {
                    if (character != '0') {
                        uint8_t length = 0;
                        for (uint32_t temp = stats.stats[Statistics::name]; temp; temp >>= 5, length++);

                        if (length < 6) {
                            stats.stats[Statistics::name] <<= 5;
                            stats.stats[Statistics::name] |= character - 'A' + 1;
                        }
                    } else if (stats.stats[Statistics::name]) {
                        stats.stats[Statistics::name] >>= 5;
                        key = 0;
                    }
                }
            }

            if (key == sk_Left || key == sk_Right || offset == Statistics::name) DisplayStatistics(stats, numberOfStats, offset);
        }

        return key;
    }

    void Board::DisplayStatistics (Statistics& stats, const uint8_t numberOfStats, const uint8_t offset) {
        gfx_ZeroScreen();
        gfx_SetTextBGColor(TRANSPARENT_COLOR);

        gfx_SetColor(LIGHT_BACKGROUND_COLOR);
        gfx_FillRectangle(0, 0, GFX_LCD_WIDTH, 18);

        gfx_SetTextFGColor(BLACK_COLOR);
        gfx_PrintStringXY("STATISTICS", GFX_LCD_WIDTH / 2 - gfx_GetStringWidth("STATISTICS") / 2, 5);

        gfx_SetTextFGColor(WHITE_COLOR);

        uint16_t longestStringWidth = 0, stringWidth = 0;
        for (int8_t index = 0; index < numberOfStats && index + offset < Statistics::numberOfStatistics; index++) {
            stringWidth = gfx_GetStringWidth(Statistics::names[index + offset]) + gfx_GetStringWidth(": ");
            for (uint32_t temp = stats.stats[index + offset]; temp; stringWidth += gfx_GetCharWidth(char(temp % 10 + '0')), temp /= 10);
            longestStringWidth = longestStringWidth < stringWidth ? stringWidth : longestStringWidth;
        }
        
        for (int8_t index = 0; index < numberOfStats && index + offset < Statistics::numberOfStatistics; index++) {
            const char* name = Statistics::names[index + offset];
            int16_t x = GFX_LCD_WIDTH / 2 - longestStringWidth / 2;
            const int16_t y = 5 * GFX_LCD_HEIGHT / 12 - 4 + index * 12;

            gfx_PrintStringXY(name, x, y);

            gfx_PrintStringXY(": ", x += gfx_GetStringWidth(name), y);

            gfx_SetTextXY(x + gfx_GetStringWidth(": "), y);
            if (index + offset != Statistics::name) gfx_PrintUInt(stats.stats[index + offset], 1);
            else {
                int16_t length = 0;
                x += 8;

                for (uint32_t value = stats.stats[Statistics::name]; value; x += gfx_GetCharWidth(IndexToCharacter[value & 0x1FU]), value >>= 5);

                for (uint32_t value = stats.stats[Statistics::name]; value; value >>= 5) {
                    const char character = IndexToCharacter[value & 0x1FU];
                    length += gfx_GetCharWidth(character);

                    gfx_SetTextXY(x - length, y);
                    gfx_PrintChar(character);
                }

                if ((clock() - programStartTime) / CLOCKS_PER_TEXT % 2) {
                    gfx_SetTextXY(x + 2, y);
                    gfx_PrintChar('|');
                }
            }
        }

        const uint8_t page = (offset / numberOfStats) + 1;
        const char pageString[3] = { char(page / 10 + '0'), char(page % 10 + '0'), '\0' };
        int16_t x = GFX_LCD_WIDTH / 2 - gfx_GetStringWidth(pageString);
        const int16_t y = 5 * GFX_LCD_HEIGHT / 12 - 4 + (numberOfStats + 1) * 12;

        gfx_PrintStringXY(pageString, x, y);

        if (offset != 0) {
            gfx_SetTextXY(x - gfx_GetCharWidth(LEFT_ARROW_TEXT) - 2, y);
            gfx_PrintChar(LEFT_ARROW_TEXT);
        }

        if (offset + numberOfStats < Statistics::numberOfStatistics) {
            gfx_SetTextXY(x + gfx_GetStringWidth(pageString) + 2, y);
            gfx_PrintChar(RIGHT_ARROW_TEXT);
        }

        gfx_BlitBuffer();
    }

    void Board::RestoreSettings () {
        settingOptions[0].value = DefaultLinearPromotionValue(promotePiece = Piece::queen);

        playerOptions[0].value = whitePlayer = HUMAN;
        playerOptions[1].value = blackPlayer = HUMAN;

        for (uint8_t i = 0; i < ply; timeRemainingAtPly[i++] = 0);
        timingOptions[0].value = isTimedGame = false;
        timingOptions[1].value = lowTimeWarning = false;
        timingOptions[2].value = whiteStartingTime = whiteTimeRemaining = 0;
        timingOptions[3].value = blackStartingTime = blackTimeRemaining = 0;
        timingOptions[4].value = bonusTime = 0;
        paused = false;

        boardOptions[0].value = boardView = FOLLOW_PLAYER;
        boardOptions[1].value = tilePalette = 0;
        boardOptions[2].value = pieceStyle = 0;
        boardOptions[3].value = background = MATCHING;
        boardOptions[4].value = checkHighlighting = true;
        boardOptions[5].value = lastMoveHighlighting = true;
        boardOptions[6].value = legalMoveDots = true;

        saveOptions[6].value = savePGNOnGameOver = false;

        resetOptions[6].value = resetBoardOn = 0;
    }
}