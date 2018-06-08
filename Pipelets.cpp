#include "mex.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

#define IMGSZ 130
#define WMAX 33
#define DEPMAX 10
#define round(x) floor((x)+0.5)
#define GRAYBIT 8

#include "PipeletsTransform.h"

int img[IMGSZ][IMGSZ];
int imgw[IMGSZ][IMGSZ][DEPMAX];
int imgk[IMGSZ][IMGSZ][DEPMAX];
int imgc[IMGSZ][IMGSZ][DEPMAX];
int imgwrd[IMGSZ][IMGSZ];
int imgkrd[IMGSZ][IMGSZ];
int imgwrdg[IMGSZ][IMGSZ];
int imgkrdg[IMGSZ][IMGSZ];

int wedn1[4*WMAX][4*WMAX];
int wedn2[4*WMAX][4*WMAX];
long long weds1[4*WMAX][4*WMAX];
long long weds2[4*WMAX][4*WMAX];
long long wedp1[4*WMAX][4*WMAX];
long long wedp2[4*WMAX][4*WMAX];

int DepWi[IMGSZ][IMGSZ][DEPMAX];
int DepWj[IMGSZ][IMGSZ][DEPMAX];
int DepWm1[IMGSZ][IMGSZ][DEPMAX];
int DepWm2[IMGSZ][IMGSZ][DEPMAX];
long long DepWd[IMGSZ][IMGSZ][DEPMAX];
int DepWr[IMGSZ][IMGSZ][DEPMAX];

int DepKi1[IMGSZ][IMGSZ][DEPMAX];
int DepKi2[IMGSZ][IMGSZ][DEPMAX];
int DepKi3[IMGSZ][IMGSZ][DEPMAX];
int DepKi4[IMGSZ][IMGSZ][DEPMAX];
int DepKm1[IMGSZ][IMGSZ][DEPMAX];
int DepKm2[IMGSZ][IMGSZ][DEPMAX];
long long DepKd[IMGSZ][IMGSZ][DEPMAX];
int DepKr[IMGSZ][IMGSZ][DEPMAX];

int DepCm[IMGSZ][IMGSZ][DEPMAX];
long long DepCd[IMGSZ][IMGSZ][DEPMAX];
int DepCr[IMGSZ][IMGSZ][DEPMAX];

int OptR[IMGSZ][IMGSZ];
long long OptD[IMGSZ][IMGSZ];

bool CheckIndex(int n, int w, int i1, int i2, int i3, int i4)
{
	if (i1==i2 || i1==i3 || i1==i4 || i2==i3 || i2==i4 || i3==i4)
		return false;

	if (i1>=0 && i1<=w && i4>=0 && i4<=w) return false;
	if (i1>=w && i1<=2*w && i4>=w && i4<=2*w) return false;
	if (i1>=2*w && i1<=3*w && i4>=2*w && i4<=3*w) return false;
	if ((i1==0 || (i1>=3*w && i1<4*w)) && (i4==0 || (i4>=3*w && i4<4*w))) return false;

	if (i2>=0 && i2<=w && i3>=0 && i3<=w) return false;
	if (i2>=w && i2<=2*w && i3>=w && i3<=2*w) return false;
	if (i2>=2*w && i2<=3*w && i3>=2*w && i3<=3*w) return false;
	if ((i2==0 || (i2>=3*w && i2<4*w)) && (i3==0 || (i3>=3*w && i3<4*w))) return false;

	if (i4>i1)
	{
		return !((i2>i1 && i2<i4) ^ (i3>i1 && i3<i4));
	}else{
		return !((i2>i4 && i2<i1) ^ (i3>i4 && i3<i1));
	}
}

