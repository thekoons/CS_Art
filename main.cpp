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
        cout << "usage:\n" << "cellular inputfile.bmp outputfile.gif" << endl;

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
        int delay = 10;
        GifWriter gifw;

        cout << "Gif Writer Created..." << endl;

#ifdef SMOL
        GifBegin(&gifw, outfile.c_str(), width * 4, height * 4, delay);
#else
        GifBegin(&gifw, outfile.c_str(), width, height, delay);
#endif 

        cout << "Gif Writer Initialized..." << endl;

        vector<Event*> events;
        vector<uint8_t> frame;
        int max_frame = 1000;
        int* args;

        //Automata Filter
        events.push_back(new Filter(0, 1000, 2, NULL, NULL));

        /*
            args = new int[5];
            args[0] = 2;
            args[1] = 3;
            args[2] = 3;
            args[3] = 3;
            args[4] = 1;
        events.push_back(new Filter(0, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 2;
            args[1] = 3;
            args[2] = 3;
            args[3] = 4;
            args[4] = 1;
        events.push_back(new Filter(100, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 1;
            args[1] = 5;
            args[2] = 2;
            args[3] = 2;
            args[4] = 4;
        events.push_back(new Filter(200, 100, 1, args, NULL));

            args = new int[5];
            args[0] = 0;
            args[1] = 0;
            args[2] = 1;
            args[3] = 1;
            args[4] = 1;
        events.push_back(new Filter(300, 100, 1, args, NULL));
        */

        for (int n = 0; n < max_frame; n++)
        {
            for (auto e : events) e -> Activate(image, n);

            getFrame(frame, image);

#ifdef SMOL
            GifWriteFrame(&gifw, frame.data(), width * 4, height * 4, delay);
#else
            GifWriteFrame(&gifw, frame.data(), width, height, delay);
#endif

            cout << "   Frame " << n << " Written" << endl;
        }

       GifEnd(&gifw);

       cout << "...Gif Written" << endl; 

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

