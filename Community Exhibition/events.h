#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "bitmap.h"

using namespace std;

#ifndef _EVENT_HPP
#define _EVENT_HPP

struct collision
{
	float* pos;
	int* color;
};

class Event {
	public:
		Event(Bitmap *b, int start, int duration) : _b(b), _start(start), _duration(duration) {}

		virtual void Activate(int frame_num) = 0;

	protected:
		Bitmap* _b; 
		int _start;
		int _duration;
};

class Circle
{
	public:
		Circle()
		{
			_pos = new float[2];
			_vel = new float[2];
			_rgb = new int[3];

			_pos[0] = ((float) (rand() % 400000)) / 1000;
			_pos[1] = ((float) (rand() % 400000)) / 1000;

			_vel[0] = ((float) (rand() % 1000)) / 2500 - 0.20;
			_vel[1] = ((float) (rand() % 1000)) / 2500 - 0.20;

			_rad = ((float) (rand() % 180) + 10);
			
			if (rand() % 1) 
			{
				_rgb[0] = 40;
				_rgb[1] = 20;
			}
			else 
			{
				_rgb[0] = 20;
				_rgb[1] = 40;
			}


			_rgb[2] = 0;
			

			/*
		
			if (_pos[0] > 300)
				_rgb[0] = 196;
			else if (_pos[0] > 200)
				_rgb[0] = 128;
			else if (_pos[0] > 100)
				_rgb[0] = 64;
			else
				_rgb[0] = 20;

			if (_pos[1] > 300)
				_rgb[2] = 196;
			else if (_pos[1] > 200)
				_rgb[2] = 128;
			else if (_pos[1] > 100)
				_rgb[2] = 64;
			else
				_rgb[2] = 20;

			_rgb[1] = 0;
			*/

			//cout << "Circle at (" << _pos[0] << ", " << _pos[1] << "), moving at <" << _vel[0] << ", " << _vel[1] << ">" <<
			//	    " with color " << _rgb[0] << ", " << _rgb[1] << ", " << _rgb[2] << endl;
		}

		Circle(float* pos, float* vel, float rad, int* rgb) : 
			_pos(pos), _vel(vel), _rad(rad), _rgb(rgb) {}

		~Circle()
		{
			delete[] _pos;
			delete[] _vel;
			delete[] _rgb;
		}

		int	   Update(int width, int height);
		float* Intersect(Circle* c);
		int*   get_color(Circle* c);

		float* get_pos() {return _pos;}
		float  get_rad() {return _rad;}
		int*   get_rgb() {return _rgb;}

	private:

		//Position of center of circle (x, y)
		float* _pos;

		//Velocity of circle
		float* _vel;

		//Radius of circle
		float _rad;

		//Color of circle
		int* _rgb;
};

int Circle::Update(int width, int height)
{
	_pos[0] += _vel[0];
	_pos[1] += _vel[1];

	if ((_pos[0] >= width) || (_pos[0] < 0) || (_pos[1] >= height) || (_pos[1] < 0))
		return 1; //Turn to 0 for images in "buggy"
	else
		return 0; //Turn to 1 for images in "buggy"
}

//Checks intersection with given circle
float* Circle::Intersect(Circle* c)
{
	float distance = sqrt(pow(c->get_pos()[0] - _pos[0], 2) + pow(c->get_pos()[1] - _pos[1], 2));

	if (c->get_rad() + _rad < distance)
		return NULL;

	float* point = new float[2];

	point[0] = (c->get_rad() * c->get_pos()[0] + _rad * _pos[0]) / (c->get_rad() + _rad);
	point[1] = (c->get_rad() * c->get_pos()[1] + _rad * _pos[1]) / (c->get_rad() + _rad);

	//cout << "	Intersection Found" << endl;
	//cout << "		distance = " << distance << endl;
	//cout << "		combined radii = " << c->get_rad() + _rad << endl;
	//cout << "		midway point = (" << point[0] << ", " << point[1] << ")" << endl;

	return point;
}

//Averages color
int* Circle::get_color(Circle* c)
{
	int* ret_rgb = new int[3];
	int* rgb = c->get_rgb();

	for (int i = 0; i < 3; i++) 
		ret_rgb[i] = (int) ((float)rgb[i] + _rgb[i]) / 2;

	return ret_rgb;
}

class Automata : public Event 
{
	public:

		Automata();
		~Automata();

	private:

		int _type;
		int* _args_i;
		float* _args_f;

		//Helper function for Automata
		int* countNeighbors(int i, int j, int AE, int* newRGB, int state_num);

		void automata_2S();		// _type = 1
		void automata_3S();		// _type = 2
};


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


class Animation : public Event {
	public:
		Animation();
		~Animation();

		//Called once per frame, calls Write_Pixel appropriately
		virtual void Activate(int frame_num);   

	private:

		int* _rgb_start;     //Initial color values of animation
		int** _rgb_script;   //Frame by frame delta color values
								//To be added to rgb values each frame

		int* _pix_start;     //Initial coordinates of central pixel
		int** _pix_script;   //Frame by frame delta coordinate values
								//To be added to coordinate each frame

		int _shape;          //One of a predetermined number of shapes
		int* _shape_params;  //Parameters unique to shape 

