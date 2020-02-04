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
		Event(int start, int duration, int width, int height) : 
			_start(start), _duration(duration), _width(width), _height(height) {}

		virtual void Activate(Bitmap &b, int frame_num) = 0;

	protected:
		int _start;
		int _duration;
		int _width;
		int _height;
};

class Filter : public Event {

	public:
		Filter(int start, int dur, int width, int height, int type, vector <vector <vector <int> > > rules, vector<int> first) :
			Event(start, dur, width, height), _step(0), _type(type), _rules(rules)
			{
				for (int i = 0; i < _height; i++)
				{
					vector <int> v;
					v.assign(_width, 0);
					_grid.push_back(v);
				}

				for (int i = 0; i < first.size(); i++)
					_grid[0][i] = first[i];
			}

		~Filter()
		{}
		
		virtual void Activate(Bitmap &b, int frame_num);
		
		Bitmap *_b;
		
	private:

		int _type;
		int _step;

		vector <vector <vector <int> > > _rules;

		vector <vector <int> > _grid;

		void Rewrite();
		int* getColor(int n);
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
	cout << "Initiating Step " << _step << "..." << endl;
	
	//Tracks column of current row
	int j_cur = 0;

	//Tracks column of output row
	int j_out = 0;

	//Gets number of rules to consider
	int rule_num = _rules.size();

	//If the current cell is live
	while(Grid[_step][j_cur] != 0)
	{
		//Tracks if any rule is implemented at all over a cell
		int no_rule = 1;

		//Iterate over all rules to see if one applies
		for (int r = 0; r < rule_num; r++)
		{
			cout << "	Testing rule " << r << endl;
			//Check if rule r is present 
			int eq = 1;

			//Get rule r
			vector<int> rule_in = _rules[r][0];
			vector<int> rule_out = _rules[r][1];

			//If length of rule r does not extend past boundary
			if (j_cur + rule_in.size() < _width)
			{
				for (int l = 0; l < rule_in.size(); l++)
				{
					if (_grid[_step][j_cur + l] != rule_in[l])
						eq = 0;
				}

				//if so, add rule r output to next string
				if (eq == 1)
				{
					cout << "		Rule " << r << " matches..." << endl;

					for (int l = 0; l < rule_out.size(); l++)
					{
						if (j_out < _width)
						{
							//Set grid cell to resulting value
							_grid[_step + 1][j_out] = rule_out[l];

							//Set color of bitmap image
							_b->getPixel(_step + l, j_out).setRGB(getColor(rule_out[l]));

							//iterate output column counter
							j_out++;
						}
					}

					eq = 0;
					j_cur += rule_in.size();
					no_rule = 0;
				} 
			} 

			if (r == rule_num - 1 && no_rule == 1)
				j_cur++;
		} 
	}
	
	cout << "Step returning..." << endl;
}

//Takes integer value and returns rgb value
int* getColor(int n)
{
	int* color = new int[3];

	switch (n)
	{
		case 1:  //White
			color[0] = 255;
			color[1] = 255;
			color[2] = 255;
			break;
		case 2:  //Red
			color[0] = 255;
			color[1] = 0;
			color[2] = 0;
			break;
		case 3:  //Blue
			color[0] = 0;
			color[1] = 0;
			color[2] = 255;
			break;
		case 4:  //Green
			color[0] = 0;
			color[1] = 255;
			color[2] = 0;
			break;
		default: //Black
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			break;
	}

	return color;
}

#endif