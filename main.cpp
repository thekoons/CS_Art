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
        cout << "usage:\n" << "physics any.bmp outputfile.bmp" << endl;

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

