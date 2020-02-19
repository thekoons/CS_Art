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

        in.open("Images/4.bmp", ios::binary);
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
        int max_frame = 0, stroke_num;
        int radii[5] = {15, 11, 7, 3, 1};

        Bitmap* r = new Bitmap(image);
        Bitmap* o = new Bitmap(image);

        int clear[3] = {255, 255, 255};
        o->clear(clear);
        cout << "Reference and Output Images Intialized..." << endl << endl;
        
        for (int i = 1; i < 5; i++)
        {
            if (i == 1){
                cout << "1  First stroke layer generating..." << endl;
                events.push_back(new Painter(o, 0, 0, r, 1, &stroke_num, 2000, radii[0]));
                cout << "1  ...First stroke layer generated " << stroke_num << " frames." << endl << endl;
            }else{
                if (i == 2)
                    in.open("Images/5.bmp", ios::binary);
                if (i == 3)
                    in.open("Images/6.bmp", ios::binary);
                if (i == 4)
                    in.open("Images/7.bmp", ios::binary);
                
                in >> image;
                in.close();

                /*
                cout << i << " First stroke layer generating..." << endl;
                delete r;
                r = new Bitmap(image);
                events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 1000, radii[0]));
                cout << i << "  ...First stroke layer generated " << stroke_num << " frames." << endl;
                */
            }


            cout << i << "  Second stroke layer generating..." << endl;
            delete r;
            r = new Bitmap(image);
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[1]));
            cout << i << "  ...Second stroke layer generated " << stroke_num << " frames." << endl << endl;

            cout << i << "  Third stroke layer generating..." << endl;
            delete r;
            r = new Bitmap(image);
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[2]));
            cout << i << "  ...Third stroke layer generated " << stroke_num << " frames." << endl << endl;
            
            cout << i << "  Fourth stroke layer generating..." << endl;
            delete r;
            r = new Bitmap(image);
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[3]));
            cout << i << "  ...Fourth stroke layer generated " << stroke_num << " frames." << endl << endl;
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[3]));
            cout << i << "  ...Fourth stroke layer generated again..." << endl << endl;
            
            cout << i << "  Fifth stroke layer generating..." << endl;
            delete r;
            r = new Bitmap(image);
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[4]));
            cout << i << "  ...Fifth stroke layer generated " << stroke_num << " frames." << endl << endl;
            events.push_back(new Painter(o, max_frame, 0, r, 0, &stroke_num, 2000, radii[4]));
            cout << i << "  ...Fifth stroke layer generated again..." << endl << endl;
            
            max_frame += 100;
            
        }

        getFrame(frame, *o);
        GifWriteFrame(&gifw, frame.data(), width, height, delay);
        char still[16] = "still0.bmp";
        
        for (int n = 0; n < max_frame; n++)
        {
            if (n % 100 == 0){
                still[5] = (char) ((n / 100) + 48);

                out.open(still, ios::binary);
                out << *o;
                out.close();
            }

            for (auto e : events) e -> Activate(n);

            getFrame(frame, *o);

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

       delete r;
       delete o;
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

