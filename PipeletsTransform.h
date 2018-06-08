#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <ctime>
#include "BinaryFile.h"
#include "HuffmanCoder.h"

using namespace std;

#define SETVER 1

#define LARGESZ
#ifdef LARGESZ
#define IMGSZR 800
#define IMGSZC 600
#else
#define IMGSZR 713
#define IMGSZC 714
#endif
#define WMAX 129
#define DEPMAX 12
#if (_MSC_VER < 1800)
#define round(x) floor((x)+0.5)
#endif
#define GRAYBIT 8
#define LAMBDALEN 1000
#define FPIXELLEN DEPMAX
#define BINARYSTREAMSZ 80

class BinaryStream
{
public:
	BinaryStream()
	{
		memset(bits, 0, sizeof(bits));
	}

	void SetBit(int pos)
	{
		int bytepos = (pos >> 3);
		int bitpos = pos - (bytepos << 3);
		bits[bytepos] |= (1 << bitpos);
	}

	void UnsetBit(int pos)
	{
		int bytepos = (pos >> 3);
		int bitpos = pos - (bytepos << 3);
		bits[bytepos] &= (unsigned char)(0xFF - (1 << bitpos));
	}

	bool State(int pos)
	{
		int bytepos = (pos >> 3);
		int bitpos = pos - (bytepos << 3);
		if ((bits[bytepos] & (1 << bitpos)) > 0)
			return true;
		else
			return false;
	}

	void ReverseBit(int pos)
	{
		int bytepos = (pos >> 3);
		int bitpos = pos - (bytepos << 3);
		if ((bits[bytepos] & (1 << bitpos)) > 0)
			bits[bytepos] &= (unsigned char)(0xFF - (1 << bitpos));
		else
			bits[bytepos] |= (1 << bitpos);
	}

	BinaryStream operator = (BinaryStream bs)
	{
		for (int i = 0; i < BINARYSTREAMSZ; i++)
			bits[i] = bs.bits[i];
		return *this;
	}
private:
	unsigned char bits[BINARYSTREAMSZ];
};

extern bool EnableConstantDep;
extern bool EnableWedgeletDep;
extern bool EnablePipeletDep;
extern bool EnableKleinletDep;

extern int DoTransformMaxDepth;
extern bool ShowMessages;

extern double LambdaArray[LAMBDALEN];
extern int LambdaLength;

extern int img[IMGSZR][IMGSZC];
extern int rimg[IMGSZR][IMGSZC];
extern bool imgmask[IMGSZR][IMGSZC];
extern int imgw[IMGSZR][IMGSZC][DEPMAX];
extern int imgp[IMGSZR][IMGSZC][DEPMAX];
extern int imgk[IMGSZR][IMGSZC][DEPMAX];
extern int imgc[IMGSZR][IMGSZC][DEPMAX];
extern int imgwrd[IMGSZR][IMGSZC];
extern int imgprd[IMGSZR][IMGSZC];
extern int imgkrd[IMGSZR][IMGSZC];
extern int imgwrdg[IMGSZR][IMGSZC];
extern int imgprdg[IMGSZR][IMGSZC];
extern int imgkrdg[IMGSZR][IMGSZC];

extern int wedn1[4*WMAX][4*WMAX];
extern int wedn2[4*WMAX][4*WMAX];
extern long long weds1[4*WMAX][4*WMAX];
extern long long weds2[4*WMAX][4*WMAX];
extern long long wedp1[4*WMAX][4*WMAX];
extern long long wedp2[4*WMAX][4*WMAX];

extern int DepWi[IMGSZR][IMGSZC][DEPMAX];
extern int DepWj[IMGSZR][IMGSZC][DEPMAX];
extern int DepWm1[IMGSZR][IMGSZC][DEPMAX];
extern int DepWm2[IMGSZR][IMGSZC][DEPMAX];
extern long long DepWd[IMGSZR][IMGSZC][DEPMAX];
extern int DepWr[IMGSZR][IMGSZC][DEPMAX];

extern int DepPi1[IMGSZR][IMGSZC][DEPMAX];
extern int DepPi2[IMGSZR][IMGSZC][DEPMAX];
extern int DepPi3[IMGSZR][IMGSZC][DEPMAX];
extern int DepPi4[IMGSZR][IMGSZC][DEPMAX];
extern int DepPm1[IMGSZR][IMGSZC][DEPMAX];
extern int DepPm2[IMGSZR][IMGSZC][DEPMAX];
extern long long DepPd[IMGSZR][IMGSZC][DEPMAX];
extern int DepPr[IMGSZR][IMGSZC][DEPMAX];
extern double DepPh[IMGSZR][IMGSZC][DEPMAX];

