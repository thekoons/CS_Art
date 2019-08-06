#include <iostream>
#include <vector>

using namespace std;

class BADHEADER : public exception {
	public:
		string msg;
};

class Pixel{
	private:
		int rgb[4];		//rgb value as number between 0-255
		int next_rgb[4];	//rgb values to taken next upon calling rectifyRGB

	public:
		Pixel();
		Pixel(int*);
		Pixel(Pixel*);
		
		int* getRGB();
		void setRGB(int*);
		void setNextRGB(int*);
		void rectifyRGB();
};

class Bitmap
{
private:
    friend istream& operator>>(istream& in, Bitmap& b);
    friend ostream& operator<<(ostream& out, const Bitmap& b);
	
	char header[139];
	int headerLength;
	
	int size[2];
	int depth;
	int maskRef[4];
	
	vector< vector<Pixel> > map;

public:
    Bitmap();
    //Bitmap(const Bitmap&);
    //Bitmap operator=(const Bitmap&);
    //Bitmap(Bitmap&&);
    ~Bitmap();
	
	//For use in overloading insertion operator
	void setHeader(char* p_header, int p_length);
	
	//For use in getting bitmap width and height
	int getSize(int n) {return size[n];}	

	//For use in setting bitmap width and height
	void setSize(int n, int p_size);

	//For use in getting/setting pixel values
	Pixel& getPixel(int i, int j) {return map[i][j];}

	//For use in overloading extraction operator
	Pixel getConstPixel(int i, int j) const {return map[i][j];}

	//For use in scaling bitmap
	vector< vector<Pixel> > getMap() {return map;}
};

void cellShade(Bitmap& b);
void grayscale(Bitmap& b);
void pixelate(Bitmap& b);
void blur(Bitmap& b);
void rotate(Bitmap& b, int n);
void flip(Bitmap& b, int n);
void scale(Bitmap& b, int percent);
