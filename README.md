# Tank Game

A 2D tank strategy game implemented in C++.

## Input guidelines

The user must provide a path to the file that contains the game board, which must follow a specific format:
1. The first row in the file will be used to specify the board's intended dimensions, using the following format:
- height space width
- height and width must both be positive integers with a space character seperating them.
2. The board's construction will begin from the file's second row down. The following characters should be used:
- ' ' (Empty space) to represent an empty tile.
- '#' to represent a wall.
- '@' to represent a mine.
- '1' to represent player 1's tank - must have at least one of these on the board, but only the first one will be registered.
- '2' to represent player 2's tank - must have at least one of these on the board, but only the first one will be registered.

## Build instructions and running

In your terminal, execute the following commands:
```bash
cmake . -G "MinGW Makefiles"
cmake --build .
```

Then, in order to run the program:
```bash
./tanks_game <input_file>
```

## Program Output

- After the program finishes successfully, its results will be written in a new file called "output_<input_file>".
- If instead the program end early due to an error, such as invalid input, the exact error will be written in a new file called "board_errors.txt".