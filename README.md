# Chess84
The Ultimate Calculator Chess Program.

![Home](https://thewarrenjames.com/projects/chess84/media/home.png)
![Board](https://thewarrenjames.com/projects/chess84/media/board.png)
![Checkmate](https://thewarrenjames.com/projects/chess84/media/checkmate.png)
![Games](https://thewarrenjames.com/projects/chess84/media/games.gif)

### Key Features
- Bots
- Timed Games
- Board Customization
- PGN Exporting

Programmed in C++ for the TI-84 Plus CE Calculator using the [CE C/C++ Toolchain](https://ce-programming.github.io/toolchain).

## Download

You can download Chess84 at [github.com/thewarrenjames/Chess84](https://github.com/thewarrenjames/Chess84) and [thewarrenjames.com/projects/chess84/Chess84.zip](https://thewarrenjames.com/projects/Chess84/Chess84.zip).

## Installation

### 1. Cesium

Your specific calculator might not require this to run programs, but if it does, download the latest release at [github.com/mateoconlechuga/cesium](https://github.com/mateoconlechuga/cesium).

### 2. Dependencies

I wasn't able to get this program working without downloading the CE libraries individually, so I would recommend downloading the `clibs_separately_in_zip.zip` at [github.com/CE-Programming/libraries/releases/tag/v12.1](https://github.com/CE-Programming/libraries/releases/tag/v12.1).

### 3. Uploading to the Calculator

Use [Ti Connect CE](https://education.ti.com/en/products/computer-software/ti-connect-ce-sw) to upload the latest release to your calculator.  

You should now be able to play chess on your calculator!

## Features

### Rules
The chess engine has additional support for the following:
- Castling
- En Passant
- Promotion
- Under Promotion
- Insufficent Material
- Threefold Repetition
- Fifty-Move Rule

### Bot
The chess bot uses these methods to play its best.

#### Search
- Negamax
- Quiescent Search
- Iterative Deepening
- Aspiration Windows
- Alpha-beta Pruning
- Null Move Pruning
- Futility Pruning
- Principal Variation
- Move Ordering
- Search Extensions
- Late Move Reductions
- Openings Book

#### Evaluation
- Piece-Square Tables
- King Safety
- Open and Semi-open Files
- Bishop Pair
- Passed, Protected, Isolated, and Doubled Pawns
- Trade While Winning
- King Moves
- Forward Moves
- Random Variation

#### Openings
The bot can play the:
- London
- Vienna Gambit
- Ponziani
- Anti-London
- Caro Kann
- Scandinavian Defense

If you want to see the PGNs the bot was trained on, see `/media/openings/`.  
Note, you cannot play timed games with bots.

## Menu

The following is the entire menu tree along with the corresponding options.  

1. **Play**
2. **Settings**:
	1. **Always Promote**: Queen, Rook, Bishop, Knight, disabled
	2. **Players**:
		1. **White**: WHITE, BOT
		2. **Black**: BLACK, BOT
	3. **Timing**:
		1. **Timed Game**: disabled, enabled
		2. **Low Time Warning**: disabled, enabled
		3. **White Starting Time**: 0:00:00 (0:00:00, 2:00:30, 0:00:30)
		4. **Black Starting Time**: 0:00:00 (0:00:00, 2:00:30, 0:00:30)
		5. **Bonus Time**: 00 (0:00, 1:00, 0:01)
	4. **Board**:
		1. **Board View**: Follow Player, Dynamic, Only White, Only Black
		2. **Tile Palette**: Coconut, Potato, Beetroot, Elderberry, Tomato, Kiwi, Kale, Blueberry, Grape
		3. **Piece Style**: Neo, Classic
        4. **Background**: Matching, Light, Dark
		5. **Check Highlighting**: enabled, disabled
		6. **Last Move Highlighting**: enabled, disabled
		7. **Hide Legal Move Dots**: enabled, disabled
3. **Memory**:
	1. **Save**:
		1. **Name**: (TEXT)
		2. **Save Game**
		3. **Save as Default**
		4. **Save PGN**
		5. **Save White Statistics**
		6. **Save Black Statistics**
		7. **Save PGN on Game Over**: disabled, enabled
	2. **Load**:
		1. **Name**: (TEXT)
		2. **Load Save**
		3. **Load Statistics for White**
		4. **Load Statistics for Black**
	3. **PGN**:
		1. **Name**: (TEXT)
		2. **View**
	4. **Statistics**:
		1. **White Statistics**
		2. **Black Statistics**
	5. **Delete**:
		1. **Name**: (TEXT)
		2. **Delete Save**
		3. **Delete Default Save**
	6. **Reset**:
		1. **Reset Board**
		2. **Reset Settings**
		3. **Reset to Default**
		4. **Reset White Statistics**
		5. **Reset Black Statistics**
		6. **Reset All**
		7. **Reset Board on**: Enter with Game Over, Enter, disabled
4. **Help**
5. **About**:
	1. **Created by Warren James**
	2. **Published 2025-06-01**
	3. **Version 1.0.0**

### Pro Tip

The fastest way to go to any option is to type the path to get there.  
For example, to Reset All from anywhere, type `0366`. If already on the home page, type just `366`.  
Here's a short list of useful shortcuts:
- `314`: Save current game as PGN
- `361`: Reset Board
- `366`: Reset All

### Tile Palettes

#### Coconut
![Coconut](https://thewarrenjames.com/projects/chess84/media/coconut.png)
#### Potato
![Potato](https://thewarrenjames.com/projects/chess84/media/potato.png)
#### Beetroot
![Beetroot](https://thewarrenjames.com/projects/chess84/media/beetroot.png)
#### Elderberry
![Elderberry](https://thewarrenjames.com/projects/chess84/media/elderberry.png)
#### Tomato
![Tomato](https://thewarrenjames.com/projects/chess84/media/tomato.png)
#### Kiwi
![Kiwi](https://thewarrenjames.com/projects/chess84/media/kiwi.png)
#### Kale
![Kale](https://thewarrenjames.com/projects/chess84/media/kale.png)
#### Blueberry
![Blueberry](https://thewarrenjames.com/projects/chess84/media/blueberry.png)
#### Grape
![Grape](https://thewarrenjames.com/projects/chess84/media/grape.png)

### Piece Styles

#### Neo
![Neo](https://thewarrenjames.com/projects/chess84/media/neo.png)
#### Classic
![Classic](https://thewarrenjames.com/projects/chess84/media/classic.png)

Images for every combination of tile palette and piece style are available under `/media/boards/`.

### Save PGN on Game Over

If this option is enabled, on game overs, the finished game will be saved as a PGN named `CHESSZZZ`. If `CHESSZZZ` is already taken, it will save as `CHESSZZY`, then `CHESSZZX`, and so on.

## Help Page

This is the help page found on the program's home screen.

```
MENU MOVEMENT
--------------
2ND or ENTER : select option
UP : up 1
DOWN : down 1
NUMBER : select nth option
LEFT or MINUS : cycle left or dec number
RIGHT or PLUS : cycle right or inc number
DECIMAL or DEL : go back
DEL : delete text
CLEAR or ZERO : exit
ZERO : home
NEGATE : play

CHESS
------
UP : up 1 square
DOWN : down 1 square
LEFT : left 1 square
RIGHT : right 1 square
2ND or ENTER : select current square
Y= : rewind to start
WINDOW : rewind 1 move
ZOOM : pause
TRACE : fast-forward 1 move
GRAPH : fast-forward to end

Moving a chess piece
1. Select square of piece to move
2. Select square you want to go

If always promote is disabled
DIVIDE + SELECT : promote to queen
MULTIPLY + SELECT : promote to rook
MINUS + SELECT : promote to bishop
PLUS + SELECT : promote to knight
```

Note, you need to upload `CHESHELP.8xv` to your calculator to read the help page.

### Promotions when Always Promote Is Disabled
- Queen: Press `Divide` then `2nd`
- Rook: Press `Multiply` then `2nd`
- Bishop: Press `Minus` then `2nd`
- Knight: Press `Plus` then `2nd`

### Pausing and Unpausing a Timed Game
Play > `Zoom`

### Exporting a Game
`031` > Type Name for Export > Save PGN > Export PGN File Using [Ti Connect CE](https://education.ti.com/en/products/computer-software/ti-connect-ce-sw)

## FAQ

### Is this project current being maintained?
No, but feel free to contribute your own code to the project.  
After over a year and a half in development, I've moved onto other projects.

### How can I see the files on my calculator?
Use either [Ti Connect CE](https://education.ti.com/en/products/computer-software/ti-connect-ce-sw) or on your calculator type `2nd` `+` `2` `1`.

### Does the chess bot have a name?
BERNARD!

### What rating would you give the bot?
Coming from [Chess.com](https://www.chess.com/), it feels like half the time Bernard plays like an 800, and the other half, he plays like a 1200.

### What is the game in the background of the home page?
I'm glad you asked! It's [Deep Blue versus Kasparov, 1996, Game 1](https://en.wikipedia.org/wiki/Deep_Blue_versus_Kasparov,_1996,_Game_1).

## License

[ISC License](https://choosealicense.com/licenses/isc/)

## Project Created By

### **Warren James**  
**🌎 View my work at [thewarrenjames.com](https://thewarrenjames.com)** and **[github.com/thewarrenjames](https://github.com/thewarrenjames)**  
**📫 Contact me at [thewarrenjames@gmail.com](mailto:thewarrenjames@gmail.com)** and **[linkedin.com/in/thewarrenjames](https://www.linkedin.com/in/thewarrenjames)**  
**❤️ Support me at [buymeacoffee.com/thewarrenjames](https://buymeacoffee.com/thewarrenjames)**