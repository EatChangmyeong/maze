#define DEBUG_MODE false

#include <stack>
#include <queue>
#include <tuple>
#include <algorithm>
#if DEBUG_MODE
#include <iostream>
#endif

#include "maze_algorithms.hpp"

using coord = std::pair<int, int>;
using it = std::vector<bool>::iterator;
template<class T> using matrix_t = std::vector<std::vector<T> >;

namespace maze {
	std::mt19937_64 random_engine;

	void randinit(uint_fast64_t seed) {
		random_engine.seed(seed);
	}
	int rand(int max) {
		std::uniform_int_distribution<> dist(0, max - 1);
		return dist(maze::random_engine);
	}
	int randbit(int bits) {
		int n = 0, t = bits, r;
		while(t) {
			n += t % 2;
			t /= 2;
		}

		if(!n)
			return 0;
		r = maze::rand(n);
		for(int i = 0;; i++) {
			if(bits & 1 << i)
				if(!r--)
					return i;
		}
	}

	structure::structure(int w, int h, bool init_value):
		width(w), height(h),
		hor(w), vert(w + 1) {
			for(std::vector<bool> &i: hor)
				i.resize(h + 1, init_value);
			for(std::vector<bool> &i: vert)
				i.resize(h, init_value);
			for(int i = 0; i < w; i++)
				hor[i][0] = hor[i][h] = true;
			for(int i = 0; i < h; i++)
				vert[0][i] = vert[w][i] = true;
		}
	template<class T> maze::matrix<T> structure::matrix(T val, T out) const {
		return maze::matrix<T>(width, height, val, out);
	}
	maze::disjoint_set structure::disjoint_set() const {
		return maze::disjoint_set(width, height);
	}
	maze::cell structure::operator()(int x, int y) {
		return maze::cell(this, x, y);
	}
	maze::cell structure::operator()(std::pair<int, int> pos) {
		return maze::cell(this, pos.first, pos.second);
	}
	std::string structure::toString(std::string &wall, std::string &blank) {
		std::string result = "";
		for(int i = 0; i < 2*height + 1; i++) {
			for(int j = 0; j < 2*width + 1; j++) {
				bool tile = false;
				switch(j%2 << 1 | i%2) {
					case 0:
						tile = true;
					break;
					case 1:
						tile = vert[j/2][i/2];
					break;
					case 2:
						tile = hor[j/2][i/2];
					break;
				}
				result += tile ? wall : blank;
			}
			result += "\n";
		}
		return result;
	}

	cell::cell(maze::structure *from_, int x_, int y_):
		from(from_), x(x_), y(y_) {}
	it cell::left() const {
		return from->vert[x].begin() + y;
	}
	it cell::right() const {
		return from->vert[x + 1].begin() + y;
	}
	it cell::up() const {
		return from->hor[x].begin() + y;
	}
	it cell::down() const {
		return from->hor[x].begin() + (y + 1);
	}

	template<class T> matrix<T>::matrix(int w, int h, T val, T out):
		width(w), height(h),
		def(out),
		data(width) {
			for(std::vector<T> &i: data) {
				i.resize(height, val);
			}
		}
	template<class T> maze::matrix_cell<T> matrix<T>::operator()(int x, int y) {
		return maze::matrix_cell<T>(this, x, y);
	}
	template<class T> maze::matrix_cell<T> matrix<T>::operator()(std::pair<int, int> pos) {
		return maze::matrix_cell<T>(this, pos.first, pos.second);
	}

	disjoint_set::disjoint_set(int w, int h):
		width(w), height(h),
		set(width) {
			for(int i = 0; i < width; i++) {
				set[i].resize(height, std::make_pair(i, 0));
				for(int j = 0; j < height; j++)
					set[i][j].second = j;
			}
		}
	bool disjoint_set::join(coord a, coord b) {
		const coord aroot = find(a), broot = find(b);
		if(aroot == broot)
			return false;
		set[broot.first][broot.second] = aroot;
		return true;
	}
	coord disjoint_set::find(coord a) {
		coord &y = set[a.first][a.second];
		return y == a ? a : (y = find(y));
	}

