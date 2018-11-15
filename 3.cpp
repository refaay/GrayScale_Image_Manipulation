#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <ctime>
#include <chrono>
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
	if (argc != 3){
		cout << " Usage: resample imagepath Mode" << endl;
		return -1;
	}
	unsigned char *in;
	int width, height;
	if (0<ReadPGM(argv[1], &in, &width, &height)){
		cout << "Unable to read " << argv[1] << endl;
		return -1;
	}
	cout << "image: " << argv[1] << ", " << width << " x " << height << " pixels" << endl;

	int mode = atoi(argv[2]);

	printf("Mode = %d.\n", mode);

	unsigned char *out = new unsigned char[width*height];
	memset(out, 0, width*height);

	// Histogram equalization
	if (mode == 0){
		cout << "Enter 9 elements for the 2D filter matrix row by row: ";
		int mat[9];
		for (int i = 0; i < 9; i++){
			cin >> mat[i];
		}
		auto started = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++){
			int av = 0, count = 0;
			if (i > 0) {
				if (j > 0){ av += mat[0] * in[(i - 1)*width + (j - 1)]; count += mat[0]; }
				av += mat[1] * in[(i - 1)*width + (j)]; count += mat[1];
				if (j < width - 1){ av += mat[2] * in[(i - 1)*width + (j + 1)]; count += mat[2]; }
			}
			if (j > 0){ av += mat[3] * in[(i)*width + (j - 1)]; count += mat[3]; }
			av += mat[4] * in[(i)*width + (j)]; count += mat[4];
			if (j < width - 1){ av += mat[5] * in[(i)*width + (j + 1)]; count += mat[5]; }
			if (i < height - 1) {
				if (j > 0){ av += mat[6] * in[(i + 1)*width + (j - 1)]; count += mat[6]; }
				av += mat[7] * in[(i + 1)*width + (j)]; count += mat[7];
				if (j < width - 1){ av += mat[8] * in[(i + 1)*width + (j + 1)]; count += mat[8]; }
			}
			out[i*width + j] = (count == 0) ? 0 : (av / count) > 255 ? 255 : (av / count) < 0 ? 0 : (av / count);
			}
		auto done = std::chrono::high_resolution_clock::now();
		cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count() << " millisecond." << endl;
	}
	else if (mode == 1){
		char c;
		cout << "Enter r if row * col or c if col * row: ";
		cin >> c;
		if (c == 'c'){
			cout << "Enter 3 elements for the 1D col followed by the 3 elements for the row: ";
			int col[3], row[3];
			for (int i = 0; i < 3; i++){
				cin >> col[i];
			}
			for (int i = 0; i < 3; i++){
				cin >> row[i];
			}
			auto started = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++){
				int av = 0, count = 0;
				if (i > 0) {
					if (j > 0){ av += col[0] * row[0] * in[(i - 1)*width + (j - 1)]; count += col[0] * row[0]; }
					av += col[0] * row[1] * in[(i - 1)*width + (j)]; count += col[0] * row[1];
					if (j < width - 1){ av += col[0] * row[2] * in[(i - 1)*width + (j + 1)]; count += col[0] * row[2]; }
				}
				if (j > 0){ av += col[1] * row[0] * in[(i)*width + (j - 1)]; count += col[1] * row[0]; }
				av += col[1] * row[1] * in[(i)*width + (j)]; count += col[1] * row[1];
				if (j < width - 1){ av += col[1] * row[2] * in[(i)*width + (j + 1)]; count += col[1] * row[2]; }
				if (i < height - 1) {
					if (j > 0){ av += col[2] * row[0] * in[(i + 1)*width + (j - 1)]; count += col[2] * row[0]; }
					av += col[2] * row[1] * in[(i + 1)*width + (j)]; count += col[2] * row[1];
					if (j < width - 1){ av += col[2] * row[2] * in[(i + 1)*width + (j + 1)]; count += col[2] * row[2]; }
				}
				out[i*width + j] = (count == 0) ? 0 : (av / count) > 255 ? 255 : (av / count) < 0 ? 0 : (av / count);
				}
			auto done = std::chrono::high_resolution_clock::now();
			cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count() << " millisecond." << endl;
		}
		else{
			cout << "Enter 3 elements for the 1D row followed by the 3 elements for the col: ";
			int col[3], row[3];
			for (int i = 0; i < 3; i++){
				cin >> row[i];
			}
			for (int i = 0; i < 3; i++){
				cin >> col[i];
			}
			auto started = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++){
				int av = 0, count = 0;
				if (i > 0) {
					if (j > 0){ av += col[0] * row[0] * in[(i - 1)*width + (j - 1)]; count += col[0] * row[0]; }
					av += col[0] * row[1] * in[(i - 1)*width + (j)]; count += col[0] * row[1];
					if (j < width - 1){ av += col[0] * row[2] * in[(i - 1)*width + (j + 1)]; count += col[0] * row[2]; }
				}
				if (j > 0){ av += col[1] * row[0] * in[(i)*width + (j - 1)]; count += col[1] * row[0]; }
				av += col[1] * row[1] * in[(i)*width + (j)]; count += col[1] * row[1];
				if (j < width - 1){ av += col[1] * row[2] * in[(i)*width + (j + 1)]; count += col[1] * row[2]; }
				if (i < height - 1) {
					if (j > 0){ av += col[2] * row[0] * in[(i + 1)*width + (j - 1)]; count += col[2] * row[0]; }
					av += col[2] * row[1] * in[(i + 1)*width + (j)]; count += col[2] * row[1];
					if (j < width - 1){ av += col[2] * row[2] * in[(i + 1)*width + (j + 1)]; count += col[2] * row[2]; }
				}
				out[i*width + j] = (count == 0) ? 0 : (av / count) > 255 ? 255 : (av / count) < 0 ? 0 : (av / count);
				}
			auto done = std::chrono::high_resolution_clock::now();
			cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count() << " millisecond." << endl;
		}
	}
	else for (int i = 0; i < height*width; i++) out[i] = in[i];
	SavePGM(out, width, height, "outsm.pgm");

	delete[]out;
	delete[]in;
	
	
	return 0;
}
