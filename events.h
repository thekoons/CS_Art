#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP


#define RED 1
#define GREEN 2
#define BLUE 3
#define BRIGHT 4

// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial


class Event {
	public:
		Event(Bitmap *b, int start, int duration) : _b(b), _start(start), _duration(duration) {}

		virtual void Activate(int frame_num) = 0;

	protected:
		Bitmap* _b; 
		int _start;
		int _duration;
};


class Sorter : public Event{

	public:
		Sorter(Bitmap *b, int start, int duration, int type, float* lower_bound, float* upper_bound) : 	Event(b, start, duration), _width(_b->getSize(1)), _height(_b->getSize(0)),
			_lower_bound(lower_bound), _upper_bound(upper_bound), _type(type) {}

		void Activate(int frame_num);

	private:

		int _type;

		int _width;
		int _height;

		float* _lower_bound;
		float* _upper_bound;

		void sort_line(int line_num);

		int getComp(vector<pair<int, Pixel*>> *arr, int index);

		int partition(vector<pair<int, Pixel*>> *arr, int low, int high);
		void quickSort(vector<pair<int, Pixel*>> *arr, int low, int high);
		void blur();

		int eval_lower(int n)
			{ 
				int val = (int) (n * _lower_bound[1] + _lower_bound[0]);

				if (val < 0)				return 0;
				if (val > _height - 1)		return _height - 1; 
				else						return val;
			}

		int eval_upper(int n)
			{ 
				int val = (int) (n * _upper_bound[1] + _upper_bound[0]);

				if (val < 0)				return 0;
				if (val > _height - 1)		return _height - 1; 
				else						return val;
			}
};

void Sorter::Activate(int frame_num)
{
	sort_line(frame_num);

#ifdef BLUR
	if (frame_num + 1 >= _width)
		blur();
#endif
}

int Sorter::getComp(vector<pair<int, Pixel*>> *arr, int index)
{
	int val = 0;

	switch (_type) {
		case RED:
			val = (*arr)[index].second->getRGB(0);
			break;
		case GREEN:
			val = (*arr)[index].second->getRGB(1);
			break;
		case BLUE:
			val = (*arr)[index].second->getRGB(2);
			break;
		case BRIGHT:
			val = (*arr)[index].second->getBRT();
			break;
	}

	return val;
}

int Sorter::partition(vector<pair<int, Pixel*>> *arr, int low, int high)
{

    // pivot (Element to be placed at right position)
    int pivot = getComp(arr, high);  
 
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {

        // If current element is smaller than the pivot
        if (getComp(arr, j) < pivot)
        {
            i++;    // increment index of smaller element
			(*arr)[i].swap((*arr)[j]);
        }
    }
			
	(*arr)[i + 1].swap((*arr)[high]);

    return (i + 1);
}

void Sorter::quickSort(vector<pair<int, Pixel*>> *arr, int low, int high)
{

    if (low < high)
    {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);  // Before pi
        quickSort(arr, pi + 1, high); // After pi
    }
}

void Sorter::sort_line(int line_num)
{
#ifdef DEBUG
	cout << "	Sort Initializing..." << endl;
#endif

	vector<pair<int, Pixel*>> pixels;

	for (int i = 0; i < _b->getSize(0); i++)
		pixels.push_back(make_pair(i, &_b->getPixel(i, line_num)));

#ifdef DEBUG
	cout << "	Sort Executing..." << endl;

	cout << "		bounds are: " << eval_lower(line_num) << ", " << eval_upper(line_num) << endl;

#endif

	quickSort(&pixels, eval_lower(line_num), eval_upper(line_num));

#ifdef DEBUG
	cout << "	Extracting values..." << endl;
#endif

	vector<int*> temps;

	for (int i = 0; i < _b->getSize(0); i++)
	{
		int* temp = new int[3];
			temp[0] = pixels[i].second->getRGB(0);
			temp[1] = pixels[i].second->getRGB(1);
			temp[2] = pixels[i].second->getRGB(2);
		temps.push_back(temp);
	}

#ifdef DEBUG
	cout << "	Plotting to Image..." << endl;
#endif

	for (int i = 0; i < _b->getSize(0); i++)
		_b->getPixel(i, line_num).setRGB(temps[i]);

#ifdef DEBUG
	cout << "	Pixel Sort finished over line " << line_num << endl;
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies gaussian blur of size 2 * R + 1
//
///////////////////////////////////////////////////////////////////////////////
void Sorter::blur()
{
	int N = 3;
	double *Gaussian = new double[N];
	double sum[3];
	double total;

	int*** temp = new int**[_height];

	for (int i = 0; i < _height; i++)
	{
		temp[i] = new int*[_width];
		for (int j = 0; j < _width; j++)
		{
			temp[i][j] = new int[3];
		}
	}

	//Calculates 1D Gaussian
	for (int k = 0; k < N; k++)
		Gaussian[k] = Binomial(N, k);

	cout << "		Initializing Gaussian Blur..." << endl;

	//Loop through image
	for (int i = 0; i < _height; i++)
	{
		for (int j = 0; j < _width; j++)
		{
			//Zero out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;

			total = 0;

			int lb = -(N / 2);
			int ub = (N / 2) + (N % 2);

			//Computes value of filter over pixel (i, j) given sized filter N
			for (int g_i = lb; g_i < ub; g_i++)
			{
				for (int g_j = lb; g_j < ub; g_j++)
				{
					//Skips values if out of bounds
					if (i + g_i >= 0 && i + g_i < _height)
					{
						if (j + g_j >= 0 && j + g_j < _width)
						{
							//Gets pixel value at specified pixel
							int* rgb = _b->getPixel(i + g_i, j + g_j).getRGB();

							//Multiplies two 1-D gaussian filter terms together
							//	Simulating 2-D gaussian filter
							for (int k = 0; k < 3; k++)
								sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * rgb[k];

							total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
						}
					}
				}
			}

			//Assigns value to temporary holding
			//	So as to not mess with other filter calculations
			for (int k = 0; k < 3; k++)
				temp[i][j][k] = (int) (sum[k] / total); //weighted average
		}
	}

	cout << "		Setting Image Values..." << endl;

	//Empties and Deletes temporary holding
	
	for (int i = 0; i < _height; i++)
	{
		for (int j = 0; j < _width; j++)
		{
			for (int k = 0; k < 3; k++)
				_b->getPixel(i, j).getRGB()[k] = temp[i][j][k];

			delete[] temp[i][j];
		}
		delete[] temp[i];
	}

	delete[] temp;
}


#endif