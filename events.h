#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP


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

class State_Machine 
{
	public:
		State_Machine();

		State_Machine(vector<int> states, vector<int> alphabet, 
		              vector<int> accept, vector<int> initial,
					  vector<vector<int>> transition, int state) :
					  _states(states), _alphabet(alphabet), _accept(accept),
					  _initial(initial), _transitions(transition), _state(state) {}

		~State_Machine();

		//Changes internal state of machine
		//	Outputs new state
		int transition(int input)
			{_state = _transitions[_state][input]; return _state;}

		//Checks if state is accept, transitive, or final
		int accept(int _state)
			{return _accept[_state];}

		//Checks if state is initial, transitive, or final
		int initial(int _state)
			{return _initial[_state];}

		//Gets current state of machine
		int state()
			{return _state;}

	private:

		//Current state
		int _state;

		//Marks which states in state vector are initial
		//	1 = is initial
		//	0 = otherwise
		vector<int> _initial;

		//Marks which states in state vector are accept
		//	1 = is accept
		//	0 = otherwise
		vector<int> _accept;

		//Marks which integers are being used as transition symbols
		vector<int> _alphabet;

		//Marks which states are in the domain
		vector<int> _states;

		//Stores 'Next States' indexed by 'Transition' indexed by 'Current state'
		vector<vector<int>> _transitions;
};

class Cellular_Automata : public Event
{
	public:

		Cellular_Automata();
		Cellular_Automata(Bitmap *b, int start, int duration, int row, int col, int* params) :
			Event(b, start, duration), _row(row), _col(col)
			{
				
			}
		~Cellular_Automata();

	private:

		//Holds grid of state machines
		//	one per pixel of final image
		vector<vector<State_Machine*>> _grid;
		int _row;
		int _col;

		//Helper function for Automata
		int* countNeighbors(int i, int j, int AE, int* newRGB, int state_num);

		void Activate(int frame_num);

		void automata_2S();		// _type = 1
		void automata_3S();		// _type = 2
};

void Automata::Activate(int frame_num)
{
	//Activate if frame number is after start but before end of duration
	if (frame_num < _start || frame_num >= _start + _duration)
		return;

	//Stores transition symbols for all automata
	int** transitions = new int*[_row];

	for (int i = 0; i < _row; i++)
		transitions[i] = new int[_col];
	
	for (int i = 0; i < _row; i++)
	{
		for (int j = 0; j < _col; j++)
		{
			transitions[i] = countNeighbors
		}
	}
}


//Counts neighbors of pixel at position i, j with area of effect AE
//	Gets new color, returned in newrgb
//	Only considers "neighbors" that are in state specified by 'states'
//		state_num determines how many states to consider
int* Automata::countNeighbors(int i, int j, int AE, int* newrgb, int state_num)
{
	int i_upper_bound, i_lower_bound;
	int j_upper_bound, j_lower_bound;
	int row = _b->getSize(0);
	int col = _b->getSize(1);

	long sumrgb[4] = {0, 0, 0, 0};

	int *neighbors = new int[state_num];
	int not_none = 0;

	for (int i = 0; i < state_num; i++)
		neighbors[i] = 0;
	
	//Ensures bounds respect edges of table given Area of Effect
	if 	(i - AE < 0)		{i_lower_bound = 0;}
	else 					{i_lower_bound = i - AE;}
	if 	(j - AE < 0) 		{j_lower_bound = 0;}
	else 					{j_lower_bound = j - AE;}
	if 	(i + AE >= row) 	{i_upper_bound = row - 1;}
	else 					{i_upper_bound = i + AE;}
	if 	(j + AE >= col) 	{j_upper_bound = col - 1;}
	else 					{j_upper_bound = j + AE;}
	
	//Counts Neighbors of current pixel
	//	in every state given by state_num
	for (int k = i_lower_bound; k <= i_upper_bound; k++) 
	{
		for (int l = j_lower_bound; l <= j_upper_bound; l++) 
		{
			if ((k != i) || (l != j))
			{
				int state = _b->getPixel(k, l).getState();
				neighbors[state]++;
				
				if (state)
				{
					//Get color (for color blending purposes)
					int* rgb = _b->getPixel(k, l).getHeldRGB();

					for (int n = 0; n < 4; n++)
						sumrgb[n] += rgb[n];

					not_none++;
				}
			}
		}
	}

	//Averages color of active neighbors (if any neighbors are alive)
	for (int n = 0; n < 4; n++) 
	{
		if (not_none > 0)
			newrgb[n] = (int) ((float)sumrgb[n] / not_none);
		else
			newrgb[n] = _b->getPixel(i, j).getHeldRGB()[n];
	}

	return neighbors;
}

