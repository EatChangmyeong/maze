# 10 maze generation algorithms, written in C++

This is a ~~unorganized~~ repository to upload my implementation of maze generation algorithms. I started this project on Feb 21, when I was undergoing a severe [BOJ](http://icpc.me/) burnout.

## How to compile

Makefile in this repository was written for Windows 10 with MinGW-w64. To compile, simply use `make`. You might want to replace `del` with `rm -f` if you are more familiar to Linux.

I've barely worked with VS; there's nothing I can help about that. 😢

## Syntax

This program works in CLI. Below is a copy-pasted description from `maze -h`:

    Usage: maze [options] width height

    Options:
        -h                  Display this message
        -a [aldous-broder|binary-tree|eller|hunt-and-kill|kruskal|prim|recursive-backtracker|recursive-division|sidewinder|wilson]
                            Algorithm for maze generation; defaults to recursive-backtracker
        -s [seed]           Random seed for maze generation; ranges from 0 to <system-dependent value>, defaults to current time in microseconds
        -o [filename]       Filename for maze output; defaults to stdout
        -w[string]          Text representation for walls; defaults to #
        -b[string]          Text representation for blank spaces; defaults to .
        -W                  Widen text representation of generated maze horizontally; equivalent to -w## -b..
        -f                  Force; don't warn about slow algorithms

    For more information about maze generation algorithms, visit http://weblog.jamisbuck.org/2011/2/7/maze-generation-algorithm-recap