void CalcOneDepth(int n, int w, int dep)
{
	int i,j,bi,bj,x,y;
	int i1,i2,i3,i4;
	int n1,n2;
	long long s1,s2,p1,p2,d1,d2,bestd;	
	int m1,m2,bestm1,bestm2,besti,bestj;
	int besti1,besti2,besti3,besti4;
	bool sgn,sgn1,sgn2;
	int indexes[4*IMGSZ][2];
	int mxindex = 4*w;
	int indrate = ceil(log((double)mxindex)/log(2.0));
	
	for (i=0;i<w;i++){
		indexes[i][0]=i;
		indexes[i][1]=0;
	}
	for (i=0;i<w;i++){
		indexes[w+i][0]=w;
		indexes[w+i][1]=i;
	}
	for (i=0;i<w;i++){
		indexes[2*w+i][0]=w-i;
		indexes[2*w+i][1]=w;
	}
	for (i=0;i<w;i++){
		indexes[3*w+i][0]=0;
		indexes[3*w+i][1]=w-i;
	}

	for (bi=0;bi<n;bi+=w)
	{
		for (bj=0;bj<n;bj+=w)
		{
			// Calculate for one Square

			// Claculate Wedgelets
			bestd = (1<<30);
			for (i=0;i<mxindex;i++)
			{
				for (j=0;j<mxindex;j++)
				{
					if (i==j) continue;
					n1=0; s1=0; p1=0; n2=0; s2=0; p2=0;
					
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							sgn = ((y-indexes[i][1])*(indexes[j][0]-indexes[i][0])-(x-indexes[i][0])*(indexes[j][1]-indexes[i][1])) >= 0;
							//if (x==0 && y==0) sgn0 = sgn;
							if (sgn)
							{
								n1++;
								s1 += img[bi+x][bj+y];
								p1 += img[bi+x][bj+y]*img[bi+x][bj+y];
							}else{
								n2++;
								s2 += img[bi+x][bj+y];
								p2 += img[bi+x][bj+y]*img[bi+x][bj+y];
							}
						}
					}

					wedn1[i][j] = n1;
					wedn2[i][j] = n2;
					weds1[i][j] = s1;
					weds2[i][j] = s2;
					wedp1[i][j] = p1;
					wedp2[i][j] = p2;					

					if (n1>0)
					{
						m1 = round((double)s1/n1);
						d1 = p1 + n1*m1*m1 - 2*m1*s1;
					}else{
						m1=0;
						d1=0;
					}
					if (n2>0)
					{
						m2 = round((double)s2/n2);
						d2 = p2 + n2*m2*m2 - 2*m2*s2;
					}else{
						m2=0;
						d2=0;
					}
					
					//cout << "dist[" << d1+d2 << "][" << bestd << "]" << endl;
					if (d1+d2<bestd){
						bestd = d1+d2;						
						besti = i;
						bestj = j;
						bestm1 = m1;
						bestm2 = m2;
					}
				}
			}

			DepWi[bi][bj][dep] = besti;
			DepWj[bi][bj][dep] = bestj;
			DepWm1[bi][bj][dep] = bestm1;
			DepWm2[bi][bj][dep] = bestm2;
			DepWd[bi][bj][dep] = bestd;
			DepWr[bi][bj][dep] = 2 * GRAYBIT + 2 * indrate;
			//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					sgn = ((y-indexes[besti][1])*(indexes[bestj][0]-indexes[besti][0])-(x-indexes[besti][0])*(indexes[bestj][1]-indexes[besti][1])) >= 0;
					if (sgn)
						imgw[bi+x][bj+y][dep] = bestm1;
					else
						imgw[bi+x][bj+y][dep] = bestm2;
				}
			}

			// Calculate Kleinlet
			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					for (i3=0;i3<mxindex;i3++)
					{
						for (i4=0;i4<mxindex;i4++)
						{
							if (CheckIndex(n,w,i1,i2,i3,i4)==false) continue;
							n1=0; s1=0; p1=0; n2=0; s2=0; p2=0;
							
							sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
							sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

							if (sgn1)
							{
								if (sgn2)
								{
									n1 = wedn1[i1][i4]-wedn2[i2][i3];
									s1 = weds1[i1][i4]-weds2[i2][i3];
									p1 = wedp1[i1][i4]-wedp2[i2][i3];
									n2 = wedn2[i1][i4]+wedn2[i2][i3];
									s2 = weds2[i1][i4]+weds2[i2][i3];
									p2 = wedp2[i1][i4]+wedp2[i2][i3];
									sgn = false;
								}else{
									n1 = wedn1[i2][i3]-wedn1[i1][i4];
									s1 = weds1[i2][i3]-weds1[i1][i4];
									p1 = wedp1[i2][i3]-wedp1[i1][i4];
									n2 = wedn2[i2][i3]+wedn1[i1][i4];
									s2 = weds2[i2][i3]+weds1[i1][i4];
									p2 = wedp2[i2][i3]+wedp1[i1][i4];
									sgn = true;
								}
							}else{
								if (sgn2)
								{
									n1 = wedn1[i1][i4]-wedn1[i2][i3];
									s1 = weds1[i1][i4]-weds1[i2][i3];
									p1 = wedp1[i1][i4]-wedp1[i2][i3];
									n2 = wedn2[i1][i4]+wedn1[i2][i3];
									s2 = weds2[i1][i4]+weds1[i2][i3];
									p2 = wedp2[i1][i4]+wedp1[i2][i3];
									sgn = true;
								}else{
									n1 = wedn2[i2][i3]-wedn1[i1][i4];
									s1 = weds2[i2][i3]-weds1[i1][i4];
									p1 = wedp2[i2][i3]-wedp1[i1][i4];
									n2 = wedn1[i2][i3]+wedn1[i1][i4];
									s2 = weds1[i2][i3]+weds1[i1][i4];
									p2 = wedp1[i2][i3]+wedp1[i1][i4];
									sgn = false;
								}
							}

							if (n1>0)
							{
								m1 = round((double)s1/n1);
								d1 = p1 + n1*m1*m1 - 2*m1*s1;
							}else{
								m1=0;
								d1=0;
							}
							if (n2>0)
							{
								m2 = round((double)s2/n2);
								d2 = p2 + n2*m2*m2 - 2*m2*s2;
							}else{
								m2=0;
								d2=0;
							}

							//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
							if (d1+d2<bestd){
								bestd = d1+d2;
								besti1 = i1;
								besti2 = i2;
								besti3 = i3;
								besti4 = i4;
								if (!sgn)
								{
									bestm1 = m2;
									bestm2 = m1;
								}
								else
								{
									bestm1 = m1;
									bestm2 = m2;
								}
								//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
								if (bestd==0){
									i1 = 1000;
									i2 = 1000;
									i3 = 1000;
									i4 = 1000;
								}
							}
						}
					}
				}
			}

			DepKi1[bi][bj][dep] = besti1;
			DepKi2[bi][bj][dep] = besti2;
			DepKi3[bi][bj][dep] = besti3;
			DepKi4[bi][bj][dep] = besti4;
			DepKm1[bi][bj][dep] = bestm1;
			DepKm2[bi][bj][dep] = bestm2;
			DepKd[bi][bj][dep] = bestd;
			DepKr[bi][bj][dep] = 3 * GRAYBIT + 4 * indrate;

			//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
					sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
					if (sgn1 ^ sgn2)
						imgk[bi+x][bj+y][dep] = bestm1;
					else
						imgk[bi+x][bj+y][dep] = bestm2;
				}
			}

		}
	}

}