//Automata transformation
//	Standard 2-state, as in Conway's Game of Life
//
void Automata::automata_2S()
{
	//Factor in area of effect to ruleset
	for (int i = 0; i < 4; i++)
		_args_i[i] * (pow(3 + (_args_i[4] - 1) * 2, 2) - 1) / 8;

	//Loop through bitmap applying automata filter
	for (int i = 0; i < _b->getSize(0); i++){

		for (int j = 0; j < _b->getSize(1); j++) {

			//Get neighbors of current pixel
			int newrgb[4] = {0, 0, 0, 0};
			int* neighbors = countNeighbors(i, j, _args_i[4], newrgb, 2);

			_b->getPixel(i,j).setHeldRGB(newrgb);

			//Calculates whether, based on active neighbors, an inactive pixel turns on
			if (_b->getPixel(i, j).getState() == 0){
				if ((neighbors[1] >= _args_i[2]) && (neighbors[1] <= _args_i[3])) {
					_b->getPixel(i, j).transition(1);
				} else {
					_b->getPixel(i, j).transition(0);
				}
			}
			
			//Calculates whether, based on inative neighbors, an active pixel turns off
			else if (_b->getPixel(i, j).getState() == 1) {
				if ((neighbors[1] >= _args_i[0]) && (neighbors[1] <= _args_i[1])) {
					_b->getPixel(i, j).transition(1);
				} else {
					_b->getPixel(i, j).transition(0);
				}
			}

			delete[] neighbors;
		}
	}

	//Rectifies changes made during cellular automata filter
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}

//Automata transformation
//	3 State - Cerebellum
void Automata::automata_3S()
{

	//Loop through bitmap applying automata filter
	for (int i = 0; i < _b->getSize(0); i++){

		for (int j = 0; j < _b->getSize(1); j++) {
			//Get neighbors of current pixel
	
			int newrgb[4] = {0, 0, 0, 0};
			int* neighbors = countNeighbors(i, j, 1, newrgb, 3);

			_b->getPixel(i,j).setHeldRGB(newrgb);


		
			//Transitions for state 0
			if (_b->getPixel(i, j).getState() == 0)
			{
				if (neighbors[1] <= 2 && neighbors[2] >= 3)
					_b->getPixel(i, j).transition(2);
				else
					_b->getPixel(i, j).transition(0);
			}
			
			//Transitions for state 1
			else if (_b->getPixel(i, j).getState() == 1) 
			{
				_b->getPixel(i, j).transition(0);
			}

			//Transitions for state 2
			else if (_b->getPixel(i, j).getState() == 2)
			{
				if (neighbors[1] >= 3 || neighbors[2] >= 4 || neighbors[2] == 0)
					_b->getPixel(i, j).transition(1);
				else
					_b->getPixel(i, j).transition(2);
			}
			
			delete[] neighbors;
		}
	}

	//Rectifies changes made during cellular automata filter
	for (int i = 0; i < _b->getSize(0); i++){
		for (int j = 0; j < _b->getSize(1); j++) {
			_b->getPixel(i, j).rectify();
		}
	}
}


class Stroke { // Data structure for holding painterly strokes.
public:
   Stroke(void);
   Stroke(unsigned int radius, unsigned int x, unsigned int y,
          unsigned char r, unsigned char g, unsigned char b, unsigned char a);
   
   // data
   unsigned int radius, x, y;	// Location for the stroke
   unsigned char r, g, b, a;	// Color
};

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
               unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
   radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{}

class Painter : public Event 
{
	public:
		Painter();
		Painter(Bitmap *b, int start, int duration, Bitmap *r, int firstLayer, int *strokeNum, int strokeMax, int radius) 
			: Event(b, start, duration), _r(r), _firstLayer(firstLayer), _strokeMax(strokeMax), _radius(radius), _width(b->getSize(0)), _height(b->getSize(1)) 
		{
			cout << "	Generating Reference..." << endl;
			Generate_Reference();
			cout << "	Generating Layer..." << endl;
			Generate_Layer();
			*strokeNum = _strokeNum / 20;
			_duration = _strokeNum / 20;
		}

