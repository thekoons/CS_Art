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

		void Activate(int frame_num) = 0;

	private:

		void sort_once(int line_num);
		void sort_line(int line_num);
		void sort_image();

}

#endif