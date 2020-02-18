#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP


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
		Sorter(Bitmap *b, int start, int duration) : 
			Event(b, start, duration) {}

		void Activate(int frame_num);

	private:

		void sort_line(int line_num);

};

void Sorter::Activate(int frame_num)
{
	sort_line(frame_num);
}

int partition(vector<pair<int, Pixel*>> *arr, int low, int high)
{

    // pivot (Element to be placed at right position)
    int pivot = (*arr)[high].second->getBRT();  
 
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {

        // If current element is smaller than the pivot
        if ((*arr)[j].second->getBRT() < pivot)
        {
            i++;    // increment index of smaller element
			(*arr)[i].swap((*arr)[j]);
        }
    }
			
	(*arr)[i + 1].swap((*arr)[high]);

    return (i + 1);
}

void quickSort(vector<pair<int, Pixel*>> *arr, int low, int high)
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
#endif

	quickSort(&pixels, 0, _b->getSize(0) - 1);

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
	{
		Pixel* p = &_b->getPixel(i, line_num);
#ifdef DEBUG
		cout << "		pixel[i] = " << p->getRGB(0) << ", " << p->getRGB(1) << ", " << p-> getRGB(2)<< endl;

		cout << "		temps[i] = " << temps[i][0] << ", " << temps[i][1] << ", " << temps[i][2] << endl;
#endif
		_b->getPixel(i, line_num).setRGB(temps[i]);

#ifdef DEBUG
		cout << "		pixel[i] = " << p->getRGB(0) << ", " << p->getRGB(1) << ", " << p-> getRGB(2)<< endl;
#endif
	}

#ifdef DEBUG
	cout << "	Pixel Sort finished over line " << line_num << endl;
#endif
}


#endif