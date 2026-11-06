/* EECS 348 Assignment 4
 *  This program is a sodoku solver meant to brute-force solutions for the timeless
 *  puzzle type. 
 *  Collaborators: Luke Tidball, ChatGPT 6
 *  Revised by: Soren Thumann, Claude.ai
 *  Revision Date: October 29th, 2026
 */

#include <iostream> // Provides console input and output.
#include <fstream> // Provides file input and output.
#include <string> // Provides the string type and to_string function.
#include <ostream> // [Thumann]: Header for ostream so output can be directed to a file.

using namespace std; // Allows standard library names to be used without std::.

class Sudoku { // Defines a class for loading, displaying, and solving Sudoku puzzles.
private: // Begins the section accessible only inside the class.
    static const int SIZE = 9; // Sets the number of rows and columns in the grid.
    int grid[SIZE][SIZE]; // Stores the Sudoku cells, with zero representing an empty cell.

    // Checks whether a number can be placed in a cell.
    bool isValid(int row, int col, int num) const { // Returns whether num can go at the specified row and column.
        // Check the row.
        for (int c = 0; c < SIZE; c++) { // Examines every column in the specified row.
            if (grid[row][c] == num) { // Checks whether the number already appears in this row.
                return false; // Rejects the number because it would duplicate a row value.
            } // Ends the row-conflict check.
        } // Ends the loop that checks the row.

        // Check the column.
        for (int r = 0; r < SIZE; r++) { // Examines every row in the specified column.
            if (grid[r][col] == num) { // Checks whether the number already appears in this column.
                return false; // Rejects the number because it would duplicate a column value.
            } // Ends the column-conflict check.
        } // Ends the loop that checks the column.

        // Find the top-left corner of the 3x3 block.
        int blockRow = (row / 3) * 3; // Calculates the first row of the cell's 3x3 block.
        int blockCol = (col / 3) * 3; // Calculates the first column of the cell's 3x3 block.

        // Check the 3x3 block.
        for (int r = blockRow; r < blockRow + 3; r++) { // Visits each row in the cell's 3x3 block.
            for (int c = blockCol; c < blockCol + 3; c++) { // Visits each column in the current block row.
                if (grid[r][c] == num) { // Checks whether the number already appears in this block.
                    return false; // Rejects the number because it would duplicate a block value.
                } // Ends the block-conflict check.
            } // Ends the loop through the block's columns.
        } // Ends the loop through the block's rows.

        return true; // Allows the number because no row, column, or block conflict was found.
    } // Ends the isValid function.
    
    // Counts how many valid numbers can be placed in an empty cell.
    int countCandidates(int row, int col) const { // Returns the number of valid digits for the specified cell.
        int count = 0; // Stores the number of valid candidates found.

        // Try every possible Sudoku digit, from 1 through 9.
        for (int num = 1; num <= 9; num++) { // Tests each possible Sudoku digit.

            // Check whether the current digit follows Sudoku's rules.
            if (isValid(row, col, num)) { // Checks whether num can be placed in the specified cell.

                count++; // Increase the count if the digit is valid.
            } // Ends the check for the current candidate.
        } // Ends the loop through possible digits.

        return count; // Return the total number of valid candidates.
    } // Ends the countCandidates function.