		~Painter();
		
		void Activate(int frame_num);

	private:
		int _firstLayer;
		int _strokeNum;
		int _strokeMax;
		int _radius;
		int _width;
		int _height;

		Bitmap* _r;
		vector<Stroke*> _s;

		//Generates reference image
		//	using variously sized gaussian blur
		void Generate_Reference();

		//Generates layer of strokes
		void Generate_Layer();

		//Paints a single stroke from layer
		void Paint_Stroke(const Stroke& s);
};


void Painter::Activate(int frame_num)
{
	if (frame_num < _start || frame_num >= _start + _duration)
		return;

	for (int i = 0; i < 20; i++)
		Paint_Stroke(*_s[(frame_num - _start) * 20 + i]);
}


///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies gaussian blur of size 2 * R + 1
//
///////////////////////////////////////////////////////////////////////////////
void Painter::Generate_Reference()
{
	int N = 2 * _radius + 1;
	double *Gaussian = new double[N];
	double sum[3];
	double total;

	int*** temp = new int**[_width];
	for (int i = 0; i < _width; i++)
	{
		temp[i] = new int*[_height];
		for (int j = 0; j < _height; j++)
		{
			temp[i][j] = new int[3];
		}
	}

	//Calculates 1D Gaussian
	for (int k = 0; k < N; k++)
		Gaussian[k] = Binomial(N, k);

	cout << "		Initializing Gaussian Blur..." << endl;

	//Loop through image
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
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
					if (i + g_i >= 0 && i + g_i < _width)
					{
						if (j + g_j >= 0 && j + g_j < _height)
						{
							//Gets pixel value at specified pixel
							int* rgb = _r->getPixel(i + g_i, j + g_j).getRGB();

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
	
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			for (int k = 0; k < 3; k++)
				_r->getPixel(i, j).getRGB()[k] = temp[i][j][k];

			delete[] temp[i][j];
		}
		delete[] temp[i];
	}

	delete[] temp;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies paint strokes to canvas
//		
///////////////////////////////////////////////////////////////////////////////
void Painter::Generate_Layer()
{
	//Create new set of strokes
	_strokeNum = 0;

	//Sets grid to f_g * R, with f_g = 1
	int grid = _radius;

	//Sets threshold to 25
	int T = 25;

	//Create difference image
	//	Only holds one value per pixel, so is only width * height
	double *D = new double[_width * _height];

	//Initializes Difference Image
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			if (!_firstLayer)
			{
				//Calculates difference image given initailized canvas
				double sum = 0;

				int* rgb_r = _r->getPixel(i, j).getRGB();
				int* rgb_o = _b->getPixel(i, j).getRGB();

				for (int k = 0; k < 3; k++)
					sum += (rgb_r[k] - rgb_o[k]) * (rgb_r[k] - rgb_o[k]);

				D[i * _height + j] = sqrt(sum);
			}
			else
			{
				//Initializes difference image, in order to initialize canvas
				D[i * _height + j] = 100;
			}
		}
	}

	//Generates Strokes
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			//To store cumulative error
			double areaError = 0;

			//To store maximum error
			double max = 0;

			//To store maximum error coordinates
			int max_i = 0, max_j = 0;
			int N = grid / 2;

			//Sum error near (i, j)
			for (int i_g = i - N; i_g < i + N + 1; i_g++)
			{
				for (int j_g = j - N; j_g < j + N + 1; j_g++)
				{
					if (i_g >= 0 && i_g < _width)
					{
						if (j_g >= 0 && j_g < _height)
						{
							//Add error to sum
							areaError += D[i_g * _height + j_g];

							//Check if error is max
							if (D[i_g * _height + j_g] > max)
							{
								max = D[i_g * _height + j_g];
								max_i = i_g;
								max_j = j_g;
							}
						}
					}
				}
			}

			//Calculate error of area
			areaError /= (grid * grid);

			//Threshold
			if (areaError > T) {

				//Get Pixel rgb value
				int* rgb = _r->getPixel(max_i, max_j).getRGB();

				//cout << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;

				//Make Stroke unsigned int iradius, unsigned int ix, unsigned int iy, unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia
				_s.push_back(new Stroke(_radius, max_j, max_i, rgb[2], rgb[1], rgb[0], 255));

				_strokeNum++;
			}
		}
	}

	//Shuffle, for natural look
	random_shuffle(_s.begin(), _s.end());

	//Limit number of strokes layer can have
	if (_strokeNum > _strokeMax) _strokeNum = _strokeMax;
}