		//Writes a color value to a pixel
		void Write_Pixel(int* coords, int* rgb);

		//Applies filter inside of shape
		void Apply_Filter(int* coords, int shape);
};


void Animation::Write_Pixel(int* coords, int* rgb)
{
	
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
bool Filter::Dither_FS()
{
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
}// Dither_FS

///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool Filter::Dither_Color()
{
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
}// Dither_Color

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool Filter::Filter_Gaussian_N( unsigned int N )
{
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
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool Filter::Filter_Edge()
{
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
}// Filter_Edge

///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool Filter::NPR_Paint()
{	
	unsigned char *save = new unsigned char[width * height * 4];
	unsigned char *reference = new unsigned char[width * height * 4];

	memcpy(save, data, width * height * 4);

	//Brush Strokes
	int radii[] = {7, 3, 1};

	//Flag variable (to be used in Difference Image Calculation)
	int hasPassed = 0;

	for (int i = 0; i < 3; i++)
	{
		//Clear reference image
		memset(reference, '\0', width * height * 4);

		//apply gaussian blur
		Blur(reference, save, radii[i]);
		
		//paint a layer
		Paint_Layer(reference, radii[i], hasPassed++);
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies gaussian blur of size 2 * R + 1
//
///////////////////////////////////////////////////////////////////////////////
void Filter::Blur(unsigned char *reference, unsigned char* save, int R)
{
	int N = 2 * R + 1;
	double *Gaussian = new double[N];
	double sum[3];
	double total;

	//Calculates 1D Gaussian
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
								sum[k] += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)] * save[(i + g_i) * (width * 4) + (j + g_j * 4) + k];

							total += Gaussian[g_i + (N / 2)] * Gaussian[g_j + (N / 2)];
						}
					}
				}
			}

			//Assigns value to temporary holding
			//	So as to not mess with other filter calculations
			for (int k = 0; k < 3; k++)
				reference[i * (width * 4) + j + k] = (unsigned char) (sum[k] / total); //weighted average

			//Saves alpha value
			reference[i * (width * 4) + j + 3] = save[i * (width * 4) + j + 3];
		}
	}

	//Reference image is now initialized
	//	Is passed back to caller function as such
}

///////////////////////////////////////////////////////////////////////////////
//
//		Helper Function for NPR_Paint
//			Applies paint strokes to canvas
//		
///////////////////////////////////////////////////////////////////////////////
void Filter::Paint_Layer(unsigned char *reference, int R, int hasPassed)
{
	//Create new set of strokes
	vector<Stroke*> S;
	int strokeNum = 0;

	//Sets grid to f_g * R, with f_g = 1
	int grid = R;

	//Sets threshold to 25
	int T = 25;

	//Create difference image
	//	Only holds one value per pixel, so is only width * height
	double *D = new double[width * height];

	//Initializes Difference Image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width * 4; j = j + 4)
		{
			if (hasPassed)
			{
				//Calculates difference image given initailized canvas
				double sum = 0;

				for (int k = 0; k < 3; k++)
					sum += (reference[i * (width * 4) + j + k] - data[i * (width * 4) + j + k]) * (reference[i * (width * 4) + j + k] - data[i * (width * 4) + j + k]);

				D[i * width + (j / 4)] = sqrt(sum);
			}
			else
			{
				//Initializes difference image, in order to initialize canvas
				D[i * width + (j / 4)] = INT_MAX;
			}
		}
	}

	//Generates Strokes
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
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
					if (i_g >= 0 && i_g < height)
					{
						if (j_g >= 0 && j_g < width)
						{
							//Add error to sum
							areaError += D[i_g * width + j_g];

							//Check if error is max
							if (D[i_g * width + j_g] > max)
							{
								max = D[i_g * width + j_g];
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
				//Make Stroke unsigned int iradius, unsigned int ix, unsigned int iy, unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia
				S.push_back(new Stroke(R, max_j, max_i, 
									   reference[max_i * (width * 4) + max_j * 4 + RED], 
									   reference[max_i * (width * 4) + max_j * 4 + GREEN],
									   reference[max_i * (width * 4) + max_j * 4 + BLUE], 
									   reference[max_i * (width * 4) + max_j * 4 + 3]));

				strokeNum++;
			}
		}
	}

	random_shuffle(S.begin(), S.end());

	//Paint all strokes
	for (int i = 0; i < strokeNum; i++)
	{
		Paint_Stroke(*S[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void Filter::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
               data[(y_loc * width + x_loc) * 4 + 0] = s.r;
               data[(y_loc * width + x_loc) * 4 + 1] = s.g;
               data[(y_loc * width + x_loc) * 4 + 2] = s.b;
               data[(y_loc * width + x_loc) * 4 + 3] = s.a;
            } else if (dist_squared == radius_squared + 1) {
               data[(y_loc * width + x_loc) * 4 + 0] = 
                  (data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
               data[(y_loc * width + x_loc) * 4 + 1] = 
                  (data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
               data[(y_loc * width + x_loc) * 4 + 2] = 
                  (data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
               data[(y_loc * width + x_loc) * 4 + 3] = 
                  (data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
            }
         }
      }
   }
}


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

void Filter::zoom()
{}

void Filter::pan()
{}

#endif