    // Finds the empty cell with the fewest valid candidate numbers.
    bool findBestEmptyCell(int &bestRow, int &bestCol) const { // Finds an empty cell with the fewest candidates and saves its coordinates.
        int minCandidates = 10; // Start above the maximum of nine candidates.
        bool foundEmpty = false; // Tracks whether an empty cell exists.
        // Examine each row of the Sudoku grid.
        for (int row = 0; row < SIZE; row++) { // Visits every row in the grid.
            // Examine each column in the current row.
            for (int col = 0; col < SIZE; col++) { // Visits every column in the current row.
                // Check whether the current cell is empty.
                if (grid[row][col] == 0) { // Checks whether the value at grid[row][col] is zero.
                    foundEmpty = true; // Record that an empty cell exists.
                    // Count the valid numbers for this empty cell.
                    int candidates = countCandidates(row, col); // Counts valid digits for this empty cell.
                    // A cell with zero candidates cannot be solved.
                    if (candidates == 0) { // Checks whether this empty cell has no valid candidates.
                        bestRow = row; // Save the impossible cell's row.
                        bestCol = col; // Save the impossible cell's column.
                        return true; // Returns so the solver can detect failure.
                    } // Ends the check for a cell with no candidates.
                    // Check whether this cell has fewer candidates than before.
                    if (candidates < minCandidates) { // Checks whether this cell has fewer candidates than the current best.
                        minCandidates = candidates; // Update the smallest count.
                        bestRow = row; // Save the selected cell's row.
                        bestCol = col; // Save the selected cell's column.
                    } // Ends the check for a new best cell.
                    // Stop searching if a cell has only one candidate.
                    if (minCandidates == 1) { // Checks whether the smallest candidate count is one.
                        return true; // Returns because one is the smallest positive candidate count.
                    } // Ends the check for a single-candidate cell.
                } // Ends the check for an empty cell.
            } // Ends the loop through columns.
        } // Ends the loop through rows.

        // Return true if an empty cell was found, or false if the grid is full.
        return foundEmpty; // Reports whether an empty cell was found.
    } // Ends the findBestEmptyCell function.

// Recursively solves the puzzle using depth-first search and backtracking.
long long solveRecursive(ostream &out) { // [Thumann]: Output stream is included for solution file writing.

    int row, col; // Store the coordinates of the selected empty cell.

    // Find the empty cell with the fewest valid candidates.
    if (!findBestEmptyCell(row, col)) { // Checks whether there are no empty cells left.

        printGrid(out); // [Thumann]: Sends the output stream to printGrid member for file writing.

        return 1; // Reports that one valid solution has been found.
    } // Ends the check for a completed grid.

    // Check whether the selected cell has any valid candidates.
    if (countCandidates(row, col) == 0) { // Checks whether the selected cell has no valid digits.

        return 0; // Reports that this search path cannot produce a solution.
    } // Ends the check for a cell with no candidates.

    long long solutions = 0; // Counts the solutions found in this search.

    // Try every possible digit from 1 through 9.
    for (int num = 1; num <= 9; num++) { // Tests each possible Sudoku digit.

        // Only attempt to place a digit if it follows Sudoku's rules.
        if (isValid(row, col, num)) { // Checks whether num can be placed in the selected cell.

            grid[row][col] = num; // Places the candidate in the selected cell.

            // Recursively search for solutions with this digit in place.
            solutions += solveRecursive(out); // [Thumann]: passes output stream to recursion.

            // Remove the digit so another candidate can be tested.
            grid[row][col] = 0; // Clears the cell to try another candidate.
        } // Ends the check for the current digit.
    } // Ends the loop through possible digits.

    return solutions; // Returns the total solutions found in this search.
} // Ends the solveRecursive function.

    // Checks whether the original puzzle has conflicting clues.
    bool hasValidStartingGrid() { // Returns whether the starting clues follow Sudoku rules.
        for (int row = 0; row < SIZE; row++) { // Visits every row in the grid.
            for (int col = 0; col < SIZE; col++) { // Visits every column in the current row.
                int num = grid[row][col]; // Stores the current cell's value.

                if (num == 0) { // Checks whether the current cell is empty.
                    continue; // Skips empty cells because they are not starting clues.
                } // Ends the empty-cell check.

                // Temporarily remove the clue to check for conflicts.
                grid[row][col] = 0; // Clears the clue so isValid can check the other cells.

                bool valid = isValid(row, col, num); // Checks whether the clue conflicts with the rest of the grid.

                // Restore the original clue.
                grid[row][col] = num; // Puts the original clue back in its cell.

                if (!valid) { // Checks whether the clue conflicts with another value.
                    return false; // Reports that the starting grid is invalid.
                } // Ends the conflict check.
            } // Ends the loop through columns.
        } // Ends the loop through rows.

        return true; // Reports that no conflicting starting clues were found.
    } // Ends the hasValidStartingGrid function.

public: // Begins the section accessible to code that uses the class.
    // Initializes an empty Sudoku grid.
    Sudoku() { // Defines the constructor that initializes a Sudoku object.
        for (int row = 0; row < SIZE; row++) { // Visits every row in the grid.
            for (int col = 0; col < SIZE; col++) { // Visits every column in the current row.
                grid[row][col] = 0; // Marks the current cell as empty.
            } // Ends the loop through columns.
        } // Ends the loop through rows.
    } // Ends the Sudoku constructor.

