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
	if (argc != 8){
		cout << " Usage: resample imagepath scalefactor tx ty px py theta" << endl;
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
	printf("Translate (%d, %d), then rotate %f theta around (%d, %d), then scale %f.", tx, ty, theta, px, py, f);
	int width2 = width*f, height2 = height*f;

	unsigned char *temp = new unsigned char[width*height];
	memset(temp, 0, width*height);
	
	unsigned char *out = new unsigned char[width2*height2];
	memset(out, 0, width2*height2);
	
	// Translate
	/*for (int i = 0; i<height; i++)
		for (int j = 0; j<width; j++)
			if (i - tx >= 0 && j - ty >=0) out[i*width + j] = in[(i - tx)*width + (j - ty)];
	*/
	//memset(in, 0, width*height);
	// Translate then Rotate around arbitrary
	for (int i = 0; i<height; i++)
		for (int j = 0; j < width; j++){
		int i2 = ((i - px)*((cos(theta*PI / 180))) - (j - py)*((sin(theta*PI / 180))) + (px - tx));
		int j2 = (((i - px)*((sin(theta*PI / 180))) + (j - py)*((cos(theta*PI / 180))) + (py - ty)));
		if (i2 >= 0 && i2 < height && j2 >= 0 && j2 < width){
			//if (i == 200 && j == 200) cout << endl << i2 << endl << j2;
			int t = i2*width + j2;
			temp[i*width + j] = in[t];
		}
		}
	
	// Scale
	
	for (int i = 0; i<height2; i++)
		for (int j = 0; j<width2; j++)
			out[i*width2 + j] = temp[((int)(i / f))*width + ((int)(j / f))];
	
	SavePGM(out, width2, height2, "outstr.pgm");

	delete[]out;
	delete[]in;
	return 0;
}