///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void Painter::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < _height && y_loc >= 0 && y_loc < _width)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
				/*
               _b->getPixel(y_loc, x_loc).getRGB()[0] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[0] + s.r) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[1] + s.g) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[2] + s.b) / 2;
				*/
               _b->getPixel(y_loc, x_loc).getRGB()[0] = s.r;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = s.g;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = s.b;
			   
            } else if (dist_squared >= radius_squared + 1 && dist_squared <= radius_squared + 2) {
               _b->getPixel(y_loc, x_loc).getRGB()[0] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[0] + s.r) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[1] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[1] + s.g) / 2;
               _b->getPixel(y_loc, x_loc).getRGB()[2] = 
                  (_b->getPixel(y_loc, x_loc).getRGB()[2] + s.b) / 2;
            }
         }
      }
   }
}


class Filter : public Event {

	public:
		Filter(Bitmap *b, int start, int dur, int type, int* args_i, float* args_f) :
			Event(b, start, dur), _type(type), _args_i(args_i), _args_f(args_f) {}

		~Filter()
		{
			if (_args_i) delete[] _args_i;
			if (_args_f) delete[] _args_f;
		}
		
		virtual void Activate(int frame_num);
				
	private:

		int _type;
		int* _args_i;
		float* _args_f;

		void Dither_FS();
		void Dither_Color();
		void Filter_Gaussian_N(unsigned int N);
		void Filter_Edge();

		void pixelate();		// _type = 3
		void blur();			// _type = 4
		void cellshade();		// _type = 5
		void zoom();			// _type = 6
		void pan();				// _type = 7
};


void Filter::Activate(int frame_num)
{
	//Activate if frame number is after start but before end of duration
	if (frame_num >= _start && frame_num < _start + _duration)
	{
		switch(_type)
		{
			case 3:
				pixelate();
				break;
			case 4:
				blur();
				break;
			case 5:
				cellshade();
				break;
			case 6:
				zoom();
				break;
			case 7:
				pan();
				break;
		}
	}
}



//Cellshades entire map contained by object of class Bitmap
//	Cellshade - round each RGB value individually
//		    such that color palette is reduced
void Filter::cellshade() {

	int* temp_rgb;
	float temp_f;

	for (int i = 0; i < _b->getSize(0); i++) {
		for (int j = 0; j < _b->getSize(1); j++) {
			temp_rgb = _b->getPixel(i, j).getRGB();
			
			//Performs cell shade by dividing rgb values such that
			//	0, 128, and 255 can be represented by 0, 1, and 2,
			//	which these values are then rounded to. Thus, after 
			//	scaling back up, the rgb values are either 0, 128, or 255.
			for (int k = 0; k < 3; k++) {
				temp_f = (float)(temp_rgb[k] * 2) / 256;
				temp_f = round(temp_f);
				temp_rgb[k] = (int)(temp_f * 256) / 2;
				if (temp_rgb[k] == 256) temp_rgb[k]--;
			}
			
			_b->getPixel(i, j).setRGB(temp_rgb);
		}
	}
}

//Pixellates entire map contained by object of class Bitmap
void Filter::pixelate() {
		
	int sum[3];
	int degree = 4;	//Divides image into 16x16 blocks
	int size = pow(2, degree);
	int* temp_rgb;

	//Loops through all pixels in steps determined by degree of pixellation
	for (int i = 0; i < _b->getSize(0) - size; i = i + size) {
		for (int j = 0; j < _b->getSize(1) - size; j = j + size) {

			//Zeroes out summing intiable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;

			//Loops through all pixels in current step to get average color
			for (int i_p = i; i_p < i + size; i_p++) {
				for (int j_p = j; j_p < j + size; j_p++) {
	
					temp_rgb = _b->getPixel(i_p, j_p).getRGB();

					for (int k = 0; k < 3; k++) 
						sum[k] += temp_rgb[k];
				}
			}
			
			//Averages color aross 2^degree x 2^degree square
			for (int k = 0; k < 3; k++)
				temp_rgb[k] = sum[k] / pow(size, 2);
			
			//Loops through all pixels in current step to assign new color
			for (int i_p = i; i_p < i + size; i_p++) {
				for (int j_p = j; j_p < j + size; j_p++) {	
					_b->getPixel(i_p, j_p).setRGB(temp_rgb);
				}
			}
		}
	}
}

