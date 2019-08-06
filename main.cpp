#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cout << "usage:\n"
             << "bitmap option inputfile.bmp outputfile.bmp\n"
             << "options:\n"
             << "  -i identity\n"
             << "  -c cell shade\n"
             << "  -g gray scale\n"
             << "  -p pixelate\n"
             << "  -b blur\n"
             << "  -r90 rotate 90\n"
             << "  -r180 rotate 180\n"
             << "  -r270 rotate 270\n"
             << "  -v flip vertically\n"
             << "  -h flip horizontally\n"
             << "  -d1 flip diagonally 1\n"
             << "  -d2 flip diagonally 2\n"
             << "  -grow scale the image by 2\n"
             << "  -shrink scale the image by .5" << endl;

        return 0;
    }

    try
    {
        string flag(argv[1]);
        string infile(argv[2]);
        string outfile(argv[3]);

        ifstream in;
        Bitmap image;
        ofstream out;

        in.open(infile, ios::binary);
        in >> image;
        in.close();

        if(flag == "-c"s)
        {
            cellShade(image);
        }
        if(flag == "-g"s)
        {
            grayscale(image);
        }
        if(flag == "-p"s)
        {
            pixelate(image);
        }
        if(flag == "-b"s)
        {
            blur(image);
        }
        if(flag == "-r90"s)
        {
            rotate(image, 1);
        }
        if(flag == "-r180"s)
        {
            rotate(image, 2);
        }
        if(flag == "-r270"s)
        {
            rotate(image, 3);
        }
        if(flag == "-v"s)
        {
            flip(image, 1);
        }
        if(flag == "-h"s)
        {
            flip(image, 2);
        }
        if(flag == "-d1"s)
        {
            flip(image, 3);
        }
        if(flag == "-d2"s)
        {
            flip(image, 4);
        }
        if(flag == "-grow"s)
        {
            scale(image, 200);
        }
        if(flag == "-shrink"s)
        {
            scale(image, 50);
        }

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

