#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP

class Event {
	public:
		Event(int start, int duration) : _start(start), _duration(duration) {}

		virtual void Activate(Bitmap &b, int frame_num) = 0;

	protected:
		int _start;
		int _duration;
};

class Filter : public Event {

	public:
		Filter(int start, int dur, int type, vector <vector <vector <int> > > args) :
			Event(start, dur), _step(0), _prev_size(0), _type(type), _args(args) {}

		~Filter()
		{
			if (_args_i) delete[] _args_i;
			if (_args_f) delete[] _args_f;
		}
		
		virtual void Activate(Bitmap &b, int frame_num);
		
		Bitmap *_b;
		
	private:

		int _type;
		int _step;
		int _prev_size;

		vector <vector <vector <int> > > _args;

		void Rewrite();
};


void Filter::Activate(Bitmap &b, int frame_num)
{
	_b = &b;

	//Activate if frame number is after start but before end of duration
	if (frame_num >= _start && frame_num < _start + _duration)
	{
		switch(_type)
		{
			case 1:
				Rewrite();
				break;
		}
	}
}

//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
void Filter::Rewrite()
{

}

#endif