void CalcConstant(int n, int w, int dep)
{
	int bi,bj,x,y;
	int m;
	int n1;
	long long s,p;
	long long d;

	for (bi=0;bi<n;bi+=w)
	{
		for (bj=0;bj<n;bj+=w)
		{
			n1=0;
			s=0;
			p=0;
			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					n1++;
					s += img[bi+x][bj+y];
					p += (img[bi+x][bj+y] * img[bi+x][bj+y]);
				}
			}
			m = round((double)s/n1);
			d = p + n1*m*m - 2*m*s;

			DepCm[bi][bj][dep] = m;
			DepCd[bi][bj][dep] = d;
			DepCr[bi][bj][dep] = GRAYBIT;

			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					imgc[bi+x][bj+y][dep] = m;
				}
			}
		}
	}
}

void PreSetArray(int n, int dep)
{
	int i,j;

	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
		{
			DepWd[i][j][dep] = (1<<30);
			DepWr[i][j][dep] = 1000;
			DepKd[i][j][dep] = (1<<30);
			DepKr[i][j][dep] = 1000;
		}
	}
}

void DoTransforms(int n)
{
	int dep,w,ndep;

	for (dep=0,w=1;w<=n;dep++,w*=2)
	{
		PreSetArray(n,dep);
		CalcConstant(n,w,dep);
	}
	ndep = dep-1;
	for (dep=1;dep<=min(4,ndep);dep++)
	{
		int w = pow(2.0,dep);
		CalcOneDepth(n,w,dep);
	}
}

