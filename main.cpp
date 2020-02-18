#include <iostream>
#include <fstream>
#include <string.h>
#include "events.h"
#include "bitmap.h"
#include "gif.h"

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        cout << "usage:\n" << "gifMaker [inputfile.bmp] outputfile.gif" << endl;

        return 0;
    }

    try
    {
        srand(time(NULL));

        ifstream in;
        Bitmap image;
        ofstream out;

        cout << "Program Starting..." << endl;

        string infile(argv[1]);
        string outfile(argv[2]);

        cout << "Seed Image Loading..." << endl;

        in.open(infile, ios::binary);
        in >> image;
        in.close();

        cout << "Seed Image Loaded..." << endl;

        int width = image.getSize(1);
        int height = image.getSize(0);
        int delay = 5;
        int max_frame = width;
        GifWriter gifw;

        cout << "Gif Writer Created..." << endl;

        GifBegin(&gifw, outfile.c_str(), width, height, delay);

        cout << "Gif Writer Initialized..." << endl;

        vector<Event*> events;
        vector<uint8_t> frame;

        events.push_back(new Sorter(&image, 0, max_frame));

        for (int n = 0; n < max_frame; n++) {

            events[0]->Activate(n);
        
            getFrame(frame, image);

            GifWriteFrame(&gifw, frame.data(), width, height, delay);

            cout << "   Frame " << n << " Written" << endl;
        }

        GifEnd(&gifw);

        cout << "...Gif Written" << endl; 

        out.open("out.bmp", ios::binary);
        out << image;
        out.close();

        for (auto e : events) delete e;
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

