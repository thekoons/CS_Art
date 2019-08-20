#include <iostream>
#include <fstream>
#include <string.h>
#include "bitmap.h"
#include "gif.h"

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
        int rules[5] = {0, 0, 1, 1, 1};
        int steps(atoi(argv[1]));
        string infile(argv[2]);
        string outfile(argv[3]);

        ifstream in;
        Bitmap image;
        ofstream out;

        cout << "Program Starting..." << endl;

        in.open(infile, ios::binary);
        in >> image;
        in.close();

        cout << "Seed Image Loaded..." << endl;

        int width = image.getSize(1);
        int height = image.getSize(0);
        int delay = 10;
        vector<uint8_t> frame;
        GifWriter gifw;

        cout << "Gif Writer Created..." << endl;

        GifBegin(&gifw, outfile.c_str(), width, height, delay);

        cout << "Gif Writer Initialized..." << endl;

        if (steps > 0)
        {
            for (int i = 0; i < steps; i++) {

                automata(image, rules, 1);

                /*cout << "Image " << i << " Processed..." << endl;

                char str[16];
                memset(str, '\0', 16);
                sprintf(str, "out%d.bmp", i);

                out.open(str, ios::binary);
                out << image;
                out.close();

                cout << "Image " << i << " Saved..." << endl;
                */

                getFrame(frame, image);

                //cout << "Frame " << i << " Created..." << endl;

                GifWriteFrame(&gifw, frame.data(), width, height, delay);

                cout << "Frame " << i << " Written..." << endl;
            }
        }

       GifEnd(&gifw);

       cout << "...Gif Written" << endl; 
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

