// Declarations and functions for project #4
#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>
using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1; // Indicates that a cell is blank
const int SquareSize = 3; // The number of cells in a small square
// (usually 3). The board has
// SquareSize^2 rows and SquareSize^2
// columns.

const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;
int numSolutions = 0;

class board
	// Stores the entire Sudoku board
{
public:
	board(int);
	void clear();
	void initialize(ifstream& fin);
	void print();
	bool isBlank(int, int);
	ValueType getCell(int, int);
	void setCell(ValueType, int, int);
	void clearCell(int, int);
    void updateConflicts();
    void printConflicts();
    bool isSolved();
	bool getConflicts();

private:
	// The following matrices go from 1 to BoardSize in each
	// dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)
	matrix<ValueType> value;
	matrix<vector<bool>> conflicts;
};

board::board(int sqSize)
	: value(BoardSize + 1, BoardSize + 1)
	// Board constructor
{
	clear();
	conflicts.resize(9, 9);
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			conflicts[i][j].resize(9);
			for (int k = 0; k < 9; k++) {
				conflicts[i][j][k] = false;
			}
		}
	}
}

void board::clear()
// Mark all possible values as legal for each board entry
{
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			value[i][j] = Blank;
		}
}

bool board::getConflicts()
//Gets the conflicts that are on the board and updates the board
{
	bool tor = false;
	// First reset all conflicts
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 9; k++) {
                conflicts[i][j][k] = false;
            }
        }
    }
	
	for (int row = 1; row <= 9; row++) { //Row of cell being examined
		for (int col = 1; col <= 9; col++) { //Col of cell being examined
				for (int i = 1; i <= 9; i++) { //Checks the value of each square in row for conflict
						if (i != col && !isBlank(row, i)) {
							conflicts[row - 1][col - 1][getCell(row, i)-1] = true;
							tor = true;
						}
					}
				
				for (int i = 1; i <= 9; i++) { //Checks the value of each square in col for conflict
					if (i != row && !isBlank(i, col)) {
						conflicts[row - 1][col - 1][getCell(i, col) - 1] = true;
						tor = true;
					}
				}
				//TO-DO: CHECK FOR CONFLICTS IN SQUARE
				int startRow = ((row - 1) / 3) * 3 + 1;
           		int startCol = ((col - 1) / 3) * 3 + 1;
            
            	for (int r = startRow; r < startRow + 3; r++) {
                	for (int c = startCol; c < startCol + 3; c++) {
                    	if ((r != row || c != col) && !isBlank(r, c)) {
                        conflicts[row - 1][col - 1][getCell(r, c)-1] = true;
						tor = true;
						}
					}
				}
			}
		}
		return tor;
}

void board::initialize(ifstream& fin)
// Read a Sudoku board from the input file
{
	char ch;
	clear();
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			fin >> ch;
			// If the read char is not Blank
			if (ch != '.')
				setCell(i, j, ch - '0'); // Convert char to int
		}
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom. Note that i and j each go from 1 to BoardSize
{
	// Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
	// coordinates of the square that i,j is in
	return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

ostream& operator<<(ostream& ostr, vector<int>& v)
// Overloaded output operator for vector class
{
	for (int i = 0; i < v.size(); i++)
		ostr << v[i] << " ";

	ostr << endl;
	return ostr;
}


ValueType board::getCell(int i, int j)
// Returns the value stored in a cell. Throws an exception
// if bad values are passed
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
		return value[i][j];
	else
		throw rangeError("bad value in getCell");
}

void board::setCell(ValueType val, int i, int j)
// Sets the cell at (i,j) to val and updates conflicts
{
	if ((i >= 1) && (i <= BoardSize) &&
		(j >= 1) && (j <= BoardSize))
	{
		value[i][j] = val;
		getConflicts();
	}
	else
		throw rangeError("bad value in setCell");
}


bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise
{
	if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
		throw rangeError("bad value in setCell");
	return (getCell(i, j) == Blank);
}

// clear the cell
void board::clearCell(int i, int j)
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize) {
        value[i][j] = Blank;
        getConflicts();
    } else {
        throw rangeError("bad value in clearCell");
    }
}