inline int mini(double a, double b, double c)
{
	if (a<=b)
	{
		if (a<=c)
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	else
	{
		if (b<=c)
		{
			return 2;
		}
		else
		{
			return 3;
		}
	}
}

inline int mini(double a, double b, double c, double d)
{
	if (a<=b)
	{
		if (a<=c)
		{
			if (a<=d)
			{
				return 1;
			}
			else
			{
				return 4;
			}
		}
		else
		{
			if (c<=d)
			{
				return 3;
			}
			else
			{
				return 4;
			}
		}
	}
	else
	{
		if (b<=c)
		{
			if (b<=d)
			{
				return 2;
			}
			else
			{
				return 4;
			}
		}
		else
		{
			if (c<=d)
			{
				return 3;
			}
			else
			{
				return 4;
			}
		}
	}
}

void RDw(int n, double lambda, double &psnr, double &bpp)
{
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;

	for (bi=0;bi<n;bi++)
	{
		for (bj=0;bj<n;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgwrd[bi][bj] = imgc[bi][bj][0];
			imgwrdg[bi][bj] = imgc[bi][bj][0];
		}
	}
	
	for (dep=1,w=2,w2=1;w<=n;w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<n;bi+=w)
		{
			for (bj=0;bj<n;bj+=w)
			{
				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (2 + DepWr[bi][bj][dep]),
					(OptD[bi][bj] + OptD[bi+w2][bj] + OptD[bi][bj+w2] + OptD[bi+w2][bj+w2])+ lambda*
					(1 + OptR[bi][bj] + OptR[bi+w2][bj] + OptR[bi][bj+w2] + OptR[bi+w2][bj+w2]));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							imgwrd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgwrdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
						}
						imgwrdg[bi+x][bj] = 127;
						imgwrdg[bi+x][bj+w-1] = 127;
						imgwrdg[bi][bj+x] = 127;
						imgwrdg[bi+w-1][bj+x] = 127;
					}
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepWr[bi][bj][dep];
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							imgwrd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgwrdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
						}
						imgwrdg[bi+x][bj] = 127;
						imgwrdg[bi+x][bj+w-1] = 127;
						imgwrdg[bi][bj+x] = 127;
						imgwrdg[bi+w-1][bj+x] = 127;
					}
				}
				else if (ind==3)
				{
					OptD[bi][bj] = OptD[bi][bj] + OptD[bi+w2][bj] + OptD[bi][bj+w2] + OptD[bi+w2][bj+w2];
					OptR[bi][bj] = 1 + OptR[bi][bj] + OptR[bi+w2][bj] + OptR[bi][bj+w2] + OptR[bi+w2][bj+w2];
				}
			}
		}
	}

    mse = (double)OptD[0][0]/(n*n);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(n*n);
}