	template<class T> matrix_cell<T>::matrix_cell(maze::matrix<T> *from_, int x_, int y_):
		from(from_),
		x(x_), y(y_) {}
	template<class T> maze::matrix_cell<T> matrix_cell<T>::left() const {
		return matrix_cell(from, x - 1, y);
	}
	template<class T> maze::matrix_cell<T> matrix_cell<T>::right() const {
		return matrix_cell(from, x + 1, y);
	}
	template<class T> maze::matrix_cell<T> matrix_cell<T>::up() const {
		return matrix_cell(from, x, y - 1);
	}
	template<class T> maze::matrix_cell<T> matrix_cell<T>::down() const {
		return matrix_cell(from, x, y + 1);
	}
	template<class T> T matrix_cell<T>::operator()() const {
		return x >= 0 && x < from->width && y >= 0 && y < from->height ? from->data[x][y] : from->def;
	}
	template<class T> T matrix_cell<T>::operator()(T n) const {
		return x >= 0 && x < from->width && y >= 0 && y < from->height ? (from->data[x][y] = n) : n;
	}

	int matrix_surrounding(const matrix_cell<bool> &cell) {
		return
			cell.left ()() << 3 |
			cell.right()() << 2 |
			cell.up   ()() << 1 |
			cell.down ()();
	};

	bool vector_join(std::vector<int> &vec, int a, int b) {
		const int aroot = maze::vector_find(vec, a), broot = maze::vector_find(vec, b);
		if(aroot == broot)
			return false;
		vec[broot] = aroot;
		return true;
	}
	int vector_find(std::vector<int> &vec, int a) {
		int &r = vec[a];
		return r == a ? a : (r = maze::vector_find(vec, r));
	}

