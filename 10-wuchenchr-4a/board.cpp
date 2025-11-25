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
	for (int row = 1; row <= 9; row++) { //Row of cell being examined
		for (int col = 1; col <= 9; col++) { //Col of cell being examined
			for (int i = 1; i <= 9; i++) { //Checks the value of each square in row for conflict
				if (i != col) {
					conflicts[row - 1][col - 1][getCell(row, i)-1] = true;
				}
			}
			for (int i = 1; i <= 9; i++) { //Checks the value of each square in col for conflict
				if (i != row) {
					conflicts[row - 1][col - 1][getCell(i, col) - 1] = true;
				}
			}
			//TO-DO: CHECK FOR CONFLICTS IN SQUARE
		}
	}
}

void board::initialize(ifstream& fin)
// Read a Sudoku board from the input file.
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
	// coordinates of the square that i,j is in.
	return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

ostream& operator<<(ostream& ostr, vector<int>& v)
// Overloaded output operator for vector class.
{
	for (int i = 0; i < v.size(); i++)
		ostr << v[i] << " ";
	cout << endl;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell. Throws an exception
// if bad values are passed.
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
		return value[i][j];
	else
		throw rangeError("bad value in getCell");
}

void board::setCell(ValueType val, int i, int j)
// Sets the cell at (i,j) to 
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
		value[i][j] = val;
		getConflicts();
	else
		throw rangeError("bad value in getCell");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
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

void board::print()
// Prints the current board.
{
	for (int i = 1; i <= BoardSize; i++)
	{
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
			if ((j - 1) % SquareSize == 0)
				cout << "|";
			if (!isBlank(i, j))
				cout << " " << getCell(i, j) << " ";
			else
				cout << " ";
		}
		cout << "|";
		cout << endl;
	}
	cout << " -";
	for (int j = 1; j <= BoardSize; j++)
		cout << "---";
	cout << "-";
	cout << endl;
}

int main()
{
	ifstream fin;
	// Read the sample grid from the file.
	string fileName = "sudoku.txt";
	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}
	try
	{
		board b1(SquareSize);
		while (fin && fin.peek() != 'Z')
		{
			b1.initialize(fin);
			cout << "Initial Board:" << endl;
			b1.print();
			b1.printConflicts();
		}
	}
	catch (indexRangeError& ex)
	{
		cout << ex.what() << endl;
		exit(1);
	}
}