// returns true if the board is completely and correctly filled
// A solved board means that ...
//   - it shoudn't have blank cells
//   - each row, column, and 3x3 square contains the digits 1-9
//     exactly once
bool board::isSolved()
{
	bool used[MaxValue + 1];

	// Checks each row 
	for (int i = 1; i <= BoardSize; i++)
	{
		for (int d = MinValue; d <= MaxValue; d++)
			used[d] = false;

		for (int j = 1; j <= BoardSize; j++)
		{
			int v = getCell(i, j);

			// cell must be non-blank and between 1 and 9
			if (v < MinValue || v > MaxValue)
			{
				cout << "Board is not solved." << endl;
				return false;
			}

			// digit must not repeat in the row
			if (used[v])
			{
				cout << "Board is not solved." << endl;
				return false;
			}

			used[v] = true;
		}
	}

	// Checks each column
	for (int j = 1; j <= BoardSize; j++)
	{
		for (int d = MinValue; d <= MaxValue; d++)
			used[d] = false;

		for (int i = 1; i <= BoardSize; i++)
		{
			int v = getCell(i, j);

			if (v < MinValue || v > MaxValue || used[v])
			{
				cout << "Board is not solved." << endl;
				return false;
			}

			used[v] = true;
		}
	}

	// Checks each 3x3 square
	for (int row = 1; row <= BoardSize; row += SquareSize)
		for (int col = 1; col <= BoardSize; col += SquareSize)
		{
			for (int d = MinValue; d <= MaxValue; d++)
				used[d] = false;

			for (int i = 0; i < SquareSize; i++)
				for (int j = 0; j < SquareSize; j++)
				{
					int v = getCell(row + i, col + j);

					if (v < MinValue || v > MaxValue || used[v])
					{
						cout << "Board is not solved." << endl;
						return false;
					}

					used[v] = true;
				}
		}

	cout << "Board is solved." << endl;
	return true;
}

// Prints for each cell which values are still possible and 
// for a blank cell (i,j) it prints the digits k (1-9) for which
// conflicts[i-1][j-1][k-1] is false
void board::printConflicts()
{
	cout << "Conflicts / possible values for each cell:" << endl;

	for (int i = 1; i <= BoardSize; i++)
	{
		for (int j = 1; j <= BoardSize; j++)
		{
			cout << "Cell (" << i << "," << j << "): ";

			if (!isBlank(i, j))
			{
				cout << "fixed value " << getCell(i, j) << endl;
				continue;
			}

			cout << "possible values = ";
			bool any = false;

			for (int v = MinValue; v <= MaxValue; v++)
			{
				if (!conflicts[i - 1][j - 1][v - 1])
				{
					cout << v << " ";
					any = true;
				}
			}

			if (!any)
				cout << "none";

			cout << endl;
		}
	}
}

// prints the current board with aligned columns
void board::print()
{
	for (int i = 1; i <= BoardSize; i++)
	{
		// prints the horizontal separator before each 3x3 block of rows
		if ((i - 1) % SquareSize == 0)
		{
			cout << " -";
			for (int j = 1; j <= BoardSize; j++)
				cout << "---";
			cout << "-";
			cout << endl;
		}

		for (int j = 1; j <= BoardSize; j++)
		{
			// the vertical separator before each 3x3 block of columns
			if ((j - 1) % SquareSize == 0)
				cout << "|";

			if (!isBlank(i, j))
				cout << " " << getCell(i, j) << " ";
			else
				// blank cell as " . "
				cout << " . ";
		}

		cout << "|";
		cout << endl;
	}

	// Bottom border
	cout << " -";
	for (int j = 1; j <= BoardSize; j++)
		cout << "---";
	cout << "-";
	cout << endl;
}


int main()
{
	ifstream fin;
	// Read the sample grid from the file
	string fileName = "sudoku"; // IF HAVING ISSUES READING THE TXT, TRY PASTING THE FULL FILE PATH :)
	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		board b1(SquareSize); // creates the board object of the correct size
		while (fin && fin.peek() != 'Z')
		{
			b1.initialize(fin); // reads one Sudoku board from file and updates all conflict flags
			cout << "Initial Board:" << endl;
			b1.print(); // prints the 9×9 Sudoku grid to the screen
			b1.printConflicts(); // prints conflict info for each cell (which digits are allowed)
			b1.isSolved(); // checks if the board is already solved

			cout << endl;
		}
	}
	catch (indexRangeError& ex)
	{
		cout << ex.what() << endl;
		exit(1);
	}
}