void RDk(int n, double lambda, double &psnr, double &bpp)
{
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;

	for (bi=0;bi<n;bi++)
	{
		for (bj=0;bj<n;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgkrd[bi][bj] = imgc[bi][bj][0];
		}
	}
	
	for (dep=1,w=2,w2=1;w<=n;w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<n;bi+=w)
		{
			for (bj=0;bj<n;bj+=w)
			{
				ind = mini(
					DepCd[bi][bj][dep] + lambda * (3 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (3 + DepWr[bi][bj][dep]),
					DepKd[bi][bj][dep] + lambda * (3 + DepKr[bi][bj][dep]),
					(OptD[bi][bj] + OptD[bi+w2][bj] + OptD[bi][bj+w2] + OptD[bi+w2][bj+w2])+ lambda*
					(1 + OptR[bi][bj] + OptR[bi+w2][bj] + OptR[bi][bj+w2] + OptR[bi+w2][bj+w2]));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepCr[bi][bj][dep];
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							imgkrd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+w-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+w-1][bj+x] = 127;
					}
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepWr[bi][bj][dep];
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							imgkrd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+w-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+w-1][bj+x] = 127;
					}
				}
				else if (ind==3)
				{
					OptD[bi][bj] = DepKd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepKr[bi][bj][dep];
					for (x=0;x<w;x++)
					{
						for (y=0;y<w;y++)
						{
							imgkrd[bi+x][bj+y] = imgk[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgk[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+w-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+w-1][bj+x] = 127;
					}
				}
				else if (ind==4)
				{
					OptD[bi][bj] = OptD[bi][bj] + OptD[bi+w2][bj] + OptD[bi][bj+w2] + OptD[bi+w2][bj+w2];
					OptR[bi][bj] = 1 + OptR[bi][bj] + OptR[bi+w2][bj] + OptR[bi][bj+w2] + OptR[bi+w2][bj+w2];
				}
			}
		}
	}

    mse = (double)OptD[0][0]/(n*n);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(n*n);
}

void StoreImage(int n, int simg[][IMGSZ], int simg2[][IMGSZ], double psnr, double bpp, char tp)
{
	int i,j;
	char name[400];
	
	sprintf(name,"img-%c-%lf-%lf.b",tp,psnr,bpp);
	ofstream foutw(name);
	foutw << n << " " << n << endl;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			foutw << simg[i][j] << " ";
		foutw << endl;
	}
	foutw.close();

	sprintf(name,"img-%c-%lf-%lf-g.b",tp,psnr,bpp);
	ofstream foutw2(name);
	foutw2 << n << " " << n << endl;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			foutw2 << simg2[i][j] << " ";
		foutw2 << endl;
	}
	foutw2.close();
}

void StoreImage(int n, int simg[][IMGSZ][DEPMAX], char tp)
{
	int i,j;
	int w,dep;
	char name[400];
	
	for (w=1,dep=0;w<=n;w*=2,dep++)
	{
		sprintf(name,"imgdep-%c-%d.b",tp,w);
		ofstream foutw(name);
		foutw << n << " " << n << endl;
		for (i=0;i<n;i++)
		{
			for (j=0;j<n;j++)
				foutw << simg[i][j][dep] << " ";
			foutw << endl;
		}
		foutw.close();
	}
}

void RD(int n)
{
	int i;
	std::vector<double> lambdas;
	double lambda;
	double psnr, bpp;

	lambda=1;
	for (i=0;i<40;i++)
	{
		lambda = lambda / 1.5;
		lambdas.push_back(lambda);
	}

	lambda=1;
	for (i=0;i<50;i++)
	{
		lambdas.push_back(lambda);
		lambda = lambda * 1.5;
	}

	sort(lambdas.begin(), lambdas.end());

	ofstream fout("rd.txt");

	fout << "Wedgelets" << endl;
	for(std::vector<double>::iterator it = lambdas.begin(); it<lambdas.end(); it++)
	{
		RDw(n,*it,psnr,bpp);
		StoreImage(n,imgwrd,imgwrdg,psnr,bpp,'w');
		fout.width(15);
		fout << std::setw(15) << std::right << *it << psnr << bpp << endl;
	}

	fout << "Kleinlets" << endl;
	for(std::vector<double>::iterator it = lambdas.begin(); it<lambdas.end(); it++)
	{
		RDk(n,*it,psnr,bpp);
		StoreImage(n,imgkrd,imgkrdg,psnr,bpp,'k');
		fout.width(15);
		fout << std::right << *it << psnr << bpp << endl;
	}

	StoreImage(n,imgw,'w');
	StoreImage(n,imgk,'k');
}

void BuildLevels(unsigned char* imgsrc, int imgRow, int imgCol)
{
    int i,j;
    
    if (imgRow != imgCol)
        printf("'rows' must be equal to 'columns' \n");
    printf("Build levels for img %dx%d\n",imgRow,imgCol);
    for (i=0;i<imgCol;i++)
        for (j=0;j<imgRow;j++)
            img[i][j] = imgsrc[i*imgRow+j];
//      for (i=0;i<imgRow;i++){
//          for (j=0;j<imgRow;j++)
//              printf("%d ", img[i][j]);
//          printf("\n");
//      }
    DoTransforms(imgRow);
}

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray* prhs[] )
     
