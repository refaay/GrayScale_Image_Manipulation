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
	if (argc != 5){
		cout << " Usage: resample imagepath Smoothing Gradient Sharpening" << endl;
		return -1;
	}
	unsigned char *in;
	int width, height;
	if (0<ReadPGM(argv[1], &in, &width, &height)){
		cout << "Unable to read " << argv[1] << endl;
		return -1;
	}
	cout << "image: " << argv[1] << ", " << width << " x " << height << " pixels" << endl;

	int sm = atoi(argv[2]);
	int gr = atoi(argv[3]);
	int sh = atoi(argv[4]);

	printf("Smoothing = %d, Gradient = %d, Sharpening = %d.\n", sm, gr, sh);

	unsigned char *out = new unsigned char[width*height];
	memset(out, 0, width*height);

	// Histogram equalization
	if (sm){
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++){
			int av = 0, count = 0;
			if (i > 0) {
				if (j > 0){ av += in[(i - 1)*width + (j - 1)]; count++; }
				av += in[(i - 1)*width + (j)]; count++;
				if (j < width - 1){ av += in[(i - 1)*width + (j + 1)]; count++; }
			}
			if (j > 0){ av += in[(i)*width + (j - 1)]; count++; }
			av += in[(i)*width + (j)]; count++;
			if (j < width - 1){ av += in[(i)*width + (j + 1)]; count++; }
			if (i < height - 1) {
				if (j > 0){ av += in[(i + 1)*width + (j - 1)]; count++; }
				av += in[(i + 1)*width + (j)]; count++;
				if (j < width - 1){ av += in[(i + 1)*width + (j + 1)]; count++; }
			}
			out[i*width + j] = av / count;
			}
	}
	else if (gr){
		//unsigned char *temp = new unsigned char[width*height];
		//memset(temp, 0, width*height);
		int avx = 0, avy = 0, count1 = 0, count2 = 0;
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				avx = 0, avy = 0, count1 = 0, count2 = 0;
				// x-gradient
				if (i > 0) {
					if (j > 0){ avx -= in[(i - 1)*width + (j - 1)]; count1++; }
					avx -= in[(i - 1)*width + (j)]; count1++;
					if (j < width - 1){ avx -= in[(i - 1)*width + (j + 1)]; count1++; }
				}
				if (i < height - 1) {
					if (j > 0){ avx += in[(i + 1)*width + (j - 1)]; count1++; }
					avx += in[(i + 1)*width + (j)]; count1++;
					if (j < width - 1){ avx += in[(i + 1)*width + (j + 1)]; count1++; }
				}
				// y-gradient
				if (j > 0) {
					if (i > 0){ avy -= in[(i - 1)*width + (j - 1)]; count2++; }
					avy -= in[(i)*width + (j - 1)]; count2++;
					if (i < height - 1){ avy -= in[(i + 1)*width + (j - 1)]; count2++; }
				}
				if (j < width - 1) {
					if (i > 0){ avy += in[(i - 1)*width + (j + 1)]; count2++; }
					avy += in[(i)*width + (j + 1)]; count2++;
					if (i < height - 1){ avy += in[(i + 1)*width + (j + 1)]; count2++; }
				}
				out[i*width + j] = abs(avx / count1) + abs(avy / count2);
				//temp[i*width + j] = abs(av / count) == 0 ? 0 : 255;
			}

		}
	}
	else if (sh){
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++){
			int av = 0, count = 0;
			if (i > 0) {
				if (j > 0){ av -= in[(i - 1)*width + (j - 1)]; count--; }
				av -= in[(i - 1)*width + (j)]; count--;
				if (j < width - 1){ av -= in[(i - 1)*width + (j + 1)]; count--; }
			}
			if (j > 0){ av -= in[(i)*width + (j - 1)]; count--; }
			av +=(9 * in[(i)*width + (j)]); count+=9;
			if (j < width - 1){ av -= in[(i)*width + (j + 1)]; count--; }
			if (i < height - 1) {
				if (j > 0){ av -= in[(i + 1)*width + (j - 1)]; count--; }
				av -= in[(i + 1)*width + (j)]; count--;
				if (j < width - 1){ av -= in[(i + 1)*width + (j + 1)]; count--; }
			}
			out[i*width + j] = (av / count) > 255 ? 255 : (av / count) < 0 ? 0 : (av / count);
			//out[i*width + j] = (av / count);
			}
	}
	else for (int i = 0; i < height*width; i++) out[i] = in[i];
	SavePGM(out, width, height, "outsm.pgm");

	delete[]out;
	delete[]in;
	return 0;
}
