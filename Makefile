smol:
	g++ main.cpp -DSMOL -o rewrite

symm:
	g++ main.cpp -DSYMM -o rewrite

symmsmol:
	g++ main.cpp -DSYMM -DSMOL -o rewrite

default:
	g++ main.cpp -o rewrite
