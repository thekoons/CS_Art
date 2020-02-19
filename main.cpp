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
        cout << "usage:\n" << "sorter inputfile.bmp outputfile.bmp" << endl;

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
        int max_frame = height / 5;

#ifdef GIF
        GifWriter gifw;

        cout << "Gif Writer Created..." << endl;

        GifBegin(&gifw, outfile.c_str(), width, height, delay);
        vector<uint8_t> frame;

        cout << "Gif Writer Initialized..." << endl;
#endif

        vector<Event*> events;

/*
        Bitmap* edge_image = new Bitmap(image);

        Edger edger = new Edger(&edge_image, 0, 0);

        vector<vector<pair<int, int>>> edges = edger.getEdges();
*/

        //FULL CUT

        float* lower_bound_1 = new float[2];
               lower_bound_1[0] = height;
               lower_bound_1[1] = 0;
        float* upper_bound_1 = new float[2];
               upper_bound_1[0] = height;
               upper_bound_1[1] = 0;

        events.push_back(new Sorter(&image, 0, max_frame, BRIGHT, lower_bound_1, upper_bound_1));

        for (int n = 0; n < max_frame; n++) {

            for (auto e : events) e->Activate(n);
        
#ifdef GIF
            getFrame(frame, image);

            GifWriteFrame(&gifw, frame.data(), width, height, delay);
#endif
            cout << "   Frame " << n << " Written" << endl;
        }

#ifdef GIF
        GifEnd(&gifw);

        cout << "...Gif Written" << endl; 
#endif

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

