#include <iostream>
#include <string.h>
#include <math.h>
#include "bitmap.h"

Pixel::Pixel() {
	for (int i = 0; i < 4; i++) {
		rgb[i] = 0;
		next_rgb[i] = 0;
	}
}

Pixel::Pixel(int* p_rgb) {
	for (int i = 0; i < 4; i++) {
		rgb[i] = p_rgb[i];
		next_rgb[i] = 0;
	}
}

Pixel::Pixel(Pixel* p) {
	int* p_rgb = p->getRGB();
	for (int i = 0; i < 4; i++) {
		rgb[i] = p_rgb[i];
		next_rgb[i] = 0;
	}
}

int* Pixel::getRGB() {return rgb;}

void Pixel::setRGB(int* p_rgb) {
	for (int i = 0; i < 4; i++)
		rgb[i] = p_rgb[i];
}
									 
void Pixel::setNextRGB(int* p_rgb) {
	for (int i = 0; i < 4; i++)
		next_rgb[i] = p_rgb[i];
}

void Pixel::rectifyRGB() {
	for (int i = 0; i < 4; i++)
		rgb[i] = next_rgb[i];

	for (int i = 0; i < 4; i++)	
		next_rgb[i] = 0;
}

Bitmap::Bitmap() {
}

Bitmap::~Bitmap() {
	for (int i = 0; i < size[0]; i++) 
		for (int j = 0; j < size[1]; j++) 
			map[i].pop_back();				
}

//Sets header member of class bitmap
void Bitmap::setHeader(char* p_header, int p_length) {
	memset(header, '\0', 139);
	memcpy(header, p_header, p_length);	
	headerLength = p_length;
}

//Sets size of bitmap 
//	both in variable and in header
void Bitmap::setSize(int n, int p_size) 
{
	char temp;

	size[n] = p_size;		//Sets variable
	for (int i = 0; i < 4; i++){ 	//Sets header
		temp = (char)(p_size >> 8*i) & 0xff;
		header[18 + i + (n - 1) * 4] = temp;
		     //18 = start of width/height 
		     //i iterates through 4 bytes 
		     //(n - 1) * 4 selects either hieght or width
		     //(p_size >> 8*i) & 0xff translates integer into char array byte by byte
	}
}	

//Cellshades entire map contained by object of class Bitmap
//	Cellshade - round each RGB value individually
//		    such that color palette is reduced
void cellShade(Bitmap& b) {

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
void grayscale(Bitmap& b) {

	int* temp_rgb;
	float sum;

	//Loops through each pixel in bitmap
	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {

			temp_rgb = b.getPixel(i, j).getRGB();
			//According to the Wikipedia article on grayscaling says that these
			//constants produce the best greyscale from rgb values
			sum = round(temp_rgb[0] * 0.2126 + temp_rgb[1] * 0.7152 + temp_rgb[2] * 0.0722);

			for (int k = 0; k < 3; k++)
				temp_rgb[k] = (int)sum;
			
			b.getPixel(i, j).setRGB(temp_rgb);
		}
	}
}
	