extern int DepKi1[IMGSZR][IMGSZC][DEPMAX];
extern int DepKi2[IMGSZR][IMGSZC][DEPMAX];
extern int DepKi3[IMGSZR][IMGSZC][DEPMAX];
extern int DepKi4[IMGSZR][IMGSZC][DEPMAX];
extern int DepKm1[IMGSZR][IMGSZC][DEPMAX];
extern int DepKm2[IMGSZR][IMGSZC][DEPMAX];
extern long long DepKd[IMGSZR][IMGSZC][DEPMAX];
extern int DepKr[IMGSZR][IMGSZC][DEPMAX];

extern int DepCm[IMGSZR][IMGSZC][DEPMAX];
extern long long DepCd[IMGSZR][IMGSZC][DEPMAX];
extern int DepCr[IMGSZR][IMGSZC][DEPMAX];

extern int OptR[IMGSZR][IMGSZC];
extern long long OptD[IMGSZR][IMGSZC];
extern double OptDp[IMGSZR][IMGSZC];

extern int FeaturePixelCount;
extern int FeaturePixelWidth[FPIXELLEN];
extern int StatisticsVar[100];

void DoTransforms(int nr, int nc);
void DoTransforms(int nr, int nc, int graybit);
void DoTransformsMasked(int nr, int nc);
void DoTransformsCoded(int nr, int nc, int ver, int graybit);
void SetLambda();
void RDw(int nr, int nc, double lambda, double &psnr, double &bpp);
void RDp(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate);
void RDpDebug(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate);
void RDk(int nr, int nc, double lambda, double &psnr, double &bpp);
void RDcoded(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate);
//void RDcodedhm(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate);
void BPPp(int nr, int nc, double bppin, double &psnr, double &bpp);
void BPPcoded(int nr, int nc, double bppin, double &psnr, double &bpp, int &rate);
void BPPcodedEn(int nr, int nc, double bppin, int ver, double &psnr, double &bpp, int &rate);
void DoFeaturePixel(int nr, int nc);
void DoFeaturePixel2(int nr, int nc);
void SaveTree(char* path);
void LoadTree(char* path);

void Statistics1(int nr, int nc);
int Encoder(int nr, int nc, int version, char filename[]);
void Decoder(char filename[]);
bool CheckEncodeDecode(int nr, int nc);

void GenerateRdType(int nr, int nc);
void Statistics2(int nr, int nc);

int GeneralEncoder(int nr, int nc, int btver, int graybit, char filename[]);
void BPPcodedGE(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);
void BPPcodedGE2(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);
void BPPcodedGE2(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[]);
void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[]);
void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out);

int GeneralEncoderW(int nr, int nc, int btver, int graybit, char filename[]);
void BPPcodedWE(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);
void BPPcodedWE2(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);

void StoreParametersW(ofstream &of, int nr, int nc);

void DoTransformsBar(int nr, int nc, int ver, int sscale, int graybit);
void BPPBar(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);
void BPPBarGE(int nr, int nc, double bppin, int ver, int sscale, int graybit, double &psnr, double &bpp, int &rate);
void BarImageCreate(int nr, int nc, int ver, int sscale, int graybit, int wscale, int &ww, int &wh);
int BarGeneralEncoder(int nr, int nc, int btver, int sscale, int graybit, char filename[]);

void BPPBar(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[]);
void BPPBar(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out);
//void DoTransformsBar2(int nr, int nc, int ver, int sscale, int graybit);

void GetBarWedgeletsCodingStream(int nr, int nc);
void DoTransformsBarT(int nr, int nc, int ver, int sscale, int graybit);
void BPPBarT(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);

void DoTransformsBarMasked(int nr, int nc, int ver, int sscale, int graybit);
void BPPBarMasked(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[]);
void BPPBarMasked(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate);
void BPPBarMasked(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out);
void BPPBarMaskedCmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out);

void Store(ofstream &out, int mat[][IMGSZC], char *name);
void ApplyZeroMaskonImg(int nr, int nc);
void StoreBarmaskedStat(ofstream &out, int nr, int nc);