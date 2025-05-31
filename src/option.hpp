/**
 * @file option.hpp
 * @author Warren James (thewarrenjames@gmail.com)
 * @brief Declaration of menu cycles, text inputs, and buttons
 * @version 1.0.0
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

namespace Chess {
    struct Option {
        enum : const uint8_t {
            button = 1,
            cycle,
            number,
            text,
            menu,
            timeInHoursMinutesSeconds = 1,
            timeInSeconds,
            GameLoop = 1,
            UpdateAlwaysPromoteToPiece,
            UpdateWhite,
            UpdateBlack,
            UpdateTimedGame,
            UpdateLowTimeWarning,
            UpdateWhiteStartingTime,
            UpdateBlackStartingTime,
            UpdateBonusTime,
            UpdateBoardView,
            UpdateTilePalette,
            UpdatePieceStyle,
            UpdateCheckHighlighting,
            UpdateLastMoveHighlighting,
            UpdateBackground,
            UpdateHideLegalMoveDots,
            SaveGame,
            SaveAsDefault,
            SavePGN,
            SaveWhiteStatistics,
            SaveBlackStatistics,
            UpdateSavePGNOnGameOver,
            LoadSave,
            LoadWhiteStatistics,
            LoadBlackStatistics,
            ViewPGN,
            ViewWhiteStatistics,
            ViewBlackStatistics,
            DeleteSave,
            DeleteDefaultSave,
            ResetBoard,
            ResetSettings,
            ResetToDefault,
            ResetWhiteStatistics,
            ResetBlackStatistics,
            ResetAll,
            UpdateResetBoardOn,
            ViewHelp
        };

        const char* names[3];
        uint8_t nameToUse = 0;
        uint8_t type = button;
        uint8_t format = 0;

        int64_t value = 0;

        Option* options = nullptr;
        uint8_t numberOfOptions = 0;

        const char** cycleValues = nullptr;
        uint8_t numberOfCycleValues = 0;

        int64_t lowerBound = 0, upperBound = 0, step = 0;

        uint8_t callBack = 0;

        /// @brief Menu
        Option (const char* _name, const uint8_t _type, Option* _options, const uint8_t _numberOfOptions) {
            names[0] = _name;
            type = _type;
            options = _options;
            numberOfOptions = _numberOfOptions;
        }

        /// @brief Cycle
        Option (const char* _name, const char* _secondaryName, const char* _tertiaryName, const uint8_t _type, const int64_t _value, const char** _cycleValues, const uint8_t _numberOfCycleValues, uint8_t _callBack) {
            names[0] = _name;
            names[1] = _secondaryName;
            names[2] = _tertiaryName;
            type = _type;
            value = _value;
            cycleValues = _cycleValues;
            numberOfCycleValues = _numberOfCycleValues;
            callBack = _callBack;
        }

        /// @brief Number
        Option (const char* _name, const char* _secondaryName, const char* _tertiaryName, const uint8_t _type, const uint8_t _format, const int64_t _value, const int64_t _lowerBound, const int64_t _upperBound, const int64_t _step, uint8_t _callBack) {
            names[0] = _name;
            names[1] = _secondaryName;
            names[2] = _tertiaryName;
            type = _type;
            format = _format;
            value = _value;
            lowerBound = _lowerBound;
            upperBound = _upperBound;
            step = _step;
            callBack = _callBack;
        }

        /// @brief Text
        Option (const char* _name, const uint8_t _type, const int64_t _value, const int64_t _upperBound, uint8_t _callBack) {
            names[0] = _name;
            type = _type;
            value = _value;
            upperBound = _upperBound;
            callBack = _callBack;
        }

        /// @brief Button
        Option (const char* _name, const char* _secondaryName, const char* _tertiaryName, const uint8_t _type, uint8_t _callBack) {
            names[0] = _name;
            names[1] = _secondaryName;
            names[2] = _tertiaryName;
            type = _type;
            callBack = _callBack;
        }
    };
}