//Pixellates entire map contained by object of class Bitmap
void pixelate(Bitmap& b) {
		
	int sum[3];
	int degree = 4;	//Divides image into 16x16 blocks
	int size = pow(2, degree);
	int* temp_rgb;

	//Loops through all pixels in steps determined by degree of pixellation
	for (int i = 0; i < b.getSize(0); i = i + size) {
		for (int j = 0; j < b.getSize(1); j = j + size) {

			//Zeroes out summing variable
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

void blur(Bitmap& b) 
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

void rotate(Bitmap& b, int n) {
/*
	if (n <= 0) return;

	int d_i, d_j;

	//Loops through each pixel in bitmap
	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {
			
			//Determines destination pixel coordinates
			d_i = j;
			d_j = b.getSize(0) - i;
			
			//Sets nextRGB of destination pixel
			//	(so as not to overwrite data before operation completes)
			b.getPixel(d_i, d_j).setNextRGB(b.getPixel(i, j).getRGB());
		}
	}
	
	//Sets all pixels to their nextRGB values	
	for (int i = 0; i < b.getSize(0); i++)
		for (int j = 0; j < b.getSize(1); j++)
			b.getPixel(i, j).rectifyRGB();

	rotate(b, n--);
*/
}


void flip(Bitmap& b, int n) {
/*
	int d_i, d_j; //Destination coordinates

	//Loops through each pixel in bitmap
	for (int i = 0; i < b.getSize(0); i++) {
		for (int j = 0; j < b.getSize(1); j++) {
			
			//Sets destination coordinates given which flip to perform
			switch(n) {
				case 1: //flip over x-axis	
					d_i = b.getSize(0) - i - 1;
					d_j = j;
					break;
				case 2: //flip over y-axis
					d_i = i;
					d_j = b.getSize(1) - j - 1;
					break;
				case 3: //flip over upper left to lower right diag
					d_i = b.getSize(1) - j - 1;
					d_j = b.getSize(0) - i - 1;
					break;
				case 4: //flip over upper right to lower left diag
					d_i = j;
					d_j = i;
					break;
			}
		
			//Sets the destination pixel's next values 
			//	(so as not to overwrite data before operation completes)
			b.getPixel(d_i, d_j).setNextRGB(b.getPixel(i, j).getRGB());
		}
	}
	
	//Sets all pixels to their nextRGB values
	for (int i = 0; i < b.getSize(0); i++)
		for (int j = 0; j < b.getSize(1); j++)
			b.getPixel(i, j).rectifyRGB();
*/
}

//Scales entire map contained in object of class Bitmap
void scale(Bitmap& b, int percent) 
{
/*
	int count;
	int size[2];
	int temp_rgb[4];

	vector< vector<Pixel> > map = b.getMap();
	vector< vector<Pixel> >::iterator pos_i;
	vector<Pixel>::iterator pos_j;

	//Adjust size to specified percentage of former size
	for (int i = 0; i < 2; i++) {
		size[i] = b.getSize(i);
		size[i] *= percent;
		size[i] /= 100;
	}

	cout << "Growing image from " << b.getSize(0) << ", " << b.getSize(1);
	cout << " to " << size[0] << ", " << size[1] << endl;
	
	//Scaling image up
	if (percent >= 100) {
		//Scale image vertically by adding horizontal lines of pixels 
		count = 0;
		for (int i = 0; i < b.getSize(0); i++) {
			//Add horizontal Line
			vector<Pixel> line;			

			for (int j = 0; j < b.getSize(1); j++) {
				Pixel* p = new Pixel(b.getPixel(i, j));
				line.push_back(p);
			}		
			
			cout << "Adding new line at i = " << i << endl;

			pos_i = map.begin();
			map.insert(pos_i + i, line);						
		}

		//Scale image horizontally by adding vertical lines of pixels
		for (int j = 0; j < b.getSize(1); j++) {
			//Add vertical Line
			cout << "Adding new line at j = " << j << endl;
			for (int i = 0; i < b.getSize(0); i++) {
				Pixel* p = new Pixel(b.getPixel(i, j));
				pos_j = map[i].begin();
				map[i].insert(pos_j + j, *p);
			}
		}
	} else {
		for (int i = 0; i < b.getSize(0); i++) {
			if (rand() % 100 + 1 >= percent) {
				//Rem Line		
			}
		}
		for (int j = 0; j < b.getSize(1); j++) {
			if (rand() % 100 + 1 >= percent) {
				//Rem Line
			}
		}
	}

	b.setSize(0, size[0]);
	b.setSize(1, size[1]);
*/	
}

//Translates mask to an integer value corresponding to the "turned on" byte
//	MASK MUST BE OF TYPE ONE-HOT
int getMask(char* mask) 
{
	for (int i = 0; i < 4; i++) {
		if (mask[i] != '\0') {  
			return 4 - i; //takes into account endianness
		}
	}
	return 0;
}

//Reads in Bitmap file to object of class Bitmap
istream& operator>>(istream& in, Bitmap& b)
{
	BADHEADER e;
	char tag[2];
	uint32_t info[6];

//First Header
	in >> tag[0] >> tag[1];			//Bitmap Tag		2 bytes - TOSS/CHECK
		if (strncmp(tag, "BM", 2) != 0)	throw e; 
	in.read((char*)&info[0], 4);		//Size of Bitmap	4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore(); 			//garbage 		4 bytes - TOSS	
	for (int i = 0; i < 4; i++)
		in.ignore();			//offset		4 bytes - TOSS

//Second Header	
	for (int i = 0; i < 4; i++)
		in.ignore();			//size of 2nd header	4 bytes - TOSS
	in.read((char*)&info[1], 4);		//Image width		4 bytes - KEEP
		b.size[1] = info[1];
	in.read((char*)&info[2], 4);		//Image height		4 bytes - KEEP
		b.size[0] = info[2];
	in.read((char*)&info[3], 2);		//# of color planes	2 bytes - TOSS/CHECK
		if (info[3] != 1) throw e;	
	in.read((char*)&info[4], 2);		//Color depth		2 bytes - KEEP/CHECK
		if (info[4] != 24 && info[4] != 32) throw e;
		b.depth = info[4];
	in.read((char*)&info[5], 4);		//Compression		4 bytes - TOSS/CHECK
		if (info[5] != 0 && info[5] != 3)  throw e;
		if (info[5] == 0 && info[4] != 24) throw e;
		if (info[5] == 3 && info[4] != 32) throw e;	
	for (int i = 0; i < 4; i++)
		in.ignore();			//size of raw data	4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();			//horz res 		4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();			//vert res		4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();			//color pallete		4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();			//Important colors	4 bytes - TOSS

//Masks - only included if color depth == 32
	char** masks;
	char maskOrder[4];
	int maskRef[4];

	if (b.depth == 32) {

		masks = new char*[4];
		for (int i = 0; i < 4; i++) 
			masks[i] = new char[4];

		in.read(masks[0], 4);		//Mask 1		4 bytes - KEEP
		in.read(masks[1], 4);		//Mask 2		4 bytes - KEEP
		in.read(masks[2], 4);		//Mask 3		4 bytes - KEEP
		in.read(masks[3], 4);		//Mask 4		4 bytes - KEEP
		for (int i = 0; i < 4; i++)
			in >> maskOrder[i];	//Mask Order		4 bytes - KEEP
		for (int i = 0; i < 64; i++)
			in.ignore();		//Color Space Info	64bytes - TOSS

		//Set maskRef to deal with bit masking input
		//Set b.maskRef to deal with bit masking output
		for (int i = 0; i < 4; i++) {
			     if (maskOrder[i] == 'R') {
				maskRef[i] = getMask(masks[0]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 'G') {
				maskRef[i] = getMask(masks[1]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 'B') {
				maskRef[i] = getMask(masks[2]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 's') {
				maskRef[i] = getMask(masks[3]);
				b.maskRef[maskRef[i]] = i;
			}
			else 
				throw e;
		}
	}

//Store header
	int length = in.tellg();	//store position in input stream as 'length'	
	char header[length];  		//create header var of size 'length'
	in.seekg(0, in.beg);		//retrace steps through input stream
	in.read(header, length);	//read in data to 'length'
	b.setHeader(header, length);	//store header in object of class Bitmap

//Store pixel information
	uint32_t pixleData[4];
	int rgb[4];

	for (int i = 0; i < b.size[0]; i++) {
	
		vector<Pixel> line;			
		
		for (int j = 0; j < b.size[1]; j++) {
			
			if (b.depth == 32) {	//Reads 32 bit pixel
				for (int k = 0; k < 4; k++) { 
					in.read((char*) &pixleData[k], 1);
					rgb[maskRef[k]] = pixleData[k];
				}	
			} else {		//Reads 24 bit pixel
				for (int k = 0; k < 3; k++) {
					in.read((char*) &pixleData[k], 1);
					rgb[k] = pixleData[k];
				}
			}

			Pixel* p = new Pixel(rgb);
			line.push_back(*p);
		}

		b.map.push_back(line);	

		//Skip input padding, if applicable
		if (b.depth == 24) {
			for (int j = 0; j < (b.size[1] % 4); j++)
				in.ignore();	
		}
	}

	return in;
}

//Converts integer to binary char array
char* getBinary(int integer) {
	char* binary = new char[1];	
	binary[0] = integer & 0xff;
	return binary;
}

ostream& operator<<(ostream& out, const Bitmap& b) {
	//Write bitmap back to file

	char* binary;
	int* rgb;
	int alpha;

	out.write(b.header, b.headerLength);

	for (int i = 0; i < b.size[0]; i++) {
		for (int j = 0; j < b.size[1]; j++) {

			rgb = b.getConstPixel(i, j).getRGB();

			if (b.depth == 32) {		//Writes 32 bit pixel
				for (int k = 0; k < 4; k++) {
					binary = getBinary(rgb[b.maskRef[k]]);
					out.write(binary, 1);
					delete[] binary;
				}
			} else {			//Writes 24 bit pixel
				for (int k = 0; k < 3; k++) {
					binary = getBinary(rgb[k]);
					out.write(binary, 1);
					delete[] binary;
				}
			}
		}	
		
		//Pad output with zeros, if applicable
		if (b.depth == 24) {
			int padding = b.size[1] % 4;

			if (padding) {
				binary = new char[padding];
				memset(binary, '\0', padding);
				out.write(binary, padding);
				delete[] binary;
			}	
		}
	}

	return out;
}
