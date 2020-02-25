#include <string>
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <random>
#include <utility>

#ifndef MAZE_ALGORITHMS_INCLUDE_GUARD
#define MAZE_ALGORITHMS_INCLUDE_GUARD

namespace maze {
	extern std::mt19937_64 random_engine;

	void randinit(uint_fast64_t seed = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::time_point_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now()
		).time_since_epoch()
	).count());
	int rand(int max);
	int randbit(int bits);

	class structure;
	class cell;
	template<class T> class matrix;
	class disjoint_set;
	template<class T> class matrix_cell;

	class structure {
		friend class maze::cell;

		int width, height;
		std::vector<std::vector<bool> > hor, vert;
	public:
		structure(int w, int h, bool init_value = true);
		template<class T> maze::matrix<T> matrix(T val, T out) const;
		maze::disjoint_set disjoint_set() const;
		maze::cell operator()(int x, int y);
		maze::cell operator()(std::pair<int, int> pos);
		std::string toString(std::string &wall, std::string &blank);
	};

	class cell {
		using it = std::vector<bool>::iterator;
		friend class maze::structure;

		maze::structure *from;
		int x, y;
		cell(maze::structure *from_, int x_, int y_);
	public:
		it left() const;
		it right() const;
		it up() const;
		it down() const;
	};

	template<class T> class matrix {
		friend class maze::structure;
		friend class maze::matrix_cell<T>;

		int width, height;
		T def;
		std::vector<std::vector<T> > data;
		matrix(int w, int h, T val, T out);
	public:
		maze::matrix_cell<T> operator()(int x, int y);
		maze::matrix_cell<T> operator()(std::pair<int, int> pos);
	};

	class disjoint_set {
		friend class maze::structure;
		
		int width, height;
		std::vector<std::vector<std::pair<int, int> > > set;
		disjoint_set(int w, int h);
	public:
		bool join(std::pair<int, int> a, std::pair<int, int> b);
		std::pair<int, int> find(std::pair<int, int> a);
	};

	template<class T> class matrix_cell {
		friend class maze::matrix<T>;

		maze::matrix<T> *from;
		int x, y;
		matrix_cell(maze::matrix<T> *from_, int x_, int y_);
	public:
		maze::matrix_cell<T> left() const;
		maze::matrix_cell<T> right() const;
		maze::matrix_cell<T> up() const;
		maze::matrix_cell<T> down() const;
		T operator()() const;
		T operator()(T n) const;
	};

	int matrix_surrounding(const matrix_cell<bool> &cell);

	bool vector_join(std::vector<int> &vec, int a, int b);
	int vector_find(std::vector<int> &vec, int a);

	maze::structure aldousBroder(int w, int h);
	maze::structure binaryTree(int w, int h);
	maze::structure eller(int w, int h);
	maze::structure huntAndKill(int w, int h);
	maze::structure kruskal(int w, int h);
	maze::structure prim(int w, int h);
	maze::structure recursiveBacktracker(int w, int h);
	maze::structure recursiveDivision(int w, int h);
	maze::structure sidewinder(int w, int h);
	maze::structure wilson(int w, int h);

	extern std::set<std::string> algo_is_slow;
	extern std::map<std::string, maze::structure (*)(int, int)> algo;
}

#endif