    // Reads a Sudoku puzzle from a file.
    bool loadPuzzle(const string &filename, ostream &out) { // [Thumann]: Output stream present so error messages are written to solution files.
    // // Loads a puzzle file and reports whether it was read successfully.
        ifstream inputFile(filename); // Opens the specified file for input.

        if (!inputFile.is_open()) { // Checks whether the file failed to open.
            out << "Error: Could not open " << filename << endl; // [Thumann]: Here the message is written to the solution file instead of terminal.
            // // Displays an error message with the filename.
            return false; // Reports that the puzzle could not be loaded.
        } // Ends the file-open check.

        for (int row = 0; row < SIZE; row++) { // Visits each row of cells to read from the file.
            for (int col = 0; col < SIZE; col++) { // Visits each column of the current row.
                string cell; // Stores the next cell value read from the file.

                if (!(inputFile >> cell)) { // Checks whether a required cell could not be read.
                    out << "Error: Invalid puzzle format in " // [Thumann]: Error written to file.
                         << filename << endl; // Adds the filename and ends the error message.
                    inputFile.close(); // Closes the file after the read error.
                    return false; // Reports that the puzzle could not be loaded.
                } // Ends the check for a missing cell.

                if (cell == "_") { // Checks whether the file marks this cell as empty.
                    grid[row][col] = 0; // Stores zero to represent an empty cell.
                } else if (cell.length() == 1 && // Checks that the cell contains exactly one character.
                           cell[0] >= '1' && cell[0] <= '9') { // Checks that the character is a digit from 1 through 9.
                    grid[row][col] = cell[0] - '0'; // Converts the digit character to an integer and stores it.
                } else { // Handles any cell value that is neither an underscore nor a valid digit.
                    out << "Error: Invalid cell in " // [Thumann]: Written to file.
                         << filename << endl; // Adds the filename and ends the error message.
                    inputFile.close(); // Closes the file after the invalid cell.
                    return false; // Reports that the puzzle could not be loaded.
                } // Ends the cell-value checks.
            } // Ends the loop through columns.
        } // Ends the loop through rows.

        // Ensure the file contains exactly 81 cells.
        string extra; // Stores a possible value remaining after the expected cells are read.

        if (inputFile >> extra) { // Checks whether the file contains another value after the 81 cells.
            out << "Error: Too many cells in " // [Thumann]: Once again, this is written to the file now.
                 << filename << endl; // Adds the filename and ends the error message.
            inputFile.close(); // Closes the file after detecting extra content.
            return false; // Reports that the puzzle file has an invalid format.
        } // Ends the check for extra cells.

        inputFile.close(); // Closes the file after reading the puzzle.
        return true; // Reports that the puzzle was loaded successfully.
    } // Ends the loadPuzzle function.

