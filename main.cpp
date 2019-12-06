#include <iostream>
#include <fstream>
#include <string.h>
#include "bitmap.h"
#include "gif.h"

int main(int argc, char** argv)
{
    
#ifdef RANDOM_RULES
    if(argc != 5)
    {
        cout << "usage:\n"
             << "cellular rulenumber steps inputfile.bmp outputfile.gif" << endl;

        return 0;
    }
#else
    if(argc != 4)
    {
        cout << "usage:\n"
             << "cellular steps inputfile.bmp outputfile.gif" << endl;

        return 0;
    }
#endif

    try
    {
        srand(time(NULL));

        ifstream in;
        Bitmap image;
        ofstream out;

        cout << "Program Starting..." << endl;

#ifdef NIGHT_MODE
        cout << "   Night Mode Active" << endl;
#else
        cout << "   Day Mode Active" << endl;
#endif 

#ifdef RANDOM_RULES
        cout << "   Using Random Ruleset" << endl;

        int rulenum(atoi(argv[1]));
        int steps(atoi(argv[2]));
        string infile(argv[3]);
        string outfile(argv[4]);
#else
        cout << "   Using Defined Ruleset" << endl;

        int rulenum = 1;
        int steps(atoi(argv[1]));
        string infile(argv[2]);
        string outfile(argv[3]);
#endif

        int **rules = new int*[rulenum];

        //Determines Ruleset given compiler flag
        for (int i = 0; i < rulenum; i++) {
            rules[i] = new int[6];

#ifdef RANDOM_RULES
            rules[i][0] = rand() % 3 + 1;
            rules[i][1] = rand() % (5 - rules[i][0]) + rules[i][0];
            rules[i][2] = rand() % 3 + 1;
            rules[i][3] = rand() % (5 - rules[i][2]) + rules[i][2];
            rules[i][4] = rand() % 2 + 1;
            rules[i][5] = steps / rulenum;
#else
            rules[i][0] = 1;
            rules[i][1] = 3;
            rules[i][2] = 3;
            rules[i][3] = 4;
            rules[i][4] = 4;
            rules[i][5] = steps / rulenum;
#endif

        }

        cout << "Seed Image Loading..." << endl;

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

        for (int n = 0; n < rulenum; n++)
        {
            cout << n << " - Writing " << rules[n][5] << " frames with (" << rules[n][0] << ", " 
                 << rules [n][1] << ", " << rules[n][2] << ", " << rules[n][3] << ", " << rules[n][4] << ")" << endl;

            for (int i = 0; i < rules[n][5]; i++) 
            {
                automata(image, rules[n]);
                getFrame(frame, image);
                GifWriteFrame(&gifw, frame.data(), width, height, delay);

                cout << "   Frame " << i << " Written" << endl;
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

