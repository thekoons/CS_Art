#include <iostream>
#include <fstream>
#include <string.h>
#include "events.h"
#include "bitmap.h"
#include "gif.h"

#define RED 2
#define BLUE 3
#define GREEN 4
#define PURPLE 5

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
        int delay = 1;
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
        int max_frame = 998;

        vector< vector< vector <int> > > rules;
        
        vector<int> rule1_in = {RED};
        vector<int> rule1_out = {RED, GREEN};

        vector< vector <int> > rule1;
        rule1.push_back(rule1_in);
        rule1.push_back(rule1_out);
        rules.push_back(rule1);

        vector<int> rule2_in = {BLUE, RED};
        vector<int> rule2_out = {RED, RED};

        vector< vector <int> > rule2;
        rule2.push_back(rule2_in);
        rule2.push_back(rule2_out);
        rules.push_back(rule2);

        vector<int> rule3_in = {BLUE, RED, RED};
        vector<int> rule3_out = {RED, BLUE};

        vector< vector <int> > rule3;
        rule3.push_back(rule3_in);
        rule3.push_back(rule3_out);
        rules.push_back(rule3);

        vector<int> rule4_in = {GREEN, RED};
        vector<int> rule4_out = {GREEN};

        vector< vector <int> > rule4;
        rule4.push_back(rule4_in);
        rule4.push_back(rule4_out);
        rules.push_back(rule4);

        vector<int> rule5_in = {GREEN, BLUE};
        vector<int> rule5_out = {RED, GREEN};

        vector< vector <int> > rule5;
        rule5.push_back(rule5_in);
        rule5.push_back(rule5_out);
        rules.push_back(rule5);

        vector<int> rule6_in = {GREEN, GREEN};
        vector<int> rule6_out = {BLUE, BLUE, PURPLE};

        vector< vector <int> > rule6;
        rule6.push_back(rule6_in);
        rule6.push_back(rule6_out);
        rules.push_back(rule6);

        vector<int> rule7_in = {PURPLE, RED};
        vector<int> rule7_out = {RED, PURPLE};

        vector< vector <int> > rule7;
        rule7.push_back(rule7_in);
        rule7.push_back(rule7_out);
        rules.push_back(rule7);

        vector<int> rule8_in = {PURPLE, BLUE};
        vector<int> rule8_out = {BLUE, PURPLE};

        vector< vector <int> > rule8;
        rule8.push_back(rule8_in);
        rule8.push_back(rule8_out);
        rules.push_back(rule8);

        vector<int> rule9_in = {PURPLE, GREEN};
        vector<int> rule9_out = {GREEN, PURPLE};

        vector< vector <int> > rule9;
        rule9.push_back(rule9_in);
        rule9.push_back(rule9_out);
        rules.push_back(rule9);

        vector<int> ruleA_in = {PURPLE, PURPLE};
        vector<int> ruleA_out = {RED};

        vector< vector <int> > ruleA;
        ruleA.push_back(ruleA_in);
        ruleA.push_back(ruleA_out);
        rules.push_back(ruleA);

        vector<int> first = {RED};

        //Automata Filter
        events.push_back(new Filter(0, max_frame, width, height, 0, rules, first));

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

       out.open("Examples/out.bmp", ios::binary);
       out << image;
       out.close();

       cout << "...Image Written." << endl;

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