	maze::structure aldousBroder(int w, int h) {
		maze::structure maze(w, h);
		int
			x = maze::rand(w), y = maze::rand(h),
			remaining = w*h - 1;
		maze::matrix<bool> visited = maze.matrix(false, false);
		visited(x, y)(true);
		while(remaining) {
			bool newCell = false;
			maze::cell here = maze(x, y);
			switch(maze::randbit(
				(x > 0)      << 3 |
				(x < w - 1)  << 2 |
				(y > 0)      << 1 |
				(y < h - 1)
			)) {
				case 3:
					if(!visited(--x, y)()) {
						*here.left() = false;
						newCell = true;
					}
				break;
				case 2:
					if(!visited(++x, y)()) {
						*here.right() = false;
						newCell = true;
					}
				break;
				case 1:
					if(!visited(x, --y)()) {
						*here.up() = false;
						newCell = true;
					}
				break;
				case 0:
					if(!visited(x, ++y)()) {
						*here.down() = false;
						newCell = true;
					}
				break;
			}
			if(newCell) {
				visited(x, y)(true);
				remaining--;
			}
		}
		return maze;
	}
	maze::structure binaryTree(int w, int h) {
		maze::structure maze(w, h);
		for(int i = 0; i < w; i++)
			for(int j = 0; j < h; j++) {
				if(!i && !j)
					continue;

				bool dir = false;
				if(i && j)
					dir = rand(2);
				else if(!i)
					dir = true;

				*(dir ? maze(i, j).up() : maze(i, j).left()) = false;
			}
		
		return maze;
	}
	maze::structure eller(int w, int h) {
		maze::structure maze(w, h);
		std::vector<int> now(w), next(w);
		for(int i = 0; i < w; i++)
			next[i] = i;
		for(int i = 0; i < h - 1; i++) {
			now.swap(next);
			for(int j = 0; j < w; j++)
				next[j] = j;
			
			for(int j = 0; j < w - 1; j++)
				if(maze::rand(2) && maze::vector_join(now, j, j + 1)) {
					*maze(j, i).right() = false;
				}
			
			matrix_t<int> setlist(w);
			for(int j = 0; j < w; j++)
				setlist[maze::vector_find(now, j)].push_back(j);
			for(const std::vector<int> &j: setlist) {
				int selected = -1;
				if(!j.size())
					continue;
				
				while(selected == -1)
					for(int k: j)
						if(maze::rand(2)) {
							if(selected == -1)
								selected = k;
							next[k] = selected;
							*maze(k, i).down() = false;
						}
			}
		}

		for(int i = 0; i < w - 1; i++)
			if(maze::vector_join(next, i, i + 1))
				*maze(i, h - 1).right() = false;
		
		return maze;
	}
	maze::structure huntAndKill(int w, int h) {
		maze::structure maze(w, h);
		int remaining = w*h - 1;
		maze::matrix<bool> visited = maze.matrix(false, true);

		int x = maze::rand(w), y = maze::rand(h);
		visited(x, y)(true);
		while(remaining) {
			const maze::matrix_cell<bool> &here = visited(x, y);
			int dir = matrix_surrounding(here) ^ 0xf;
			here(true);

			if(dir) {
				const maze::cell &pos = maze(x, y);
				switch(maze::randbit(dir)) {
					case 3:
						*pos.left() = false;
						x--;
					break;
					case 2:
						*pos.right() = false;
						x++;
					break;
					case 1:
						*pos.up() = false;
						y--;
					break;
					case 0:
						*pos.down() = false;
						y++;
					break;
				}
				visited(x, y)(true);
				remaining--;
			} else {
				x = -1;
				for(int i = 0; i < w && x == -1; i++)
					for(int j = 0; j < h && x == -1; j++)
						if(visited(i, j)() && matrix_surrounding(visited(i, j)) != 0xf) {
							x = i;
							y = j;
						}
			}
		}

		return maze;
	}
	maze::structure kruskal(int w, int h) {
		using edge = std::tuple<bool, int, int>;

		maze::structure maze(w, h);
		maze::disjoint_set ds = maze.disjoint_set();
		std::vector<edge> edgeset;
		edgeset.reserve(2*w*h - w - h);
		for(int i = 0; i < w; i++)
			for(int j = 0; j < h - 1; j++)
				edgeset.emplace_back(false, i, j);
		for(int i = 0; i < w - 1; i++)
			for(int j = 0; j < h; j++)
				edgeset.emplace_back(true, i, j);
		std::shuffle(edgeset.begin(), edgeset.end(), maze::random_engine);

		for(const edge &i: edgeset) {
			const int x = std::get<1>(i), y = std::get<2>(i);
			const bool vert = std::get<0>(i);
			const coord from(x, y), to(x + vert, y + !vert);
			if(ds.join(from, to))
				*(vert ? maze(from).right() : maze(from).down()) = false;
		}

		return maze;
	}
	maze::structure prim(int w, int h) {
		using edge = std::tuple<bool, int, int>;

		maze::structure maze(w, h);
		maze::matrix<bool> visited = maze.matrix(false, true);
		std::vector<edge> edgeset;
		edgeset.emplace_back(false, maze::rand(w), maze::rand(h));
		bool init = true;
		
		while(!edgeset.empty()) {
			int sz = edgeset.size(), ix = maze::rand(sz);
			std::swap(edgeset[ix], edgeset.back());
			edge edge = edgeset.back();
			edgeset.pop_back();

			int px = std::get<1>(edge), py = std::get<2>(edge), x = px, y = py;
			bool vert = std::get<0>(edge);

			if(visited(x, y)())
				vert ? x++ : y++;
			if(visited(x, y)())
				continue;
			visited(x, y)(true);
			
			if(init)
				init = false;
			else
				*(vert ? maze(px, py).right() : maze(px, py).down()) = false;
			
			if(!visited(x, y).left()())
				edgeset.emplace_back(true, x - 1, y);
			if(!visited(x, y).right()())
				edgeset.emplace_back(true, x, y);
			if(!visited(x, y).up()())
				edgeset.emplace_back(false, x, y - 1);
			if(!visited(x, y).down()())
				edgeset.emplace_back(false, x, y);
		}

		return maze;
	}
	maze::structure recursiveBacktracker(int w, int h) {
		maze::structure maze(w, h);
		std::stack<coord> history;
		maze::matrix<bool> visited = maze.matrix(false, true);

		history.emplace(maze::rand(w), maze::rand(h));
		while(!history.empty()) {
			const coord pos = history.top();
			const maze::matrix_cell<bool> &here = visited(pos);
			int dir = matrix_surrounding(here) ^ 0xf;

			here(true);
			if(dir) {
				const maze::cell &mazePos = maze(pos);
				switch(maze::randbit(dir)) {
					case 3:
						*mazePos.left() = false;
						history.emplace(pos.first - 1, pos.second);
					break;
					case 2:
						*mazePos.right() = false;
						history.emplace(pos.first + 1, pos.second);
					break;
					case 1:
						*mazePos.up() = false;
						history.emplace(pos.first, pos.second - 1);
					break;
					case 0:
						*mazePos.down() = false;
						history.emplace(pos.first, pos.second + 1);
					break;
				}
			} else
				history.pop();
		}

		return maze;
	}
	maze::structure recursiveDivision(int w, int h) {
		using args = std::tuple<int, int, int, int>;
		maze::structure maze(w, h, false);
		std::queue<args> queue;
		queue.emplace(0, 0, w, h);
		while(!queue.empty()) {
			const int
				x = std::get<0>(queue.front()), y = std::get<1>(queue.front()),
				w = std::get<2>(queue.front()), h = std::get<3>(queue.front());
			queue.pop();
			
			if(w == 1 || h == 1)
				continue;
			
			int divSpots = w + h - 2, divSpot = maze::rand(divSpots);
			if(divSpot >= h - 1) {
				int t = divSpot - h + 1, hole = maze::rand(h);
				for(int i = 0; i < h; i++)
					if(i != hole)
						*maze(x + t, y + i).right() = true;
				t++;
				queue.emplace(x, y, t, h);
				queue.emplace(x + t, y, w - t, h);
			} else {
				int t = divSpot, hole = maze::rand(w);
				for(int i = 0; i < w; i++)
					if(i != hole)
						*maze(x + i, y + t).down() = true;
				t++;
				queue.emplace(x, y, w, t);
				queue.emplace(x, y + t, w, h - t);
			}
		}
		return maze;
	}
	maze::structure sidewinder(int w, int h) {
		maze::structure maze(w, h);
		for(int i = 1; i < w; i++)
			*maze(i, 0).left() = false;
		
		for(int i = 1; i < h; i++) {
			int r = 0;
			for(int j = 0; j < w; j++) {
				r++;
				if(r != 1)
					*maze(j, i).left() = false;
				if(j == w - 1 || maze::rand(2)) {
					*maze(j - r + maze::rand(r) + 1, i).up() = false;
					r = 0;
				}
			}
		}

		return maze;
	}
	maze::structure wilson(int w, int h) {
		maze::structure maze(w, h);
		maze::matrix<int> direction = maze.matrix(0, 0);
		direction(0, 0)(4);

		for(int i = 0; i < w; i++)
			for(int j = 0; j < h; j++) {
				if(direction(i, j)() == 4)
					continue;
				
				int x = i, y = j;
				while(direction(x, y)() != 4) {
					int dir = maze::randbit(
						(x > 0)      << 3 |
						(x < w - 1)  << 2 |
						(y > 0)      << 1 |
						(y < h - 1)
					);
					direction(x, y)(dir);
					switch(dir) {
						case 3:
							x--;
						break;
						case 2:
							x++;
						break;
						case 1:
							y--;
						break;
						case 0:
							y++;
						break;
					}
				}
				bool broken = false;
				x = i; y = j;
				while(!broken) {
					int dir = direction(x, y)();
					if(dir == 4) {
						broken = true;
						break;
					}

					direction(x, y)(4);
					switch(dir) {
						case 3:
							*maze(x--, y).left() = false;
						break;
						case 2:
							*maze(x++, y).right() = false;
						break;
						case 1:
							*maze(x, y--).up() = false;
						break;
						case 0:
							*maze(x, y++).down() = false;
						break;
					}
				}
			}
		
		return maze;
	}
	
	std::map<std::string, maze::structure (*)(int, int)> algo{
		std::make_pair("aldous-broder", aldousBroder),
		std::make_pair("binary-tree", binaryTree),
		std::make_pair("eller", eller),
		std::make_pair("hunt-and-kill", huntAndKill),
		std::make_pair("kruskal", kruskal),
		std::make_pair("prim", prim),
		std::make_pair("recursive-backtracker", recursiveBacktracker),
		std::make_pair("recursive-division", recursiveDivision),
		std::make_pair("sidewinder", sidewinder),
		std::make_pair("wilson", wilson)
	};
	std::set<std::string> algo_is_slow{
		"aldous-broder",
		"wilson"
	};
}