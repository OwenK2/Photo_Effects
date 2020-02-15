#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stackEffects.h"

ImageStack::ImageStack::ImageStack() {}
ImageStack::ImageStack(const char* path) {
	addImage(path);
}
ImageStack::ImageStack(std::initializer_list<const char*> paths) {
	for(const char* path : paths) {
		addImage(path);
	}
}
ImageStack::~ImageStack() {
	clearImages();
}

void ImageStack::init(int _w, int _h) {
	w = _w;
	h = _h;
	totalBytes = w * h * CHANNELS;
	data = new byte[totalBytes];
}

void ImageStack::writeImage(const char* path, byte* pixels) {
	int result;
	std::string ext = getExt(path);
	
	if(ext == "png") {
		result = stbi_write_png(path, w, h, CHANNELS, pixels, 0);
	}
	else if(ext == "jpg") {
		result = stbi_write_jpg(path, w, h, CHANNELS, pixels, 100);
	}
	else if(ext == "bmp") {
    result = stbi_write_bmp(path, w, h, CHANNELS, pixels);
	}
	else {
		printf("\e[31m⚠ Failed to create %s, %s is an invalid extension\e[0m\n", path, ext.c_str());
		return;
	}

	
	if(result > 0) {
		printf("\e[32m✓ Created %s\e[0m\n", path);
	}
	else {
		printf("\e[31m⚠ Failed to create %s\e[0m\n", path);
	}
}

void ImageStack::addImage(const char* path) {
	int _w, _h;
	byte* buffer = stbi_load(path, &_w, &_h, NULL, CHANNELS);
	if(buffer == NULL) {
		printf("\e[31m⚠ Failed to read %s\e[0m\n", path);
	}
	else {
		if(numImages != 0 && (w != _w || h != _h)) {
			printf("\e[31m⚠ %s is not the same size as the previous images added. Should be %dx%d but was %dx%d\e[0m\n", path, w, h, _w, _h);
		}
		else {
			numImages ++;
			if(numImages == 1) {
				init(_w, _h);
			}
			else {
				data = (byte*)realloc(data, totalBytes * numImages);
			}
			memcpy(&data[totalBytes*(numImages-1)], buffer, totalBytes);
			printf("\e[32m✓ Loaded %s\e[0m\n", path);
		}
	}
	stbi_image_free(buffer);
}
void ImageStack::clearImages() {
	numImages = 0;
	delete[] data;
}
void ImageStack::medianBlend(const char* path) {
	printf("\n\e[36mPerforming Median Blend...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < w*h;++i) {
		byte* reds = new byte[numImages];
		byte* greens = new byte[numImages];
		byte* blues = new byte[numImages];
		for(int j = 0;j < numImages;++j) {
			reds[j] = data[j*totalBytes+i*CHANNELS];
			greens[j] = data[j*totalBytes+i*CHANNELS+1];
			blues[j] = data[j*totalBytes+i*CHANNELS+2];
		}

		std::sort(reds,reds+numImages); 
		std::sort(greens,greens+numImages); 
		std::sort(blues,blues+numImages); 

		img[i*CHANNELS] = reds[numImages/2];
		img[i*CHANNELS+1] = greens[numImages/2];
		img[i*CHANNELS+2] = blues[numImages/2];

		delete[] reds;
		delete[] greens;
		delete[] blues;
	}

	writeImage(path, img);

	delete[] img;
}
void ImageStack::meanBlend(const char* path) {
	printf("\n\e[36mPerforming Mean Blend...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < w*h;++i) {
		int r = 0, g = 0, b = 0;
		for(int j = 0;j < numImages;++j) {
			r += data[j*totalBytes+i*CHANNELS];
			g += data[j*totalBytes+i*CHANNELS+1];
			b += data[j*totalBytes+i*CHANNELS+2];
		}
		img[i*CHANNELS] = r/numImages;
		img[i*CHANNELS+1] = g/numImages;
		img[i*CHANNELS+2] = b/numImages;
	}

	writeImage(path, img);

	delete[] img;
}

