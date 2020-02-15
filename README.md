# Photo Effects
A c++ CLI that allows you to apply various effects to photos. See below for options.

## Usage
imgstack mode [-o outputFile | -m] files

**mode** see below

**-o outputFile**: the name of the file(s) to create use .png, .jpg, or .bmp
As a side note some effects create 1 picture, others create a picture for each element in the stack and are named n_outputFile where n is their location in the stack

**-m # # #**: define 3 floats [0-1] to multiply each pixel's RGB value to respectively (used only for colormask mode); defaults to 1 1 1

**files** just a space delimited list of input files

#### Modes
1. **median**: use median filtering to remove objects from a stack of images
2. **mean**: use mean to blur stack of images ontop of eachother
3. **mingray**: make grayscale image based on min RGB color value
4. **avggray**: make grayscale image based on average RGB color value 
5. **maxgray**: make grayscale image based on max RGB color value 
6. **gammagray**:make grayscale image using Colorimetric (perceptual luminance-preserving) algorithm
7. **colormask**: apply a color mask to each pixel in image stack