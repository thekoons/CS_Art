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
/*
        cout << "Gif Writer Created..." << endl;

#ifdef SMOL
        GifBegin(&gifw, outfile.c_str(), width * 4, height * 4, delay);
#else
        GifBegin(&gifw, outfile.c_str(), width, height, delay);
#endif 

        cout << "Gif Writer Initialized..." << endl;
*/
        vector<Event*> events;
        vector<uint8_t> frame;

        int max_frame = 15000;
        int* args;

        vector<Circle*> circles;
        collision*** matrix;
        double*** color_map;

        color_map = new double**[width];
        for (int i = 0; i < width; i++)
        {
            color_map[i] = new double*[height];
            for (int j = 0; j < height; j++)
            {
                color_map[i][j] = new double[3];
                color_map[i][j][0] = 0;
                color_map[i][j][1] = 0;
                color_map[i][j][2] = 0;
            }
        }

        cout << "Color map initialized..." << endl;

        for(int i = 0; i < 100; i++)
            circles.push_back(new Circle());

        cout << "Circles Initiated..." << endl;

        for (int n = 0; n < max_frame; n++)
        {
            if (n % 100 == 0)
                cout << "   Frame " << n << " calculating..." << endl;

            for (int i = 0; i < 100; i++)
            {
                for (int j = i + 1; j < 100; j++)
                {
                    float* point = circles[i] -> Intersect(circles[j]);

                    if (point)
                    {
                        int* color = circles[i] -> get_color(circles[j]);
                        
                        //cout << "color: " << color[0] << ", " << color[1] << ", " << color[2] << endl;

                        /*
                        matrix[i][j][n]->pos[0] = point[0];9
                        matrix[i][j][n]->pos[1] = point[1];
                        matrix[i][j][n]->color[0] = color[0];
                        matrix[i][j][n]->color[1] = color[1];
                        matrix[i][j][n]->color[2] = color[2];
                        */

                        int x = floor(point[0] * 10);
                        int y = floor(point[1] * 10);

                        if (x >= width)
                            x = width - 1;
                        if (x < 0)
                            x = 0;
                        if (y >= height)
                            y = height - 1;
                        if (y < 0)
                            y = 0;

                        for (int c = 0; c < 3; c++){
                            color_map[x][y][c] += color[c];
                        }

                        //cout << "               Collision marked at: " << x << ", " << y << endl;
                        //cout << "               Color_map: " << color_map[x][y][0] << ", " << color_map[x][y][1] << ", " << color_map[x][y][2] << endl;

                        delete[] point;
                    }
                }
            }

            //cout << "   Frame " << n << " cleaning..." << endl;

            for (int i = 0; i < 100; i++)
            {
                if (circles[i] -> Update(width, height))
                {
                    Circle* temp;
                    temp = circles[i];
                    circles[i] = new Circle();
                    delete temp;
                }
            }
        }

        cout << "Formatting to Bitmap..." << endl;
        

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int* rgb = new int[3];

                for (int k = 0; k < 3; k++) {

                    rgb[k] = floor(color_map[i][j][k]);
                    
                    if (rgb[k] > 255)
                        rgb[k] = 255;
                }

                if (rgb[0] != 0){
                    //cout << "Color_map (" << i << ", " << j << "): " << color_map[i][j][0] << ", " << color_map[i][j][1] << ", " << color_map[i][j][2] << endl;
                    //cout << "Pixel (" << i << ", " << j << "): " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
                }

                image.getPixel(i, j).setRGB(rgb);
            }
        }

        cout << "Writing Bitmap..." << endl;

        out.open(outfile, ios::binary);
        out << image;
        out.close();

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                delete[] color_map[i][j];
            }
            delete[] color_map[i];
        }
        delete[] color_map;

        cout << "Finished." << endl;

        /*
        //Automata Filter
        events.push_back(new Filter(0, 1000, 2, NULL, NULL));

        
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
        

        for (int n = 0; n < max_frame; n++)
        {
            for (auto e : events) e -> Activate(n);

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
        */
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