void ImageStack::gammaGray(const char* path) {
	printf("\n\e[36mPerforming Gamma Grayscale Transformation...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < numImages; ++i) {
		memcpy(img, &data[i*totalBytes], totalBytes);
		for(int j = 0;j < w*h;++j) {
			double cLinear = 0.2126*(img[j*CHANNELS]/255.0) + 0.7152*(img[j*CHANNELS+1]/255.0) + 0.0722*(img[j*CHANNELS+2]/255.0);
			if(cLinear <= 0.0031308) {
				memset(&img[j*CHANNELS], (int)(12.92*cLinear*255), 3);
			}
			else {
				memset(&img[j*CHANNELS], (int)((1.055 * pow(cLinear, 1/2.4) - 0.055)*255), 3);
			}
		}
		std::string name = numImages > 1 ? std::to_string(i+1) + "_" + path : std::string(path);
		writeImage(name.c_str(), img);
	}
	delete[] img;
}
void ImageStack::minGray(const char* path) {
	printf("\n\e[36mPerforming Min Grayscale Transformation...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < numImages; ++i) {
		memcpy(img, &data[i*totalBytes], totalBytes);
		for(int j = 0;j < w*h;++j) {
			int min = img[j*CHANNELS];
			if(img[j*CHANNELS+1] < min) {min = img[j*CHANNELS+1];}
			if(img[j*CHANNELS+2] < min) {min = img[j*CHANNELS+2];}
			memset(&img[j*CHANNELS], min, 3);
		}
		std::string name = numImages > 1 ? std::to_string(i+1) + "_" + path : std::string(path);
		writeImage(name.c_str(), img);
	}
	delete[] img;
}
void ImageStack::avgGray(const char* path) {
	printf("\n\e[36mPerforming Average Grayscale Transformation...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < numImages; ++i) {
		memcpy(img, &data[i*totalBytes], totalBytes);
		for(int j = 0;j < w*h;++j) {
			int avg = (img[j*CHANNELS] +  img[j*CHANNELS+1] + img[j*CHANNELS+2])/3;
			memset(&img[j*CHANNELS], avg, 3);
		}
		std::string name = numImages > 1 ? std::to_string(i+1) + "_" + path : std::string(path);
		writeImage(name.c_str(), img);
	}
	delete[] img;
}
void ImageStack::maxGray(const char* path) {
	printf("\n\e[36mPerforming Max Grayscale Transformation...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < numImages; ++i) {
		memcpy(img, &data[i*totalBytes], totalBytes);
		for(int j = 0;j < w*h;++j) {
			int max = img[j*CHANNELS];
			if(img[j*CHANNELS+1] > max) {max = img[j*CHANNELS+1];}
			if(img[j*CHANNELS+2] > max) {max = img[j*CHANNELS+2];}
			memset(&img[j*CHANNELS], max, 3);
		}
		std::string name = numImages > 1 ? std::to_string(i+1) + "_" + path : std::string(path);
		writeImage(name.c_str(), img);
	}
	delete[] img;
}
void ImageStack::colorMask(const char* path, double* mask) {
	printf("\n\e[36mPerforming Color Mask Transformation...\e[0m\n");
	byte* img = new byte[totalBytes];

	for(int i = 0;i < numImages; ++i) {
		memcpy(img, &data[i*totalBytes], totalBytes);
		for(int j = 0;j < w*h;++j) {
			img[j*CHANNELS]   *= mask[0];
			img[j*CHANNELS+1] *= mask[1];
			img[j*CHANNELS+2] *= mask[2];
		}
		std::string name = numImages > 1 ? std::to_string(i+1) + "_" + path : std::string(path);
		writeImage(name.c_str(), img);
	}
	delete[] img;
}

std::string ImageStack::getExt(const char* path) {
	std::string s = std::string(path);
  size_t i = s.rfind('.', s.length());
  if(i != std::string::npos) {
    return(s.substr(i+1, s.length() - i));
  }
  return("");
}