    // Prints the Sudoku grid.
    // [Thumann]: Similar to other functions, the output stream is included and used to write the outputs to the solution file.
    void printGrid(ostream &out) const { 
        for (int row = 0; row < SIZE; row++) { // Visits every row in the grid.
            if (row % 3 == 0) { // Checks whether this row begins a 3-row block.
                out << "+-------+-------+-------+" << endl; // Prints a horizontal separator between blocks.
            } // Ends the block-row check.

            for (int col = 0; col < SIZE; col++) { // Visits every column in the current row.
                if (col % 3 == 0) { // Checks whether this column begins a 3-column block.
                    out << "| "; // Prints a vertical separator before the block.
                } // Ends the block-column check.

                if (grid[row][col] == 0) { // Checks whether the current cell is empty.
                    out << "_ "; // Prints an underscore for an empty cell.
                } else { // Handles a cell that contains a digit.
                    out << grid[row][col] << ' '; // Prints the digit followed by a space.
                } // Ends the empty-cell check.
            } // Ends the loop through columns.

            out << "|" << endl; // Prints the right edge of the row and moves to the next line.
        } // Ends the loop through rows.

        out << "+-------+-------+-------+" << endl; // Prints the bottom border of the grid.
    } // Ends the printGrid function.

    // Solves the puzzle and prints all solutions.
    // [Thumann]: Similar to other functions, the output stream is included and used to write the outputs to the solution file.
    void solve(ostream &out) {
    // // Checks the starting grid, searches for solutions, and reports the result.
        if (!hasValidStartingGrid()) { // Checks whether the starting clues conflict.
            out << "No solution found" << endl; // Reports that the invalid starting grid has no solution.
            return; // Stops solving this puzzle.
        } // Ends the starting-grid validation check.

        long long solutions = solveRecursive(out); // Searches for solutions and stores the number found.

        if (solutions == 0) { // Checks whether the search found no solutions.
            out << "No solution found" << endl; // Reports that no valid solution exists.
        } else { // Handles the case where one or more solutions were found.
            out << "Total solutions found: " // Begins displaying the number of solutions.
                 << solutions << endl; // Prints the solution count and ends the line.
        } // Ends the check for whether solutions were found.
    } // Ends the solve function.
}; // Ends the Sudoku class definition.

int main() { // Begins the program's main function.
    // Process all five puzzle files.
    for (int i = 1; i <= 5; i++) { // Loops through puzzle numbers 1 to 5.
        string filename = "puzzle" + to_string(i) + ".txt"; // Builds the path for the current puzzle file.
                                                            // [Thumann]: Had to remove general directory "../" as it caused errors when attempting to load files.
        // [Thumann]: This block is for testing if the input file could be found in a more thorough manner using a dummy test file.
        ifstream testFile(filename);
        if (!testFile.is_open()) { 
            cout << "Could not open " << filename << " - no solution file created." << endl;
            continue; 
        } 
        testFile.close(); 

        // [Thumann]: This block is for testing if the output file could be made without errors (not dummy file, actual output created here)
        string outName = "solution" + to_string(i) + ".txt"; 
        ofstream outFile(outName); 
        if (!outFile.is_open()) { 
            cout << "Error: Could not create " << outName << endl; 
            continue; 
        } 

        // [Thumann]: From here all cout have been replaced with the output stream to the solution file.

        outFile << "====================================" << endl; // Prints a separator at the top of the solution file.
        outFile << "Puzzle file: " << filename << endl; // Writes the input puzzle file name to the solution file.
        outFile << "====================================" << endl; // Prints a separator below the file name.

        Sudoku sudoku; // Creates a Sudoku object with an initially empty grid.

        if (sudoku.loadPuzzle(filename, outFile)) { // Loads the puzzle; load errors go to the solution file.
            outFile << "\nOriginal puzzle:" << endl; // Writes a label for the puzzle as loaded.
            sudoku.printGrid(outFile); // Writes the original puzzle grid to the solution file.

            outFile << "\nSolution(s):" << endl; // Writes a label before the solver's output.
            sudoku.solve(outFile); // Solves the puzzle, writing all solutions (or "No solution found") to the file.
        } // Ends the check for successful puzzle loading.

        outFile.close(); // [Thumann] Close solution file
        cout << "Wrote " << outName << " for " << filename << endl; // [Terminal message confirming the creation of a particular solution file.]
        
    } // Ends the loop through the five puzzle files.

    return 0; // Reports that the program completed successfully.
} // Ends the main function.
