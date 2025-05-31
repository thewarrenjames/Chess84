/**
 * @file board.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of chess board
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    struct BoardDefinitions {
        char startingFen[MAX_FEN_LENGTH];

        Move moveHistory[MAX_MOVE_HISTORY];
        uint8_t ply = 0;
        bool gameOver = false;

        uint8_t promotePiece = Piece::queen;

        uint8_t whitePlayer = 0;
        uint8_t blackPlayer = 0;

        bool isTimedGame = false;
        bool lowTimeWarning = false;
        int64_t timeRemainingAtPly[MAX_MOVE_HISTORY];
        int64_t blackStartingTime = 0;
        int64_t whiteStartingTime = 0;
        int64_t blackTimeRemaining = blackStartingTime;
        int64_t whiteTimeRemaining = whiteStartingTime;
        int64_t bonusTime = 0;
        bool paused = false;

        uint8_t boardView = FOLLOW_PLAYER;
        uint8_t tilePalette = 0;
        uint8_t pieceStyle = 0;
        uint8_t background = MATCHING;
        bool checkHighlighting = true;
        bool lastMoveHighlighting = true;
        bool legalMoveDots = true;

        Statistics whiteStatistics { WHITE_ENCODED };
        Statistics blackStatistics { BLACK_ENCODED };

        bool savePGNOnGameOver = false;

        uint8_t resetBoardOn = 0;

        BoardDefinitions () {}
    };

    extern BoardDefinitions newBoard;

    struct Board : public BoardDefinitions {
        piece matrix[64];

        uint8_t blackPawnsCount = 0;
        uint8_t blackKnightsCount = 0;
        uint8_t blackBishopsCount = 0;
        uint8_t blackRooksCount = 0;
        uint8_t blackQueensCount = 0;
        uint8_t blackPiecesCount = 0;
        uint8_t whitePawnsCount = 0;
        uint8_t whiteKnightsCount = 0;
        uint8_t whiteBishopsCount = 0;
        uint8_t whiteRooksCount = 0;
        uint8_t whiteQueensCount = 0;
        uint8_t whitePiecesCount = 0;

        uint8_t blackKingSquare = 0;
        uint8_t blackKingFile = 0;
        uint8_t blackKingRank = 0;
        uint8_t whiteKingSquare = 0;
        uint8_t whiteKingFile = 0;
        uint8_t whiteKingRank = 0;

        Move lastMove;

        Move legalMoves[MAX_LEGAL_MOVES];
        uint8_t movesAvailable = 0;

        Move pseudoLegalMoves[MAX_PSEUDO_LEGAL_MOVES];
        uint8_t pseudoLegalMovesAvailable = 0;
        Move pseudoLegalFirstStrikes[MAX_PSEUDO_LEGAL_MOVES];
        uint8_t pseudoLegalFirstStrikesAvailable = 0;
        Move pseudoLegalResponses[MAX_PSEUDO_LEGAL_MOVES];
        uint8_t pseudoLegalResponsesAvailable = 0;

        uint8_t sideToMove = Piece::white;
        bool inCheck = false;
        bool initalInCheck = false;
        uint8_t enPassantSquare = OFFBOARD;
        uint8_t fiftyMoveCount = 0;
        uint8_t initialFiftyMoveCount = 0;
        uint8_t repetitions = 0;

        bool blackKingCastleRights = true;
        bool blackQueenCastleRights = true;
        bool whiteKingCastleRights = true;
        bool whiteQueenCastleRights = true;
        bool initialBlackKingCastleRights = true;
        bool initialBlackQueenCastleRights = true;
        bool initialWhiteKingCastleRights = true;
        bool initialWhiteQueenCastleRights = true;
        bool blackCastled = false;
        bool whiteCastled = false;

        uint8_t selection = A1;
        uint8_t selected = OFFBOARD;

        bool blackWins = false;
        bool whiteWins = false;
        bool blackWinsByTimeout = false;
        bool whiteWinsByTimeout = false;
        bool blackWinsByResignation = false;
        bool whiteWinsByResignation = false;

        bool stalemate = false;
        bool insufficentMaterial = false;
        bool fiftyMoveRule = false;
        bool threefoldRepetition = false;
        bool timeoutWithInsufficentMaterial = false;
        bool drawByAgreement = false;

        char* gameState = nullptr;
        char* gameStateReasoning = nullptr;

        hash zobristHashHistory[MAX_MOVE_HISTORY];
        hash lastZobristHash = 0;
        hash initialZobristHash = 0;
        hash zobristPieceDataKeys[64][13] = { { 0xC6509CD1, 0x8FB13D35, 0x4F96AFBA, 0xA7941E6D, 0x82F183ED, 0xE16FF915, 0xCCB5F9A7, 0x3B067B9B, 0x49B00830, 0x117CA65A, 0x26A50C10, 0x8A769848, 0xA845D64A }, { 0x2FC0FA4B, 0x658B9B97, 0x56B3C8A8, 0xCFDD5BDE, 0xC7A489CD, 0xDEA1F562, 0x3887F711, 0xAB5AA6BC, 0xDBD06236, 0xA63B2FC9, 0x22127667, 0x2A87B91C, 0xC5A52E81 }, { 0xDA190FC0, 0xD2DDCB87, 0x1B613BBB, 0x96EB8E5A, 0x2B918BB4, 0x894B071E, 0x0C8ADE44, 0xAE3E817F, 0xA3D420C9, 0xF7656E2B, 0xA1B141D7, 0x510AB483, 0x6B1DADE1 }, { 0x2D1976D5, 0x453E20CE, 0x83AEB567, 0x179A993A, 0xEE34309A, 0xE16251B3, 0xFA69D9C3, 0x78141756, 0xD75624CA, 0x76DC602F, 0x41DEE56A, 0x7F9EEDDE, 0x988F49AC }, { 0xB2DAB0CB, 0xE3B254C1, 0x75F26150, 0xD0356637, 0xCB882DE0, 0xE51D91FE, 0x9B0935E3, 0xCD4ACA5A, 0x43EFAEAE, 0xF5081C4D, 0x4404B352, 0x6695E4A0, 0x47EC745D }, { 0xFF9F1381, 0x703C2F6A, 0x281CE65B, 0x495D5A77, 0x62090673, 0xD6659812, 0x05EABB66, 0x057DE278, 0xBAD4EB0F, 0xD3E7D803, 0x5BF16361, 0x3234A55D, 0x63059946 }, { 0x7F22EB47, 0xF887BD0E, 0xE946FC3A, 0x707B559B, 0x6040F324, 0xEAA3B3A4, 0xF373B3F4, 0xFE24EAB7, 0x50988BB9, 0xCD9AA083, 0x0785FBD8, 0x031CC812, 0xBDAE0C72 }, { 0xCBDEE36A, 0xF3B52F6C, 0xF2512CCA, 0xD5DE795B, 0xC11534D8, 0x4899EBA4, 0xB1826FE2, 0xE3BA9042, 0x500546AF, 0x0BAFB294, 0x1C62685F, 0x16D6FA46, 0xF771E982 }, { 0xCD5117A7, 0xC60DD635, 0xD460B86E, 0xA08997B0, 0x6F0C6210, 0xE6228F81, 0x5039C168, 0x5ACCB27A, 0xD7535C27, 0x2A53989E, 0x27B4B97E, 0x83011C08, 0x9319263F }, { 0x6831B7C4, 0xF00307AE, 0x03582785, 0x9CBFAFDD, 0x85342369, 0x3ACC0E9B, 0x2D9E660B, 0xE238E1CA, 0x884788ED, 0xC28C2D88, 0x9185D913, 0x78A38678, 0x9CC10278 }, { 0x7263CBAA, 0x6EFBCDBE, 0x78D1F615, 0x972FCED4, 0x67877973, 0xBE2ACAEC, 0xBBA318B0, 0x2FDAD119, 0xF4EB0970, 0x70A3DD3D, 0x30AD8FA0, 0xCE673D39, 0x9940EC6F }, { 0xEFB9CC28, 0x94B8F3BE, 0xA8DF039B, 0x88979246, 0x926A9184, 0xB1839A56, 0xD90FFC3D, 0xF6A7C88B, 0xE6A101DD, 0x09EDFC9F, 0xBAA0FE3C, 0x85E72AB5, 0xA8463A2A }, { 0xDC4291FF, 0x9CCAD3DE, 0x0BFF3FE1, 0xFAC43844, 0x143D2D61, 0x0A7249D9, 0x8A35B0E7, 0xDACA68F0, 0xE9C1E548, 0x134B9AD2, 0x872F0DE9, 0x22512337, 0x85B555DE }, { 0x42846271, 0x73620E2B, 0xDAFBE59E, 0x930C160A, 0x44AC0181, 0x6C43485C, 0xF689D8A0, 0x717E37E0, 0x3ABEBBBF, 0x0FBBF03B, 0xC8488B53, 0xCEC4CC6D, 0x69CE70D0 }, { 0x48DFB1A7, 0x2B87B235, 0x4B97FE75, 0xC58F3E0D, 0xCC3E7C34, 0xE3EF47CE, 0xBDBEF37D, 0x68D00918, 0xC1E21D47, 0xE97142BC, 0x20B5DB4C, 0xDF82E860, 0x87EB41E8 }, { 0xB7BE1AD6, 0xE92657B2, 0xD5907A1E, 0xE9109EB1, 0x5E8A524E, 0xB616E828, 0x831C86E9, 0x32CDFEF4, 0xC4680BDD, 0x4CA3867C, 0x6FB06C27, 0x796CFF50, 0x5D5A8B6B }, { 0x7AD96FC5, 0xE8F274DC, 0x7FD840FF, 0xF87C4115, 0xA4748994, 0xF4FBDEE4, 0x0C1BF9DC, 0xE480BC64, 0x4B6D64C1, 0x3C299A46, 0xDDF31254, 0xBECA11D9, 0x96F375BC }, { 0xC5A62FE0, 0x7474C92F, 0xC7C80E6D, 0xCD8C2AE5, 0x20B515C9, 0x5E088E0B, 0xA103F5FF, 0xFC9660D6, 0x9788A537, 0x1CDFAB3A, 0x5B5E1BBD, 0xE59E46A6, 0xB051F9AF }, { 0x4A48165E, 0x4897E0A0, 0x4631C2B9, 0x94E05145, 0x0F275C74, 0x13B2D5A3, 0xBADA9945, 0xBD227238, 0xB8C3946F, 0x3F9C4432, 0xAC8EA49C, 0x4FF666D3, 0x73AC0D0C }, { 0x7A58FB3C, 0xF1AE114B, 0xDF133EE3, 0x5F8A190A, 0x2D88DC7A, 0x55C87364, 0xD583A69F, 0xC09B9567, 0x39FE8420, 0x337BAFCA, 0x7A300C2D, 0x1FFEBF93, 0x87469D15 }, { 0x5081F68E, 0xCF8D8754, 0xC9014495, 0x44290483, 0xB69053A3, 0xE1B08AF4, 0x62E20BF6, 0xC9B29F15, 0x96CEBC1C, 0x03DC68A8, 0x6E28B527, 0x7A627C60, 0x84346EB3 }, { 0x44C020C7, 0x8F7B2367, 0x8DAFB8DA, 0x8A9B75DF, 0x91345D76, 0x0A3916DD, 0xD03D473C, 0x3CBA608D, 0xA64A1317, 0x2DD1F645, 0x61B559AA, 0x670D7055, 0xDDFFCB4A }, { 0x635675C2, 0xCC123CF4, 0xC7F64572, 0xE1635E5C, 0x4419E1B2, 0xC41AFCC8, 0x38F2F858, 0xCD79F91D, 0x1183D559, 0x7E05BF5D, 0x482E0526, 0x5AA24EED, 0x9AA88304 }, { 0x6BDAD2F1, 0x0E167113, 0xA5AB0BA0, 0xD83835F7, 0xA21A7F77, 0x62A368F2, 0xFA1AF1CA, 0xDB4E3B89, 0x7C437DEA, 0xB8835E5D, 0xDE9A1026, 0xB7906636, 0xE74586EA }, { 0xD45BEDEA, 0x6362D71F, 0x8D21B57F, 0x341D6A5B, 0x5F4D220F, 0xC63C4F45, 0xD401FE39, 0x11731F4B, 0xC6637A43, 0xBD02E853, 0xE23684EE, 0xF5765561, 0xF3A98774 }, { 0x5E5BF717, 0xEDE7E78F, 0x4F3C2431, 0x7C1CCE3E, 0xF73CBCEA, 0xE8AB24C1, 0x00330FC0, 0x0E9AD8B7, 0x17010B33, 0x09BF55E4, 0xF094B47B, 0xCEE8DD7F, 0xDA2CB058 }, { 0x212F9F0D, 0x474ECBFC, 0x8B4DD95B, 0xD0689956, 0x43F0CCC8, 0x516BDC0A, 0x652DC83E, 0xADD5B744, 0x6DD6A372, 0x988D6BB5, 0x9689CECA, 0xEFEF6512, 0xE7634CCF }, { 0xDD807801, 0x97AF4B81, 0x8D843135, 0x64E7567A, 0x94C07A23, 0x1E571CF4, 0xEB54DC9E, 0xBE3DC052, 0xC1375A61, 0x84354B73, 0x481DEEEC, 0x0D08831F, 0x1C96414C }, { 0xEF430A46, 0x1E8FEF4F, 0x109373B1, 0x6FE333E0, 0x48ED8EA3, 0xBE2B7F14, 0xBD8949AA, 0xB6945F1C, 0x613AC212, 0x3933278C, 0x7BBA220B, 0xA2E5DEA7, 0xF54C9C9C }, { 0x1E4B02EF, 0x7E8A2335, 0x3E9572EB, 0xCB4B5A70, 0x001D61D7, 0x544C6F80, 0x93C39393, 0xBE6F42BE, 0x47FA45D5, 0xE22D5F07, 0x638FC509, 0xF29509E4, 0x17B18C0B }, { 0x8A2C512C, 0x1C0F7B62, 0x63A21F15, 0xBB35F4FC, 0xF43C4958, 0xA31A6B6C, 0x356BE260, 0x0684E1F9, 0xEBD2562D, 0x868EC13C, 0xF60ED24B, 0xB188C3A4, 0x06322DEB }, { 0x625DE25E, 0xA4A84EE1, 0x5A9B0598, 0x59687E90, 0x22B83737, 0xA35E47B0, 0x5F44659B, 0x2EA2F512, 0x6F853CA7, 0x44EBB7DE, 0xC88FC1E9, 0xA327E33C, 0xDB881BC3 }, { 0x91C8151B, 0xCD97BC66, 0x56C6A921, 0x648A7484, 0x5D9D2EE8, 0x2B53F0B4, 0x5C37CE3D, 0x88F633D1, 0x06E4DC50, 0x9A0AEAA0, 0xE270B66E, 0x6377A6F0, 0x3C91F17A }, { 0xFE407EA9, 0x3C151AB1, 0xBF05A69D, 0x2FF09E36, 0xF0EA2EFA, 0x1F450E08, 0x19FFEF24, 0xA5850E1E, 0x39DCDD4C, 0x6D23D574, 0xF562EBF7, 0xF91C7C7A, 0xEACC4BE1 }, { 0xB999FD2B, 0x1637077A, 0xEAE830DB, 0x19972049, 0x748F0302, 0xD6581730, 0x7D0E85C7, 0x571C1F8A, 0x3FAEA341, 0xD690FE0A, 0xF70CDFB7, 0xE224A206, 0x1DA5E081 }, { 0x11DEAA0E, 0x5E8A62C7, 0xCE69DF98, 0x6E195D4A, 0x2211D79D, 0xAD204B2B, 0x19EC90C9, 0x6C7E216D, 0xADC9876B, 0xA6FE4D03, 0xFC42AE2B, 0x595F14B6, 0x12C49A0D }, { 0x21F6A05C, 0x652B0C39, 0x4742DEFA, 0x7383C3E9, 0x87139362, 0xE0217B0D, 0x800ED816, 0x7B512770, 0x3E2D14CC, 0x08686BDA, 0x4D431195, 0x552F8C9C, 0xEEBF6ABB }, { 0x9BEFCFD3, 0x708A9AF9, 0x46F99AC1, 0xBF36CCE1, 0x9A00D636, 0x55D55FCB, 0x65F5213F, 0x6392EA57, 0xA9EBE331, 0xFA23A620, 0x2FBFA7FE, 0x3D42A882, 0xDA30E4F8 }, { 0xDB4DEF88, 0xE30DBD2C, 0x4E4EEA25, 0xBA4462DE, 0x55C1B6C7, 0x3CC7D0C9, 0x51B047D4, 0x4FB3CED4, 0x007F8D87, 0x8966F524, 0x82BF6616, 0xA720F770, 0xDE805ABF }, { 0xA584DEBC, 0xC7287AF4, 0xC8A8EE17, 0x9ED82A7D, 0x2650F35E, 0x3A1862FF, 0x0694DB18, 0xD4920BDD, 0x692D75AD, 0x902A25AC, 0x9EDB5EA1, 0x33A3ED7D, 0x10B6F487 }, { 0xF1B75A2C, 0x9F7B7C9C, 0x83E15F5F, 0xFCA8D98D, 0x8F362C0E, 0x11B22E6F, 0xD9832DA6, 0x2CDA1D46, 0xC6FF8946, 0x78E03205, 0x60C609B3, 0xB103C175, 0xD98DB0AC }, { 0x206645AC, 0x5193BC0E, 0xF70431D9, 0x4B7ABE23, 0x6B67F7A8, 0x4DB4FF0F, 0xEE53CA45, 0x6119A65C, 0x64BE87B1, 0x2CE142F5, 0xF5FA7C4D, 0xBB7BBFBC, 0xDB39C002 }, { 0x0A1476EA, 0x65A3513E, 0x50C3F5C8, 0xD9C3356E, 0x01A436F7, 0x5A2C5660, 0xDD19BE4E, 0x1EFB6E66, 0x62A5CA0D, 0x2682EDA2, 0x5A948659, 0x8AEFAFF1, 0x9343F29E }, { 0x46D87C82, 0x226355DF, 0xE0BE431E, 0x295D1791, 0x8D69DB5A, 0xED8A2A81, 0x21464BF4, 0x5D7F3868, 0x6EC4B6B6, 0x344C2E96, 0x0B2345D5, 0xD4606138, 0x3894D2DF }, { 0x3B5DAF4F, 0x91B5F11F, 0xC1981893, 0x2B3AAC45, 0x48504BA6, 0x6D3C7C61, 0x45724975, 0x99CB20F6, 0xE6296F74, 0x4297BEB4, 0x35BC3ED1, 0xC69DDD1D, 0xF5082BA6 }, { 0x1311E92C, 0x1FAADEC7, 0xF841792A, 0xF2124544, 0xBA1F058D, 0x3AF8FD13, 0xB830301F, 0x9752EB03, 0x2B6934BA, 0xD34D3FF0, 0xEB76BF43, 0x12F44D83, 0xF96B4963 }, { 0x0B608695, 0x682E8576, 0x47F3C42F, 0x667EEAD2, 0x7BB17E70, 0xCB95FF13, 0xDD4D5E0D, 0xB8DE26B3, 0xB9722295, 0xE0282EFE, 0xD45D2CFC, 0x3254C512, 0xCCC48FE6 }, { 0x1EEF4402, 0xFE2E1429, 0xB97B1C77, 0x677FCA64, 0x313230D7, 0x6EED034D, 0x9FB46CFF, 0xAC0CDCF5, 0x56B28136, 0x59F5C5E2, 0x223D82E8, 0x7711B1C8, 0xC3CB2063 }, { 0x127EFAF9, 0xDFB9A26E, 0xBA831D43, 0x90903CDA, 0x0EB52DC8, 0xBF4203DB, 0x1C5E876B, 0xE2F120C3, 0xCD6E309D, 0xBE151351, 0xAFB44DA7, 0xD67B75F2, 0x2DEAA32F }, { 0xAE9DCFAA, 0xACC79C21, 0x79B0D7A5, 0xBCE0FCD9, 0xF0AD79AD, 0xFD610F3D, 0x943E201D, 0xDB095E5C, 0x39F4CDA8, 0x3BF9AAC9, 0xFADBFFB3, 0x26799982, 0xB62A8978 }, { 0x2431EF02, 0x571C0DA2, 0xC92ED64D, 0x801F9052, 0x59668D3F, 0xD63F653F, 0xED9CF884, 0xAFC82F45, 0xFDF8EDF4, 0x15B88FC5, 0x0A61DAC3, 0xAAF6F21A, 0xBECEB171 }, { 0xF5899B4F, 0x0DEE3169, 0xC620F053, 0x861AAD51, 0x2A42C3C7, 0x6C45491F, 0x6A253E28, 0xF8AC7D31, 0x20F67D1F, 0xE4A32392, 0xB3DBD137, 0x3215D9F9, 0x20C203C9 }, { 0x0FAADB0C, 0xD93FBEBF, 0x0C7C6C76, 0x6A607A2B, 0x863EDD3B, 0xE31D6E18, 0x5D804FEA, 0x4A8B3D13, 0xF3DB5673, 0x81CD460B, 0xCB13564A, 0x97A677FD, 0xF5697B8D }, { 0x5EC50830, 0x54A8AC5D, 0xEA49A779, 0x0A19EC56, 0x87D64E7A, 0xEC501E26, 0x3F343C21, 0x4F835844, 0x32018CB5, 0x320A2ABC, 0x0AED02F1, 0xDAD6C4DC, 0x92578A5F }, { 0xD4B8E8FF, 0x2DF9C936, 0x9D8B3FA0, 0x6342119E, 0xA4D9B5EA, 0x14A2A29B, 0x1714DD36, 0xFF87CB55, 0xDB83E3B1, 0x8C34DB41, 0x88D7D2E5, 0x9439A96C, 0x8842BD46 }, { 0x2EE774A8, 0x63BD4AA3, 0xECC2A0F1, 0x2BBB66FF, 0xE4604ECA, 0x520F3328, 0xCBBF2419, 0x337A2D2E, 0xCC4C0D74, 0x96F9160B, 0x29EE78A7, 0x78C35760, 0xC3A4E75A }, { 0x8203386C, 0x3539C137, 0x2053C03B, 0x38183794, 0x069D2005, 0x94B217B9, 0x051D1F34, 0xA9C4B5A5, 0xCD397C7D, 0xCFF63EFD, 0x9E381929, 0xECBD7E4F, 0x3849A4E1 }, { 0x806A5AC5, 0xBEDFD2F6, 0x33FBA048, 0xC359137F, 0x8BD8FF05, 0x30EC4E19, 0xF62D162A, 0x20D16A00, 0x6F3192FF, 0x22797DD8, 0xF6985898, 0xE2603351, 0x76E5CE9F }, { 0x626189A4, 0x74787497, 0xD8FC8217, 0xB5D52358, 0x027477FF, 0x837F2488, 0x422A02CF, 0x9B02D6FA, 0x065D1BCD, 0xEE58CA34, 0x9A412E40, 0xFE17C84F, 0x1AB64E5E }, { 0x0385F236, 0x61A7DD72, 0x32A9D780, 0x01D04364, 0x2BD2B805, 0xF86BA2AC, 0x03B77C61, 0x9C5D81F3, 0xF8C2F9E9, 0x03B8702D, 0xFADE3258, 0x7F83EF04, 0x646C2270 }, { 0xA0070811, 0x145F6AFA, 0x0A32B363, 0xDD152D1C, 0xC12854F0, 0xE4E166FD, 0x7B76287C, 0xF772EB23, 0x67CF56B4, 0xA9816F22, 0x95E1003D, 0x5F89F510, 0x07B6034D }, { 0xE78D92E8, 0x26416BA7, 0x4D3BB992, 0xAA73E661, 0x18A29D69, 0x09EAAD3C, 0x35E6D4E4, 0x9DA690E2, 0x18251FC7, 0xC2E23AE2, 0x1F2A45E8, 0x0A352E65, 0x48609512 }, { 0x6754F7F0, 0x170DCE1B, 0x5C7EC084, 0x50B3349B, 0x3A384E26, 0x8382BBD7, 0x7B5DB0AB, 0xB7A489BA, 0xD97E347C, 0x433B8590, 0xF5ACFD5A, 0x03E50D08, 0x19B7BCAB }, { 0x13A2D5D0, 0x77BC0EFA, 0x95EFAF22, 0x31A492F7, 0xD41EC8D5, 0x4E7D63B9, 0xEB85C0A6, 0xE03ED262, 0xAB2DF0AB, 0xAAFDFC35, 0xF2DD935A, 0x99C815C1, 0xC2406D57 } };
        hash zobristSideToMoveKey = 0x113398FA;
        hash zobristCastleRightsKeys[16] = { 0xFEC06FEC, 0x6104DA79, 0xEFC39BEB, 0x7A0B41BD, 0xD443DC94, 0xE1E2C637, 0xD0D07FDD, 0x984D6089, 0xD2F286BE, 0x0535BF85, 0x84D1453E, 0x2C768F2D, 0x664203BD, 0xFEEC2931, 0x1993458D, 0xAD03ADD7 };

        int64_t deltaTime = 0;
        clock_t previousClockTime = 0;

        const char* savedMessage = "Saved";
        const char* loadedMessage = "Loaded";
        const char* deletedMessage = "Deleted";
        const char* cannotAccessDuringGame = "DENIED";
        const char* failureMessage = "FAILED";
        const char* whiteAndBlackCycle[2] = { nullptr, "BOT" };
        Option playerOptions[2] = {
            Option("White:", cannotAccessDuringGame, nullptr, Option::cycle, whitePlayer, whiteAndBlackCycle, 2, Option::UpdateWhite),
            Option("Black:", cannotAccessDuringGame, nullptr, Option::cycle, blackPlayer, whiteAndBlackCycle, 2, Option::UpdateBlack)
        };
        const char* disabledEnabledCycle[2] = { "disabled", "enabled" };
        Option timingOptions[5] = {
            Option("Timed Game:", cannotAccessDuringGame, nullptr, Option::cycle, isTimedGame, disabledEnabledCycle, 2, Option::UpdateTimedGame),
            Option("Low Time Warning:", nullptr, nullptr, Option::cycle, isTimedGame, disabledEnabledCycle, 2, Option::UpdateLowTimeWarning),
            Option("White Starting Time:", cannotAccessDuringGame, nullptr, Option::number, Option::timeInHoursMinutesSeconds, whiteStartingTime, 0, 7200000, 30000, Option::UpdateWhiteStartingTime),
            Option("Black Starting Time:", cannotAccessDuringGame, nullptr, Option::number, Option::timeInHoursMinutesSeconds, blackStartingTime, 0, 7200000, 30000, Option::UpdateBlackStartingTime),
            Option("Bonus Time:", cannotAccessDuringGame, nullptr, Option::number, Option::timeInSeconds, bonusTime, 0, 59000, 1000, Option::UpdateBonusTime)
        };
        const char* boardViewCycle[4] = { "Follow Player", "Dynamic", "Only White", "Only Black" };
        const char* tilePaletteCycle[9] = { "Coconut", "Potato", "Beetroot", "Elderberry", "Tomato", "Kiwi", "Kale", "Blueberry", "Grape" };
        const char* pieceStyleCycle[2] = { "Neo", "Classic" };
        const char* backgroundCycle[3] = { "Matching", "Light", "Dark" };
        Option boardOptions[7] = {
            Option("Board View:", nullptr, nullptr, Option::cycle, boardView, boardViewCycle, 4, Option::UpdateBoardView),
            Option("Tile Palette:", nullptr, nullptr, Option::cycle, tilePalette, tilePaletteCycle, 9, Option::UpdateTilePalette),
            Option("Piece Style:", nullptr, nullptr, Option::cycle, pieceStyle, pieceStyleCycle, 2, Option::UpdatePieceStyle),
            Option("Background:", nullptr, nullptr, Option::cycle, background, backgroundCycle, 3, Option::UpdateBackground),
            Option("Check Highlighting:", nullptr, nullptr, Option::cycle, checkHighlighting, disabledEnabledCycle, 2, Option::UpdateCheckHighlighting),
            Option("Last Move Highlighting:", nullptr, nullptr, Option::cycle, lastMoveHighlighting, disabledEnabledCycle, 2, Option::UpdateLastMoveHighlighting),
            Option("Legal Move Dots:", nullptr, nullptr, Option::cycle, legalMoveDots, disabledEnabledCycle, 2, Option::UpdateHideLegalMoveDots)
        };
        const char* promotionCycle[5] = { "Queen", "Rook", "Bishop", "Knight", "disabled" };
        Option settingOptions[4] = {
            Option("Always Promote:", nullptr, nullptr, Option::cycle, 0, promotionCycle, 5, Option::UpdateAlwaysPromoteToPiece),
            Option("Players", Option::menu, playerOptions, 2),
            Option("Timing", Option::menu, timingOptions, 5),
            Option("Board", Option::menu, boardOptions, 7)
        };
        Option saveOptions[7] = {
            Option("Name:", Option::text, 0, 8, 0),
            Option("Save Game", savedMessage, failureMessage, Option::button, Option::SaveGame),
            Option("Save as Default", savedMessage, failureMessage, Option::button, Option::SaveAsDefault),
            Option("Save PGN", savedMessage, failureMessage, Option::button, Option::SavePGN),
            Option("Save White Statistics", savedMessage, failureMessage, Option::button, Option::SaveWhiteStatistics),
            Option("Save Black Statistics", savedMessage, failureMessage, Option::button, Option::SaveBlackStatistics),
            Option("Save PGN on Game Over:", nullptr, nullptr, Option::cycle, savePGNOnGameOver, disabledEnabledCycle, 2, Option::UpdateSavePGNOnGameOver)
        };
        Option loadOptions[4] = {
            Option("Name:", Option::text, 0, 8, 0),
            Option("Load Save", loadedMessage, failureMessage, Option::button, Option::LoadSave),
            Option("Load Statistics for White", loadedMessage, failureMessage, Option::button, Option::LoadWhiteStatistics),
            Option("Load Statistics for Black", loadedMessage, failureMessage, Option::button, Option::LoadBlackStatistics)
        };
        Option pgnOptions[2] = {
            Option("Name:", Option::text, 0, 8, 0),
            Option("View", nullptr, failureMessage, Option::button, Option::ViewPGN)
        };
        Option statisticsOptions[2] = {
            Option("White Statistics", nullptr, nullptr, Option::button, Option::ViewWhiteStatistics),
            Option("Black Statistics", nullptr, nullptr, Option::button, Option::ViewBlackStatistics)
        };
        Option deleteOptions[3] = {
            Option("Name:", Option::text, 0, 8, 0),
            Option("Delete Save", deletedMessage, failureMessage, Option::button, Option::DeleteSave),
            Option("Delete Default Save", deletedMessage, failureMessage, Option::button, Option::DeleteDefaultSave)
        };
        const char* resetBoardCycle[3] = { "Enter with Game Over", "Enter", "disabled" };
        Option resetOptions[7] = {
            Option("Reset Board", "Board Reset", nullptr, Option::button, Option::ResetBoard),
            Option("Reset Settings", "Settings Reset", nullptr, Option::button, Option::ResetSettings),
            Option("Reset to Default", "Defaults Restored", nullptr, Option::button, Option::ResetToDefault),
            Option("Reset White Statistics", "Statistics Reset", nullptr, Option::button, Option::ResetWhiteStatistics),
            Option("Reset Black Statistics", "Statistics Reset", nullptr, Option::button, Option::ResetBlackStatistics),
            Option("Reset All", "All Reset", nullptr, Option::button, Option::ResetAll),
            Option("Reset Board on:", nullptr, nullptr, Option::cycle, resetBoardOn, resetBoardCycle, 3, Option::UpdateResetBoardOn)
        };
        Option memoryOptions[6] = {
            Option("Save", Option::menu, saveOptions, 7),
            Option("Load", Option::menu, loadOptions, 4),
            Option("PGN", Option::menu, pgnOptions, 2),
            Option("Statistics", Option::menu, statisticsOptions, 2),
            Option("Delete", Option::menu, deleteOptions, 3),
            Option("Reset", Option::menu, resetOptions, 7)
        };
        Option aboutOptions[3] = {
            Option("Created by Warren James", nullptr, nullptr, Option::button, 0),
            Option("Published 2025-06-01", nullptr, nullptr, Option::button, 0),
            Option("Version 1.0.0", nullptr, nullptr, Option::button, 0)
        };
        Option mainMenu[5] = {
            Option("Play", nullptr, nullptr, Option::button, Option::GameLoop),
            Option("Settings", Option::menu, settingOptions, 4),
            Option("Memory", Option::menu, memoryOptions, 6),
            Option("Help", nullptr, failureMessage, Option::button, Option::ViewHelp),
            Option("About", Option::menu, aboutOptions, 3)
        };

        Bot bot { this };
        bool thinking = false;

        char* CheckGameState ();

        void MakeMove (const Move& move, const bool ignoreTime = false);
        void UnMakeMove (const bool temporary = false, const bool ignoreTime = false);
        inline void RedoMove ();
        inline void MakeNullMove () {
            ply++;
            Piece::SetToOppositeSide(sideToMove);
        };
        inline void UnMakeNullMove () {
            ply--;
            Piece::SetToOppositeSide(sideToMove);
        };
        inline bool IsNextMoveNull ();

        void UpdateInCheck ();
        bool UpdateInsufficentMaterial ();
        bool DoesBlackHaveInsufficentMaterial ();
        bool DoesWhiteHaveInsufficentMaterial ();
        void UpdateFiftyMoveCount ();
        void UpdateThreefoldRepetition ();
        void UpdateWhitesCastleRights ();
        void UpdateBlacksCastleRights ();

        uint8_t GenerateLegalMoves (Move* moves, uint8_t& available, const bool onlyCaptures = false);
        void GeneratePseudoLegalMoves (Move* moves, uint8_t& offset, const bool onlyCaptures = false);
        void GeneratePawnMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        void GenerateKnightMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        void GenerateBishopMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        void GenerateRookMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        void GenerateQueenMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        void GenerateKingMoves (Move* moves, uint8_t& offset, const uint8_t square, const bool onlyCaptures = false);
        int8_t AddMoveToSquare (Move* moves, uint8_t& offset, const uint8_t square, int8_t file, int8_t rank, const bool onlyCaptures = false);

        uint8_t Menu (Option* currentOptions, const uint8_t numberOfOptions);
        inline uint8_t OptionCallBack (Option* option);
        inline bool IsMoveKey (const uint8_t key, const uint8_t comparisonKey);
        inline bool IsBoardUpright ();
        inline uint8_t AdjustedLinearPromotionValue (uint8_t value);
        inline uint8_t DefaultLinearPromotionValue (uint8_t value);

        uint8_t GameLoop ();

        void DisplayGame ();
        //DEBUG void PrintGame ();
        inline gfx_sprite_t* GetSprite (piece pieceToReference);
        inline void DisplayTimeRemaining (const bool blitText);

        void DisplayOptions (const Option* options, const uint8_t numberOfOptions, const uint8_t selectedOption);

        //DEBUG inline void DisplayBoardInformation ();

        void PlacePieceOnSquare (const piece pieceToPlace, const uint8_t square);

        hash GenerateZobristHash ();

        void UpdateTimeRemaining ();
        inline void UpdateTimeRemainingForMove ();

        void ImportFen (const char* fen);
        void ExportFen (char* fen);

        inline void SetAlgebraicNotations (Move* moves, const uint8_t available);
        inline void SetCheckmateAlgebraicNotation ();

        uint16_t GeneratePGN (char* pgn);
        inline void AddTagToPGN (char* pgn, uint16_t& appendIndex, const char* name, const char* value);
        inline void AddTextToPGN (char* pgn, uint16_t& appendIndex, const char* text, const int16_t length = -1);
        inline void AddNumberToPGN (char* pgn, uint16_t& appendIndex, uint16_t value, const int8_t minimumCharacters = 0);
        inline void AddDateToPGN (char* pgn, uint16_t& appendIndex);

        inline void NameToString (int64_t name, char* nameToString);

        void SaveBoard (BoardDefinitions& newBoard, const bool onlyStatistics = false);
        void LoadBoard (BoardDefinitions& newBoard);
        uint8_t SaveGame (char* saveName, const bool overwrite);
        inline uint8_t SaveGame ();
        uint8_t LoadSave (char* saveName, const bool overwrite);
        inline uint8_t LoadSave ();
        uint8_t DeleteSave (char* saveName, const bool overwrite);
        inline uint8_t DeleteSave ();
        uint8_t ExportPGN (char* saveName, const bool overwrite);
        inline uint8_t ExportPGN ();
        inline uint8_t ExportDefaultPGN ();
        uint8_t SaveStatistics (Statistics& stats, char* saveName, const bool overwrite);
        inline uint8_t SaveStatistics (Statistics& stats);
        uint8_t LoadStatistics (Statistics& stats, const bool forWhite, char* saveName, const bool overwrite);
        inline uint8_t LoadStatistics (Statistics& stats, const bool forWhite);

        uint8_t ViewText (const char* title, char* saveName, const bool overwrite);
        inline uint8_t ViewText (const char* title);
        uint8_t DisplayText (const char* title, char* text, const uint16_t textSize, const uint8_t offset, const uint8_t maxLines = 0, const bool getFullLineCount = false);

        inline void AddMoveToStatistics ();
        uint8_t ViewStatistics (Statistics& stats);
        void DisplayStatistics (Statistics& stats, const uint8_t numberOfStats, const uint8_t offset);

        void RestoreSettings ();

        Board () {
            LoadSave(defaultSaveName, false);
        }
        
        Board (const char* fen) {
            if (LoadSave(defaultSaveName, false) == FILE_FAILURE) ImportFen(fen);
        }
    };
}