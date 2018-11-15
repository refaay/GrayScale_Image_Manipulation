#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cmath>
using namespace std;

#define PI 3.14159265

int ReadPGM(const char* filename, unsigned char **buff, int *wid, int *hi)
{
	FILE *inFile = fopen(filename, "rb");
	if (inFile == NULL)return -1;
	*wid = 0, *hi = 0;
	char str[100];
	fgets(str, 100, inFile);
	fgets(str, 100, inFile);
	fscanf(inFile, "%d %d\r\n255\r\n", wid, hi);
	fseek(inFile, -*wid**hi, SEEK_END);
	*buff = new unsigned char[*wid**hi];
	fread(*buff, 1, *wid**hi, inFile);
	fclose(inFile);
	return 0;
}

void SavePGM(const unsigned char *pbSrc2, int SrcWid, int SrcHi, const char* filename)
{
	FILE *outFile = fopen(filename, "wb");
	char st[100];
	sprintf(st, "P5\r\n%d %d\r\n255\r\n", SrcWid, SrcHi);
	fwrite(st, 1, strlen(st), outFile);
	fwrite(pbSrc2, 1, SrcWid*SrcHi, outFile);
	fclose(outFile);
}

int main(int argc, char** argv)
{
	if (argc != 10){
		cout << " Usage: resample imagepath scalefactor tx ty px py theta lambda histogram" << endl;
		return -1;
	}
	unsigned char *in;
	int width, height;
	if (0<ReadPGM(argv[1], &in, &width, &height)){
		cout << "Unable to read " << argv[1] << endl;
		return -1;
	}
	cout << "image: " << argv[1] << ", " << width << " x " << height << " pixels" << endl;

	float f = atof(argv[2]);
	int tx = atoi(argv[3]);
	int ty = atoi(argv[4]);
	int px = atoi(argv[5]);
	int py = atoi(argv[6]);
	float theta = atof(argv[7]);
	float y = atof(argv[8]);
	int his = atoi(argv[9]);
	theta = -theta;
	printf("Translate (%d, %d), then rotate %f theta around (%d, %d), then scale %f. Power transformation: y = %f. Histogram = %d\n", tx, ty, -theta, px, py, f, y, his);
	int width2 = width*f, height2 = height*f;

	unsigned char *temp = new unsigned char[width*height];
	memset(temp, 0, width*height);
	
	unsigned char *out1 = new unsigned char[width2*height2];
	memset(out1, 0, width2*height2);

	unsigned char *out = new unsigned char[width2*height2];
	memset(out, 0, width2*height2);
	
	// Translate
	/*for (int i = 0; i<height; i++)
		for (int j = 0; j<width; j++)
			if (i - tx >= 0 && j - ty >=0) out[i*width + j] = in[(i - tx)*width + (j - ty)];
	*/
	

	// Translate then Rotate around arbitrary
	for (int i = 0; i<height; i++)
		for (int j = 0; j < width; j++){
		int i2 = ((i - px)*((cos(theta*PI / 180))) - (j - py)*((sin(theta*PI / 180))) + (px - tx));
		int j2 = (((i - px)*((sin(theta*PI / 180))) + (j - py)*((cos(theta*PI / 180))) + (py - ty)));
		if (i2 >= 0 && i2 < height && j2 >= 0 && j2 < width){
			//if (i == 200 && j == 200) cout << endl << i2 << endl << j2;
			int t = i2*width + j2;
			//temp[t] = in[i*width + j]; // mfr4
			temp[i*width + j] = in[t];
		}
		}

	// Scale
	for (int i = 0; i<height2; i++)
		for (int j = 0; j < width2; j++){
		//int yt = temp[((int)(i / f))*width + ((int)(j / f))]; // original
		//int yt = abs(255-temp[((int)(i / f))*width + ((int)(j / f))]); // -ve
		//int yt = 8*log2(1 + temp[((int)(i / f))*width + ((int)(j / f))]); // log
		//int yt = 2^(1 + temp[((int)(i / f))*width + ((int)(j / f))]); // inverse log
		int yt = pow(temp[((int)(i / f))*width + ((int)(j / f))], y) * pow(255,  (1 - y)); // nth power (c = 255^(1-y))
		//int yt = temp[((int)(i / f))*width + ((int)(j / f))] ^ 2; // power
		out1[i*width2 + j] = yt > 255 ? 255 : (yt < 0 ? 0 : yt);
		}
	
	// Histogram equalization
	if (his){
		float intensity[256]; // pixels count array by intensity
		//memset(intensity, 0, 256);
		for (int i = 0; i < 256; i++) intensity[i] = 0;
		for (int j = 0; j < width2*height2; j++){
			intensity[out1[j]]++;
		}

		float prob[256]; // probabilities of each intensity
		for (int i = 0; i < 256; i++) { 
			prob[i] = intensity[i] / (width2*height2); 
			//cout << "intensity " << i << " " << intensity[i] << endl;
		}

		float cp[256]; // cumulative probability array
		cp[0] = prob[0];
		for (int i = 1; i < 256; i++) cp[i] = cp[i - 1] + prob[i];

		int flr[256]; // floored array
		for (int i = 0; i < 256; i++) flr[i] = floor(cp[i] * 255);

		for (int j = 0; j < width2*height2; j++){ // reassigning histogram result
			out[j] = flr[out1[j]];
		}
	}
	else out = out1;
	SavePGM(out, width2, height2, "outno1.pgm");

	delete[]out;
	delete[]in;
	return 0;
}