{
    // ## Input arguments
    // Argument 0 : Mode = 'img' , 'file'
    // Mode 'img'
    // Argument 1 : name
    // Argument 2 : image
    // Mode 'file'
    // Argument 1 : name
    
    // # Output arguments
    // Argument 0 : lambda
    
    // Options
    // SquareSize : maximum size of square e.g. '32'
    
    // [MidStruct, RDStruct] = Pipelets('img','name',img)
    
    //---------------------------------------------
    int ImgRow,ImgCol;
    
    
    // Read argument 1 : mode
    char mode[20];
    mxGetString(prhs[0], (char *)&mode, sizeof mode);
    
    // Read argument 2 : name
    char name[200];
    mxGetString(prhs[1], (char *)&name, sizeof name);    
    
    // Mode 'img'
    if (_stricmp(mode,"img")==0)
    {        
        printf("Mode 'img'\n");
        ImgRow = mxGetM(prhs[2]);
        ImgCol = mxGetN(prhs[2]);    
        unsigned char* Img = (unsigned char*)mxGetPr(prhs[2]);
        printf("Call BuildLevels\n");
        BuildLevels(Img, ImgRow, ImgCol);        
        printf("end Call BuildLevels\n");
    }
    
    // Mode 'file'
    if (_stricmp(mode,"file")==0)
    {
    }
    
    mwSignedIndex *plhs0data;
    mwSignedIndex dims[3];
    int i,j,k;
    switch (nlhs)
    {
        case 1:
            break;
        case 6:
            dims[0] = ImgRow;
            dims[1] = ImgCol;
            dims[2] = DEPMAX;
            plhs[3] = mxCreateNumericArray(3, dims, mxINT64_CLASS, mxREAL);
            plhs[4] = mxCreateNumericArray(3, dims, mxINT64_CLASS, mxREAL);
            plhs[5] = mxCreateNumericArray(3, dims, mxINT64_CLASS, mxREAL);

            plhs0data = (mwSignedIndex*)mxGetData(plhs[0]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){                        
                        *plhs0data = DepCd[i][j][k];                        
                        plhs0data++;
                    }
                }
            }
            
            plhs0data = (mwSignedIndex*)mxGetData(plhs[1]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){
                        *plhs0data = DepWd[i][j][k];                        
                        plhs0data++;
                    }
                }
            }

            plhs0data = (mwSignedIndex*)mxGetData(plhs[2]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){
                        *plhs0data = DepKd[i][j][k];
                        plhs0data++;
                    }
                }
            }            
        case 3:
            dims[0] = ImgRow;
            dims[1] = ImgCol;
            dims[2] = DEPMAX;
            plhs[0] = mxCreateNumericArray(3, dims, mxINT32_CLASS, mxREAL);
            plhs[1] = mxCreateNumericArray(3, dims, mxINT32_CLASS, mxREAL);
            plhs[2] = mxCreateNumericArray(3, dims, mxINT32_CLASS, mxREAL);

            plhs0data = (mwSignedIndex*)mxGetData(plhs[0]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){                        
                        *plhs0data = imgc[i][j][k];
                        //printf("%d -- %d \n",imgc[i][j][k],*plhs0data);
                        plhs0data++;
                    }
                }
            }
            
            plhs0data = (mwSignedIndex*)mxGetData(plhs[1]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){
                        *plhs0data = imgw[i][j][k];                        
                        plhs0data++;
                    }
                }
            }

            plhs0data = (mwSignedIndex*)mxGetData(plhs[2]);            
            for (k=0;k<dims[2];k++){
                for(j=0;j<dims[1];j++){
                    for (i=0;i<dims[0];i++){
                        *plhs0data = imgk[i][j][k];
                        plhs0data++;
                    }
                }
            }
                        
            break;
        default:
            break;
    }    
}