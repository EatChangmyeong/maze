#include <iostream>
#include <fstream>
#include <map>

#include "maze_algorithms.hpp"

struct Config {
	uint16_t width = 0, height = 0;
	std::string wallStr = "#", blankStr = ".", algo = "recursive-backtracker";
	uint_fast64_t seed;
	bool seed_set = false;
};

template<class T> T parse_int(const std::string &str, const std::string &var_name, bool allow_zero = false) {
	unsigned long long result, max = std::numeric_limits<T>::max();
	std::string max_string = std::to_string(max);
	try {
		result = std::stoull(str);
	} catch(const std::invalid_argument &err) {
		throw "malformed argument " + var_name;
	} catch(const std::out_of_range &err) {
		throw var_name + " is out of its allowed range; try " + max_string + " or less";
	}
	
	if(!allow_zero && !result)
		throw var_name + " should be nonzero";
	if(result > max)
		throw var_name + " is out of its allowed range; try " + max_string + " or less";
	return result;
}

template<class T, class U> std::string join_map(const std::map<T, U> &list, const std::string &delim) {
	std::string result = "";
	typename std::map<T, U>::const_iterator it = list.begin();
	for(int i = 0; it != list.end(); i++, it++) {
		if(i)
			result += delim;
		result += it->first;
	}
	return result;
}

void printUsage() {
	std::cout
			<< "Usage: maze [options] width height\n\n"
			
			<< "Options:\n"
			<< "    -h                  Display this message\n"
			<< "    -a [" << join_map(maze::algo, "|") << "]\n"
			<< "                        Algorithm for maze generation; defaults to recursive-backtracker\n"
			<< "    -s [seed]           Random seed for maze generation; ranges from 0 to " << UINT_FAST64_MAX << ", defaults to current time in microseconds\n"
			<< "    -o [filename]       Filename for maze output; defaults to stdout\n"
			<< "    -w[string]          Text representation for walls; defaults to #\n"
			<< "    -b[string]          Text representation for blank spaces; defaults to .\n"
			<< "    -W                  Widen text representation of generated maze horizontally; equivalent to -w## -b..\n"
			<< "    -f                  Force; don't warn about slow algorithms\n\n"
			
			<< "For more information about maze generation algorithms, visit http://weblog.jamisbuck.org/2011/2/7/maze-generation-algorithm-recap\n"
			<< std::endl;
}

void panic(const std::string &msg, bool usage = false) {
	std::cerr << "Error: " << msg << "\n" << std::endl;
	if(usage)
		printUsage();
	std::exit(1);
}

void warn(const std::string &msg) {
	std::string response;
	std::cout << msg << "\n";
	while(true) {
		std::cout << "Continue? (Y/N) ";
		std::cin >> response;
		switch(response[0]) {
			case 'Y': case 'y':
			return;
			default:
			std::exit(0);
		}
	}
}

void readArgs(const int argc, const char **argv, Config &cfg, std::string &fname, bool &helpMode, bool &force) {
	char argChain = '\0';

	for(int i = 1; i < argc && !helpMode; i++) {
		if(argChain) {
			switch(argChain) {
				case 'a':
					cfg.algo = argv[i];
				break;
				case 'o':
					fname = argv[i];
				break;
				case 's':
					cfg.seed = parse_int<uint_fast64_t>(argv[i], "seed", true);
					cfg.seed_set = true;
				break;
			}
			argChain = '\0';
		} else if(argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'a':
				case 'o':
				case 's':
					argChain = argv[i][1];
				break;
				case 'b':
					cfg.blankStr = argv[i] + 2;
				break;
				case 'f':
					force = true;
				break;
				case 'h':
					helpMode = true;
				break;
				case 'w':
					cfg.wallStr = argv[i] + 2;
				break;
				case 'W':
					cfg.wallStr = "##";
					cfg.blankStr = "..";
				break;
				default:
					throw std::string("unknown argument ") + argv[i];
			}
		} else if(!cfg.width)
			cfg.width = parse_int<uint16_t>(argv[i], "width");
		else if(!cfg.height)
			cfg.height = parse_int<uint16_t>(argv[i], "height");
	}
}

int main(int argc, const char** argv) {
	Config cfg;

	std::string fname = "", err = "";
	bool helpMode = false, force = false;
	
	try {
		readArgs(argc, argv, cfg, fname, helpMode, force);
	} catch(const std::string &msg) {
		err = msg;
	}
	if(!err.size() && !maze::algo.count(cfg.algo))
		err = "unknown algorithm " + cfg.algo;
	if(err.size())
		panic(err, true);
	
	if(argc == 1 || helpMode || !cfg.width || !cfg.height) {
		printUsage();
		return 0;
	}

	if(cfg.seed_set)
		maze::randinit(cfg.seed);
	else
		maze::randinit();
	
	if(!force && maze::algo_is_slow.count(cfg.algo) && 2*cfg.width*cfg.height - cfg.width - cfg.height >= 100000)
		warn("The algorithm '" + cfg.algo + "' is considerably slower than other algorithms, especially with large mazes. You can always abort by pressing Ctrl+Z.");
	try {
		std::ostream *preout = &std::cout;
		std::ofstream *outfile;
		bool file_output = false;
		if(fname.size()) {
			file_output = true;
			outfile = new std::ofstream(fname);
			if(!outfile->is_open())
				throw "couldn't open " + fname;
			preout = outfile;
		}
		std::ostream &out = *preout;

		out << maze::algo[cfg.algo](cfg.width, cfg.height).toString(cfg.wallStr, cfg.blankStr);

		if(file_output)
			outfile->close();
	} catch(const std::string &msg) {
		panic(msg);
	}
	
	return 0;
}