void Filter::blur() 
{
	int blurArray[] = {1, 4, 6, 4, 1};
	double sum[3];
	int total;
	int* temp_rgb;

	//Loops through all pixels
	for (int i = 0; i < _b->getSize(0); i++) {
		for (int j = 0; j < _b->getSize(1); j++) {

			//Zeroes out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;
			total = 0;

			//Loops through whatever of the gaussian blur matrix is in-bounds 
			//	With gaussian matrix centered on pixel(i, j)
			for (int i_b = -2; i_b < 3; i_b++) {
				for (int j_b = -2; j_b < 3; j_b++) {
					if (i + i_b >= 0 && i + i_b < _b->getSize(0)) { //Skips entry if out of bounds
					if (j + j_b >= 0 && j + j_b < _b->getSize(1)) {
						
						temp_rgb = _b->getPixel(i + i_b, j + j_b).getRGB();
						
						//Multiplies value by gaussian coefficients
						for (int k = 0; k < 3; k++) 
							sum[k] += blurArray[i_b + 2] * blurArray[j_b + 2] * temp_rgb[k];

						//Totals gaussian coefficients (for averaging purposes)
						total += (blurArray[i_b + 2] * blurArray[j_b + 2]);
					}
					}
				}
			}

			temp_rgb = _b->getPixel(i, j).getRGB();
			
			//Averages out pixel RGB values
			for (int k = 0; k < 3; k++)
				temp_rgb[k] = sum[k] / total;

			//Assigns nextRGB value to pixel (i, j)
			_b->getPixel(i, j).setNextRGB(temp_rgb);
		}
	}

	//Sets all pixels to their nextRGB values
	for (int i = 0; i < _b->getSize(0); i++)
		for (int j = 0; j < _b->getSize(1); j++)
			_b->getPixel(i, j).rectifyRGB();
}

///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Filter::Dither_FS()
{
	/*
	float *out_data = new float[width * height];
	float *acc_data = new float[width * height];

	for (int i = 0; i < width * height; i++)
	{
		out_data[i] = 0;
		acc_data[i] = 0;
	}

	//Pass over image top to bottom, perform FS Dithering
	for (int i = 0; i < height; i++)
	{
		//Pass over image from left to right
		for (int j = 0; j < width; j++)
		{
			//Get grayscaled pixel value
			acc_data[i * width + j] += (.299 * (float) data[i * (width * 4) + (j * 4) + RED]
									  + .587 * (float) data[i * (width * 4) + (j * 4) + GREEN]
									  + .114 * (float) data[i * (width * 4) + (j * 4) + BLUE]) / 256;

			//Threshold and output
			if (acc_data[i * width + j] <= 0.5)
				out_data[i * width + j] = 0;
			else
				out_data[i * width + j] = 1;

			//Calculate error between acc and out
			float e = acc_data[i * width + j] - out_data[i * width + j];

			//Propogate error to other pixels' i_acc
			if ((j + 1) < width) 
				acc_data[i * width + j + 1] += ((float)7 / 16) * e;
			

			if ((i + 1) < height)
			{
				acc_data[(i + 1) * width + j] += ((float)5/16) * e;

				if ((j - 1) >= 0) 
					acc_data[(i + 1) * width + j - 1] += ((float)3/16) * e;
					
				if ((j + 1) < width) 
					acc_data[(i + 1) * width + j + 1] += ((float)1/16) * e;
			}
		}

		//Checks if one more row is available to zag
		if (i + 1 < height)
		{
			//Iterates one row down
			i++;

			//Pass over image from right to left
			for (int j = width - 1; j >= 0; j--)
			{
				//Get grayscaled pixel value
				acc_data[i * width + j] += (.299 * (float) data[i * (width * 4) + (j * 4) + RED]
					                      + .587 * (float) data[i * (width * 4) + (j * 4) + GREEN]
					                      + .114 * (float) data[i * (width * 4) + (j * 4) + BLUE]) / 256;

				//Threshold and output
				if (acc_data[i * width + j] <= 0.5)
					out_data[i * width + j] = 0;
				else
					out_data[i * width + j] = 1;

				//Calculate error between acc and out
				float e = acc_data[i * width + j] - out_data[i * width + j];

				//Propogate error to other pixels' i_acc
				//	Where applicable
				if ((j - 1) >= 0)
					acc_data[i * width + j - 1] += ((float)7 / 16) * e;

				if ((i + 1) < height)
				{
					acc_data[(i + 1) * width + j] += ((float)5 / 16) * e;

					if ((j + 1) < width)
						acc_data[(i + 1) * width + j + 1] += ((float)3 / 16) * e;

					if ((j - 1) >= 0)
						acc_data[(i + 1) * width + j - 1] += ((float)1 / 16) * e;
				}
			}
		}
	}

	//Put dithered image into data
	for (int i = 0; i < height * width; i++)
	{
		if (out_data[i] < 0)
			out_data[i] = 0;
		if (out_data[i] == 1)
			out_data[i] = ((float)255 / 256);

		data[i * 4 + RED]	= (unsigned char) floor(out_data[i] * 256);
		data[i * 4 + GREEN] = (unsigned char) floor(out_data[i] * 256);
		data[i * 4 + BLUE]	= (unsigned char) floor(out_data[i] * 256);
	}

    return true;
	*/
}// Dither_FS

