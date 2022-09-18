#include <iostream>
#include <deque>
#include <string>
#include <vector>

const char cAssassin = 'A';
const char cFree = '.';
const char cGuardLeft = '<';
const char cGuardRight = '>';
const char cGuardUp = '^';
const char cGuardDown = 'v';
const char cOccupied = 'X';
const char cVisited = '+';

struct Point
{
	size_t x;
	size_t y;

	Point(size_t x, size_t y): x(x), y(y) {
	}

	bool operator == (const Point &p) const {
		return p.x == x && p.y == y;
	}
};

class MazeWrapper
{
public:
	MazeWrapper(std::vector<std::string> &maze): maze(maze) {
	}

	void print() const {
		for (size_t i = 0; i < maze.size(); i++) {
			std::cout << maze[i] << std::endl;
		}
		std::cout << std::endl;
	}

	void set(const Point &p, char c) {
		maze[p.y][p.x] = c;
	}

	char get(const Point &p) const {
		return maze[p.y][p.x];
	}

	size_t max_row() const {
		return maze.size();
	}

	size_t max_col() const {
		return maze.size() > 0 ? maze[0].size() : 0;
	}

	bool is_empty() const {
		return max_row() == 0 || max_col() == 0;
	}

	Point find(char c) const {
		for (size_t i = 0; i < max_row(); i++) {
			for (size_t j = 0; j < max_col(); j++) {
				Point p(j, i);
				if (get(p) == c)
					return p;
			}
		}
		return Point(max_col(), max_row()); // not found
	}

private:
	std::vector<std::string> &maze;
};

void prepare_maze(MazeWrapper &maze)
{
	for (size_t i = 0; i < maze.max_row(); i++) {
		for (size_t j = 0; j < maze.max_col(); j++) {
			Point p(j, i);
			switch(maze.get(p)) {
				case cGuardLeft:
					do {
						maze.set(p, cOccupied);
					} while(p.x-- > 0 && maze.get(p) == cFree);
					break;
				case cGuardRight:
					do {
						maze.set(p, cOccupied);
					} while(++p.x < maze.max_col() && maze.get(p) == cFree);
					break;
				case cGuardUp:
					do {
						maze.set(p, cOccupied);
					} while(p.y-- > 0 && maze.get(p) == cFree);
					break;
				case cGuardDown:
					do {
						maze.set(p, cOccupied);
					} while(++p.y < maze.max_row() && maze.get(p) == cFree);
					break;
			}
		}
	}
}

std::vector<Point> get_free_points_around(MazeWrapper &maze, const Point &p)
{
	std::vector<Point> result;

	if (p.x + 1 < maze.max_col()) {
		Point right(p.x + 1, p.y);
		if (maze.get(right) == cFree) {
			maze.set(right, cVisited);
			result.push_back(right);
		}
	}
	if (p.y + 1 < maze.max_row()) {
		Point down(p.x, p.y + 1);
		if (maze.get(down) == cFree) {
			maze.set(down, cVisited);
			result.push_back(down);
		}
	}
	if (p.x > 0) {
		Point left(p.x - 1, p.y);
		if (maze.get(left) == cFree) {
			maze.set(left, cVisited);
			result.push_back(left);
		}
	}
	if (p.y > 0) {
		Point up(p.x, p.y - 1);
		if (maze.get(up) == cFree) {
			maze.set(up, cVisited);
			result.push_back(up);
		}
	}

	return result;
}

bool is_path_exists(MazeWrapper &maze, const Point &begin, const Point &end)
{
	Point p(begin.x, begin.y);
	std::deque<Point> q;

	for (;;) {
		if (p == end)
			return true; // found

		std::vector<Point> points = get_free_points_around(maze, p);
		q.insert(q.begin(), points.begin(), points.end());

		if (q.empty())
			break; // not found

		p = q.front();
		q.pop_front();
	}

	return false;
}

int main()
{
	std::vector<std::string> maze_v;
	maze_v.push_back(".....");
	maze_v.push_back("...v.");
	maze_v.push_back("A....");
	maze_v.push_back(".^...");
	maze_v.push_back(".....");

	MazeWrapper maze(maze_v);
	maze.print();

	try {
		if (maze.is_empty())
			throw "Maze is empty";

		prepare_maze(maze);
		maze.print();

		Point begin = maze.find(cAssassin);
		if (begin.x == maze.max_col())
			throw "Start point not found";

		Point end = Point(maze.max_col() - 1, maze.max_row() - 1);
		if (maze.get(end) != cFree)
			throw "Destination point is occupied";

		if (!is_path_exists(maze, begin, end))
			throw "Path not found";
	}
	catch(const char *ex) {
		std::cout << "Error: " << ex << std::endl;
		return -1;
	}

	std::cout << "Path found!" << std::endl;
	maze.print();
	return 0;
}
