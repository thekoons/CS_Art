#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cout << "usage:\n"
             << "bitmap steps inputfile.bmp outputfile.bmp" << endl;

        return 0;
    }

    try
    {
        int steps(atoi(argv[1]);
        string infile(argv[2]);
        string outfile(argv[3]);

        ifstream in;
        Bitmap image;
        ofstream out;

        in.open(infile, ios::binary);
        in >> image;
        in.close();

        automata(image, steps);

        out.open(outfile, ios::binary);
        out << image;
        out.close();
    }
    catch(BADHEADER)
    {
	cout << "Error: file not recognized." << endl;
    }
    catch(...)
    {
        cout << "Error: an uncaught exception occured." << endl;
    }

    return 0;
}