///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Filter::Dither_Color()
{
	/*
	unsigned char *out_data = new unsigned char[width * height * 4];
	unsigned char *acc_data = new unsigned char[width * height * 4];
	memset(out_data, '\0', width * height * 4);
	memset(acc_data, '\0', width * height * 4);

	//Pass over image top to bottom, perform FS Dithering
	for (int i = 0; i < height; i++)
	{
		//Pass over image from left to right
		for (int j = 0; j < width * 4; j = j + 4)
		{
			//Loops thru RGB values per each pixel
			for (int k = 0; k < 3; k++)
			{
				//Add image pixel value to acc
				acc_data[i * (width * 4) + j + k] += data[i * (width * 4) + j + k];

				//Output
				out_data[i * (width * 4) + j + k] = acc_data[i * (width * 4) + j + k];
				
				//Threshold
				if (k == RED) 
				{
					out_data[i * (width * 4) + j + k] /= 32;
					out_data[i * (width * 4) + j + k] *= 32;
				}
				else if (k == GREEN) 
				{
					out_data[i * (width * 4) + j + k] /= 32;
					out_data[i * (width * 4) + j + k] *= 32;
				}
				else if (k == BLUE)
				{
					out_data[i * (width * 4) + j + k] /= 64;
					out_data[i * (width * 4) + j + k] *= 64;
				}

				//Calculate error between acc and out
				int e = acc_data[i * (width * 4) + j + k] - out_data[i * (width * 4) + j + k];

				//Propogate error to other pixels' i_acc
				if (((j / 4) + 1) < width)
					acc_data[i * (width * 4) + j + k + 4] += ((float)7 / 16) * e;

				if ((i + 1) < height)
				{
					acc_data[(i + 1) * (width * 4) + j + k] += ((float)5 / 16) * e;

					if (((j / 4) - 1) >= 0)
						acc_data[(i + 1) * (width * 4) + j + k - 4] += ((float)3 / 16) * e;

					if (((j / 4) + 1) < width)
						acc_data[(i + 1) * (width * 4) + j + k + 4] += ((float)1 / 16) * e;
				}

			}
		}

		//Checks if one more row is available to zag
		if (i + 1 < height)
		{
			//Iterates one row down
			i++;

			//Pass over image from right to left
			for (int j = (width - 1) * 4; j >= 0; j = j - 4)
			{
				//Loops thru RGB values per each pixel
				for (int k = 0; k < 3; k++)
				{
					//Add image pixel value to acc
					acc_data[i * (width * 4) + j + k] += data[i * (width * 4) + j + k];

					//Output
					out_data[i * (width * 4) + j + k] = acc_data[i * (width * 4) + j + k];

					//Threshold
					if (k == RED) 
					{
						out_data[i * (width * 4) + j + k] /= 32;
						out_data[i * (width * 4) + j + k] *= 32;
					}
					else if (k == GREEN)
					{
						out_data[i * (width * 4) + j + k] /= 32;
						out_data[i * (width * 4) + j + k] *= 32;
					}
					else
					{
						out_data[i * (width * 4) + j + k] /= 64;
						out_data[i * (width * 4) + j + k] *= 64;
					}

					//Calculate error between acc and out
					int e = acc_data[i * (width * 4) + j + k] - out_data[i * (width * 4) + j + k];

					//Propogate error to other pixels' i_acc
					if (((j / 4) - 1) >= 0)
						acc_data[i * (width * 4) + j + k - 4] += ((float)7 / 16) * e;

					if ((i + 1) < height)
					{
						acc_data[(i + 1) * (width * 4) + j + k] += ((float)5 / 16) * e;

						if (((j / 4) + 1) < width)
							acc_data[(i + 1) * (width * 4) + j + k + 4] += ((float)3 / 16) * e;

						if (((j / 4) - 1) >= 0)
							acc_data[(i + 1) * (width * 4) + j + k - 4] += ((float)1 / 16) * e;
					}
				}
			}
		}
	}

	//Clamp output
	for (int i = 0; i < height * width * 4; i++)
	{
		if (out_data[i] < 0)
			out_data[i] = 0;
		if (out_data[i] > 255)
			out_data[i] = 255;
	}

	//Put dithered image into data
	for (int i = 0; i < height * width * 4; i = i + 4)
	{
		data[i + RED] = out_data[i + RED];
		data[i + GREEN] = out_data[i + GREEN];
		data[i + BLUE] = out_data[i + BLUE];
	}

	return true;
	*/
}// Dither_Color

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

