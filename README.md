CellAutoAnimation
=================

This library implements an animation program designed to create gifs of cellular automata, based on permutations of the classic Conway's Game of Life.

In Conway's game, every cell has two states: alive or dead. This creates four transition functions: alive -> alive, alive -> dead, dead -> dead, and dead -> alive, each governed by how many live neighbors a cell might have. This implementation allows control over these four functions. Setting them to occur at different values alters the behavior of the automata in incredible ways!

This implementation takes a bitmap image as input, and outputs a gif file. The gif file will be massive, because the gif library I found seems to be a little unoptimized. 

Options for compilation (flags to set) are in the makefile.

Have fun! 
