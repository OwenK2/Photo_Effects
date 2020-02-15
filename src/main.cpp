#include "stackEffects.h"


int main(int argc, char** argv) {

	ImageStack stack;
	bool namedOutput = false;
	double mask[3] = {1,1,1};
	char output[255];

	if(argc > 2) {
		for(int i = 2;i < argc; ++i) {
			if(strcmp(argv[i], "-o") == 0) {
				strcpy(output, argv[++i]);
				namedOutput = true;
			}
			else if(strcmp(argv[i], "-m") == 0) {
				mask[0] = std::stod(argv[++i]);
				mask[1] = std::stod(argv[++i]);
				mask[2] = std::stod(argv[++i]);
			}
			else {
				stack.addImage(argv[i]);
			}
		}
		if(!namedOutput) {
			strcpy(output, "output.png");
		}

		if(strcmp(argv[1], "median") == 0) {
			stack.medianBlend(output);
		}
		else if(strcmp(argv[1], "mean") == 0) {
			stack.meanBlend(output);
		}
		else if(strcmp(argv[1], "mingray") == 0) {
			stack.minGray(output);
		}
		else if(strcmp(argv[1], "avggray") == 0) {
			stack.avgGray(output);
		}
		else if(strcmp(argv[1], "maxgray") == 0) {
			stack.maxGray(output);
		}
		else if(strcmp(argv[1], "gammagray") == 0) {
			stack.gammaGray(output);
		}
		else if(strcmp(argv[1], "colormask") == 0) {
			stack.colorMask(output, mask);
		}
		else {
			printf("\e[31mUnreconized Mode %s \nUsage: imgstack <mode> [-o outputFile] <files> \n\tModes: median, mean, mingray, avggray, maxgray, gammagray, colorMask\e[0m\n", argv[1]);
		}
	}
	else {
		printf("\e[31mUsage: imgstack <mode> [-o outputFile] <files> \n\tModes: median, mean, mingray, avggray, maxgray, gammagray, colorMask\t-m redMask blueMask greenMask (0-1 used only for colorMask mode)\e[0m\n");
	}

	return 0;
}