void Filter::Filter_Gaussian_N( unsigned int N )
{
	/*
	unsigned char *temp_data = new unsigned char[width * height * 4];
	memset(temp_data, '\0', width * height * 4);

	double *Gaussian = new double[N];
	
	double sum[3];
	double total;

	for (int k = 0; k < N; k++)
		Gaussian[k] = Binomial(N, k);

	//Loop through image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width * 4; j = j + 4)
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
					if (i + g_i >= 0 && i + g_i < height)
					{
						if (((j / 4) + g_j >= 0) && ((j / 4) + g_j < width))
						{
							//Multiplies two 1-D gaussian filter terms together
							//	Simulating 2-D gaussian filter
							for (int k = 0; k < 3; k++)
								sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * data[(i + g_i) * (width * 4) + (j + g_j * 4) + k];

							total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
						}
					}
				}
			}

			//Assigns value to temporary holding
			//	So as to not mess with other filter calculations
			for (int k = 0; k < 3; k++)
				temp_data[i * (width * 4) + j + k] = (unsigned char) (sum[k] / total);

			//Preserves Alpha value
			temp_data[i * (width * 4) + j + 3] = data[i * (width * 4) + j + 3];
		}
	}

	//Assigns temporary data to real data
	//	Finishing the filter operation
	for (int i = 0; i < height * width * 4; i++)
		data[i] = temp_data[i];

	return true;
	*/
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Filter::Filter_Edge()
{
	/*
	unsigned char *temp_data = new unsigned char[width * height * 4];
	memset(temp_data, '\0', width * height * 4);

	double Gaussian[] = { 1, 4, 6, 4, 1 };
	double sum[3];
	double total;

	//Loop through image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width * 4; j = j + 4)
		{
			//Zero out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;
			total = 0;

			//Computes value of filter over pixel (i, j)
			for (int g_i = -2; g_i < 3; g_i++)
			{
				for (int g_j = -2; g_j < 3; g_j++)
				{
					//Skips values if out of bounds
					if (i + g_i >= 0 && i + g_i < height)
					{
						if (((j / 4) + g_j >= 0) && ((j / 4) + g_j < width))
						{
							//Log the subtraction of 2D Gaussian from pixel value
							for (int k = 0; k < 3; k++)
								sum[k] -= Gaussian[g_i + 2] * Gaussian[g_j + 2] * data[(i + g_i) * (width * 4) + (j + g_j * 4) + k];

							total += Gaussian[g_i + 2] * Gaussian[g_j + 2];
						}
					}
				}
			}

			//Assigns value to temporary holding
			//	So as to not mess with other filter calculations
			for (int k = 0; k < 3; k++)
				temp_data[i * (width * 4) + j + k] = ((data[i * (width * 4) + j + k] * total) + sum[k]) / total;
		
			temp_data[i * (width * 4) + j + 3] = data[i * (width * 4) + j + 3];
		}
	}

	//Assigns temporary data to real data
	//	Finishing the filter operation
	for (int i = 0; i < height * width * 4; i++) {
		data[i] = temp_data[i];

		if (data[i] < 0)
			data[i] = 0;
		if (data[i] > 255)
			data[i] = 255;
	}

	return true;
	*/
}// Filter_Edge


void Filter::zoom()
{}

void Filter::pan()
{}

#endif