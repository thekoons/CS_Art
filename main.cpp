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
        cout << "usage:\n" << "fractal inputfile.bmp outputfile.gif" << endl;

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

        int max_frame = 50;

        int clear[3] = {0, 0, 0};
        image.clear(clear);
        cout << "Reference and Output Images Intialized..." << endl << endl;

        events.push_back(new Fractal(&image, 0, 50, width, height, 0));
        
        for (int n = 0; n < max_frame; n++)
        {
            for (auto e : events) e -> Activate(n);

            getFrame(frame, image);

            GifWriteFrame(&gifw, frame.data(), width, height, delay);
                
            cout << "   Frame " << n << " Written" << endl;

            if (n % 10 == 0)
            {
                string fileName = "out" + to_string(n) + ".bmp";

                out.open(fileName, ios::binary);
                out << image;
                out.close();

                cout << "   Image " << n << " Written" << endl;
            }

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

