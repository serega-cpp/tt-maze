/* Having the text defined 2D maze, like:
	......
	.>..<.
	A.....
	...v..
	.^..<.
	......

   We need find a path from 'A' to the last cell.
   '.' - is a free cell, '<', '>', '^', 'v' - are
   "guards" which extends to the particular
   direction thougth the free cells until end of
   maze or occupied cell will be reached, so the
   sample will become:
    ......
	.XXXX.
	AX....
	.X.X..
	.XXXX.
	...X..

   where 'X' - occupied cell.

   Question: Is path exists?
*/
#include <iostream>
#include <deque>
#include <string>
#include <vector>

// Original set
const char cAssassin   = 'A';
const char cGuardLeft  = '<';
const char cGuardRight = '>';
const char cGuardUp    = '^';
const char cGuardDown  = 'v';
const char cFree       = '.';

// Internal set
const char cOccupied = 'x';

struct Cell {
	size_t row;
	size_t col;
	Cell(size_t row, size_t col): row(row), col(col) {}
	bool operator == (const Cell &o) { return row == o.row && col == o.col; }
};

class MazeType: public std::vector<std::string>
{
public:
	MazeType() {
	}

	MazeType(size_t rows, size_t cols, char c) {
		resize(rows);
		for (size_t i = 0; i < rows; i++) {
			at(i).resize(cols, c);
		}
	}

	void set(size_t row, size_t col, char c) {
		at(row)[col] = c;
	}

	char get(size_t row, size_t col) const {
		return at(row)[col];
	}

	bool compare_and_set(size_t row, size_t col, char c, char s) {
		if (get(row, col) == c) {
			set(row, col, s);
			return true;
		}
		return false;
	}

	size_t rows() const {
		return size();
	}

	size_t cols() const {
		return size() > 0 ? at(0).size() : 0;
	}

	bool empty() const {
		return rows() == 0;
	}

	void print() const {
		for (size_t i = 0; i < size(); i++) {
			std::cout << at(i) << std::endl;
		}
		std::cout << std::endl;
	}

	bool operator == (const MazeType &other) const {
		if (size() != other.size())
			return false;

		for (size_t i = 0; i < size(); i++) {
			if (at(i) != other[i])
				return false;
		}

		return true;
	}

	bool operator != (const MazeType &other) const {
		return !(*this == other);
	}
};

class Maze
{
public:
	Maze(const MazeType &maze)
		: _maze(maze.rows(), maze.cols(), cFree)
		, _beg(0, 0)
		, _end(0, 0)
	{
		if (maze.rows() > 0 && maze.cols() > 0) {
			_end.row = maze.rows() - 1;
			_end.col = maze.cols() - 1;
		}

		for (size_t r = 0; r < maze.rows(); r++) {
			for (size_t c = 0; c < maze.cols(); c++) {
				size_t beg, end;
				switch(maze.get(r, c)) {
					case cGuardLeft:
						beg = 0, end = c + 1;
						if (c > 0) {
							for (size_t i = c - 1; i > 0; i--) {
								if (maze.get(r, i) != cFree) {
									beg = i + 1;
									break;
								}
							}
						}
						for (size_t i = beg; i < end; i++) {
							_maze.set(r, i, cOccupied);
						}
						break;
					case cGuardRight:
						beg = c, end = maze.cols();
						for (size_t i = c + 1; i < maze.cols(); i++) {
							if (maze.get(r, i) != cFree) {
								end = i;
								break;
							}
						}
						for (size_t i = beg; i < end; i++) {
							_maze.set(r, i, cOccupied);
						}
						break;
					case cGuardUp:
						beg = 0, end = r + 1;
						if (r > 0) {
							for (size_t j = r - 1; j > 0; j--) {
								if (maze.get(j, c) != cFree) {
									beg = j + 1;
									break;
								}
							}
						}
						for (size_t j = beg; j < end; j++) {
							_maze.set(j, c, cOccupied);
						}
						break;
					case cGuardDown:
						beg = r, end = maze.rows();
						for (size_t j = r + 1; j < maze.rows(); j++) {
							if (maze.get(j, c) != cFree) {
								end = j;
								break;
							}
						}
						for (size_t j = beg; j < end; j++) {
							_maze.set(j, c, cOccupied);
						}
						break;
					case cAssassin:
						std::cout << "Start at row " << r << " col " << c << std::endl;
						_beg.row = r;
						_beg.col = c;
						break;
				}
			}
		}
	}

	std::vector<Cell> get_free_cells_around(const Cell &cell, char visited) {
		std::vector<Cell> cells;
		if (cell.row + 1 < _maze.rows()) { // down
			Cell next(cell.row + 1, cell.col);
			if (_maze.compare_and_set(next.row, next.col, cFree, visited)) {
				cells.push_back(next);
			}
		}
		if (cell.col + 1 < _maze.cols()) { // right
			Cell next(cell.row, cell.col + 1);
			if (_maze.compare_and_set(next.row, next.col, cFree, visited)) {
				cells.push_back(next);
			}
		}
		if (cell.row > 0) { // up
			Cell next(cell.row - 1, cell.col);
			if (_maze.compare_and_set(next.row, next.col, cFree, visited)) {
				cells.push_back(next);
			}
		}
		if (cell.col > 0) { // left
			Cell next(cell.row, cell.col - 1);
			if (_maze.compare_and_set(next.row, next.col, cFree, visited)) {
				cells.push_back(next);
			}
		}
		return cells;
	}

	bool is_path_exists() {
		Cell cell(_beg.row, _beg.col);
		_maze.set(cell.row, cell.col, 'A');
		std::deque<Cell> cells;

		for (;;) {
			if (cell == _end)
				return true; // found

			char visited = _maze.get(cell.row, cell.col) + 1;
			std::vector<Cell> next = get_free_cells_around(cell, visited);
			cells.insert(cells.begin(), next.begin(), next.end());

			if (cells.empty())
				break; // not found

			cell = cells.front();
			cells.pop_front();
		}

		return false;
	}

	const MazeType & get_maze() const {
		return _maze;
	}

private:
	MazeType _maze;
	Cell     _beg;
	Cell     _end;
};

MazeType get_maze() {
	MazeType maze;
	maze.push_back(".^..^");
	maze.push_back("...v.");
	maze.push_back("Av...");
	maze.push_back(".^...");
	maze.push_back("<....");
	maze.push_back("<..^.");
	maze.push_back("<....");
	maze.push_back("<.vv.");
	return maze;
}

MazeType get_maze_prepared() {
	MazeType maze;
	maze.push_back(".x..x");
	maze.push_back("...x.");
	maze.push_back(".x.x.");
	maze.push_back(".x.x.");
	maze.push_back("x..x.");
	maze.push_back("x..x.");
	maze.push_back("x....");
	maze.push_back("x.xx.");
	return maze;
}

int main()
{
	MazeType maze_ = get_maze();
	Maze maze(maze_);
	maze_.print();

	MazeType maze_ok = get_maze_prepared();
	if (maze_ok != maze.get_maze()) {
		std::cout << "Validation failed! Actual:" << std::endl;
		maze.get_maze().print();
		std::cout << "Expected:" << std::endl;
		maze_ok.print();
		return -1;
	}

	if (maze.is_path_exists()) {
		std::cout << "Path found!" << std::endl;
		maze.get_maze().print();
	}
	else
		std::cout << "Path not found." << std::endl;

	return 0;
}
