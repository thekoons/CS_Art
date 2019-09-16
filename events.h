#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

class Event {
	public:
		Event(int start, int duration);

		virtual void Activate() = 0;

	private:
		int start;
		int duration;
}

class Animation : public Event {
	public:
		Animation();
		~Animation();

		//Called once per frame, calls Write_Pixel appropriately
		void Activate();   

	private:
		int* rgb_start;     //Initial color values of animation
		int** rgb_script;   //Frame by frame delta color values
								//To be added to rgb values each frame

		int* pix_start;     //Initial coordinates of central pixel
		int** pix_script;   //Frame by frame delta coordinate values
								//To be added to coordinate each frame

		int shape;          //One of a predetermined number of shapes
		int* shape_params;  //Parameters unique to shape 

		//Writes a color value to a pixel
		void Write_Pixel(Bitmap& b, int* coords, int* rgb);

		//Applies filter inside of shape
		void Apply_Filter(Bitmap& b, int* coords, int shape);
}

class Filter : public Event {
	public:
		Filter(int type);

		void Activate();

	private:
		int _type;

		void Apply_Filter(Bitmap& b);

		void pixelate(Bitmap& b);
		void grayscale(Bitmap& b);
		void blur(Bitmap& b);
		void cellshade(Bitmap& b);
}

/*** following filters are out of date and will not work ***/


//Cellshades entire map contained by object of class Bitmap
//	Cellshade - round each RGB value individually
//		    such that color palette is reduced
void Filter::cellshade(Bitmap& b) {

	int* temp_rgb;
	float temp_f;

	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {
			temp_rgb = b.getPixel(i, j).getRGB();
			
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
			
			b.getPixel(i, j).setRGB(temp_rgb);
		}
	}
}

//Grayscales entire map contained object of class Bitmap
//	Grayscale - take a weighted average of RGB values
//	            together, such that no color is left
//		    in image. 
//
//	Weighted average ~ 21% R, 72% G, 7% B 
void Filter::grayscale(Bitmap& b) {

	int* temp_rgb;
	float sum;

	//Loops through each pixel in bitmap
	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {

			temp_rgb = b.getPixel(i, j).getRGB();
			//According to the Wikipedia article on grayscaling
			//these constants produce the best greyscale from rgb values
			sum = round(temp_rgb[0] * 0.2126 + temp_rgb[1] * 0.7152 + temp_rgb[2] * 0.0722);

			for (int k = 0; k < 3; k++)
				temp_rgb[k] = (int)sum;
			
			b.getPixel(i, j).setRGB(temp_rgb);
		}
	}
}
	
//Pixellates entire map contained by object of class Bitmap
void Filter::pixelate(Bitmap& b) {
		
	int sum[3];
	int degree = 4;	//Divides image into 16x16 blocks
	int size = pow(2, degree);
	int* temp_rgb;

	//Loops through all pixels in steps determined by degree of pixellation
	for (int i = 0; i < b.getSize(0); i = i + size) {
		for (int j = 0; j < b.getSize(1); j = j + size) {

			//Zeroes out summing intiable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;

			//Loops through all pixels in current step to get average color
			for (int i_p = i; i_p < i + size; i_p++) {
				for (int j_p = j; j_p < j + size; j_p++) {
	
					temp_rgb = b.getPixel(i_p, j_p).getRGB();

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
					b.getPixel(i_p, j_p).setRGB(temp_rgb);
				}
			}
		}
	}
}

void Filter::blur(Bitmap& b) 
{
	int blurArray[] = {1, 4, 6, 4, 1};
	double sum[3];
	int total;
	int* temp_rgb;

	//Loops through all pixels
	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {

			//Zeroes out summing variable
			for (int k = 0; k < 3; k++)
				sum[k] = 0;
			total = 0;

			//Loops through whatever of the gaussian blur matrix is in-bounds 
			//	With gaussian matrix centered on pixel(i, j)
			for (int i_b = -2; i_b < 3; i_b++) {
				for (int j_b = -2; j_b < 3; j_b++) {
					if (i + i_b >= 0 && i + i_b < b.getSize(0)) { //Skips entry if out of bounds
					if (j + j_b >= 0 && j + j_b < b.getSize(1)) {
						
						temp_rgb = b.getPixel(i + i_b, j + j_b).getRGB();
						
						//Multiplies value by gaussian coefficients
						for (int k = 0; k < 3; k++) 
							sum[k] += blurArray[i_b + 2] * blurArray[j_b + 2] * temp_rgb[k];

						//Totals gaussian coefficients (for averaging purposes)
						total += (blurArray[i_b + 2] * blurArray[j_b + 2]);
					}
					}
				}
			}

			temp_rgb = b.getPixel(i, j).getRGB();
			
			//Averages out pixel RGB values
			for (int k = 0; k < 3; k++)
				temp_rgb[k] = sum[k] / total;

			//Assigns nextRGB value to pixel (i, j)
			b.getPixel(i, j).setNextRGB(temp_rgb);
		}
	}

	//Sets all pixels to their nextRGB values
	for (int i = 0; i < b.getSize(0); i++)
		for (int j = 0; j < b.getSize(1); j++)
			b.getPixel(i, j).rectifyRGB();
}