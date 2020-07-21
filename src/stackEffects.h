#include <algorithm>
#include <cstdlib>
#include <string>
#include <cmath>
#define CHANNELS 3 //Color channels
typedef unsigned char byte;

class ImageStack {
private:
	int h;
	int w;
	int totalBytes = 0;
	int numImages = 0;
	byte* data = NULL;

	void init(int w, int h);
	void writeImage(const char* path, byte* pixels);
	std::string getExt(const char* path);

public:
	ImageStack();
	ImageStack(const char* path);
	ImageStack(std::initializer_list<const char*> paths);
	~ImageStack();

	void addImage(const char* path);
	void clearImages();

	void medianBlend(const char* path);
	void meanBlend(const char* path);
	void gammaGray(const char* path);
	void minGray(const char* path);
	void avgGray(const char* path);
	void maxGray(const char* path);
	void colorMask(const char* path, double* mask);

};