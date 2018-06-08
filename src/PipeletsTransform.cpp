#include "PipeletsTransform.h"
#include <map>

bool EnableConstantDep = false;
bool EnableWedgeletDep = false;
bool EnablePipeletDep = false;
bool EnableKleinletDep = false;

int DoTransformMaxDepth = 4;
int DoTransformMaxCurveBit = 3;
bool ShowMessages = true;

double LambdaArray[LAMBDALEN];
int LambdaLength = 0;

int img[IMGSZR][IMGSZC];
int rimg[IMGSZR][IMGSZC];
bool imgmask[IMGSZR][IMGSZC];
int imgc[IMGSZR][IMGSZC][DEPMAX];
int imgw[IMGSZR][IMGSZC][DEPMAX];
int imgp[IMGSZR][IMGSZC][DEPMAX];
int imgk[IMGSZR][IMGSZC][DEPMAX];
int imgwrd[IMGSZR][IMGSZC];
int imgprd[IMGSZR][IMGSZC];
int imgkrd[IMGSZR][IMGSZC];
int imgwrdg[IMGSZR][IMGSZC];
int imgprdg[IMGSZR][IMGSZC];
int imgkrdg[IMGSZR][IMGSZC];

int wedn1[4*WMAX][4*WMAX];
int wedn2[4*WMAX][4*WMAX];
long long weds1[4*WMAX][4*WMAX];
long long weds2[4*WMAX][4*WMAX];
long long wedp1[4*WMAX][4*WMAX];
long long wedp2[4*WMAX][4*WMAX];

int DepCm[IMGSZR][IMGSZC][DEPMAX];
long long DepCd[IMGSZR][IMGSZC][DEPMAX];
int DepCr[IMGSZR][IMGSZC][DEPMAX];

int DepWi[IMGSZR][IMGSZC][DEPMAX];
int DepWj[IMGSZR][IMGSZC][DEPMAX];
int DepWm1[IMGSZR][IMGSZC][DEPMAX];
int DepWm2[IMGSZR][IMGSZC][DEPMAX];
long long DepWd[IMGSZR][IMGSZC][DEPMAX];
int DepWr[IMGSZR][IMGSZC][DEPMAX];

BinaryStream DepWbs[IMGSZR][IMGSZC][DEPMAX];

int DepPi1[IMGSZR][IMGSZC][DEPMAX];
int DepPi2[IMGSZR][IMGSZC][DEPMAX];
int DepPi3[IMGSZR][IMGSZC][DEPMAX];
int DepPi4[IMGSZR][IMGSZC][DEPMAX];
int DepPm1[IMGSZR][IMGSZC][DEPMAX];
int DepPm2[IMGSZR][IMGSZC][DEPMAX];
long long DepPd[IMGSZR][IMGSZC][DEPMAX];
int DepPr[IMGSZR][IMGSZC][DEPMAX];
double DepPh[IMGSZR][IMGSZC][DEPMAX];

int DepGi1[IMGSZR][IMGSZC][DEPMAX];
int DepGi2[IMGSZR][IMGSZC][DEPMAX];
int DepGi3[IMGSZR][IMGSZC][DEPMAX];
int DepGi4[IMGSZR][IMGSZC][DEPMAX];
int DepGm1[IMGSZR][IMGSZC][DEPMAX];
int DepGm2[IMGSZR][IMGSZC][DEPMAX];
long long DepGd[IMGSZR][IMGSZC][DEPMAX];
int DepGr[IMGSZR][IMGSZC][DEPMAX];

int DepKi1[IMGSZR][IMGSZC][DEPMAX];
int DepKi2[IMGSZR][IMGSZC][DEPMAX];
int DepKi3[IMGSZR][IMGSZC][DEPMAX];
int DepKi4[IMGSZR][IMGSZC][DEPMAX];
int DepKm1[IMGSZR][IMGSZC][DEPMAX];
int DepKm2[IMGSZR][IMGSZC][DEPMAX];
long long DepKd[IMGSZR][IMGSZC][DEPMAX];
int DepKr[IMGSZR][IMGSZC][DEPMAX];

int OptR[IMGSZR][IMGSZC];
long long OptD[IMGSZR][IMGSZC];
double OptDp[IMGSZR][IMGSZC];
int OptS[IMGSZR][IMGSZC][DEPMAX];

int FeaturePixelCount;
int FeaturePixelWidth[FPIXELLEN];

int ComponentCnt[IMGSZR][IMGSZC][3];
int StatisticsVar[100];

int Dist[IMGSZR][IMGSZC];
int StatVars[IMGSZR + IMGSZC][4];
int wDist[IMGSZR][IMGSZC];

bool CheckIndex(int wr, int wc, int i1, int i2, int i3, int i4)
{
	int b1=wr, b2=wr+wc, b3=2*wr+wc, b4=2*wr+2*wc;
	if (i1==i2 || i1==i3 || i1==i4 || i2==i3 || i2==i4 || i3==i4)
		return false;

	if (i1>=0 && i1<=b1 && i4>=0 && i4<=b1) return false;
	if (i1>=b1 && i1<=b2 && i4>=b1 && i4<=b2) return false;
	if (i1>=b2 && i1<=b3 && i4>=b2 && i4<=b3) return false;
	if ((i1==0 || (i1>=b3 && i1<b4)) && (i4==0 || (i4>=b3 && i4<b4))) return false;

	if (i2>=0 && i2<=b1 && i3>=0 && i3<=b1) return false;
	if (i2>=b1 && i2<=b2 && i3>=b1 && i3<=b2) return false;
	if (i2>=b2 && i2<=b3 && i3>=b2 && i3<=b3) return false;
	if ((i2==0 || (i2>=b3 && i2<b4)) && (i3==0 || (i3>=b3 && i3<b4))) return false;

	if (i4>i1)
	{
		return !((i2>i1 && i2<i4) ^ (i3>i1 && i3<i4));
	}else{
		return !((i2>i4 && i2<i1) ^ (i3>i4 && i3<i1));
	}
}

bool CheckIndex(int wr, int wc, int i1, int i2)
{
	int b1=wr, b2=wr+wc, b3=2*wr+wc, b4=2*wr+2*wc;
	if (i1==i2)
		return false;

	if (i1>=0 && i1<=b1 && i2>=0 && i2<=b1) return false;
	if (i1>=b1 && i1<=b2 && i2>=b1 && i2<=b2) return false;
	if (i1>=b2 && i1<=b3 && i2>=b2 && i2<=b3) return false;
	if ((i1==0 || (i1>=b3 && i1<b4)) && (i2==0 || (i2>=b3 && i2<b4))) return false;
	return true;
}


inline void GetXY(int wr, int wc, int ind, int &x, int &y)
{
	if (ind<wr)
	{
		x=ind;
		y=0;
	}
	else if (ind<wr+wc)
	{
		x = wr;
		y = ind-wr;
	}
	else if (ind<wr+wc+wr)
	{
		x = wr+wc+wr-ind;
		y = wc;
	}
	else if (ind<=wr+wc+wr+wc)
	{
		x = 0;
		y = wr+wc+wr+wc-ind;
	}
	else
	{
		printf("Error in index\n");
	}
}

inline int GetIndex(int wr, int wc, int x, int y)
{
	if (y==0)
		return x;
	if (x==wr)
		return wr+y;
	if (y==wc)
		return 2*wr+wc-x;
	if (x==0)
		return 2*wr+2*wc-y;
	return -1;
}

bool GetIndex4(int wr, int wc, int x1, int y1, int x2, int y2, int x3, int y3, int &i4)
{
	int x4,y4;
	double x4_,y4_;

	if (x2==x3)
	{
		if (y1!=0 && y1!=wc) return false;
		y4 = wc-y1;
		x4 = x1;
		i4 = GetIndex(wr,wc,x4,y4);
		return true;
	}
	
	// x=0
	y4_ = (y3-y2)*(-x1)/((double)(x3-x2))+y1;
	if (x1!=0 && y4_>=0 && y4_<=wc)
	{
		x4 = 0;
		y4 = (int)round(y4_);
		i4 = GetIndex(wr,wc,x4,y4);
		return true;
	}

	// x=wr
	y4_ = (y3-y2)*(wr-x1)/((double)(x3-x2))+y1;
	if (x1!=wr && y4_>=0 && y4_<=wc)
	{
		x4 = wr;
		y4 = (int)round(y4_);
		i4 = GetIndex(wr,wc,x4,y4);
		return true;
	}

	// y=0
	x4_ = (x3-x2)*(-y1)/((double)(y3-y2))+x1;
	if (y1!=0 && x4_>=0 && x4_<=wr)
	{
		x4 = (int)round(x4_);
		y4 = 0;
		i4 = GetIndex(wr,wc,x4,y4);
		return true;
	}

	// y=wc
	x4_ = (x3-x2)*(wc-y1)/((double)(y3-y2))+x1;
	if (y1!=wc && x4_>=0 && x4_<=wr)
	{
		x4 = (int)round(x4_);
		y4 = wc;
		i4 = GetIndex(wr,wc,x4,y4);
		return true;
	}

	return false;
}

void CalcOneDepthMasked(int nr, int nc, int w, int dep)
{
	cout << "CalcOneDepth for dep : " << dep << endl;
	int i,j,bi,bj,x,y;
	int wr,wc;
	int i1,i2,i3,i4;
	int n1,n2;
	long long s1,s2,p1,p2,d1,d2,bestd;	
	int m1,m2,bestm1,bestm2,besti,bestj;
	int besti1,besti2,besti3,besti4;
	bool sgn,sgn1,sgn2;
	int indexes[4*WMAX][2];
	int mxindex = 0;
	int indrate = 0;
	
	for (bi=0;bi<nr;bi+=w)
	{
		for (bj=0;bj<nc;bj+=w)
		{
			// Calculate for one Square
			wr = min(w,nr-bi);
			wc = min(w,nc-bj);

			mxindex = 0;
			for (i=0;i<wr;i++){
				indexes[mxindex][0]=i;
				indexes[mxindex++][1]=0;
			}
			for (i=0;i<wc;i++){
				indexes[mxindex][0]=wr;
				indexes[mxindex++][1]=i;
			}
			for (i=0;i<wr;i++){
				indexes[mxindex][0]=wr-i;
				indexes[mxindex++][1]=wc;
			}
			for (i=0;i<wc;i++){
				indexes[mxindex][0]=0;
				indexes[mxindex++][1]=wc-i;
			}

			indrate = (int)ceil(log((double)mxindex)/log(2.0));
						
			// Claculate Wedgelets
			bestd = (1<<30);
			for (i=0;i<mxindex;i++)
			{
				for (j=0;j<mxindex;j++)
				{
					if (i==j) continue;
					n1=0; s1=0; p1=0; n2=0; s2=0; p2=0;
					
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							if (imgmask[bi+x][bj+y]==false) continue;
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
						m1 = (int)round((double)s1/n1);
						d1 = p1 + n1*m1*m1 - 2*m1*s1;
					}else{
						m1=0;
						d1=0;
					}
					if (n2>0)
					{
						m2 = (int)round((double)s2/n2);
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
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					if (imgmask[bi+x][bj+y]==false)
					{
						imgw[bi+x][bj+y][dep] = 0;						
					}
					else
					{
						sgn = ((y-indexes[besti][1])*(indexes[bestj][0]-indexes[besti][0])-(x-indexes[besti][0])*(indexes[bestj][1]-indexes[besti][1])) >= 0;
						if (sgn)
							imgw[bi+x][bj+y][dep] = bestm1;
						else
							imgw[bi+x][bj+y][dep] = bestm2;
					}
				}
			}

			// Calculate Pipelets
			if (EnablePipeletDep)
			{			
			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					if (i1==i2) continue;
					for (i3=0;i3<mxindex;i3++)
					{
						if (i3==i1 || i3==i2) continue;
						if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
						if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
						
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
							m1 = (int)round((double)s1/n1);
							d1 = p1 + n1*m1*m1 - 2*m1*s1;
						}else{
							m1=0;
							d1=0;
						}
						if (n2>0)
						{
							m2 = (int)round((double)s2/n2);
							d2 = p2 + n2*m2*m2 - 2*m2*s2;
						}else{
							m2=0;
							d2=0;
						}

						//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
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
								i1 = 10000;
								i2 = 10000;
								i3 = 10000;
							}
						}
					}
				}
			}

			DepPi1[bi][bj][dep] = besti1;
			DepPi2[bi][bj][dep] = besti2;
			DepPi3[bi][bj][dep] = besti3;
			DepPi4[bi][bj][dep] = besti4;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					if (imgmask[bi+x][bj+y]==false)
					{
						imgp[bi+x][bj+y][dep] = 0;
					}
					else
					{
						sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
						sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
						if (sgn1 ^ sgn2)
							imgp[bi+x][bj+y][dep] = bestm1;
						else
							imgp[bi+x][bj+y][dep] = bestm2;
					}
				}
			}

			}

			// Calculate PipeletsGauss
			//if (EnablePipeletGaussDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			for (x=0;x<wr;x++)
			//			{
			//				for (y=0;y<wr;y++)
			//				{
			//					sgn1t = ((y-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(x-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//					sgn2t = ((y-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(x-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;
			//					if (sgn1t ^ sgn2t == true)
			//					{

			//					}
			//				}
			//			}

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<wc;x++)
			//{
			//	for (y=0;y<wr;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

			// Calculate Kleinlet
			if (EnableKleinletDep)
			{

			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					for (i3=0;i3<mxindex;i3++)
					{
						for (i4=0;i4<mxindex;i4++)
						{
							if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;
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
								m1 = (int)round((double)s1/n1);
								d1 = p1 + n1*m1*m1 - 2*m1*s1;
							}else{
								m1=0;
								d1=0;
							}
							if (n2>0)
							{
								m2 = (int)round((double)s2/n2);
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
									i1 = 10000;
									i2 = 10000;
									i3 = 10000;
									i4 = 10000;
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
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
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

			// Calculate Gaussian
			//if (EnableGaussianDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<w;x++)
			//{
			//	for (y=0;y<w;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

		}
	}

}


void CalcOneDepth(int nr, int nc, int w, int dep)
{
	cout << "CalcOneDepth for dep : " << dep << endl;
	int i,j,bi,bj,x,y;
	int wr,wc;
	int i1,i2,i3,i4;
	int n1,n2;
	long long s1,s2,p1,p2,d1,d2,bestd;	
	int m1,m2,bestm1,bestm2,besti,bestj;
	int besti1,besti2,besti3,besti4;
	bool sgn,sgn1,sgn2;
	int indexes[4*WMAX][2];
	int mxindex = 0;
	int indrate = 0;
	
	for (bi=0;bi<nr;bi+=w)
	{
		for (bj=0;bj<nc;bj+=w)
		{
			// Calculate for one Square
			wr = min(w,nr-bi);
			wc = min(w,nc-bj);

			mxindex = 0;
			for (i=0;i<wr;i++){
				indexes[mxindex][0]=i;
				indexes[mxindex++][1]=0;
			}
			for (i=0;i<wc;i++){
				indexes[mxindex][0]=wr;
				indexes[mxindex++][1]=i;
			}
			for (i=0;i<wr;i++){
				indexes[mxindex][0]=wr-i;
				indexes[mxindex++][1]=wc;
			}
			for (i=0;i<wc;i++){
				indexes[mxindex][0]=0;
				indexes[mxindex++][1]=wc-i;
			}

			indrate = (int)ceil(log((double)mxindex)/log(2.0));
						
			// Claculate Wedgelets
			bestd = (1<<30);
			for (i=0;i<mxindex;i++)
			{
				for (j=0;j<mxindex;j++)
				{
					if (i==j) continue;
					n1=0; s1=0; p1=0; n2=0; s2=0; p2=0;
					
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
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
						m1 = (int)round((double)s1/n1);
						d1 = p1 + n1*m1*m1 - 2*m1*s1;
					}else{
						m1=0;
						d1=0;
					}
					if (n2>0)
					{
						m2 = (int)round((double)s2/n2);
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
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					sgn = ((y-indexes[besti][1])*(indexes[bestj][0]-indexes[besti][0])-(x-indexes[besti][0])*(indexes[bestj][1]-indexes[besti][1])) >= 0;
					if (sgn)
						imgw[bi+x][bj+y][dep] = bestm1;
					else
						imgw[bi+x][bj+y][dep] = bestm2;
				}
			}

			// Calculate Pipelets
			if (EnablePipeletDep)
			{			
			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					if (i1==i2) continue;
					for (i3=0;i3<mxindex;i3++)
					{
						if (i3==i1 || i3==i2) continue;
						if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
						if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
						
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
							m1 = (int)round((double)s1/n1);
							d1 = p1 + n1*m1*m1 - 2*m1*s1;
						}else{
							m1=0;
							d1=0;
						}
						if (n2>0)
						{
							m2 = (int)round((double)s2/n2);
							d2 = p2 + n2*m2*m2 - 2*m2*s2;
						}else{
							m2=0;
							d2=0;
						}

						//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
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
								i1 = 10000;
								i2 = 10000;
								i3 = 10000;
							}
						}
					}
				}
			}

			DepPi1[bi][bj][dep] = besti1;
			DepPi2[bi][bj][dep] = besti2;
			DepPi3[bi][bj][dep] = besti3;
			DepPi4[bi][bj][dep] = besti4;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
					sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
					if (sgn1 ^ sgn2)
						imgp[bi+x][bj+y][dep] = bestm1;
					else
						imgp[bi+x][bj+y][dep] = bestm2;
				}
			}

			}

			// Calculate PipeletsGauss
			//if (EnablePipeletGaussDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			for (x=0;x<wr;x++)
			//			{
			//				for (y=0;y<wr;y++)
			//				{
			//					sgn1t = ((y-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(x-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//					sgn2t = ((y-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(x-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;
			//					if (sgn1t ^ sgn2t == true)
			//					{

			//					}
			//				}
			//			}

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<wc;x++)
			//{
			//	for (y=0;y<wr;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

			// Calculate Kleinlet
			if (EnableKleinletDep)
			{

			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					for (i3=0;i3<mxindex;i3++)
					{
						for (i4=0;i4<mxindex;i4++)
						{
							if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;
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
								m1 = (int)round((double)s1/n1);
								d1 = p1 + n1*m1*m1 - 2*m1*s1;
							}else{
								m1=0;
								d1=0;
							}
							if (n2>0)
							{
								m2 = (int)round((double)s2/n2);
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
									i1 = 10000;
									i2 = 10000;
									i3 = 10000;
									i4 = 10000;
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
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
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

			// Calculate Gaussian
			//if (EnableGaussianDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<w;x++)
			//{
			//	for (y=0;y<w;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

		}
	}

}

void CalcOneDepth(int nr, int nc, int w, int dep, int graybit)
{
	cout << "CalcOneDepth for dep : " << dep << " with " << graybit << " graybit" << endl;
	int i, j, bi, bj, x, y;
	int wr, wc;
	int i1, i2, i3, i4;
	int n1, n2;
	long long s1, s2, p1, p2, d1, d2, bestd;
	int m1, m2, bestm1, bestm2, besti, bestj;
	int besti1, besti2, besti3, besti4;
	bool sgn, sgn1, sgn2;
	int indexes[4 * WMAX][2];
	int mxindex = 0;
	int indrate = 0;
	int pow2rem = 1 << (8 - graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	for (bi = 0;bi<nr;bi += w)
	{
		for (bj = 0;bj<nc;bj += w)
		{
			// Calculate for one Square
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);

			mxindex = 0;
			for (i = 0;i<wr;i++) {
				indexes[mxindex][0] = i;
				indexes[mxindex++][1] = 0;
			}
			for (i = 0;i<wc;i++) {
				indexes[mxindex][0] = wr;
				indexes[mxindex++][1] = i;
			}
			for (i = 0;i<wr;i++) {
				indexes[mxindex][0] = wr - i;
				indexes[mxindex++][1] = wc;
			}
			for (i = 0;i<wc;i++) {
				indexes[mxindex][0] = 0;
				indexes[mxindex++][1] = wc - i;
			}

			indrate = (int)ceil(log((double)mxindex) / log(2.0));

			// Claculate Wedgelets
			bestd = (1 << 30);
			for (i = 0;i<mxindex;i++)
			{
				for (j = 0;j<mxindex;j++)
				{
					if (i == j) continue;
					n1 = 0; s1 = 0; p1 = 0; n2 = 0; s2 = 0; p2 = 0;

					for (x = 0;x<wr;x++)
					{
						for (y = 0;y<wc;y++)
						{
							sgn = ((y - indexes[i][1])*(indexes[j][0] - indexes[i][0]) - (x - indexes[i][0])*(indexes[j][1] - indexes[i][1])) >= 0;
							//if (x==0 && y==0) sgn0 = sgn;
							if (sgn)
							{
								n1++;
								s1 += img[bi + x][bj + y];
								p1 += img[bi + x][bj + y] * img[bi + x][bj + y];
							}
							else {
								n2++;
								s2 += img[bi + x][bj + y];
								p2 += img[bi + x][bj + y] * img[bi + x][bj + y];
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
						m1 = ((int)round((double)s1 / n1 / pow2rem))*pow2rem;
						d1 = p1 + n1*m1*m1 - 2 * m1*s1;
					}
					else {
						m1 = 0;
						d1 = 0;
					}
					if (n2>0)
					{
						m2 = ((int)round((double)s2 / n2 / pow2rem))*pow2rem;
						d2 = p2 + n2*m2*m2 - 2 * m2*s2;
					}
					else {
						m2 = 0;
						d2 = 0;
					}

					//cout << "dist[" << d1+d2 << "][" << bestd << "]" << endl;
					if (d1 + d2<bestd) {
						bestd = d1 + d2;
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
			DepWr[bi][bj][dep] = 2 * graybit + 2 * indrate;
			//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			for (x = 0;x<wr;x++)
			{
				for (y = 0;y<wc;y++)
				{
					sgn = ((y - indexes[besti][1])*(indexes[bestj][0] - indexes[besti][0]) - (x - indexes[besti][0])*(indexes[bestj][1] - indexes[besti][1])) >= 0;
					if (sgn)
						imgw[bi + x][bj + y][dep] = bestm1;
					else
						imgw[bi + x][bj + y][dep] = bestm2;
				}
			}

			// Calculate Pipelets
			if (EnablePipeletDep)
			{
				bestd = (1 << 30);
				for (i1 = 0;i1<mxindex;i1++)
				{
					for (i2 = 0;i2<mxindex;i2++)
					{
						if (i1 == i2) continue;
						for (i3 = 0;i3<mxindex;i3++)
						{
							if (i3 == i1 || i3 == i2) continue;
							if (GetIndex4(wr, wc, indexes[i1][0], indexes[i1][1], indexes[i2][0], indexes[i2][1], indexes[i3][0], indexes[i3][1], i4) == false) continue;
							if (CheckIndex(wr, wc, i1, i2, i3, i4) == false) continue;

							sgn1 = ((indexes[i1][1] - indexes[i2][1])*(indexes[i3][0] - indexes[i2][0]) - (indexes[i1][0] - indexes[i2][0])*(indexes[i3][1] - indexes[i2][1])) >= 0;
							sgn2 = ((indexes[i3][1] - indexes[i1][1])*(indexes[i4][0] - indexes[i1][0]) - (indexes[i3][0] - indexes[i1][0])*(indexes[i4][1] - indexes[i1][1])) >= 0;

							if (sgn1)
							{
								if (sgn2)
								{
									n1 = wedn1[i1][i4] - wedn2[i2][i3];
									s1 = weds1[i1][i4] - weds2[i2][i3];
									p1 = wedp1[i1][i4] - wedp2[i2][i3];
									n2 = wedn2[i1][i4] + wedn2[i2][i3];
									s2 = weds2[i1][i4] + weds2[i2][i3];
									p2 = wedp2[i1][i4] + wedp2[i2][i3];
									sgn = false;
								}
								else {
									n1 = wedn1[i2][i3] - wedn1[i1][i4];
									s1 = weds1[i2][i3] - weds1[i1][i4];
									p1 = wedp1[i2][i3] - wedp1[i1][i4];
									n2 = wedn2[i2][i3] + wedn1[i1][i4];
									s2 = weds2[i2][i3] + weds1[i1][i4];
									p2 = wedp2[i2][i3] + wedp1[i1][i4];
									sgn = true;
								}
							}
							else {
								if (sgn2)
								{
									n1 = wedn1[i1][i4] - wedn1[i2][i3];
									s1 = weds1[i1][i4] - weds1[i2][i3];
									p1 = wedp1[i1][i4] - wedp1[i2][i3];
									n2 = wedn2[i1][i4] + wedn1[i2][i3];
									s2 = weds2[i1][i4] + weds1[i2][i3];
									p2 = wedp2[i1][i4] + wedp1[i2][i3];
									sgn = true;
								}
								else {
									n1 = wedn2[i2][i3] - wedn1[i1][i4];
									s1 = weds2[i2][i3] - weds1[i1][i4];
									p1 = wedp2[i2][i3] - wedp1[i1][i4];
									n2 = wedn1[i2][i3] + wedn1[i1][i4];
									s2 = weds1[i2][i3] + weds1[i1][i4];
									p2 = wedp1[i2][i3] + wedp1[i1][i4];
									sgn = false;
								}
							}

							if (n1>0)
							{
								m1 = ((int)round((double)s1 / n1 / pow2rem))*pow2rem;
								d1 = p1 + n1*m1*m1 - 2 * m1*s1;
							}
							else {
								m1 = 0;
								d1 = 0;
							}
							if (n2>0)
							{
								m2 = ((int)round((double)s2 / n2 / pow2rem))*pow2rem;
								d2 = p2 + n2*m2*m2 - 2 * m2*s2;
							}
							else {
								m2 = 0;
								d2 = 0;
							}

							//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
							if (d1 + d2<bestd) {
								bestd = d1 + d2;
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
								if (bestd == 0) {
									i1 = 10000;
									i2 = 10000;
									i3 = 10000;
								}
							}
						}
					}
				}

				DepPi1[bi][bj][dep] = besti1;
				DepPi2[bi][bj][dep] = besti2;
				DepPi3[bi][bj][dep] = besti3;
				DepPi4[bi][bj][dep] = besti4;
				DepPm1[bi][bj][dep] = bestm1;
				DepPm2[bi][bj][dep] = bestm2;
				DepPd[bi][bj][dep] = bestd;
				DepPr[bi][bj][dep] = 2 * graybit + 3 * indrate;

				//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
				for (x = 0;x<wr;x++)
				{
					for (y = 0;y<wc;y++)
					{
						sgn1 = ((y - indexes[besti1][1])*(indexes[besti4][0] - indexes[besti1][0]) - (x - indexes[besti1][0])*(indexes[besti4][1] - indexes[besti1][1])) >= 0;
						sgn2 = ((y - indexes[besti2][1])*(indexes[besti3][0] - indexes[besti2][0]) - (x - indexes[besti2][0])*(indexes[besti3][1] - indexes[besti2][1])) >= 0;
						if (sgn1 ^ sgn2)
							imgp[bi + x][bj + y][dep] = bestm1;
						else
							imgp[bi + x][bj + y][dep] = bestm2;
					}
				}

			}

			// Calculate PipeletsGauss
			//if (EnablePipeletGaussDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			for (x=0;x<wr;x++)
			//			{
			//				for (y=0;y<wr;y++)
			//				{
			//					sgn1t = ((y-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(x-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//					sgn2t = ((y-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(x-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;
			//					if (sgn1t ^ sgn2t == true)
			//					{

			//					}
			//				}
			//			}

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<wc;x++)
			//{
			//	for (y=0;y<wr;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

			// Calculate Kleinlet
			if (EnableKleinletDep)
			{

				bestd = (1 << 30);
				for (i1 = 0;i1<mxindex;i1++)
				{
					for (i2 = 0;i2<mxindex;i2++)
					{
						for (i3 = 0;i3<mxindex;i3++)
						{
							for (i4 = 0;i4<mxindex;i4++)
							{
								if (CheckIndex(wr, wc, i1, i2, i3, i4) == false) continue;
								n1 = 0; s1 = 0; p1 = 0; n2 = 0; s2 = 0; p2 = 0;

								sgn1 = ((indexes[i1][1] - indexes[i2][1])*(indexes[i3][0] - indexes[i2][0]) - (indexes[i1][0] - indexes[i2][0])*(indexes[i3][1] - indexes[i2][1])) >= 0;
								sgn2 = ((indexes[i3][1] - indexes[i1][1])*(indexes[i4][0] - indexes[i1][0]) - (indexes[i3][0] - indexes[i1][0])*(indexes[i4][1] - indexes[i1][1])) >= 0;

								if (sgn1)
								{
									if (sgn2)
									{
										n1 = wedn1[i1][i4] - wedn2[i2][i3];
										s1 = weds1[i1][i4] - weds2[i2][i3];
										p1 = wedp1[i1][i4] - wedp2[i2][i3];
										n2 = wedn2[i1][i4] + wedn2[i2][i3];
										s2 = weds2[i1][i4] + weds2[i2][i3];
										p2 = wedp2[i1][i4] + wedp2[i2][i3];
										sgn = false;
									}
									else {
										n1 = wedn1[i2][i3] - wedn1[i1][i4];
										s1 = weds1[i2][i3] - weds1[i1][i4];
										p1 = wedp1[i2][i3] - wedp1[i1][i4];
										n2 = wedn2[i2][i3] + wedn1[i1][i4];
										s2 = weds2[i2][i3] + weds1[i1][i4];
										p2 = wedp2[i2][i3] + wedp1[i1][i4];
										sgn = true;
									}
								}
								else {
									if (sgn2)
									{
										n1 = wedn1[i1][i4] - wedn1[i2][i3];
										s1 = weds1[i1][i4] - weds1[i2][i3];
										p1 = wedp1[i1][i4] - wedp1[i2][i3];
										n2 = wedn2[i1][i4] + wedn1[i2][i3];
										s2 = weds2[i1][i4] + weds1[i2][i3];
										p2 = wedp2[i1][i4] + wedp1[i2][i3];
										sgn = true;
									}
									else {
										n1 = wedn2[i2][i3] - wedn1[i1][i4];
										s1 = weds2[i2][i3] - weds1[i1][i4];
										p1 = wedp2[i2][i3] - wedp1[i1][i4];
										n2 = wedn1[i2][i3] + wedn1[i1][i4];
										s2 = weds1[i2][i3] + weds1[i1][i4];
										p2 = wedp1[i2][i3] + wedp1[i1][i4];
										sgn = false;
									}
								}

								if (n1>0)
								{
									m1 = ((int)round((double)s1 / n1 / pow2rem))*pow2rem;
									d1 = p1 + n1*m1*m1 - 2 * m1*s1;
								}
								else {
									m1 = 0;
									d1 = 0;
								}
								if (n2>0)
								{
									m2 = ((int)round((double)s2 / n2 / pow2rem))*pow2rem;
									d2 = p2 + n2*m2*m2 - 2 * m2*s2;
								}
								else {
									m2 = 0;
									d2 = 0;
								}

								//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
								if (d1 + d2<bestd) {
									bestd = d1 + d2;
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
									if (bestd == 0) {
										i1 = 10000;
										i2 = 10000;
										i3 = 10000;
										i4 = 10000;
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
				DepKr[bi][bj][dep] = 3 * graybit + 4 * indrate;

				//cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
				for (x = 0;x<wr;x++)
				{
					for (y = 0;y<wc;y++)
					{
						sgn1 = ((y - indexes[besti1][1])*(indexes[besti4][0] - indexes[besti1][0]) - (x - indexes[besti1][0])*(indexes[besti4][1] - indexes[besti1][1])) >= 0;
						sgn2 = ((y - indexes[besti2][1])*(indexes[besti3][0] - indexes[besti2][0]) - (x - indexes[besti2][0])*(indexes[besti3][1] - indexes[besti2][1])) >= 0;
						if (sgn1 ^ sgn2)
							imgk[bi + x][bj + y][dep] = bestm1;
						else
							imgk[bi + x][bj + y][dep] = bestm2;
					}
				}

			}

			// Calculate Gaussian
			//if (EnableGaussianDep)
			//{			
			//bestd = (1<<30);
			//for (i1=0;i1<mxindex;i1++)
			//{
			//	for (i2=0;i2<mxindex;i2++)
			//	{
			//		if (i1==i2) continue;
			//		for (i3=0;i3<mxindex;i3++)
			//		{
			//			if (i3==i1 || i3==i2) continue;
			//			if (GetIndex4(wr, wc,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
			//			if (CheckIndex(wr,wc,i1,i2,i3,i4)==false) continue;						
			//			
			//			sgn1 = ((indexes[i1][1]-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(indexes[i1][0]-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
			//			sgn2 = ((indexes[i3][1]-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(indexes[i3][0]-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;

			//			if (sgn1)
			//			{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn2[i2][i3];
			//					s1 = weds1[i1][i4]-weds2[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp2[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn2[i2][i3];
			//					s2 = weds2[i1][i4]+weds2[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp2[i2][i3];
			//					sgn = false;
			//				}else{
			//					n1 = wedn1[i2][i3]-wedn1[i1][i4];
			//					s1 = weds1[i2][i3]-weds1[i1][i4];
			//					p1 = wedp1[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn2[i2][i3]+wedn1[i1][i4];
			//					s2 = weds2[i2][i3]+weds1[i1][i4];
			//					p2 = wedp2[i2][i3]+wedp1[i1][i4];
			//					sgn = true;
			//				}
			//			}else{
			//				if (sgn2)
			//				{
			//					n1 = wedn1[i1][i4]-wedn1[i2][i3];
			//					s1 = weds1[i1][i4]-weds1[i2][i3];
			//					p1 = wedp1[i1][i4]-wedp1[i2][i3];
			//					n2 = wedn2[i1][i4]+wedn1[i2][i3];
			//					s2 = weds2[i1][i4]+weds1[i2][i3];
			//					p2 = wedp2[i1][i4]+wedp1[i2][i3];
			//					sgn = true;
			//				}else{
			//					n1 = wedn2[i2][i3]-wedn1[i1][i4];
			//					s1 = weds2[i2][i3]-weds1[i1][i4];
			//					p1 = wedp2[i2][i3]-wedp1[i1][i4];
			//					n2 = wedn1[i2][i3]+wedn1[i1][i4];
			//					s2 = weds1[i2][i3]+weds1[i1][i4];
			//					p2 = wedp1[i2][i3]+wedp1[i1][i4];
			//					sgn = false;
			//				}
			//			}

			//			if (n1>0)
			//			{
			//				m1 = (int)round((double)s1/n1);
			//				d1 = p1 + n1*m1*m1 - 2*m1*s1;
			//			}else{
			//				m1=0;
			//				d1=0;
			//			}
			//			if (n2>0)
			//			{
			//				m2 = (int)round((double)s2/n2);
			//				d2 = p2 + n2*m2*m2 - 2*m2*s2;
			//			}else{
			//				m2=0;
			//				d2=0;
			//			}

			//			//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
			//			if (d1+d2<bestd){
			//				bestd = d1+d2;
			//				besti1 = i1;
			//				besti2 = i2;
			//				besti3 = i3;
			//				besti4 = i4;
			//				if (!sgn)
			//				{
			//					bestm1 = m2;
			//					bestm2 = m1;
			//				}
			//				else
			//				{
			//					bestm1 = m1;
			//					bestm2 = m2;
			//				}
			//				//cout << "( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << endl;
			//				if (bestd==0){
			//					i1 = 10000;
			//					i2 = 10000;
			//					i3 = 10000;
			//				}
			//			}
			//		}
			//	}
			//}

			//DepPi1[bi][bj][dep] = besti1;
			//DepPi2[bi][bj][dep] = besti2;
			//DepPi3[bi][bj][dep] = besti3;
			//DepPi4[bi][bj][dep] = besti4;
			//DepPm1[bi][bj][dep] = bestm1;
			//DepPm2[bi][bj][dep] = bestm2;
			//DepPd[bi][bj][dep] = bestd;
			//DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			////cout << "(" << bi << "," << bj << ") -- " << bestd << endl;
			//for (x=0;x<w;x++)
			//{
			//	for (y=0;y<w;y++)
			//	{
			//		sgn1 = ((y-indexes[besti1][1])*(indexes[besti4][0]-indexes[besti1][0])-(x-indexes[besti1][0])*(indexes[besti4][1]-indexes[besti1][1])) >= 0;
			//		sgn2 = ((y-indexes[besti2][1])*(indexes[besti3][0]-indexes[besti2][0])-(x-indexes[besti2][0])*(indexes[besti3][1]-indexes[besti2][1])) >= 0;
			//		if (sgn1 ^ sgn2)
			//			imgp[bi+x][bj+y][dep] = bestm1;
			//		else
			//			imgp[bi+x][bj+y][dep] = bestm2;
			//	}
			//}

			//}

		}
	}

}

void CalcConstant(int nr, int nc, int w, int dep, int graybit)
{
	int bi,bj,x,y;
	int wr,wc;
	int m;
	int n1;
	long long s,p;
	long long d;
	int pow2rem = 1<<(8-graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	for (bi=0;bi<nr;bi+=w)
	{
		for (bj=0;bj<nc;bj+=w)
		{
			wr = min(w,nr-bi);
			wc = min(w,nc-bj);
			n1=0;
			s=0;
			p=0;
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					n1++;
					s += img[bi+x][bj+y];
					p += (img[bi+x][bj+y] * img[bi+x][bj+y]);
				}
			}
			m = ((int)round((double)s/n1/pow2rem))*pow2rem;
			if (m > maxgray) m = maxgray;
			d = p + (long long)n1*m*m - 2*m*s;
			
			DepCm[bi][bj][dep] = m;
			DepCd[bi][bj][dep] = d;
			DepCr[bi][bj][dep] = graybit;

			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					imgc[bi+x][bj+y][dep] = m;
				}
			}
		}
	}
}


void CalcConstantMasked(int nr, int nc, int w, int dep)
{
	int bi,bj,x,y;
	int wr,wc;
	int m;
	int n1;
	long long s,p;
	long long d;

	for (bi=0;bi<nr;bi+=w)
	{
		for (bj=0;bj<nc;bj+=w)
		{
			wr = min(w,nr-bi);
			wc = min(w,nc-bj);
			n1=0;
			s=0;
			p=0;
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					if (imgmask[bi+x][bj+y]==true)
					{
						n1++;
						s += img[bi+x][bj+y];
						p += (img[bi+x][bj+y] * img[bi+x][bj+y]);
					}
				}
			}
			if (n1==0)
			{
				m = 0;
				d = 0;
			}
			else
			{
				m = (int)round((double)s/n1);
				d = p + (long long)n1*m*m - 2*m*s;
			}
			
			DepCm[bi][bj][dep] = m;
			DepCd[bi][bj][dep] = d;
			DepCr[bi][bj][dep] = GRAYBIT;

			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					imgc[bi+x][bj+y][dep] = m;
				}
			}
		}
	}
}


void CalcConstantMasked2(int nr, int nc, int w, int dep)
{
	int bi,bj,x,y;
	int wr,wc;
	int m;
	int n1;
	long long s,p;
	long long d;

	for (bi=0;bi<nr;bi+=w)
	{
		for (bj=0;bj<nc;bj+=w)
		{
			wr = min(w,nr-bi);
			wc = min(w,nc-bj);
			n1=0;
			s=0;
			p=0;
			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					if (imgmask[bi+x][bj+y]==true)
					{
						n1++;
						s += img[bi+x][bj+y];
						p += (img[bi+x][bj+y] * img[bi+x][bj+y]);
					}
				}
			}
			if (n1==0)
			{
				m = 0;
				d = 0;
				DepCm[bi][bj][dep] = m;
				DepCd[bi][bj][dep] = d;
				DepCr[bi][bj][dep] = 0;
			}
			else
			{
				m = (int)round((double)s/n1);
				d = p + (long long)n1*m*m - 2*m*s;
				DepCm[bi][bj][dep] = m;
				DepCd[bi][bj][dep] = d;
				DepCr[bi][bj][dep] = GRAYBIT;
			}

			for (x=0;x<wr;x++)
			{
				for (y=0;y<wc;y++)
				{
					if (imgmask[bi+x][bj+y]==false)
						imgc[bi+x][bj+y][dep]=0;
					else
						imgc[bi+x][bj+y][dep] = m;
				}
			}
		}
	}
}


void GetNeighbours(BinaryStream num, int len, BinaryStream *neighbours)
{
	int i;
	for (i = 0; i < len; i++)
	{
		neighbours[i] = num;
		neighbours[i].ReverseBit(i);
	}
}

int GetVertexWeight(BinaryStream num, int len, int graybit, int &c1, int &c2)
{
	int j;
	int n1, n2, s1, s2, p1, p2;
	int pow2rem = 1<<(8-graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	n1 = n2 = 0;
	s1 = s2 = 0;
	p1 = p2 = 0;
	
	for (j = 0; j < len; j++)
	{
		if (num.State(j))
		{
			n1 += StatVars[j][0];
			s1 += StatVars[j][1];
			p1 += StatVars[j][2];
		}
		else
		{
			n2 += StatVars[j][0];
			s2 += StatVars[j][1];
			p2 += StatVars[j][2];
		}
	}
	if (n1 > 0)
	{
		c1 = ((int)((double)s1 / n1 / pow2rem))*pow2rem;
		if (c1 > maxgray) c1 = maxgray;
	}
	else
		c1 = 0;
	if (n2 > 0)
	{
		c2 = ((int)((double)s2 / n2 / pow2rem))*pow2rem;
		if (c2 > maxgray) c2 = maxgray;
	}
	else
		c2 = 0;
	return p1 + n1*c1*c1 - 2 * c1*s1 + p2 + n2*c2*c2 - 2 * c2*s2;

}

BinaryStream GetInitState(int len)
{
	int i;
	int sums = 0, sumn = 0, prec;
	BinaryStream initstate;
	for (i = 0; i < len; i++)
	{
		StatVars[i][3] = (int)round((double)StatVars[i][1] / StatVars[i][0]);
		sums += StatVars[i][1];
		sumn += StatVars[i][0];
	}
	prec = (int)round((double)sums / sumn);
	for (i = 0; i < len; i++)
	{
		if (StatVars[i][3] > prec)
			initstate.SetBit(i);
	}
	return initstate;
}

BinaryStream GetRandomState(int len)
{
	int i;
	BinaryStream randstate;
	for (i = 0; i < len; i++)
	{
		if (rand()%2==0)
			randstate.SetBit(i);
	}
	return randstate;
}

void EvaluateFromState(BinaryStream initstate, int len, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2)
{
	int j;
	int minw, ming1, ming2, gr1, gr2;
	BinaryStream minv;

	bestv = initstate;
	bestvw = GetVertexWeight(bestv, len, graybit, g1, g2);
	BinaryStream neighbours[IMGSZR + IMGSZC];

	//cout << "Begin : " << bestv << " (" << bestvw << ")" << endl;

	while (true)
	{
		minw = (1 << 30);
		GetNeighbours(bestv, len, neighbours);
		for (j = 0; j < len; j++)
		{
			int w = GetVertexWeight(neighbours[j], len, graybit, gr1, gr2);
			//cout << w << " , ";
			if (w < minw)
			{
				minw = w;
				minv = neighbours[j];
				ming1 = gr1;
				ming2 = gr2;
			}
		}
		//cout << endl;
		//cout << "BestV : " << minv << " (" << minw << ")" << endl;
		if (minw < bestvw)
		{
			bestv = minv;
			bestvw = minw;
			g1 = ming1;
			g2 = ming2;
		}
		else
		{
			break;
		}
	}
}

void ComputeBestAssignment(int len, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2)
{
	int i;
	int gr1, gr2;
	long long vw;
	BinaryStream v,state;

	state = GetInitState(len);
	EvaluateFromState(state, len, graybit, bestv, bestvw, g1, g2);
	for (i = 0; i < 0; i++)
	{
		state = GetRandomState(len);
		EvaluateFromState(state, len, graybit, v, vw, gr1, gr2);
		if (vw < bestvw)
		{
			bestv = v;
			bestvw = vw;
			g1 = gr1;
			g2 = gr2;
		}
	}
}

void ComputeBlock(int rb, int re, int cb, int ce, int oor, int ooc, double curve, int ver, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2, int &codedlinelength)
{
	int r, c, i, j;
	double dr, dc, munder, b;
	double theta, sind, cosd, r2, c2;
	int mindist, maxdist;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	//cout << "orientation [" << or << " : " << oc << "] to [" << cr << " : " << cc << "]";
	if (cr == oor && cc == ooc)
	{
		bestvw = (1 << 30);
		//cout << endl;
		return;
	}

	// calculate distance to determine lines
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr),abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		theta = atan2(dc,dr);
		sind = sin(-theta);
		cosd = cos(-theta);
		munder = max(abs(sind),abs(cosd));
		b = max(re-rb+1,ce-cb+1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r-oor)+sind*(c-ooc);
				c2 = -sind*(r-oor)+cosd*(c-ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder)-0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}

	//if (maxdist-mindist>200)
	//	printf("D| %d \n",maxdist-mindist);

	// set statistical variables to zero
	for (i = 0; i <= maxdist - mindist; i++)
		for (j = 0; j < 3; j++)
			StatVars[i][j] = 0;

	// gathering line statistics n,s,p
	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			StatVars[Dist[r][c] - mindist][0]++; // n
			StatVars[Dist[r][c] - mindist][1] += img[r][c]; // s
			StatVars[Dist[r][c] - mindist][2] += img[r][c] * img[r][c]; // p
		}
	}

	codedlinelength = maxdist - mindist + 1;
	//cout << " L " << codedlinelength << endl;
	//for (i = 0; i < codedlinelength; i++)
	//	cout << "State " << i << " [" << StatVars[i][0] << "-" << StatVars[i][1] << "-" << StatVars[i][2] << "-" << (int)round(StatVars[i][1] / (double)StatVars[i][0]) << "]" << endl;
	ComputeBestAssignment(codedlinelength, graybit, bestv, bestvw, g1, g2);
	//cout << "Result : " << bestv << " (" << bestvw << ")" << endl;
	//cout << endl << "-------------------------" << endl;
}

void ComputeBlock(int rb, int re, int cb, int ce, int ver, int graybit, long long &d, BinaryStream &codedline, int &theta, int &curve, int &g1, int &g2, int &codedlinelength, int &curvelength)
{
	int r, c;
	int gr1, gr2,codelen,selcodelen;
	int curveb, curvee, bcurve;
	BinaryStream bestv, selv;
	long long bestvw, selvw;

	//cout << "ComputeBlock (" << rb << "," << re << "),(" << cb << "," << ce << ")  " << re-rb+1 << "-" << ce-cb+1 << endl;

	if (ver==3 && max(re-rb+1,ce-cb+1)>=8)
	{
		curvelength = (1<<DoTransformMaxCurveBit);
		curveb = -(curvelength/2)+1;
		curvee = curveb + curvelength - 1;
	}
	else
	{
		curveb = 0;
		curvee = 0;
		curvelength = 1;
	}

	selvw = (1 << 30);
	selcodelen = 10000;	
	for (bcurve = curveb; bcurve<=curvee;bcurve++)
	{
		for (r = rb; r < re; r++)
		{
			ComputeBlock(rb, re, cb, ce, r, cb, bcurve, ver, graybit, bestv, bestvw, gr1, gr2, codelen);
			if (bestvw<selvw)
			{
				selv = bestv;
				selvw = bestvw;
				theta = r - rb;
				curve = bcurve;
				g1 = gr1;
				g2 = gr2;
				selcodelen = codelen;
			}
		}
		for (c = cb; c <= ce; c++)
		{
			ComputeBlock(rb, re, cb, ce, re, c, bcurve, ver, graybit, bestv, bestvw, gr1, gr2, codelen);
			if (bestvw<selvw)
			{
				selv = bestv;
				selvw = bestvw;
				theta = re - rb + c - cb;
				curve = bcurve;
				g1 = gr1;
				g2 = gr2;
				selcodelen = codelen;
			}
		}
	}

	d = selvw;
	codedline = selv;
	codedlinelength = selcodelen;
}

void EvaluateFromState2(BinaryStream initstate, int len, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2)
{
	int j;
	int minw, ming1, ming2;
	int n1, n2, s1, s2, p1, p2;
	int nn1, nn2, ns1, ns2, np1, np2;
	int c1, c2;
	int w;
	int minvj;
	int pow2rem = 1 << (8 - graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	bestv = initstate;

	n1 = n2 = 0;
	s1 = s2 = 0;
	p1 = p2 = 0;

	for (j = 0; j < len; j++)
	{
		if (bestv.State(j))
		{
			n1 += StatVars[j][0];
			s1 += StatVars[j][1];
			p1 += StatVars[j][2];
		}
		else
		{
			n2 += StatVars[j][0];
			s2 += StatVars[j][1];
			p2 += StatVars[j][2];
		}
	}

	if (n1 > 0)
	{
		c1 = ((int)((double)s1 / n1 / pow2rem))*pow2rem;
		if (c1 > maxgray) c1 = maxgray;
	}
	else
		c1 = 0;
	if (n2 > 0)
	{
		c2 = ((int)((double)s2 / n2 / pow2rem))*pow2rem;
		if (c2 > maxgray) c2 = maxgray;
	}
	else
		c2 = 0;
	bestvw = p1 + n1*c1*c1 - 2 * c1*s1 + p2 + n2*c2*c2 - 2 * c2*s2;
	g1 = c1; g2 = c2;

	//cout << "Begin : " << bestv << " (" << bestvw << ")" << endl;

	while (true)
	{
		minw = (1 << 30);

		for (j = 0; j < len; j++)
		{
			if (bestv.State(j) == true)
			{
				nn1 = n1 - StatVars[j][0];
				ns1 = s1 - StatVars[j][1];
				np1 = p1 - StatVars[j][2];

				nn2 = n2 + StatVars[j][0];
				ns2 = s2 + StatVars[j][1];
				np2 = p2 + StatVars[j][2];
			}
			else
			{
				nn1 = n1 + StatVars[j][0];
				ns1 = s1 + StatVars[j][1];
				np1 = p1 + StatVars[j][2];

				nn2 = n2 - StatVars[j][0];
				ns2 = s2 - StatVars[j][1];
				np2 = p2 - StatVars[j][2];
			}

			if (nn1 > 0)
			{
				c1 = ((int)((double)ns1 / nn1 / pow2rem))*pow2rem;
				if (c1 > maxgray) c1 = maxgray;
			}
			else
				c1 = 0;
			if (n2 > 0)
			{
				c2 = ((int)((double)ns2 / nn2 / pow2rem))*pow2rem;
				if (c2 > maxgray) c2 = maxgray;
			}
			else
				c2 = 0;
			w = np1 + nn1*c1*c1 - 2 * c1*ns1 + np2 + nn2*c2*c2 - 2 * c2*ns2;

			//cout << w << " , ";
			if (w < minw)
			{
				minw = w;
				minvj = j;
				ming1 = c1;
				ming2 = c2;
			}
		}
		//cout << endl;
		//cout << "BestV : " << minv << " (" << minw << ")" << endl;
		if (minw < bestvw)
		{
			if (bestv.State(minvj) == true)
			{
				nn1 = n1 - StatVars[minvj][0];
				ns1 = s1 - StatVars[minvj][1];
				np1 = p1 - StatVars[minvj][2];

				nn2 = n2 + StatVars[minvj][0];
				ns2 = s2 + StatVars[minvj][1];
				np2 = p2 + StatVars[minvj][2];
			}
			else
			{
				nn1 = n1 + StatVars[minvj][0];
				ns1 = s1 + StatVars[minvj][1];
				np1 = p1 + StatVars[minvj][2];

				nn2 = n2 - StatVars[minvj][0];
				ns2 = s2 - StatVars[minvj][1];
				np2 = p2 - StatVars[minvj][2];
			}

			n1 = nn1; n2 = nn2;
			s1 = ns1; s2 = ns2;
			p1 = np1; p2 = np2;

			bestv.ReverseBit(minvj);
			bestvw = minw;
			g1 = ming1;
			g2 = ming2;
		}
		else
		{
			break;
		}
	}
}

void ComputeBestAssignment2(int len, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2)
{
	int i;
	int gr1, gr2;
	long long vw;
	BinaryStream v, state;

	state = GetInitState(len);
	EvaluateFromState2(state, len, graybit, bestv, bestvw, g1, g2);
	for (i = 0; i < 0; i++)
	{
		state = GetRandomState(len);
		EvaluateFromState2(state, len, graybit, v, vw, gr1, gr2);
		if (vw < bestvw)
		{
			bestv = v;
			bestvw = vw;
			g1 = gr1;
			g2 = gr2;
		}
	}
}

void ComputeBlock2(int rb, int re, int cb, int ce, int oor, int ooc, double curve, int ver, int graybit, BinaryStream &bestv, long long &bestvw, int &g1, int &g2, int &codedlinelength)
{
	int r, c, i, j;
	double dr, dc, munder, b;
	double theta, sind, cosd, r2, c2;
	int mindist, maxdist;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	//cout << "orientation [" << or << " : " << oc << "] to [" << cr << " : " << cc << "]";
	if (cr == oor && cc == ooc)
	{
		bestvw = (1 << 30);
		//cout << endl;
		return;
	}

	// calculate distance to determine lines
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr), abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		theta = atan2(dc, dr);
		sind = sin(-theta);
		cosd = cos(-theta);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}

	//if (maxdist-mindist>200)
	//	printf("D| %d \n",maxdist-mindist);

	// set statistical variables to zero
	for (i = 0; i <= maxdist - mindist; i++)
		for (j = 0; j < 3; j++)
			StatVars[i][j] = 0;

	// gathering line statistics n,s,p
	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			StatVars[Dist[r][c] - mindist][0]++; // n
			StatVars[Dist[r][c] - mindist][1] += img[r][c]; // s
			StatVars[Dist[r][c] - mindist][2] += img[r][c] * img[r][c]; // p
		}
	}

	codedlinelength = maxdist - mindist + 1;
	//cout << " L " << codedlinelength << endl;
	//for (i = 0; i < codedlinelength; i++)
	//	cout << "State " << i << " [" << StatVars[i][0] << "-" << StatVars[i][1] << "-" << StatVars[i][2] << "-" << (int)round(StatVars[i][1] / (double)StatVars[i][0]) << "]" << endl;
	ComputeBestAssignment2(codedlinelength, graybit, bestv, bestvw, g1, g2);
	//cout << "Result : " << bestv << " (" << bestvw << ")" << endl;
	//cout << endl << "-------------------------" << endl;
}

void ComputeBlock2(int rb, int re, int cb, int ce, int ver, int graybit, long long &d, BinaryStream &codedline, int &theta, int &curve, int &g1, int &g2, int &codedlinelength, int &curvelength)
{
	int r, c;
	int gr1, gr2, codelen, selcodelen;
	int curveb, curvee, bcurve;
	BinaryStream bestv, selv;
	long long bestvw, selvw;

	//cout << "ComputeBlock (" << rb << "," << re << "),(" << cb << "," << ce << ")  " << re-rb+1 << "-" << ce-cb+1 << endl;

	if (ver == 3 && max(re - rb + 1, ce - cb + 1) >= 8)
	{
		curvelength = (1 << DoTransformMaxCurveBit);
		curveb = -(curvelength / 2) + 1;
		curvee = curveb + curvelength - 1;
	}
	else
	{
		curveb = 0;
		curvee = 0;
		curvelength = 1;
	}

	selvw = (1 << 30);
	selcodelen = 10000;
	for (bcurve = curveb; bcurve <= curvee;bcurve++)
	{
		for (r = rb; r < re; r++)
		{
			ComputeBlock2(rb, re, cb, ce, r, cb, bcurve, ver, graybit, bestv, bestvw, gr1, gr2, codelen);
			if (bestvw<selvw)
			{
				selv = bestv;
				selvw = bestvw;
				theta = r - rb;
				curve = bcurve;
				g1 = gr1;
				g2 = gr2;
				selcodelen = codelen;
			}
		}
		for (c = cb; c <= ce; c++)
		{
			ComputeBlock2(rb, re, cb, ce, re, c, bcurve, ver, graybit, bestv, bestvw, gr1, gr2, codelen);
			if (bestvw<selvw)
			{
				selv = bestv;
				selvw = bestvw;
				theta = re - rb + c - cb;
				curve = bcurve;
				g1 = gr1;
				g2 = gr2;
				selcodelen = codelen;
			}
		}
	}

	d = selvw;
	codedline = selv;
	codedlinelength = selcodelen;
}

void RebuildBlock(int rb, int re, int cb, int ce, int dep, int ver, int graybit, BinaryStream codedline, int theta, int curve, int g1, int g2)
{
	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	// second line
	int oor, ooc;
	if (theta<re - rb)
	{
		oor = theta + rb;
		ooc = cb;
	}
	else
	{
		oor = re;
		ooc = theta - (re - rb) + cb;
	}

	// calculate distance to determine lines
	int r, c;
	double dr, dc, munder;
	int mindist, maxdist;
	double thetarc, sind, cosd, b, r2, c2;
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	//int len = maxdist - mindist + 1;

	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr),abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		thetarc = atan2(dc,dr);
		sind = sin(-thetarc);
		cosd = cos(-thetarc);
		munder = max(abs(sind),abs(cosd));
		b = max(re-rb+1,ce-cb+1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r-oor)+sind*(c-ooc);
				c2 = -sind*(r-oor)+cosd*(c-ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder)-0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}

	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if (codedline.State(Dist[r][c] - mindist))
				imgw[r][c][dep] = g1;
			else
				imgw[r][c][dep] = g2;
		}
	}

}

void CalcOneDepthCoded(int nr, int nc, int w, int dep, int ver, int graybit)
{
	cout << "CalcOneDepthCoded for dep : " << dep << endl;
	int bi, bj;
	int wr, wc;
	long long bestd;
	int bestm1, bestm2;
	int thetarate, codedlinerate, curverate;
	BinaryStream codedline;
	int theta, curve, codedlinelength, curvelength;

	for (bi = 0; bi<nr; bi += w)
	{
		for (bj = 0; bj<nc; bj += w)
		{
			// Calculate for one Square
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);

			ComputeBlock2(bi, bi + wr - 1, bj, bj + wc - 1, ver, graybit, bestd, codedline, theta, curve, bestm1, bestm2, codedlinelength, curvelength);

			thetarate = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinerate = (int)ceil(log((double)codedlinelength) / log(2.0));
			curverate = (int)ceil(log((double)curvelength) / log(2.0));

			DepWbs[bi][bj][dep] = codedline;
			DepWj[bi][bj][dep] = theta;
			DepWm1[bi][bj][dep] = bestm1;
			DepWm2[bi][bj][dep] = bestm2;
			DepWd[bi][bj][dep] = bestd;
			DepWr[bi][bj][dep] = 2 * graybit + thetarate + codedlinerate + curverate;

			RebuildBlock(bi, bi + wr - 1, bj, bj + wc - 1, dep, ver, graybit, codedline, theta, curve, bestm1, bestm2);
		}
	}

}


void PreSetArray(int nr, int nc, int dep)
{
	int i,j;

	for (i=0;i<nr;i++)
	{
		for (j=0;j<nc;j++)
		{
			DepCd[i][j][dep] = (1 << 30);
			DepCr[i][j][dep] = 1000;
			DepWd[i][j][dep] = (1 << 30);
			DepWr[i][j][dep] = 1000;
			DepPd[i][j][dep] = (1 << 30);
			DepPr[i][j][dep] = 1000;
			DepKd[i][j][dep] = (1<<30);
			DepKr[i][j][dep] = 1000;
		}
	}
}

void DoTransforms(int nr, int nc)
{
	clock_t depTimeC;
	double depTime;
	int dep,w,ndep;

	cout << "Status : wedgelet["<<EnableWedgeletDep<<"] pipelet["<<EnablePipeletDep<<"] kleinlet["<<EnableKleinletDep<<"]" << endl;

	for (dep=0,w=1;(w>>1)<max(nr,nc);dep++,w*=2)
	{
		PreSetArray(nr,nc,dep);
		CalcConstant(nr,nc,w,dep,GRAYBIT);
	}
	ndep = dep-1;
	for (dep=1;dep<=min(DoTransformMaxDepth,ndep);dep++)
	{
		int w = (int)pow(2.0,dep);
		depTimeC = clock();
		CalcOneDepth(nr,nc,w,dep);
		depTime = (clock()-depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}

void DoTransforms(int nr, int nc, int graybit)
{
	clock_t depTimeC;
	double depTime;
	int dep, w, ndep;

	cout << "Status : wedgelet[" << EnableWedgeletDep << "] pipelet[" << EnablePipeletDep << "] kleinlet[" << EnableKleinletDep << "]" << endl;

	for (dep = 0, w = 1;(w >> 1)<max(nr, nc);dep++, w *= 2)
	{
		PreSetArray(nr, nc, dep);
		CalcConstant(nr, nc, w, dep, graybit);
	}
	ndep = dep - 1;
	for (dep = 1;dep <= min(DoTransformMaxDepth, ndep);dep++)
	{
		int w = (int)pow(2.0, dep);
		depTimeC = clock();
		CalcOneDepth(nr, nc, w, dep, graybit);
		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}


void DoTransformsMasked(int nr, int nc)
{
	clock_t depTimeC;
	double depTime;
	int dep,w,ndep;

	cout << "Status : wedgelet["<<EnableWedgeletDep<<"] pipelet["<<EnablePipeletDep<<"] kleinlet["<<EnableKleinletDep<<"]" << endl;

	for (dep=0,w=1;w<=max(nr,nc);dep++,w*=2)
	{
		PreSetArray(nr,nc,dep);
		CalcConstantMasked2(nr,nc,w,dep);
	}
	ndep = dep-1;
	for (dep=1;dep<=min(DoTransformMaxDepth,ndep);dep++)
	{
		int w = (int)pow(2.0,dep);
		depTimeC = clock();
		CalcOneDepthMasked(nr,nc,w,dep);
		depTime = (clock()-depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}

void DoTransformsCoded(int nr, int nc, int ver, int graybit)
{
	clock_t depTimeC;
	double depTime;
	int dep, w, ndep;

	cout << "Status : Coded Type" << endl;

	for (dep = 0, w = 1; (w >> 1)<max(nr, nc); dep++, w *= 2)
	{
		PreSetArray(nr, nc, dep);
		if (EnableConstantDep)
			CalcConstant(nr, nc, w, dep, graybit);
	}
	ndep = dep - 1;
	for (dep = 1; dep <= min(DoTransformMaxDepth, ndep); dep++)
	{
		int w = (int)pow(2.0, dep);
		depTimeC = clock();
		CalcOneDepthCoded(nr, nc, w, dep, ver, graybit);
		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
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

void RDw(int nr, int nc, double lambda, double &psnr, double &bpp)
{
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr,wc;

	for (bi=0;bi<nr;bi++)
	{
		for (bj=0;bj<nc;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgwrd[bi][bj] = imgc[bi][bj][0];
			imgwrdg[bi][bj] = imgc[bi][bj][0];
		}
	}
	
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<nr;bi+=w)
		{
			for (bj=0;bj<nc;bj+=w)
			{
				wr = min(w,nr-bi);
				wc = min(w,nc-bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];
				if (bi+w2<nr)
				{
					chD += OptD[bi+w2][bj];
					chR += OptR[bi+w2][bj];
				}
				if (bj+w2<nc)
				{
					chD += OptD[bi][bj+w2];
					chR += OptR[bi][bj+w2];
				}
				if (bi+w2<nr && bj+w2<nc)
				{
					chD += OptD[bi+w2][bj+w2];
					chR += OptR[bi+w2][bj+w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (2 + DepWr[bi][bj][dep]),
					chD + lambda * (1+chR));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgwrd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgwrdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
						}
						imgwrdg[bi+x][bj] = 127;
						imgwrdg[bi+x][bj+wc-1] = 127;
						imgwrdg[bi][bj+x] = 127;
						imgwrdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepWr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgwrd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgwrdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
						}
						imgwrdg[bi+x][bj] = 127;
						imgwrdg[bi+x][bj+wc-1] = 127;
						imgwrdg[bi][bj+x] = 127;
						imgwrdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==3)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
				}
			}
		}
	}

    mse = (double)OptD[0][0]/(nr*nc);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(nr*nc);
}

void RDp(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{
	//cout << "Begin RDp for lambda : " << lambda << endl;
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr,wc;

	for (bi=0;bi<nr;bi++)
	{
		for (bj=0;bj<nc;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgprd[bi][bj] = imgc[bi][bj][0];
			imgprdg[bi][bj] = imgc[bi][bj][0];
			OptDp[bi][bj] = (double)DepCd[bi][bj][0];
		}
	}
	
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<nr;bi+=w)
		{
			for (bj=0;bj<nc;bj+=w)
			{
				wr = min(w,nr-bi);
				wc = min(w,nc-bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];
				if (bi+w2<nr)
				{
					chD += OptD[bi+w2][bj];
					chR += OptR[bi+w2][bj];
				}
				if (bj+w2<nc)
				{
					chD += OptD[bi][bj+w2];
					chR += OptR[bi][bj+w2];
				}
				if (bi+w2<nr && bj+w2<nc)
				{
					chD += OptD[bi+w2][bj+w2];
					chR += OptR[bi+w2][bj+w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (3 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (3 + DepWr[bi][bj][dep]),
					DepPd[bi][bj][dep] + lambda * (3 + DepPr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepCr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepWr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==3)
				{
					OptD[bi][bj] = DepPd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepPr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgp[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgp[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==4)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
				}
			}
		}
	}

	rate = OptR[0][0];
    mse = (double)OptD[0][0]/(nr*nc);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(nr*nc);		
}


void RDpDebug(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{	
	//cout << "Begin RDp for lambda : " << lambda << endl;
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr,wc;
	int cmpcnt[4];

	for (bi=0;bi<nr;bi++)
	{
		for (bj=0;bj<nc;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgprd[bi][bj] = imgc[bi][bj][0];
			imgprdg[bi][bj] = imgc[bi][bj][0];
			OptDp[bi][bj] = (double)DepCd[bi][bj][0];
			ComponentCnt[bi][bj][0] = 1;
			ComponentCnt[bi][bj][1] = 0;
			ComponentCnt[bi][bj][2] = 0;
			OptS[bi][bj][0]=1;
		}
	}
	
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<nr;bi+=w)
		{
			for (bj=0;bj<nc;bj+=w)
			{
				wr = min(w,nr-bi);
				wc = min(w,nc-bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];
				
				cmpcnt[0]=ComponentCnt[bi][bj][0];
				cmpcnt[1]=ComponentCnt[bi][bj][1];
				cmpcnt[2]=ComponentCnt[bi][bj][2];
				
				if (bi+w2<nr)
				{
					chD += OptD[bi+w2][bj];
					chR += OptR[bi+w2][bj];
					cmpcnt[0]+=ComponentCnt[bi+w2][bj][0];
					cmpcnt[1]+=ComponentCnt[bi+w2][bj][1];
					cmpcnt[2]+=ComponentCnt[bi+w2][bj][2];
				}
				if (bj+w2<nc)
				{
					chD += OptD[bi][bj+w2];
					chR += OptR[bi][bj+w2];
					cmpcnt[0]+=ComponentCnt[bi][bj+w2][0];
					cmpcnt[1]+=ComponentCnt[bi][bj+w2][1];
					cmpcnt[2]+=ComponentCnt[bi][bj+w2][2];
				}
				if (bi+w2<nr && bj+w2<nc)
				{
					chD += OptD[bi+w2][bj+w2];
					chR += OptR[bi+w2][bj+w2];
					cmpcnt[0]+=ComponentCnt[bi+w2][bj+w2][0];
					cmpcnt[1]+=ComponentCnt[bi+w2][bj+w2][1];
					cmpcnt[2]+=ComponentCnt[bi+w2][bj+w2][2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (3 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (3 + DepWr[bi][bj][dep]),
					DepPd[bi][bj][dep] + lambda * (3 + DepPr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepCr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
					ComponentCnt[bi][bj][0]=1;
					ComponentCnt[bi][bj][1]=0;
					ComponentCnt[bi][bj][2]=0;
					OptS[bi][bj][dep]=1;
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepWr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
					ComponentCnt[bi][bj][0]=0;
					ComponentCnt[bi][bj][1]=1;
					ComponentCnt[bi][bj][2]=0;
					OptS[bi][bj][dep]=2;
				}
				else if (ind==3)
				{
					OptD[bi][bj] = DepPd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepPr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgprd[bi+x][bj+y] = imgp[bi+x][bj+y][dep];
							imgprdg[bi+x][bj+y] = imgp[bi+x][bj+y][dep];
							OptDp[bi+x][bj+y] = ((double)OptD[bi][bj])/(wr*wc);
						}
						imgprdg[bi+x][bj] = 127;
						imgprdg[bi+x][bj+wc-1] = 127;
						imgprdg[bi][bj+x] = 127;
						imgprdg[bi+wr-1][bj+x] = 127;
					}
					ComponentCnt[bi][bj][0]=0;
					ComponentCnt[bi][bj][1]=0;
					ComponentCnt[bi][bj][2]=1;
					OptS[bi][bj][dep]=3;
				}
				else if (ind==4)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
					ComponentCnt[bi][bj][0]=cmpcnt[0];
					ComponentCnt[bi][bj][1]=cmpcnt[1];
					ComponentCnt[bi][bj][2]=cmpcnt[2];
					OptS[bi][bj][dep]=0;
				}
			}
		}
	}

	rate = OptR[0][0];
    mse = (double)OptD[0][0]/(nr*nc);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(nr*nc);
	//printf("%7.4f %7.5f %10d (C:%7d W:%7d P:%7d)\n",psnr,bpp,rate,ComponentCnt[0][0][0],ComponentCnt[0][0][1],ComponentCnt[0][0][2]);
}

void RDk(int nr, int nc, double lambda, double &psnr, double &bpp)
{
	int dep,w,w2;
	int bi,bj,x,y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr,wc;

	for (bi=0;bi<nr;bi++)
	{
		for (bj=0;bj<nc;bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgkrd[bi][bj] = imgc[bi][bj][0];
			imgkrdg[bi][bj] = imgc[bi][bj][0];
		}
	}
	
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++)
	{
		for (bi=0;bi<nr;bi+=w)
		{
			for (bj=0;bj<nc;bj+=w)
			{
				wr = min(w,nr-bi);
				wc = min(w,nc-bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];
				if (bi+w2<nr)
				{
					chD += OptD[bi+w2][bj];
					chR += OptR[bi+w2][bj];
				}
				if (bj+w2<nc)
				{
					chD += OptD[bi][bj+w2];
					chR += OptR[bi][bj+w2];
				}
				if (bi+w2<nr && bj+w2<nc)
				{
					chD += OptD[bi+w2][bj+w2];
					chR += OptR[bi+w2][bj+w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (3 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (3 + DepWr[bi][bj][dep]),
					DepKd[bi][bj][dep] + lambda * (3 + DepKr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind==1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepCr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgkrd[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgc[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+wc-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepWr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgkrd[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgw[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+wc-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==3)
				{
					OptD[bi][bj] = DepKd[bi][bj][dep];
					OptR[bi][bj] = 3 + DepKr[bi][bj][dep];
					for (x=0;x<wr;x++)
					{
						for (y=0;y<wc;y++)
						{
							imgkrd[bi+x][bj+y] = imgk[bi+x][bj+y][dep];
							imgkrdg[bi+x][bj+y] = imgk[bi+x][bj+y][dep];
						}
						imgkrdg[bi+x][bj] = 127;
						imgkrdg[bi+x][bj+wc-1] = 127;
						imgkrdg[bi][bj+x] = 127;
						imgkrdg[bi+wr-1][bj+x] = 127;
					}
				}
				else if (ind==4)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
				}
			}
		}
	}

    mse = (double)OptD[0][0]/(nr*nc);
    psnr = 10 * log(255.0*255.0/mse) / log(10.0);
	bpp = (double)OptR[0][0]/(nr*nc);
}

void RDcoded(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{
	int dep, w, w2;
	int bi, bj, x, y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr, wc;
	int cmpcnt[4];

	for (bi = 0; bi<nr; bi++)
	{
		for (bj = 0; bj<nc; bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgwrd[bi][bj] = imgc[bi][bj][0];
			imgwrdg[bi][bj] = imgc[bi][bj][0];
			ComponentCnt[bi][bj][0] = 1;
			ComponentCnt[bi][bj][1] = 0;
			OptS[bi][bj][0] = 1;
		}
	}

	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++)
	{
		for (bi = 0; bi<nr; bi += w)
		{
			for (bj = 0; bj<nc; bj += w)
			{
				wr = min(w, nr - bi);
				wc = min(w, nc - bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];

				cmpcnt[0]=ComponentCnt[bi][bj][0];
				cmpcnt[1]=ComponentCnt[bi][bj][1];

				if (bi + w2<nr)
				{
					chD += OptD[bi + w2][bj];
					chR += OptR[bi + w2][bj];
					cmpcnt[0]+=ComponentCnt[bi+w2][bj][0];
					cmpcnt[1]+=ComponentCnt[bi+w2][bj][1];
				}
				if (bj + w2<nc)
				{
					chD += OptD[bi][bj + w2];
					chR += OptR[bi][bj + w2];
					cmpcnt[0]+=ComponentCnt[bi][bj+w2][0];
					cmpcnt[1]+=ComponentCnt[bi][bj+w2][1];
				}
				if (bi + w2<nr && bj + w2<nc)
				{
					chD += OptD[bi + w2][bj + w2];
					chR += OptR[bi + w2][bj + w2];
					cmpcnt[0]+=ComponentCnt[bi+w2][bj+w2][0];
					cmpcnt[1]+=ComponentCnt[bi+w2][bj+w2][1];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepWd[bi][bj][dep] + lambda * (2 + DepWr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind == 1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgwrd[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
							imgwrdg[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
						}
						imgwrdg[bi + x][bj] = 127;
						imgwrdg[bi + x][bj + wc - 1] = 127;
						imgwrdg[bi][bj + x] = 127;
						imgwrdg[bi + wr - 1][bj + x] = 127;
					}
					ComponentCnt[bi][bj][0]=1;
					ComponentCnt[bi][bj][1]=0;
					OptS[bi][bj][dep] = 1;
				}
				else if (ind == 2)
				{
					OptD[bi][bj] = DepWd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepWr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgwrd[bi + x][bj + y] = imgw[bi + x][bj + y][dep];
							imgwrdg[bi + x][bj + y] = imgw[bi + x][bj + y][dep];
						}
						imgwrdg[bi + x][bj] = 127;
						imgwrdg[bi + x][bj + wc - 1] = 127;
						imgwrdg[bi][bj + x] = 127;
						imgwrdg[bi + wr - 1][bj + x] = 127;
					}
					ComponentCnt[bi][bj][0]=0;
					ComponentCnt[bi][bj][1]=1;
					OptS[bi][bj][dep] = 2;
				}
				else if (ind == 3)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
					ComponentCnt[bi][bj][0]=cmpcnt[0];
					ComponentCnt[bi][bj][1]=cmpcnt[1];
					OptS[bi][bj][dep] = 0;
				}
			}
		}
	}

	rate = OptR[0][0];
	mse = (double)OptD[0][0] / (nr*nc);
	psnr = 10 * log(255.0*255.0 / mse) / log(10.0);
	bpp = (double)OptR[0][0] / (nr*nc);
}

//void RDcodedhm(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
//{
//	int dep, w, w2;
//	int bi, bj, x, y;
//	int ind;
//	double mse;
//	long long chD;
//	int chR;
//	int wr, wc;
//	int cmpcnt[4];
//
//	map<pair<int,int>,vector<pair<int, long long>>> rdlist;
//
//	for (bi = 0; bi<nr; bi++)
//	{
//		for (bj = 0; bj<nc; bj++)
//		{
//			OptR[bi][bj] = DepCr[bi][bj][0];
//			OptD[bi][bj] = DepCd[bi][bj][0];
//			imgwrd[bi][bj] = imgc[bi][bj][0];
//			imgwrdg[bi][bj] = imgc[bi][bj][0];
//			ComponentCnt[bi][bj][0] = 1;
//			ComponentCnt[bi][bj][1] = 0;
//			OptS[bi][bj][0] = 1;
//			vector<pair<int, long long>> v;
//			v.push_back(pair<int, long long>(OptR[bi][bj], OptD[bi][bj]));
//			rdlist[pair<int, int>(bi, bj)] = v;
//		}
//	}
//
//	for (dep = 1, w = 2, w2 = 1;w2<max(nr, nc);w *= 2, w2 *= 2, dep++)
//	{
//		cout << "Rdcodedhm(" << dep << ")" << endl;
//		for (vector<pair<int, long long>>::iterator pvit = rdlist[pair<int, int>(0, 0)].begin(); pvit != rdlist[pair<int, int>(0, 0)].end();pvit++)
//		{
//			cout << (*pvit).first << " - " << (*pvit).second << endl;
//		}
//		cout.flush();
//		for (bi = 0; bi<nr; bi += w)
//		{
//			for (bj = 0; bj<nc; bj += w)
//			{
//				wr = min(w, nr - bi);
//				wc = min(w, nc - bj);
//
//				chD = OptD[bi][bj];
//				chR = OptR[bi][bj];
//
//				cmpcnt[0] = ComponentCnt[bi][bj][0];
//				cmpcnt[1] = ComponentCnt[bi][bj][1];
//
//				vector<pair<int, long long>> v,vt;
//				v = rdlist[pair<int, int>(bi, bj)];
//
//				if (bi + w2 < nr)
//				{
//					chD += OptD[bi + w2][bj];
//					chR += OptR[bi + w2][bj];
//					cmpcnt[0] += ComponentCnt[bi + w2][bj][0];
//					cmpcnt[1] += ComponentCnt[bi + w2][bj][1];
//
//					vector<pair<int, long long>> *pvc = &rdlist[pair<int, int>(bi + w2, bj)];
//					vt.clear();
//					for (vector<pair<int, long long>>::iterator pvit = v.begin(); pvit != v.end();pvit++)
//					{
//						for (vector<pair<int, long long>>::iterator pvcit = pvc->begin(); pvcit != pvc->end();pvcit++)
//						{
//							vt.push_back(pair<int, long long>((*pvit).first + (*pvcit).first, (*pvit).second + (*pvcit).second));
//						}
//					}
//					v = vt;
//					sort(v.begin(), v.end());
//					for (int ii = 0; ii < v.size() - 1; ii++)
//					{
//						if (v[ii + 1].first == v[ii].first)
//						{
//							v.erase(v.begin() + ii + 1);
//							ii--;
//						}
//						else
//						{
//							if (v[ii + 1].second >= v[ii].second)
//							{
//								v.erase(v.begin() + ii + 1);
//								ii--;
//							}
//						}
//					}
//				}
//				if (bj + w2<nc)
//				{
//					chD += OptD[bi][bj + w2];
//					chR += OptR[bi][bj + w2];
//					cmpcnt[0] += ComponentCnt[bi][bj + w2][0];
//					cmpcnt[1] += ComponentCnt[bi][bj + w2][1];
//
//					vector<pair<int, long long>> *pvc = &rdlist[pair<int, int>(bi, bj + w2)];
//					vt.clear();
//					for (vector<pair<int, long long>>::iterator pvit = v.begin(); pvit != v.end();pvit++)
//					{
//						for (vector<pair<int, long long>>::iterator pvcit = pvc->begin(); pvcit != pvc->end();pvcit++)
//						{
//							vt.push_back(pair<int, long long>((*pvit).first + (*pvcit).first, (*pvit).second + (*pvcit).second));
//						}
//					}
//					v = vt;
//					sort(v.begin(), v.end());
//					for (int ii = 0; ii < v.size() - 1; ii++)
//					{
//						if (v[ii + 1].first == v[ii].first)
//						{
//							v.erase(v.begin() + ii + 1);
//							ii--;
//						}
//						else
//						{
//							if (v[ii + 1].second >= v[ii].second)
//							{
//								v.erase(v.begin() + ii + 1);
//								ii--;
//							}
//						}
//					}
//				}
//				if (bi + w2<nr && bj + w2<nc)
//				{
//					chD += OptD[bi + w2][bj + w2];
//					chR += OptR[bi + w2][bj + w2];
//					cmpcnt[0] += ComponentCnt[bi + w2][bj + w2][0];
//					cmpcnt[1] += ComponentCnt[bi + w2][bj + w2][1];
//
//					vector<pair<int, long long>> *pvc = &rdlist[pair<int, int>(bi + w2, bj + w2)];
//					vt.clear();
//					for (vector<pair<int, long long>>::iterator pvit = v.begin(); pvit != v.end();pvit++)
//					{
//						for (vector<pair<int, long long>>::iterator pvcit = pvc->begin(); pvcit != pvc->end();pvcit++)
//						{
//							vt.push_back(pair<int, long long>((*pvit).first + (*pvcit).first, (*pvit).second + (*pvcit).second));
//						}
//					}
//					v = vt;
//					sort(v.begin(), v.end());
//					for (int ii = 0; ii < v.size() - 1; ii++)
//					{
//						if (v[ii + 1].first == v[ii].first)
//						{
//							v.erase(v.begin() + ii + 1);
//							ii--;
//						}
//						else
//						{
//							if (v[ii + 1].second >= v[ii].second)
//							{
//								v.erase(v.begin() + ii + 1);
//								ii--;
//							}
//						}
//					}
//				}				
//
//				v.push_back(pair<int, long long>(2 + DepCr[bi][bj][dep], DepCd[bi][bj][dep]));
//				v.push_back(pair<int, long long>(2 + DepWr[bi][bj][dep], DepWd[bi][bj][dep]));
//
//				sort(v.begin(), v.end());
//				for (vector<int, long long>::iterator veit = v.begin();veit != v.end();veit)
//				for (int ii = 0; ii < v.size()-1; ii++)
//				{
//					if (v[ii + 1].first == v[ii].first)
//					{
//						v.erase(v.begin() + ii + 1);
//						ii--;
//					}
//					else
//					{
//						if (v[ii + 1].second >= v[ii].second)
//						{
//							v.erase(v.begin() + ii + 1);
//							ii--;
//						}
//					}
//				}
//
//				rdlist[pair<int, int>(bi, bj)] = v;
//
//				ind = mini(
//					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
//					DepWd[bi][bj][dep] + lambda * (2 + DepWr[bi][bj][dep]),
//					chD + lambda * (1 + chR));
//
//				if (ind == 1)
//				{
//					OptD[bi][bj] = DepCd[bi][bj][dep];
//					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
//					for (x = 0; x<wr; x++)
//					{
//						for (y = 0; y<wc; y++)
//						{
//							imgwrd[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
//							imgwrdg[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
//						}
//						imgwrdg[bi + x][bj] = 127;
//						imgwrdg[bi + x][bj + wc - 1] = 127;
//						imgwrdg[bi][bj + x] = 127;
//						imgwrdg[bi + wr - 1][bj + x] = 127;
//					}
//					ComponentCnt[bi][bj][0] = 1;
//					ComponentCnt[bi][bj][1] = 0;
//					OptS[bi][bj][dep] = 1;
//				}
//				else if (ind == 2)
//				{
//					OptD[bi][bj] = DepWd[bi][bj][dep];
//					OptR[bi][bj] = 2 + DepWr[bi][bj][dep];
//					for (x = 0; x<wr; x++)
//					{
//						for (y = 0; y<wc; y++)
//						{
//							imgwrd[bi + x][bj + y] = imgw[bi + x][bj + y][dep];
//							imgwrdg[bi + x][bj + y] = imgw[bi + x][bj + y][dep];
//						}
//						imgwrdg[bi + x][bj] = 127;
//						imgwrdg[bi + x][bj + wc - 1] = 127;
//						imgwrdg[bi][bj + x] = 127;
//						imgwrdg[bi + wr - 1][bj + x] = 127;
//					}
//					ComponentCnt[bi][bj][0] = 0;
//					ComponentCnt[bi][bj][1] = 1;
//					OptS[bi][bj][dep] = 2;
//				}
//				else if (ind == 3)
//				{
//					OptD[bi][bj] = chD;
//					OptR[bi][bj] = 1 + chR;
//					ComponentCnt[bi][bj][0] = cmpcnt[0];
//					ComponentCnt[bi][bj][1] = cmpcnt[1];
//					OptS[bi][bj][dep] = 0;
//				}
//			}
//		}
//	}
//
//	rate = OptR[0][0];
//	mse = (double)OptD[0][0] / (nr*nc);
//	psnr = 10 * log(255.0*255.0 / mse) / log(10.0);
//	bpp = (double)OptR[0][0] / (nr*nc);
//}

void StoreImage(int nr, int nc, int simg[][IMGSZC], int simg2[][IMGSZC], double psnr, double bpp, char tp)
{
	int i,j;
	char name[400];
	
	sprintf_s(name,"img-%c-%lf-%lf.b",tp,psnr,bpp);
	ofstream foutw(name);
	foutw << nr << " " << nc << endl;
	for (i=0;i<nr;i++)
	{
		for (j=0;j<nc;j++)
			foutw << simg[i][j] << " ";
		foutw << endl;
	}
	foutw.close();

	sprintf_s(name,"img-%c-%lf-%lf-g.b",tp,psnr,bpp);
	ofstream foutw2(name);
	foutw2 << nr << " " << nc << endl;
	for (i=0;i<nr;i++)
	{
		for (j=0;j<nc;j++)
			foutw2 << simg2[i][j] << " ";
		foutw2 << endl;
	}
	foutw2.close();
}

void StoreImage(int nr, int nc, int simg[][IMGSZC][DEPMAX], char tp)
{
	int i,j;
	int w,dep;
	char name[400];
	
	for (w=1,dep=0;w<=max(nr,nc);w*=2,dep++)
	{
		sprintf_s(name,"imgdep-%c-%d.b",tp,w);
		ofstream foutw(name);
		foutw << nr << " " << nc << endl;
		for (i=0;i<nr;i++)
		{
			for (j=0;j<nc;j++)
				foutw << simg[i][j][dep] << " ";
			foutw << endl;
		}
		foutw.close();
	}
}

void RD(int nr, int nc)
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
		RDw(nr,nc,*it,psnr,bpp);
		StoreImage(nr,nc,imgwrd,imgwrdg,psnr,bpp,'w');
		fout.width(15);
		fout << std::setw(15) << std::right << *it << psnr << bpp << endl;
	}

	fout << "Kleinlets" << endl;
	for(std::vector<double>::iterator it = lambdas.begin(); it<lambdas.end(); it++)
	{
		RDk(nr,nc,*it,psnr,bpp);
		StoreImage(nr,nc,imgkrd,imgkrdg,psnr,bpp,'k');
		fout.width(15);
		fout << std::right << *it << psnr << bpp << endl;
	}

	StoreImage(nr,nc,imgw,'w');
	StoreImage(nr,nc,imgk,'k');
}

void BPPp(int nr, int nc, double bppin, double &psnr, double &bpp)
{
	int rate;
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;
	
	cout << "BPPp function called for " << bppin << endl;
	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDp(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}


void BPPcoded(int nr, int nc, double bppin, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;
	
	cout << "BPPcoded function called for " << bppin << endl;
	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPcodedEn(int nr, int nc, double bppin, int ver, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;
	double bpp1;

	cout << "BPPcodedEn function called for " << bppin << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	cout << "uncompressed bpp out : " << bpp << endl;
	rate = Encoder(nr, nc, ver, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		rate = Encoder(nr, nc, ver, "v:\\inapp.bppcodeden");
		bpp1 = (double)rate / nr / nc;
		if (bpp1 < bpp) bpp = bpp1;
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPcodedGE(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
		bpp = (double)rate / nr / nc;
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

//void BPPcodedGEr(int nr, int nc, int ver, int graybit, double bppin[], int downind, int upind, double lambdamin, double lambdamax, int iter)
//{
//	int maxiter = 100;
//	double lambda;
//
//	if (downind > upind) return;
//
//	if (iter > maxiter)
//	{
//		// Do output
//	}
//
//	//fabs(bppin - bpp) > 0.001
//	lambda = (lambdamin + lambdamax) / 2;
//	//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
//	RDcoded(nr, nc, lambda, psnr, bpp, rate);
//	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
//	bpp = (double)rate / nr / nc;
//	//cout << psnr << " , " << bpp << endl;
//	if (bpp < bppin)
//		lambdamax = lambda;
//	else
//		lambdamin = lambda;
//	iter++;
//}
//
//void BPPcodedGEr(int nr, int nc, double bppin[], int bppinlength, int ver, int graybit)
//{
//	double psnr, bpp;
//	int rate;
//	double lambda;
//	double lambdamin = 0, lambdamax = 1e10;
//	int iter = 0;
//	int maxiter = 100;
//	int downind = 0, upind = bppinlength - 1;
//
//	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;
//
//	RDcoded(nr, nc, 0, psnr, bpp, rate);
//	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
//	bpp = (double)rate / nr / nc;
//	while (bppin[downind] >= bpp)
//	{
//		// Do output
//		downind++;
//	}
//	BPPcodedGEr(nr, nc, bppin, downind, upind, lambdamin, lambdamax, ver, graybit);
//}

void BPPcodedGE2(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPcodedGE2(int nr, int nc, int graybit, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[])
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDcoded(nr, nc, lambda, psnr, bpp, rate);

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
		}
	}

	BPPcodedGE2(nr, nc, graybit, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r);
	BPPcodedGE2(nr, nc, graybit, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r);
}

void BPPcodedGE2(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[])
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE2 function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPcodedGE2(nr, nc, graybit, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r);
}

void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[])
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDcoded(nr, nc, lambda, psnr, bpp, rate);
	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
		}
	}

	BPPcodedGE2Cmprs(nr, nc, graybit, ver, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r);
	BPPcodedGE2Cmprs(nr, nc, graybit, ver, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r);
}

void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[])
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE2 function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPcodedGE2Cmprs(nr, nc, graybit, ver, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r);
}

void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDcoded(nr, nc, lambda, psnr, bpp, rate);
	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			out << i << endl;
			Store(out, imgwrd, "imgrd");
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
			out << ind2 << endl;
			Store(out, imgwrd, "imgrd");
		}
	}

	BPPcodedGE2Cmprs(nr, nc, graybit, ver, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r, out);
	BPPcodedGE2Cmprs(nr, nc, graybit, ver, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r, out);
}

void BPPcodedGE2Cmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE2 function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	rate = GeneralEncoder(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			out << i << endl;
			Store(out, imgwrd, "imgrd");
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPcodedGE2Cmprs(nr, nc, graybit, ver, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r, out);
}


void BPPcodedWE(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedWE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	rate = GeneralEncoderW(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGW");
	bpp = (double)rate / nr / nc;
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		//cout << " >>> bpp " << bpp;
		rate = GeneralEncoderW(nr, nc, ver, graybit, "v:\\inapp.BPPcodedGE");
		bpp = (double)rate / nr / nc;
		//cout << " to " << bpp << endl;
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPcodedWE2(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedWE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDcoded(nr, nc, 0, psnr, bpp, rate);
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		//cout << " >>> bpp " << bpp;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void SetLambda()
{
	int i;
	double lambda;

	LambdaLength = 0;

	lambda=1;
	for (i=0;i<40;i++)
	{
		lambda = lambda / 1.5;
		LambdaArray[39-i] = lambda;
		LambdaLength++;
	}

	lambda=1;
	for (i=0;i<50;i++)
	{
		LambdaArray[LambdaLength++] = lambda;
		lambda = lambda * 1.5;
	}	
}

void DoFeaturePixel(int nr, int nc, int w, int dep)
{
	cout << "Do Feature Pixel for width " << w << endl; cout.flush();

	int i,j,bi,bj,x,y;
	int i1,i2,i3,i4;
	int n1,n2;
	long long s,s1,s2,p,p1,p2,d,d1,d2,bestd;	
	int m,m1,m2,bestm1,bestm2,besti,bestj;
	int besti1,besti2,besti3,besti4;
	bool sgn,sgn1,sgn2;
	int indexes[4*WMAX][2];
	int mxindex = 0;
	int indrate = 0;
	int wn,wp;

	wn=w/2;
	wp=-wn+w-1;
	
	mxindex = 0;
	for (i=0;i<w;i++){
		indexes[mxindex][0]=i;
		indexes[mxindex++][1]=0;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=w;
		indexes[mxindex++][1]=i;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=w-i;
		indexes[mxindex++][1]=w;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=0;
		indexes[mxindex++][1]=w-i;
	}


	for (bi=wn;bi+wp<nr;bi++)
	{
		//cout << "Do for bi = " << bi << endl;
		for (bj=wn;bj+wp<nr;bj++)
		{
			// Calculate Constant Features
			n1=0;
			s=0;
			p=0;
			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					n1++;
					s += img[bi-wn+x][bj-wn+y];
					p += (img[bi-wn+x][bj-wn+y] * img[bi-wn+x][bj-wn+y]);
				}
			}
			m = (int)round((double)s/n1);
			d = p + n1*m*m - 2*m*s;

			DepCm[bi][bj][dep] = m;
			DepCd[bi][bj][dep] = d;
			DepCr[bi][bj][dep] = GRAYBIT;

			// Calculate Wedgelet Features
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
								s1 += img[bi-wn+x][bj-wn+y];
								p1 += img[bi-wn+x][bj-wn+y]*img[bi-wn+x][bj-wn+y];
							}else{
								n2++;
								s2 += img[bi-wn+x][bj-wn+y];
								p2 += img[bi-wn+x][bj-wn+y]*img[bi-wn+x][bj-wn+y];
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
						m1 = (int)round((double)s1/n1);
						d1 = p1 + n1*m1*m1 - 2*m1*s1;
					}else{
						m1=0;
						d1=0;
					}
					if (n2>0)
					{
						m2 = (int)round((double)s2/n2);
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

			// Calculate Pipelets Features
			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					if (i1==i2) continue;
					for (i3=0;i3<mxindex;i3++)
					{
						if (i3==i1 || i3==i2) continue;
						if (GetIndex4(w, w,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
						if (CheckIndex(w,w,i1,i2,i3,i4)==false) continue;						
						
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
							m1 = (int)round((double)s1/n1);
							d1 = p1 + n1*m1*m1 - 2*m1*s1;
						}else{
							m1=0;
							d1=0;
						}
						if (n2>0)
						{
							m2 = (int)round((double)s2/n2);
							d2 = p2 + n2*m2*m2 - 2*m2*s2;
						}else{
							m2=0;
							d2=0;
						}

						//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
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
								i1 = 10000;
								i2 = 10000;
								i3 = 10000;
							}
						}
					}
				}
			}

			DepPi1[bi][bj][dep] = besti1;
			DepPi2[bi][bj][dep] = besti2;
			DepPi3[bi][bj][dep] = besti3;
			DepPi4[bi][bj][dep] = besti4;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;
		}
	}
}

void DoFeaturePixel(int nr, int nc)
{
	int i;
	for (i=0;i<FeaturePixelCount;i++)
		DoFeaturePixel(nr,nc,FeaturePixelWidth[i],i);
}

void DoFeaturePixel2(int nr, int nc, int w, int dep)
{
	cout << "Do Feature Pixel2 for width " << w << endl; cout.flush();

	int i,j,bi,bj,x,y;
	int i1,i2,i3,i4;
	int n1,n2;
	long long s,s1,s2,p,p1,p2,d,d1,d2,bestd;	
	int m,m1,m2,bestm1,bestm2,besti,bestj;
	int besti1,besti2,besti3,besti4;
	bool sgn,sgn1,sgn2;
	int indexes[4*WMAX][2];
	int mxindex = 0;
	int indrate = 0;
	int wn,wp;
	double xdef,ydef,x0def,y0def;

	wn=w/2;
	wp=-wn+w-1;
	
	mxindex = 0;
	for (i=0;i<w;i++){
		indexes[mxindex][0]=i;
		indexes[mxindex++][1]=0;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=w;
		indexes[mxindex++][1]=i;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=w-i;
		indexes[mxindex++][1]=w;
	}
	for (i=0;i<w;i++){
		indexes[mxindex][0]=0;
		indexes[mxindex++][1]=w-i;
	}

	cout << "Completed "; cout.flush();
	for (bi=wn;bi+wp<nr;bi++)
	{
		if (bi%10==0){
			cout << (int)((bi*100.0)/nr) << ","; cout.flush();
		}
		for (bj=wn;bj+wp<nr;bj++)
		{
			// Calculate Constant Features
			n1=0;
			s=0;
			p=0;
			for (x=0;x<w;x++)
			{
				for (y=0;y<w;y++)
				{
					n1++;
					s += img[bi-wn+x][bj-wn+y];
					p += (img[bi-wn+x][bj-wn+y] * img[bi-wn+x][bj-wn+y]);
				}
			}
			m = (int)round((double)s/n1);
			d = p + n1*m*m - 2*m*s;

			DepCm[bi][bj][dep] = m;
			DepCd[bi][bj][dep] = d;
			DepCr[bi][bj][dep] = GRAYBIT;

			// Calculate Wedgelet Features
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
								s1 += img[bi-wn+x][bj-wn+y];
								p1 += img[bi-wn+x][bj-wn+y]*img[bi-wn+x][bj-wn+y];
							}else{
								n2++;
								s2 += img[bi-wn+x][bj-wn+y];
								p2 += img[bi-wn+x][bj-wn+y]*img[bi-wn+x][bj-wn+y];
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
						m1 = (int)round((double)s1/n1);
						d1 = p1 + n1*m1*m1 - 2*m1*s1;
					}else{
						m1=0;
						d1=0;
					}
					if (n2>0)
					{
						m2 = (int)round((double)s2/n2);
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

			// Calculate Pipelets Features
			bestd = (1<<30);
			for (i1=0;i1<mxindex;i1++)
			{
				for (i2=0;i2<mxindex;i2++)
				{
					if (i1==i2) continue;
					for (i3=0;i3<mxindex;i3++)
					{
						if (i3==i1 || i3==i2) continue;
						if (GetIndex4(w, w,indexes[i1][0],indexes[i1][1],indexes[i2][0],indexes[i2][1],indexes[i3][0],indexes[i3][1],i4) == false) continue;
						if (CheckIndex(w,w,i1,i2,i3,i4)==false) continue;						

						sgn1 = ((wn-indexes[i2][1])*(indexes[i3][0]-indexes[i2][0])-(wn-indexes[i2][0])*(indexes[i3][1]-indexes[i2][1])) >= 0;
						sgn2 = ((wn-indexes[i1][1])*(indexes[i4][0]-indexes[i1][0])-(wn-indexes[i1][0])*(indexes[i4][1]-indexes[i1][1])) >= 0;
						//printf("SGN i(%d,%d,%d,%d) sgn(%c,%c):%c\n",i1,i2,i3,i4,sgn1?'1':'0',sgn2?'1':'0',(sgn1^sgn2)?'1':'0');
						if ((sgn1^sgn2)==false) continue;

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
							m1 = (int)round((double)s1/n1);
							d1 = p1 + n1*m1*m1 - 2*m1*s1;
						}else{
							m1=0;
							d1=0;
						}
						if (n2>0)
						{
							m2 = (int)round((double)s2/n2);
							d2 = p2 + n2*m2*m2 - 2*m2*s2;
						}else{
							m2=0;
							d2=0;
						}

						//cout << "Pipelet( " << i1 << " , " << i2 << " , " << i3 << " , " << i4 << " ) dist : " << d1+d2 << " -- " << bestd << endl;
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
								i1 = 10000;
								i2 = 10000;
								i3 = 10000;
							}
						}
					}
				}
			}

			DepPi1[bi][bj][dep] = besti1;
			DepPi2[bi][bj][dep] = besti2;
			DepPi3[bi][bj][dep] = besti3;
			DepPi4[bi][bj][dep] = besti4;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * GRAYBIT + 3 * indrate;

			xdef = indexes[besti3][0]-indexes[besti2][0];
			ydef = indexes[besti3][1]-indexes[besti2][1];
			x0def = indexes[besti1][0]-indexes[besti2][0];
			y0def = indexes[besti1][1]-indexes[besti2][1];
			DepPh[bi][bj][dep] = abs(x0def*ydef-xdef*y0def)/sqrt((double)(xdef*xdef+ydef*ydef));
		}
	}
}

void DoFeaturePixel2(int nr, int nc)
{
	int i;
	for (i=0;i<FeaturePixelCount;i++)
		DoFeaturePixel2(nr,nc,FeaturePixelWidth[i],i);
}

void SaveTree(char* path)
{
	FILE* f;
	fopen_s(&f, path, "wb");

	fwrite(imgc, 1, sizeof(imgc), f);
	fwrite(imgw, 1, sizeof(imgw), f);
	fwrite(imgp, 1, sizeof(imgc), f);
	fwrite(imgk, 1, sizeof(imgc), f);

	fwrite(DepCd, 1, sizeof(DepCd), f);
	fwrite(DepCd, 1, sizeof(DepCd), f);
	fwrite(DepCr, 1, sizeof(DepCr), f);

	fwrite(DepWi, 1, sizeof(DepWi), f);
	fwrite(DepWj, 1, sizeof(DepWj), f);
	fwrite(DepWm1, 1, sizeof(DepWm1), f);
	fwrite(DepWm2, 1, sizeof(DepWm2), f);
	fwrite(DepWd, 1, sizeof(DepWd), f);
	fwrite(DepWr, 1, sizeof(DepWr), f);

	fwrite(DepPi1, 1, sizeof(DepPi1), f);
	fwrite(DepPi2, 1, sizeof(DepPi2), f);
	fwrite(DepPi3, 1, sizeof(DepPi3), f);
	fwrite(DepPi4, 1, sizeof(DepPi4), f);
	fwrite(DepPm1, 1, sizeof(DepPm1), f);
	fwrite(DepPm2, 1, sizeof(DepPm2), f);
	fwrite(DepPd, 1, sizeof(DepPd), f);
	fwrite(DepPr, 1, sizeof(DepPr), f);
	fwrite(DepPh, 1, sizeof(DepPh), f);

	fwrite(DepGi1, 1, sizeof(DepGi1), f);
	fwrite(DepGi2, 1, sizeof(DepGi2), f);
	fwrite(DepGi3, 1, sizeof(DepGi3), f);
	fwrite(DepGi4, 1, sizeof(DepGi4), f);
	fwrite(DepGm1, 1, sizeof(DepGm1), f);
	fwrite(DepGm2, 1, sizeof(DepGm2), f);
	fwrite(DepGd, 1, sizeof(DepGd), f);
	fwrite(DepGr, 1, sizeof(DepGr), f);

	fwrite(DepKi1, 1, sizeof(DepKi1), f);
	fwrite(DepKi2, 1, sizeof(DepKi2), f);
	fwrite(DepKi3, 1, sizeof(DepKi3), f);
	fwrite(DepKi4, 1, sizeof(DepKi4), f);
	fwrite(DepKm1, 1, sizeof(DepKm1), f);
	fwrite(DepKm2, 1, sizeof(DepKm2), f);
	fwrite(DepKd, 1, sizeof(DepKd), f);
	fwrite(DepKr, 1, sizeof(DepKr), f);

	fclose(f);
}

void LoadTree(char* path)
{
	FILE* f;
	fopen_s(&f, path, "rb");

	fread(imgc, 1, sizeof(imgc), f);
	fread(imgw, 1, sizeof(imgw), f);
	fread(imgp, 1, sizeof(imgc), f);
	fread(imgk, 1, sizeof(imgc), f);

	fread(DepCd, 1, sizeof(DepCd), f);
	fread(DepCd, 1, sizeof(DepCd), f);
	fread(DepCr, 1, sizeof(DepCr), f);

	fread(DepWi, 1, sizeof(DepWi), f);
	fread(DepWj, 1, sizeof(DepWj), f);
	fread(DepWm1, 1, sizeof(DepWm1), f);
	fread(DepWm2, 1, sizeof(DepWm2), f);
	fread(DepWd, 1, sizeof(DepWd), f);
	fread(DepWr, 1, sizeof(DepWr), f);

	fread(DepPi1, 1, sizeof(DepPi1), f);
	fread(DepPi2, 1, sizeof(DepPi2), f);
	fread(DepPi3, 1, sizeof(DepPi3), f);
	fread(DepPi4, 1, sizeof(DepPi4), f);
	fread(DepPm1, 1, sizeof(DepPm1), f);
	fread(DepPm2, 1, sizeof(DepPm2), f);
	fread(DepPd, 1, sizeof(DepPd), f);
	fread(DepPr, 1, sizeof(DepPr), f);
	fread(DepPh, 1, sizeof(DepPh), f);

	fread(DepGi1, 1, sizeof(DepGi1), f);
	fread(DepGi2, 1, sizeof(DepGi2), f);
	fread(DepGi3, 1, sizeof(DepGi3), f);
	fread(DepGi4, 1, sizeof(DepGi4), f);
	fread(DepGm1, 1, sizeof(DepGm1), f);
	fread(DepGm2, 1, sizeof(DepGm2), f);
	fread(DepGd, 1, sizeof(DepGd), f);
	fread(DepGr, 1, sizeof(DepGr), f);

	fread(DepKi1, 1, sizeof(DepKi1), f);
	fread(DepKi2, 1, sizeof(DepKi2), f);
	fread(DepKi3, 1, sizeof(DepKi3), f);
	fread(DepKi4, 1, sizeof(DepKi4), f);
	fread(DepKm1, 1, sizeof(DepKm1), f);
	fread(DepKm2, 1, sizeof(DepKm2), f);
	fread(DepKd, 1, sizeof(DepKd), f);
	fread(DepKr, 1, sizeof(DepKr), f);

	fclose(f);
}

void SaveTreePDebug(char* path)
{
	FILE* f;
	fopen_s(&f, path, "wb");

	fwrite(imgc, 1, sizeof(imgc), f);
	fwrite(imgw, 1, sizeof(imgw), f);
	fwrite(imgp, 1, sizeof(imgc), f);
	fwrite(imgk, 1, sizeof(imgc), f);

	fwrite(DepCd, 1, sizeof(DepCd), f);
	fwrite(DepCd, 1, sizeof(DepCd), f);
	fwrite(DepCr, 1, sizeof(DepCr), f);

	fwrite(DepWi, 1, sizeof(DepWi), f);
	fwrite(DepWj, 1, sizeof(DepWj), f);
	fwrite(DepWm1, 1, sizeof(DepWm1), f);
	fwrite(DepWm2, 1, sizeof(DepWm2), f);
	fwrite(DepWd, 1, sizeof(DepWd), f);
	fwrite(DepWr, 1, sizeof(DepWr), f);

	fwrite(DepPi1, 1, sizeof(DepPi1), f);
	fwrite(DepPi2, 1, sizeof(DepPi2), f);
	fwrite(DepPi3, 1, sizeof(DepPi3), f);
	fwrite(DepPi4, 1, sizeof(DepPi4), f);
	fwrite(DepPm1, 1, sizeof(DepPm1), f);
	fwrite(DepPm2, 1, sizeof(DepPm2), f);
	fwrite(DepPd, 1, sizeof(DepPd), f);
	fwrite(DepPr, 1, sizeof(DepPr), f);
	fwrite(DepPh, 1, sizeof(DepPh), f);

	//fwrite(DepGi1, 1, sizeof(DepGi1), f);
	//fwrite(DepGi2, 1, sizeof(DepGi2), f);
	//fwrite(DepGi3, 1, sizeof(DepGi3), f);
	//fwrite(DepGi4, 1, sizeof(DepGi4), f);
	//fwrite(DepGm1, 1, sizeof(DepGm1), f);
	//fwrite(DepGm2, 1, sizeof(DepGm2), f);
	//fwrite(DepGd, 1, sizeof(DepGd), f);
	//fwrite(DepGr, 1, sizeof(DepGr), f);

	//fwrite(DepKi1, 1, sizeof(DepKi1), f);
	//fwrite(DepKi2, 1, sizeof(DepKi2), f);
	//fwrite(DepKi3, 1, sizeof(DepKi3), f);
	//fwrite(DepKi4, 1, sizeof(DepKi4), f);
	//fwrite(DepKm1, 1, sizeof(DepKm1), f);
	//fwrite(DepKm2, 1, sizeof(DepKm2), f);
	//fwrite(DepKd, 1, sizeof(DepKd), f);
	//fwrite(DepKr, 1, sizeof(DepKr), f);

	fclose(f);
}

void LoadTreePDebug(char* path)
{
	FILE* f;
	fopen_s(&f, path, "rb");

	fread(imgc, 1, sizeof(imgc), f);
	fread(imgw, 1, sizeof(imgw), f);
	fread(imgp, 1, sizeof(imgc), f);
	fread(imgk, 1, sizeof(imgc), f);

	fread(DepCd, 1, sizeof(DepCd), f);
	fread(DepCd, 1, sizeof(DepCd), f);
	fread(DepCr, 1, sizeof(DepCr), f);

	fread(DepWi, 1, sizeof(DepWi), f);
	fread(DepWj, 1, sizeof(DepWj), f);
	fread(DepWm1, 1, sizeof(DepWm1), f);
	fread(DepWm2, 1, sizeof(DepWm2), f);
	fread(DepWd, 1, sizeof(DepWd), f);
	fread(DepWr, 1, sizeof(DepWr), f);

	fread(DepPi1, 1, sizeof(DepPi1), f);
	fread(DepPi2, 1, sizeof(DepPi2), f);
	fread(DepPi3, 1, sizeof(DepPi3), f);
	fread(DepPi4, 1, sizeof(DepPi4), f);
	fread(DepPm1, 1, sizeof(DepPm1), f);
	fread(DepPm2, 1, sizeof(DepPm2), f);
	fread(DepPd, 1, sizeof(DepPd), f);
	fread(DepPr, 1, sizeof(DepPr), f);
	fread(DepPh, 1, sizeof(DepPh), f);

	//fread(DepGi1, 1, sizeof(DepGi1), f);
	//fread(DepGi2, 1, sizeof(DepGi2), f);
	//fread(DepGi3, 1, sizeof(DepGi3), f);
	//fread(DepGi4, 1, sizeof(DepGi4), f);
	//fread(DepGm1, 1, sizeof(DepGm1), f);
	//fread(DepGm2, 1, sizeof(DepGm2), f);
	//fread(DepGd, 1, sizeof(DepGd), f);
	//fread(DepGr, 1, sizeof(DepGr), f);

	//fread(DepKi1, 1, sizeof(DepKi1), f);
	//fread(DepKi2, 1, sizeof(DepKi2), f);
	//fread(DepKi3, 1, sizeof(DepKi3), f);
	//fread(DepKi4, 1, sizeof(DepKi4), f);
	//fread(DepKm1, 1, sizeof(DepKm1), f);
	//fread(DepKm2, 1, sizeof(DepKm2), f);
	//fread(DepKd, 1, sizeof(DepKd), f);
	//fread(DepKr, 1, sizeof(DepKr), f);

	fclose(f);
}

int GetGrayLevel(int x, int y, int fx, int fy, int w2, int dep)
{
	int px,py,w;
	if (fx<0 || fy<0) return -1;
	w = (w2>0)?2*w2:1;
	if (fx>=x && fx<x+w && fy>=y && fy<y+w) // IN
	{
		switch (OptS[x][y][dep])
		{
			case 0:
				px = (fx<x+w2)?x:x+w2;
				py = (fy<y+w2)?y:y+w2;
				return GetGrayLevel(px,py,fx,fy,w2/2,dep-1);				
			case 1:
				return imgc[fx][fy][dep];
			case 2:
				return imgw[fx][fy][dep];
			case 3:
				return imgp[fx][fy][dep];
			default:
				return 0;
		}
	}
	else // OUT
	{		
		px=x; if ((x/w)%2==1) px-=w;
		py=y; if ((y/w)%2==1) py-=w;
		return GetGrayLevel(px,py,fx,fy,w,dep+1);
	}
}

void FillConstantDiff(int x, int y, int nr, int nc, int w2, int dep, int method, int* diff)
{
	int gray1,gray2,g1,g2,g3;
	if (x>=nr || y>=nc) return;
	if (OptS[x][y][dep]==0)
	{
		FillConstantDiff(x,y,nr,nc,w2/2,dep-1,method,diff);
		FillConstantDiff(x+w2,y,nr,nc,w2/2,dep-1,method,diff);
		FillConstantDiff(x,y+w2,nr,nc,w2/2,dep-1,method,diff);
		FillConstantDiff(x+w2,y+w2,nr,nc,w2/2,dep-1,method,diff);
	}
	else if (OptS[x][y][dep]==1)
	{
		gray1 = imgc[x][y][dep];
		switch (method)
		{
			case 0:
				gray2=0;
				break;
			case 1:
				gray2 = GetGrayLevel(x,y,x-1,y,w2,dep);
				break;
			case 2:
				gray2 = GetGrayLevel(x,y,x,y-1,w2,dep);
				break;
			case 3:
				g1 = GetGrayLevel(x,y,x-1,y,w2,dep);
				g2 = GetGrayLevel(x,y,x,y-1,w2,dep);
				gray2 = (abs(g1-gray1)<abs(g2-gray1))?g1:g2;
				break;
			case 4:
				g1 = GetGrayLevel(x,y,x-1,y,w2,dep);
				g2 = GetGrayLevel(x,y,x,y-1,w2,dep);
				g3 = GetGrayLevel(x,y,x-1,y-1,w2,dep);
				gray2 = g1+g2-g3;
				break;
			case 5:
				g1 = GetGrayLevel(x,y,x-1,y,w2,dep);
				g2 = GetGrayLevel(x,y,x,y-1,w2,dep);
				g3 = GetGrayLevel(x,y,x-1,y-1,w2,dep);
				gray2 = g2+(g1-g3)/2;
				break;
			case 6:
				g1 = GetGrayLevel(x,y,x-1,y,w2,dep);
				g2 = GetGrayLevel(x,y,x,y-1,w2,dep);
				g3 = GetGrayLevel(x,y,x-1,y-1,w2,dep);
				gray2 = g1+(g2-g3)/2;
				break;
			case 7:
				g1 = GetGrayLevel(x,y,x-1,y,w2,dep);
				g2 = GetGrayLevel(x,y,x,y-1,w2,dep);				
				gray2 = (g1+g2)/2;
				break;
		}
		if (gray2>=0)
			diff[gray1-gray2+300]++;
	}
}

void FindNodeType(int x, int y, int tx, int ty, int nr, int nc, int dep, int &nodetype, int &gray1, int &gray2, bool &swapgray)
{
	int w2 = (1 << (dep - 1));
	int cx, cy;
	swapgray = false;
	if (tx < 0 || tx >= nr || ty < 0 || ty >= nc)
	{
		nodetype = -1;
		return;
	}
	if (OptS[x][y][dep] == 0)
	{		
		if (tx < x + w2)
			cx = x;
		else
			cx = x + w2;
		if (ty < y + w2)
			cy = y;
		else
			cy = y + w2;
		FindNodeType(cx, cy, tx, ty, nr, nc, dep - 1, nodetype, gray1, gray2, swapgray);
	}
	else if (OptS[x][y][dep] == 1)
	{
		nodetype = 1;
		gray1 = DepCm[x][y][dep];
	}
	else if (OptS[x][y][dep] == 2)
	{
		nodetype = 2;
		gray1 = DepWm1[x][y][dep];
		gray2 = DepWm2[x][y][dep];
		if (gray1<gray2){
			swap(gray1, gray2);
			swapgray = true;
		}
	}
}

void CalculateCodedGray(int x, int y, int dep, int nr, int nc, int deporg, int method, int &cgray1, int &cgray2, bool &grayswap, bool &needcoding)
{
	int nodetype, gray1, gray2;
	needcoding = true;
	switch(method)
	{
		case 0:
			if (OptS[x][y][dep]==1)
			{
				cgray1 = DepCm[x][y][dep];
			}
			else if (OptS[x][y][dep]==2)
			{
				cgray1 = DepWm1[x][y][dep];
				cgray2 = DepWm2[x][y][dep];
			}
			return;
		case 1:
			FindNodeType(0,0,x-1,y,nr,nc,deporg,nodetype,gray1,gray2,grayswap);
			break;
		case 2:
			FindNodeType(0,0,x,y-1,nr,nc,deporg,nodetype,gray1,gray2,grayswap);
			break;
		case 3:
			FindNodeType(0,0,x-1,y-1,nr,nc,deporg,nodetype,gray1,gray2,grayswap);
			break;
	}
	if (nodetype==-1)
	{
		if (OptS[x][y][dep]==1)
		{
			cgray1 = DepCm[x][y][dep];
		}
		else if (OptS[x][y][dep]==2)
		{
			cgray1 = DepWm1[x][y][dep];
			cgray2 = DepWm2[x][y][dep];
		}
		needcoding = false;
	}
	else if (nodetype==1)
	{
		if (OptS[x][y][dep]==1)
		{
			cgray1 = gray1 - DepCm[x][y][dep];
		}
		else if (OptS[x][y][dep]==2)
		{
			cgray1 = gray1 - DepWm1[x][y][dep];
			cgray2 = gray1 - DepWm2[x][y][dep];
		}
	}
	else if (nodetype==2)
	{
		if (OptS[x][y][dep]==1)
		{
			cgray1 = gray1 - DepCm[x][y][dep];
		}
		else if (OptS[x][y][dep]==2)
		{
			cgray1 = gray1 - DepWm1[x][y][dep];
			cgray2 = gray2 - DepWm2[x][y][dep];
		}
	}
}

void FillCodedDiff(int x, int y, int nr, int nc, int w2, int dep, int deporg, int method, int* diff)
{
	int gray1, gray2;
	int nodetype01, nodetype10, nodetype11;
	int gray011, gray012, gray101, gray102, gray111, gray112;
	bool sg;
	int mid = 300;
	if (x >= nr || y >= nc) return;

	if (OptS[x][y][dep] == 0)
	{
		FillCodedDiff(x, y, nr, nc, w2 / 2, dep - 1,deporg, method, diff);
		FillCodedDiff(x + w2, y, nr, nc, w2 / 2, dep - 1, deporg, method, diff);
		FillCodedDiff(x, y + w2, nr, nc, w2 / 2, dep - 1, deporg, method, diff);
		FillCodedDiff(x + w2, y + w2, nr, nc, w2 / 2, dep - 1, deporg, method, diff);
	}
	else
	{
		FindNodeType(0,0,x - 1, y, nr, nc, deporg, nodetype10, gray101, gray102, sg);
		FindNodeType(0,0,x, y-1, nr, nc, deporg, nodetype01, gray011, gray012, sg);
		FindNodeType(0,0,x - 1, y-1, nr, nc, deporg, nodetype11, gray111, gray112, sg);

		if ( (nodetype10!=-1 && (gray101<0 || gray101>255)) || 
			 (nodetype01!=-1 && (gray011<0 || gray011>255)) ||
			 (nodetype11!=-1 && (gray111<0 || gray111>255)) ||
			 (nodetype10!=-1 && nodetype10!=1 &&(gray102<0 || gray102>255)) || 
			 (nodetype01!=-1 && nodetype01!=1 && (gray012<0 || gray012>255)) ||
			 (nodetype11!=-1 && nodetype11!=1 && (gray112<0 || gray112>255))
			 )
			printf("Error gray Findnodetype1: (%d,%d,%d)\n",x,y,dep);

		if (OptS[x][y][dep] == 1){
			ComponentCnt[1][1][0]++;
			gray1 = DepCm[x][y][dep];
			if (gray1<0 || gray1>255)
				printf("Error gray DepCm: %d (%d,%d,%d)\n",gray1,x,y,dep);
			switch (method)
			{
			case 0:
				diff[gray1 + mid]++;
				break;
			case 1:
				if (nodetype10 == -1)
					diff[0]++;
				else
					diff[gray101 - gray1 + mid]++;
				break;
			case 2:
				if (nodetype01 == -1)
					diff[0]++;
				else
					diff[gray011 - gray1 + mid]++;
				break;
			case 3:
				if (nodetype11 == -1)
					diff[0]++;
				else
					diff[gray111 - gray1 + mid]++;
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			}
		}
		else if (OptS[x][y][dep] == 2)
		{
			ComponentCnt[1][1][1]++;
			gray1 = DepWm1[x][y][dep];
			gray2 = DepWm2[x][y][dep];
			if (gray1<0 || gray1>255)
				printf("Error gray DepWm1: %d (%d,%d,%d)\n",gray1,x,y,dep);
			if (gray2<0 || gray2>255)
				printf("Error gray DepWm2: %d (%d,%d,%d)\n",gray2,x,y,dep);
			if (gray1 < gray2)
				swap(gray1, gray2);
			switch (method)
			{
			case 0:				
				diff[gray1 + mid]++;
				diff[gray2 + mid]++;
				break;
			case 1:
				if (nodetype10 == -1)
				{
					diff[0] += 2;
				}
				else if (nodetype10==1)
				{
					diff[gray101 - gray1 + mid]++;
					diff[gray101 - gray2 + mid]++;
				}
				else
				{
					diff[gray101 - gray1 + mid]++;
					diff[gray102 - gray2 + mid]++;
				}
				break;
			case 2:
				if (nodetype01 == -1)
				{
					diff[0] += 2;
				}
				else if (nodetype01 == 1)
				{
					diff[gray011 - gray1 + mid]++;
					diff[gray011 - gray2 + mid]++;
				}
				else
				{
					diff[gray011 - gray1 + mid]++;
					diff[gray011 - gray2 + mid]++;
				}
				break;
			case 3:
				if (nodetype11 == -1)
				{
					diff[0] += 2;
				}
				else if (nodetype11 == 1)
				{
					diff[gray111 - gray1 + mid]++;
					diff[gray111 - gray2 + mid]++;
				}
				else
				{
					diff[gray111 - gray1 + mid]++;
					diff[gray112 - gray2 + mid]++;
				}
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			}
		}
	}
}

// Diffrence between constant and adjucant gray level
void Statistics1(int nr, int nc)
{
	int dep,w,w2,i;
	int diff[600]; // mid=300
	memset(diff,0,sizeof(diff));
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);	
	FillConstantDiff(0,0,nr,nc,w2,dep,1,diff);
	//printf("Statistics1 - Show diffrence \n");
	//for (i=0;i<600;i++)
	//{
	//	printf("[%4d] = %7d",i-300,diff[i]);
	//	if (i%4==3)
	//		printf("\n");
	//	else
	//		printf(" -- ");
	//}
	//printf("\n");

	int orgbit=0;
	for (i=0;i<600;i++)
		orgbit+=(diff[i]*8);
	printf("orginal bits : %d\n",orgbit);	
	printf("Method 1 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,2,diff);
	printf("Method 2 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,3,diff);
	printf("Method 3 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*(b+1),bitszo*(b+9),bitsz*(b+1)+bitszo*(b+9));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,4,diff);
	printf("Method 4 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,5,diff);
	printf("Method 5 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,6,diff);
	printf("Method 6 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,7,diff);
	printf("Method 7 : \n");
	for (int b=2;b<=8;b++)
	{
		int rng=(1<<(b-1))-1;
		int bitsz=0, bitszo=0;
		for (i=0;i<600;i++)
			if (i-300>=-rng && i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		printf("for %d bit : IN [%8d] OUT[%8d] (%7d,%7d,%7d)\n",b,bitsz,bitszo,bitsz*b,bitszo*(b+8),bitsz*b+bitszo*(b+8));
	}

}


#if (_MSC_VER>=1800)
void Statistics2(int nr, int nc)
{
	int dep,w,w2,i,x,y;
	int diff[300][3]; // mid=300
	memset(diff,0,sizeof(diff));
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++);
	vector<pair<int,pair<int,int>>> v;
	v.push_back(pair<int,pair<int,int>>(dep,pair<int,int>(0,0)));
	while(v.size()>0)
	{
		dep = v.back().first;
		x = v.back().second.first;
		y = v.back().second.second;
		w = (1<<dep);
		w2 = w/2;
		v.pop_back();
		if (x>=nr || y>=nc) continue;
		switch(OptS[x][y][dep])
		{
		case 0:
			v.push_back(pair<int,pair<int,int>>(dep-1,pair<int,int>(x,y)));
			v.push_back(pair<int,pair<int,int>>(dep-1,pair<int,int>(x+w2,y)));
			v.push_back(pair<int,pair<int,int>>(dep-1,pair<int,int>(x,y+w2)));
			v.push_back(pair<int,pair<int,int>>(dep-1,pair<int,int>(x+w2,y+w2)));
			break;
		case 1:
			diff[DepCm[x][y][dep]][0]++;
			break;
		case 2:
			if (DepWm1[x][y][dep]>DepWm2[x][y][dep])
			{
				diff[DepWm1[x][y][dep]][1]++;
				diff[DepWm2[x][y][dep]][2]++;
			}
			else
			{
				diff[DepWm2[x][y][dep]][1]++;
				diff[DepWm1[x][y][dep]][2]++;
			}
			break;
		}
	}
	for (i=0;i<256;i++)
		printf("%4d : %5d %5d %5d\n",i,diff[i][0],diff[i][1],diff[i][2]);
}
#else
void Statistics2(int nr, int nc)
{
	int dep,w,w2,i,x,y;
	int diff[300][3]; // mid=300
	memset(diff,0,sizeof(diff));
	for (dep=1,w=2,w2=1;w2<max(nr,nc);w*=2,w2*=2,dep++);
	vector<pair<int,pair<int,int>>> v;
	v.push_back(make_pair<int,pair<int,int>>(dep,make_pair<int,int>(0,0)));
	while(v.size()>0)
	{
		dep = v.back().first;
		x = v.back().second.first;
		y = v.back().second.second;
		w = (1<<dep);
		w2 = w/2;
		v.pop_back();
		if (x>=nr || y>=nc) continue;
		switch(OptS[x][y][dep])
		{
			case 0:
				v.push_back(make_pair<int,pair<int,int>>(dep-1,make_pair<int,int>(x,y)));
				v.push_back(make_pair<int,pair<int,int>>(dep-1,make_pair<int,int>(x+w2,y)));
				v.push_back(make_pair<int,pair<int,int>>(dep-1,make_pair<int,int>(x,y+w2)));
				v.push_back(make_pair<int,pair<int,int>>(dep-1,make_pair<int,int>(x+w2,y+w2)));
				break;
			case 1:
				diff[DepCm[x][y][dep]][0]++;
				break;
			case 2:
				if (DepWm1[x][y][dep]>DepWm2[x][y][dep])
				{
					diff[DepWm1[x][y][dep]][1]++;
					diff[DepWm2[x][y][dep]][2]++;
				}
				else
				{
					diff[DepWm2[x][y][dep]][1]++;
					diff[DepWm1[x][y][dep]][2]++;
				}
				break;
		}
	}
	for (i=0;i<256;i++)
		printf("%4d : %5d %5d %5d\n",i,diff[i][0],diff[i][1],diff[i][2]);
}
#endif

unsigned char cmbits[1000000];
HuffmanCoder huffman;
int CompressMemory1(MemoryFile* mem)
{
	int i,j;
	int len = mem->GetLengthByte();
	// method 1
	/*
	for(i=0;i<len;i++)
	{
		for (j=0;j<8;j++)
			if ((mem->memoryspace[i] & (1<<j))>0)
				cmbits[i+j*len]=1;
			else
				cmbits[i+j*len]=0;
	}
	len = len*8;
	*/
	//method 2
	int gray=0;
	for(i=0;i<len;i++)
	{
		if (i>0)
			gray = mem->memoryspace[i] - mem->memoryspace[i-1];
		else
			gray = mem->memoryspace[i];
		for(j=0;j<8;j++)
			if ((abs(gray) & (1<<j))>0)
				cmbits[i+j*len]=1;
			else
				cmbits[i+j*len]=0;
		if (gray<0)
			cmbits[i+8*len]=1;
		else
			cmbits[i+8*len]=0;
	}
	len = len*9;

	int bestrate = (1<<30);
	int huffclen, huffplen;
	for (huffclen=2;huffclen<=5;huffclen++)
	{
		for (huffplen=8;huffplen<=24;huffplen++)
		{
			int r = 2+4; //2:huffclen , 4:huffplen
			for(i=0;i<len;i+=huffplen*huffclen)
			{
				// for all methods and return best methods
				r += huffman.CodeRate(huffclen,cmbits,i,min(len,i+huffplen*huffclen)) + huffclen;
			}			
			if (r<bestrate)
				bestrate = r;
		}
	}
	return bestrate;
}

void DecideGrayMethod(int nr, int nc, int &method, int &codebits)
{
	int dep,w,w2,i,b,m;
	int minsum=1<<30,sum;
	int diff[600]; // mid=300
	int A[]={0,0,0,1,0,0,0,0};
	int B[]={0,8,8,9,8,8,8,8};
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);

	//m=0;
	memset(diff,0,sizeof(diff));
	FillConstantDiff(0,0,nr,nc,w2,dep,0,diff);
	for (b=2;b<=8;b++)
	{
		int rng=(1<<b)-1;
		int bitsz=0, bitszo=0, bitszp=0;
		for (i=0;i<600;i++)
			if (i-300<rng)
				bitsz+=diff[i];
			else
				bitszo+=diff[i];
		bitszp = ComponentCnt[0][0][0]-bitsz-bitszo;
		sum = bitsz*b+bitszo*(b+8)+bitszp*8;
		if (sum<minsum)
		{
			minsum=sum;
			method=0;
			codebits=b;
		}
	}

	for (m=1;m<=2;m++)
	{
		memset(diff,0,sizeof(diff));
		FillConstantDiff(0,0,nr,nc,w2,dep,m,diff);
		for (b=2;b<=9;b++)
		{
			int rng=(b<9)?(1<<(b-1))-1:(1<<(b-1));
			int bitsz=0, bitszo=0, bitszp=0;
			for (i=0;i<600;i++)
				if (i-300>=-rng && i-300<rng)
					bitsz+=diff[i];
				else
					bitszo+=diff[i];
			bitszp = ComponentCnt[0][0][0]-bitsz-bitszo;
			sum = bitsz*(b+A[m])+bitszo*(b+B[m])+bitszp*8;
			if (sum<minsum)
			{
				minsum=sum;
				method=m;
				codebits=b;
			}
		}
	}
}

void DecideGrayMethodCoded(int nr, int nc, int &method, int &codebits)
{
	int dep, w, w2, i, b, m;
	int minsum = 1 << 30, sum;
	int diff[600]; // mid=300
	int A[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int B[] = { 0, 8, 8, 8, 8, 8, 8, 8 };
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);

	//m=0;
	memset(diff, 0, sizeof(diff));
	ComponentCnt[1][1][0]=0; ComponentCnt[1][1][1]=0; ComponentCnt[1][1][2]=0;
	FillCodedDiff(0, 0, nr, nc, w2, dep, dep, 0, diff);
	for (b = 2; b <= 8; b++)
	{
		int rng = (1 << b) - 1;
		int bitsz = 0, bitszo = 0, bitszp = 0;
		for (i = 1; i<600; i++)
			if (i - 300<rng)
				bitsz += diff[i];
			else
				bitszo += diff[i];
		bitszp = diff[0];
		sum = bitsz*b + bitszo*(b + 8) + bitszp * 8;
		if (sum<minsum)
		{
			minsum = sum;
			method = 0;
			codebits = b;
		}
	}

	for (m = 1; m <= 3; m++)
	{
		memset(diff, 0, sizeof(diff));
		ComponentCnt[1][1][0]=0; ComponentCnt[1][1][1]=0; ComponentCnt[1][1][2]=0;
		FillCodedDiff(0, 0, nr, nc, w2, dep, dep, m, diff);
		for (b = 2; b <= 9; b++)
		{
			int rng = (b<9) ? (1 << (b - 1)) - 1 : (1 << (b - 1));
			int bitsz = 0, bitszo = 0, bitszp = 0;
			for (i = 1; i<600; i++)
				if (i - 300 >= -rng && i - 300<rng)
					bitsz += diff[i];
				else
					bitszo += diff[i];
			bitszp = diff[0];
			sum = bitsz*(b + A[m]) + bitszo*(b + B[m]) + bitszp * 8;
			if (sum<minsum)
			{
				minsum = sum;
				method = m;
				codebits = b;
			}
		}
	}
}

int DecideMaxInDepth(int x, int y, int nr, int nc, int w2, int dep)
{
	int id1,id2,id3,id4;
	if (x>=nr || y>=nc) return 0;
	if (OptS[x][y][dep]==0)
	{
		id1 = DecideMaxInDepth(x,y,nr,nc,w2/2,dep-1);
		id2 = DecideMaxInDepth(x,y+w2,nr,nc,w2/2,dep-1);
		id3 = DecideMaxInDepth(x+w2,y,nr,nc,w2/2,dep-1);
		id4 = DecideMaxInDepth(x+w2,y+w2,nr,nc,w2/2,dep-1);
		return max(id1,max(id2,max(id3,id4)));
	}
	return dep;
}

void EncodeGray(int x, int y, int method, int codebits, int &gray1, int &gray1bits, int &gray2, int &gray2bits)
{
	int rng;

	switch(method)
	{
		case 0: // x,y
			gray1 = imgprd[x][y];
			break;
		case 1: // x-1,y
			if (x==0)
			{
				gray1 = imgprd[x][y];
				gray1bits = 8;
				gray2bits = 0;
				return;
			}
			gray1 = imgprd[x][y]-imgprd[x-1][y];
			break;
		case 2: // x,y-1
			if (y==0)
			{
				gray1 = imgprd[x][y];
				gray1bits = 8;
				gray2bits = 0;
				return;
			}
			gray1 = imgprd[x][y]-imgprd[x][y-1];
			break;
	}
	if (method==0)
	{
		if (codebits==8)
		{
			gray1bits=8;
			gray2bits=0;
			return;
		}
		rng = (1<<codebits)-1;
		if (gray1<rng)
		{
			gray1bits=codebits;
			gray2bits=0;
		}
		else
		{
			gray2=gray1;
			gray1=rng;
			gray1bits=codebits;
			gray2bits=8;
		}
		return;
	}
	rng = (1<<(codebits-1));
	gray1bits=codebits;
	if (-rng+1<=gray1 && gray1<rng-1)
	{
		gray1 = gray1+rng;
		gray2bits = 0;
	}
	else if (gray1<0)
	{
		gray2=-gray1;
		gray1=0;
		gray2bits=8;
	}
	else if (gray1>0)
	{
		gray2=gray1;
		gray1=(1<<codebits)-1;
		gray2bits=8;
	}
}

int DecodeGray(int x, int y, int method, int codebits, BinaryFile &bf)
{
	int gray,gray1;

	if (method==0)
	{
		gray=bf.ReadBits(codebits);
		if (gray==((1<<codebits)-1))
			gray=bf.ReadBits(8);
		return gray;
	}

	switch(method)
	{
		case 1: // x-1,y
			if (x==0)
				return bf.ReadBits(8);
			break;
		case 2: // x,y-1
			if (y==0)
				return bf.ReadBits(8);
			break;
	}

	gray1 = bf.ReadBits(codebits);
	if (gray1==0)
	{
		gray = -(bf.Read1Byte());
	}
	else if (gray1==(1<<codebits)-1)
	{
		gray = bf.Read1Byte();
	}
	else
	{
		gray = gray1-(1<<(codebits-1));
	}

	switch(method)
	{
		case 1: // x-1,y			
			return gray + rimg[x-1][y];
		case 2: // x,y-1
			return gray + rimg[x][y-1];
	}
	return 0;
}

void EncodeGrayCoded(int gray, bool needcode, int method, int codebits, int &gray1, int &gray1bits, int &gray2, int &gray2bits)
{
	int rng;

	gray1 = gray;
	if (needcode==false)
	{				
		gray1bits = 8;
		gray2bits = 0;
		return;
	}

	if (method==0)
	{
		if (codebits==8)
		{
			gray1bits=8;
			gray2bits=0;
			return;
		}
		rng = (1<<codebits)-1;
		if (gray1<rng)
		{
			gray1bits=codebits;
			gray2bits=0;
		}
		else
		{
			gray2=gray1;
			gray1=rng;
			gray1bits=codebits;
			gray2bits=8;
		}
		return;
	}
	rng = (1<<(codebits-1));
	gray1bits=codebits;
	if (-rng+1<=gray1 && gray1<rng-1)
	{
		gray1 = gray1+rng;
		gray2bits = 0;
	}
	else if (gray1<0)
	{
		gray2=-gray1;
		gray1=0;
		gray2bits=8;
	}
	else if (gray1>0)
	{
		gray2=gray1;
		gray1=(1<<codebits)-1;
		gray2bits=8;
	}
}

void CodecV1Encode(int x, int y, int nr, int nc, int w, int dep, int method, int codebits, BinaryFile &bf)
{
	int w2;
	int gray1,gray1bits,gray2,gray2bits;
	int bits;
	w2 = w/2;

	if (x>=nr || y>=nc) return;

	switch(OptS[x][y][dep])
	{
		case 0:
			bf.WriteBit(0);
			CodecV1Encode(x,y,nr,nc,w2,dep-1,method,codebits,bf);
			CodecV1Encode(x,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
			CodecV1Encode(x+w2,y,nr,nc,w2,dep-1,method,codebits,bf);
			CodecV1Encode(x+w2,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
			StatisticsVar[0]++;
			break;
		case 1:
			if (dep>0)
			{
				bf.WriteBit(1,0);
				StatisticsVar[4]++;
			}
			StatisticsVar[1]++;
			EncodeGray(x,y,method,codebits,gray1,gray1bits,gray2,gray2bits);
			bf.WriteBits(gray1,gray1bits);
			bf.WriteBits(gray2,gray2bits);
			if (gray2bits>0)
				StatisticsVar[6]++;
			else
				StatisticsVar[5]++;
			break;
		case 2:
			bf.WriteBit(1,1,0);
			StatisticsVar[2]++;
			bits = DepWr[x][y][dep]-2*GRAYBIT;
			bits >>= 1;
			bf.WriteBits(DepWi[x][y][dep],bits);
			bf.WriteBits(DepWj[x][y][dep],bits);
			bf.Write1Byte(DepWm1[x][y][dep]);
			bf.Write1Byte(DepWm2[x][y][dep]);
			break;
		case 3:
			bf.WriteBit(1,1,1);
			StatisticsVar[3]++;
			bits = DepPr[x][y][dep]-2*GRAYBIT;
			bits = bits/3;
			bf.WriteBits(DepPi1[x][y][dep],bits);
			bf.WriteBits(DepPi2[x][y][dep],bits);
			bf.WriteBits(DepPi3[x][y][dep],bits);
			bf.Write1Byte(DepPm1[x][y][dep]);
			bf.Write1Byte(DepPm2[x][y][dep]);
			break;
	}
}

void CodecV1Encode(int nr, int nc, BinaryFile &bf)
{
	int dep,w,w2;
	int method,codebits;
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);
	DecideGrayMethod(nr,nc,method,codebits);
	bf.Write2Byte(nr);
	bf.Write2Byte(nc);
	bf.WriteBits(method,4);
	bf.WriteBits(codebits,4);
	memset(StatisticsVar,0,sizeof(StatisticsVar));
	StatisticsVar[7]=method;
	StatisticsVar[8]=codebits;
	CodecV1Encode(0,0,nr,nc,w,dep,method,codebits,bf);
}

void DecoderMakeWedgelet(int bi, int bj, int wr, int wc, int wi, int wj, int m1, int m2)
{
	int wix,wiy,wjx,wjy;
	int x,y;
	bool sgn;

	GetXY(wr,wc,wi,wix,wiy);
	GetXY(wr,wc,wj,wjx,wjy);
	for (x=0;x<wr;x++)
	{
		for (y=0;y<wc;y++)
		{
			sgn = ((y-wiy)*(wjx-wix)-(x-wix)*(wjy-wiy)) >= 0;
			if (sgn)
				rimg[bi+x][bj+y] = m1;
			else
				rimg[bi+x][bj+y] = m2;
		}
	}
}

void DecoderMakePipelet(int bi, int bj, int wr, int wc, int pi1, int pi2, int pi3, int m1, int m2)
{
	int i1x,i1y,i2x,i2y,i3x,i3y,pi4,i4x,i4y;
	int x,y;
	bool sgn1,sgn2;
	
	GetXY(wr,wc,pi1,i1x,i1y);
	GetXY(wr,wc,pi2,i2x,i2y);
	GetXY(wr,wc,pi3,i3x,i3y);
	GetIndex4(wr,wc,i1x,i1y,i2x,i2y,i3x,i3y,pi4);
	GetXY(wr,wc,pi4,i4x,i4y);
	for (x=0;x<wr;x++)
	{
		for (y=0;y<wc;y++)
		{
			sgn1 = ((y-i1y)*(i4x-i1x)-(x-i1x)*(i4y-i1y)) >= 0;
			sgn2 = ((y-i2y)*(i3x-i2x)-(x-i2x)*(i3y-i2y)) >= 0;
			if (sgn1 ^ sgn2)
				rimg[bi+x][bj+y] = m1;
			else
				rimg[bi+x][bj+y] = m2;
		}
	}

}

void CodecV1Decode(int x, int y, int nr, int nc, int w, int dep, int method, int codebits, BinaryFile &bf)
{
	int w2,v,wr,wc,mxindex;
	int bits;
	int wi,wj,m1,m2,pi1,pi2,pi3;
	int gray,xx,yy;

	w2 = w/2;

	if (x>=nr || y>=nc) return;

	if (dep==0)
		v=1;
	else
		v = bf.ReadBit();
	if (v==0) // Node with childs
	{
		CodecV1Decode(x,y,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV1Decode(x,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV1Decode(x+w2,y,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV1Decode(x+w2,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
	}
	else
	{
		wr = min(w,nr-x);
		wc = min(w,nc-y);

		if (dep==0)
			v=0;
		else
			v=bf.ReadBit();
		if (v==0) // Constant
		{
			gray=DecodeGray(x,y,method,codebits,bf);
			for (xx=0;xx<wr;xx++)
				for (yy=0;yy<wc;yy++)
					rimg[x+xx][y+yy] = gray;
		}
		else // Wedgelet or Pipelet
		{
			mxindex = 2*(wr+wc);
			bits = (int)ceil(log((double)mxindex)/log(2.0));
			v = bf.ReadBit();
			if (v==0) // Wedgelet
			{
				wi = bf.ReadBits(bits);
				wj = bf.ReadBits(bits);
				m1 = bf.Read1Byte();
				m2 = bf.Read1Byte();
				DecoderMakeWedgelet(x,y,wr,wc,wi,wj,m1,m2);
			}
			else // Pipelet
			{
				pi1 = bf.ReadBits(bits);
				pi2 = bf.ReadBits(bits);
				pi3 = bf.ReadBits(bits);
				m1 = bf.Read1Byte();
				m2 = bf.Read1Byte();
				DecoderMakePipelet(x,y,wr,wc,pi1,pi2,pi3,m1,m2);
			}
		}
	}
}

void CodecV1Decode(BinaryFile &bf)
{
	int dep,w,w2;
	int nr,nc;
	int method,codebits;
	nr = bf.Read2Byte();
	nc = bf.Read2Byte();
	method=bf.ReadBits(4);
	codebits=bf.ReadBits(4);
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);
	CodecV1Decode(0,0,nr,nc,w,dep,method,codebits,bf);
}

void CodecV2Encode(int nr, int nc, BinaryFile &bf)
{
	int i,j;
	int dep,w,w2;
	int method,codebits,indepth;
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);
	DecideGrayMethod(nr,nc,method,codebits);
	indepth = DecideMaxInDepth(0,0,nr,nc,w2,dep);
	bf.Write2Byte(nr);
	bf.Write2Byte(nc);
	bf.WriteBits(method,4);
	bf.WriteBits(codebits,4);
	bf.WriteBits(indepth,4);
	memset(StatisticsVar,0,sizeof(StatisticsVar));
	StatisticsVar[7]=method;
	StatisticsVar[8]=codebits;
	StatisticsVar[9]=indepth;
	int indepthw = (1<<indepth);
	for (i=0;i<nr;i+=indepthw)
		for (j=0;j<nc;j+=indepthw)
			CodecV1Encode(i,j,nr,nc,indepthw,indepth,method,codebits,bf);
}

void CodecV2Decode(BinaryFile &bf)
{
	int i,j;
	int nr,nc;
	int method,codebits,indepth;
	nr = bf.Read2Byte();
	nc = bf.Read2Byte();
	method=bf.ReadBits(4);
	codebits=bf.ReadBits(4);
	indepth=bf.ReadBits(4);
	int indepthw = (1<<indepth);
	for (i=0;i<nr;i+=indepthw)
		for (j=0;j<nc;j+=indepthw)
			CodecV1Decode(i,j,nr,nc,indepthw,indepth,method,codebits,bf);
}

void CodecV8Encode(int x, int y, int nr, int nc, int w, int dep, int deporg, int method, int codebits, BinaryFile &bf)
{
	int w2,wr,wc;
	int cgray1,cgray2;
	bool cgrayswap, cneedcode;
	int gray1, gray1bits, gray2, gray2bits;
	int thetabits, codedlinebits;
	int i;
	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
		case 0:
			bf.WriteBit(0);
			CodecV8Encode(x, y, nr, nc, w2, dep - 1, deporg, method, codebits, bf);
			CodecV8Encode(x, y + w2, nr, nc, w2, dep - 1, deporg, method, codebits, bf);
			CodecV8Encode(x + w2, y, nr, nc, w2, dep - 1, deporg, method, codebits, bf);
			CodecV8Encode(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, method, codebits, bf);
			StatisticsVar[0]++;
			break;
		case 1:
			//encoding structure
			if (dep > 0)
			{
				bf.WriteBit(1, 0);
				StatisticsVar[4]++;
			}
			StatisticsVar[1]++;

			//encoding gray
			CalculateCodedGray(x,y,dep,nr,nc,deporg,method,cgray1,cgray2,cgrayswap,cneedcode);
			EncodeGrayCoded(cgray1, cneedcode, method, codebits, gray1, gray1bits, gray2, gray2bits);
			bf.WriteBits(gray1, gray1bits);
			bf.WriteBits(gray2, gray2bits);
			if (gray2bits > 0)
				StatisticsVar[6]++;
			else
				StatisticsVar[5]++;
			break;
		case 2:
			//encoding structure
			bf.WriteBit(1, 1);
			StatisticsVar[2]++;
			
			//encoding gray
			CalculateCodedGray(x,y,dep,nr,nc,deporg,method,cgray1,cgray2,cgrayswap,cneedcode);
			EncodeGrayCoded(cgray1, cneedcode, method, codebits, gray1, gray1bits, gray2, gray2bits);
			bf.WriteBits(gray1, gray1bits);
			bf.WriteBits(gray2, gray2bits);
			if (gray2bits > 0)
				StatisticsVar[6]++;
			else
				StatisticsVar[5]++;
			EncodeGrayCoded(cgray2, cneedcode, method, codebits, gray1, gray1bits, gray2, gray2bits);
			bf.WriteBits(gray1, gray1bits);
			bf.WriteBits(gray2, gray2bits);
			if (gray2bits > 0)
				StatisticsVar[6]++;
			else
				StatisticsVar[5]++;

			//encoding theta & codebits
			wr = min(w, nr - x);
			wc = min(w, nc - y);

			thetabits = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

			bf.WriteBits(DepWj[x][y][dep], thetabits);
			for (i=0;i<codedlinebits;i++)
				bf.WriteBit((DepWbs[x][y][dep].State(i)^cgrayswap)?'1':'0');			

			break;
	}
}

void CodecV8Encode(int nr, int nc, BinaryFile &bf)
{	
	int i, j;
	int dep, w, w2;
	int method, codebits, indepth;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	DecideGrayMethodCoded(nr, nc, method, codebits);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	bf.Write2Byte(nr);
	bf.Write2Byte(nc);
	bf.WriteBits(method, 4);
	bf.WriteBits(codebits, 4);
	bf.WriteBits(indepth, 4);
	memset(StatisticsVar, 0, sizeof(StatisticsVar));
	StatisticsVar[7] = method;
	StatisticsVar[8] = codebits;
	StatisticsVar[9] = indepth;
	int indepthw = (1 << indepth);
	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			CodecV8Encode(i, j, nr, nc, indepthw, indepth, dep, method, codebits, bf);
}

void CodecV8Decode(int x, int y, int nr, int nc, int w, int dep, int method, int codebits, BinaryFile &bf)
{
	int w2,v,wr,wc,i;
	int gray1,xx,yy;
	int thetabits, codedlinebits;
	BinaryStream codedline;
	int theta;
	int bit;

	w2 = w/2;

	if (x>=nr || y>=nc) return;

	if (dep==0)
		v = 1;
	else
		v = bf.ReadBit();
	if (v==0) // Node with childs
	{
		CodecV8Decode(x,y,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV8Decode(x,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV8Decode(x+w2,y,nr,nc,w2,dep-1,method,codebits,bf);
		CodecV8Decode(x+w2,y+w2,nr,nc,w2,dep-1,method,codebits,bf);
	}
	else
	{
		wr = min(w,nr-x);
		wc = min(w,nc-y);

		if (dep==0)
			v=0;
		else
			v=bf.ReadBit();
		if (v==0) // Constant
		{
			//DecodeGrayCoded(x,y,dep,method,codebits,bf,1,gray1,gray2);
			gray1 = 0;
			for (xx=0;xx<wr;xx++)
				for (yy=0;yy<wc;yy++)
					rimg[x+xx][y+yy] = gray1;
		}
		else // Multiline
		{
			thetabits = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinebits = 0;

			//DecodeGrayCoded(x,y,dep,method,codebits,bf,2,gray1,gray2);
			theta = bf.ReadBits(thetabits);
			for(i=0;i<codedlinebits;i++)
			{
				bit = bf.ReadBit();
				if (bit==1)
					codedline.SetBit(i);
				else
					codedline.UnsetBit(i);
			}

		}
	}
}

void CodecV8Decode(BinaryFile &bf)
{
	int i, j;
	int nr, nc;
	int method, codebits, indepth;
	nr = bf.Read2Byte();
	nc = bf.Read2Byte();
	method = bf.ReadBits(4);
	codebits = bf.ReadBits(4);
	indepth = bf.ReadBits(4);
	int indepthw = (1 << indepth);
	for (i = 0; i < nr; i += indepthw)
		for (j = 0; j < nc; j += indepthw)
			CodecV8Decode(i, j, nr, nc, indepthw, indepth, method, codebits, bf);
}

void CodecV9Encode(int x, int y, int nr, int nc, int w, int dep, int deporg, int method, int codebits, MemoryFile mems[])
{
	int w2,wr,wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
		case 0:
			mems[0].WriteBit(0);
			CodecV9Encode(x, y, nr, nc, w2, dep - 1, deporg, method, codebits, mems);
			CodecV9Encode(x, y + w2, nr, nc, w2, dep - 1, deporg, method, codebits, mems);
			CodecV9Encode(x + w2, y, nr, nc, w2, dep - 1, deporg, method, codebits, mems);
			CodecV9Encode(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, method, codebits, mems);
			StatisticsVar[0]++;
			break;
		case 1:
			//encoding structure
			if (dep > 0)
			{
				mems[0].WriteBit(1, 0);
				StatisticsVar[4]++;
			}
			StatisticsVar[1]++;

			//encoding gray
			mems[1].Write1Byte(DepCm[x][y][dep]);
			break;
		case 2:
			//encoding structure
			mems[0].WriteBit(1, 1);
			StatisticsVar[2]++;
			
			//encoding gray
			cgrayswap = false;
			mems[1].Write1Byte(DepWm1[x][y][dep]);
			mems[2].Write1Byte(DepWm2[x][y][dep]);

			//encoding theta & codebits
			wr = min(w, nr - x);
			wc = min(w, nc - y);

			thetabits = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

			mems[0].WriteBits(DepWj[x][y][dep], thetabits);
			for (i=0;i<codedlinebits;i++)
				mems[0].WriteBit((DepWbs[x][y][dep].State(i)^cgrayswap)?'1':'0');			

			break;
	}
}

void CodecV9Encode(int nr, int nc, MemoryFile mems[])
{	
	int i, j;
	int dep, w, w2;
	int method, codebits, indepth;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	//DecideGrayMethodCoded(nr, nc, method, codebits);
	method=0; codebits=0;
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	//bf.WriteBits(method, 4);
	//bf.WriteBits(codebits, 4);
	mems[0].WriteBits(indepth, 4);
	memset(StatisticsVar, 0, sizeof(StatisticsVar));
	//StatisticsVar[7] = method;
	//StatisticsVar[8] = codebits;
	StatisticsVar[9] = indepth;	
	int indepthw = (1 << indepth);
	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			CodecV9Encode(i, j, nr, nc, indepthw, indepth, dep, method, codebits, mems);
	//int memclen1 = CompressMemory1(&mems[1]);
	int memclen2 = CompressMemory1(&mems[2]);
	//printf("Memory1 : %d to %d\n",mems[1].GetLengthBit(),memclen1);
	printf("Memory2 : %d to %d\n",mems[2].GetLengthBit(),memclen2);
}

MemoryFile mems[4];
int Encoder(int nr, int nc, int version, char filename[])
{
	BinaryFile bf(filename);
	bf.OpenWrite();
	bf.Write1Byte(version);
	switch(version)
	{
		case 1:
			CodecV1Encode(nr,nc,bf);
			break;
		case 2:
			CodecV2Encode(nr, nc, bf);
			break;
		case 8:
			CodecV8Encode(nr, nc, bf);
			break;
		case 9:
			mems[0].Reset(); mems[1].Reset(); mems[2].Reset();
			CodecV9Encode(nr, nc, mems);
			int memclen1 = CompressMemory1(&mems[1]);
			int memclen2 = CompressMemory1(&mems[2]);
			//return (int)ceil((mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit())/8.0);
			return (int)ceil((mems[0].GetLengthBit() + memclen1 + memclen2) / 8.0);
	}
	bf.Close();
	return bf.GetLength();
}

int NumSequence[6][100000];
int NumSequenceCount[6];
int NumSequenceBit[6];

int GCompressBitCodePos(int ind)
{
	int i,rate,bit,rng;
	int bestrate = (1<<30);
	if (NumSequenceCount[ind]==0)
		return 0;
	for (i=0;i<NumSequenceCount[ind];i++)
		if (NumSequence[ind][i]<0)
			return (1<<30);
	for (bit=2;bit<NumSequenceBit[ind];bit++)
	{
		rate=3;
		rng = (1<<bit)-2;
		for (i=0;i<NumSequenceCount[ind];i++)
			if (NumSequence[ind][i]<=rng)
				rate += bit;
			else
				rate += bit + NumSequenceBit[ind];
		if (rate<bestrate)
			bestrate = rate;
	}
	for (bit=2;bit<NumSequenceBit[ind];bit++)
	{
		rate=3;
		rng = (1<<bit)-1;
		for (i=0;i<NumSequenceCount[ind];i++)
			if (NumSequence[ind][i]<=rng)
				rate += 1 + bit;
			else
				rate += 1 + NumSequenceBit[ind];
		if (rate<bestrate)
			bestrate = rate;
	}
	return bestrate + 1;
}

int GCompressBitCode(int ind)
{
	int i,rate,bit,rng;
	int bestrate = (1<<30);
	if (NumSequenceCount[ind]==0)
		return 0;
	for (bit=2;bit<NumSequenceBit[ind];bit++)
	{
		rate=3;
		rng = (1<<(bit-1))-1;
		for (i=0;i<NumSequenceCount[ind];i++)
			if (-rng<NumSequence[ind][i] && NumSequence[ind][i]<=rng)
				rate += bit;
			else
				rate += bit + NumSequenceBit[ind];
		if (rate<bestrate)
			bestrate = rate;
	}
	for (bit=2;bit<NumSequenceBit[ind];bit++)
	{
		rate=3;
		rng = (1<<(bit-1))-1;
		for (i=0;i<NumSequenceCount[ind];i++)
			if (-rng<=NumSequence[ind][i] && NumSequence[ind][i]<=rng)
				rate += bit;
			else
				rate += bit + 1 + NumSequenceBit[ind];
		if (rate<bestrate)
			bestrate = rate;
	}
	return bestrate + 1;
}

int GCompressGolomb(int ind)
{
	int i, rate, bit;
	int M, r;
	int bestrate = (1 << 30);
	if (NumSequenceCount[ind] == 0)
		return 0;
	bool isPositive = true;
	for (i = 0; i<NumSequenceCount[ind] && isPositive; i++)
		if (NumSequence[ind][i]<0)
			isPositive = false;
	for (bit = 1; bit<NumSequenceBit[ind]; bit++)
	{
		M = (1 << bit);
		rate = 3;
		for (i = 0; i<NumSequenceCount[ind]; i++)
		{
			r = abs(NumSequence[ind][i]) / M;
			rate += (isPositive ? r + 1 : 2 * r + (NumSequence[ind][i]<0 ? 1 : 0)) + bit;
		}
		if (rate<bestrate)
			bestrate = rate;
	}
	return bestrate;
}

int GCompressPrediction(int ind, int tempind)
{
	int i,rate;
	int bestrate = (1<<30);

	NumSequenceCount[tempind]=1;
	NumSequenceBit[tempind] = NumSequenceBit[ind];
	NumSequence[tempind][0] = NumSequence[ind][0];
	for (i=1;i<NumSequenceCount[ind];i++)
	{
		NumSequence[tempind][NumSequenceCount[tempind]++] = NumSequence[ind][i]-NumSequence[ind][i-1];
	}	
	rate = GCompressBitCode(tempind);
	if (rate<bestrate)
		bestrate = rate;
	rate = GCompressGolomb(tempind);
	if (rate<bestrate)
		bestrate = rate;

	NumSequenceCount[tempind]=2;
	NumSequenceBit[tempind] = NumSequenceBit[ind];
	NumSequence[tempind][0] = NumSequence[ind][0];
	NumSequence[tempind][1] = NumSequence[ind][1] - NumSequence[ind][0];
	for (i=2;i<NumSequenceCount[ind];i++)
	{
		NumSequence[tempind][NumSequenceCount[tempind]++] = NumSequence[ind][i]-(NumSequence[ind][i-1] + NumSequence[ind][i-2])/2;
	}	
	rate = GCompressBitCode(tempind);
	if (rate<bestrate)
		bestrate = rate;
	rate = GCompressGolomb(tempind);
	if (rate<bestrate)
		bestrate = rate;

	return rate + 2;
}

int GCodeSequence(int ind)
{
	int ratec;
	int rate = NumSequenceCount[ind]*NumSequenceBit[ind];

	/*printf("Seq %d \n", ind);
	for (int i = 0; i < NumSequenceCount[ind]; i++)
		printf("%5d", NumSequence[ind][i]);
	printf("\n\n");*/

	if (ShowMessages) printf("--------------- \n Compress Sequence %d \n RATE : %d \n", ind, rate);
	
	ratec = GCompressBitCodePos(ind);
	if (ShowMessages) printf("GCompressBitCodePos : %d (%lf)\n", ratec, (double)ratec / rate);
	if (ratec<rate)
		rate=ratec;

	ratec = GCompressBitCode(ind);
	if (ShowMessages) printf("GCompressBitCode : %d (%lf)\n", ratec, (double)ratec / rate);
	if (ratec<rate)
		rate=ratec;
	
	ratec = GCompressPrediction(ind,4);
	if (ShowMessages) printf("GCompressPrediction : %d (%lf)\n", ratec, (double)ratec / rate);
	if (ratec<rate)
		rate=ratec;

	ratec = GCompressGolomb(ind);
	if (ShowMessages) printf("GCompressGolomb : %d (%lf)\n", ratec, (double)ratec / rate);
	if (ratec<rate)
		rate=ratec;

	rate+=3; // None + GCompressBitCodePos + GCompressBitCode + GCompressPrediction + GCompressGolomb
	return rate;
}

void GEncoderNormalSequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, MemoryFile mems[])
{
	int w2,wr,wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
		case 0:
			mems[0].WriteBit(0);
			GEncoderNormalSequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
			GEncoderNormalSequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
			GEncoderNormalSequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
			GEncoderNormalSequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
			break;
		case 1:
			//encoding structure
			if (dep > 0)
			{
				mems[0].WriteBit(1, 0);
			}

			//encoding gray
			NumSequence[0][NumSequenceCount[0]++] = DepCm[x][y][dep] >> (8 - graybit);
			break;
		case 2:
			//encoding structure
			mems[0].WriteBit(1, 1);
			
			//encoding gray
			cgrayswap = false;
			NumSequence[0][NumSequenceCount[0]++] = DepWm1[x][y][dep] >> (8 - graybit);
			NumSequence[0][NumSequenceCount[0]++] = DepWm2[x][y][dep] >> (8 - graybit);

			//encoding theta & codebits
			wr = min(w, nr - x);
			wc = min(w, nc - y);

			thetabits = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinebits = DepWr[x][y][dep] - 2 * graybit - thetabits;

			mems[0].WriteBits(DepWj[x][y][dep], thetabits);
			for (i=0;i<codedlinebits;i++)
				mems[0].WriteBit((DepWbs[x][y][dep].State(i)^cgrayswap)?'1':'0');			

			break;
	}
}

int GEncoderNormalSequence(int nr, int nc, int graybit, MemoryFile mems[])
{
	int dep,w,w2,indepth;
	int i,j;
	NumSequenceBit[0]=graybit; NumSequenceBit[1]=graybit; NumSequenceBit[2]=graybit; NumSequenceBit[3]=graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);	
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			GEncoderNormalSequence(i, j, nr, nc, indepthw, indepth, dep, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for(i=0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}

void GEncoderLocoISequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, MemoryFile mems[])
{
	int w2,wr,wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	int predict,px01,px10,px11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
		case 0:
			mems[0].WriteBit(0);
			GEncoderLocoISequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
			GEncoderLocoISequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
			GEncoderLocoISequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit
				, mems);
			GEncoderLocoISequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
			break;
		case 1:
			//encoding structure
			if (dep > 0)
			{
				mems[0].WriteBit(1, 0);
			}

			//encoding gray
			if (x>0 && y>0)
			{
				px10 = imgwrd[x-1][y];
				px01 = imgwrd[x][y-1];
				px11 = imgwrd[x-1][y-1];
				if (px11>=max(px10,px01))
					predict=min(px10,px01);
				else if (px11<=min(px10,px01))
					predict=max(px10,px01);
				else
					predict=px10+px01-px11;
			}
			else
			{
				predict=0;
			}
			NumSequence[0][NumSequenceCount[0]++] = (DepCm[x][y][dep] >> (8 - graybit)) - (predict >> (8 - graybit));
			break;
		case 2:
			//encoding structure
			mems[0].WriteBit(1, 1);
			
			//encoding gray
			cgrayswap = false;
			NumSequence[1][NumSequenceCount[1]++] = (DepWm1[x][y][dep]) >> (8 - graybit);
			NumSequence[1][NumSequenceCount[1]++] = (DepWm2[x][y][dep]) >> (8 - graybit);

			//encoding theta & codebits
			wr = min(w, nr - x);
			wc = min(w, nc - y);

			thetabits = (int)ceil(log((double)(wr+wc-1)) / log(2.0));
			codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

			mems[0].WriteBits(DepWj[x][y][dep], thetabits);
			for (i=0;i<codedlinebits;i++)
				mems[0].WriteBit((DepWbs[x][y][dep].State(i)^cgrayswap)?'1':'0');			

			break;
	}
}
int GEncoderLocoISequence(int nr, int nc, int graybit, MemoryFile mems[])
{
	int dep,w,w2,indepth;
	int i,j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);	
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			GEncoderLocoISequence(i, j, nr, nc, indepthw, indepth, dep, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for(i=0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}

void GEFindNeighbour(int x, int y, int dep, int xs, int ys, int nr, int nc, int &xf, int &yf, int &depf, int &typef)
{
	int w2 = (1<<(dep-1));

	if (xs<0 || ys<0)
	{
		typef=0;
		return;
	}

	switch(OptS[x][y][dep])
	{
		case 0:
			if (xs<x+w2 && ys<y+w2)
				GEFindNeighbour(x,y,dep-1,xs,ys,nr,nc,xf,yf,depf,typef);
			else if (xs<x+w2)
				GEFindNeighbour(x,y+w2,dep-1,xs,ys,nr,nc,xf,yf,depf,typef);
			else if (ys<y+w2)
				GEFindNeighbour(x+w2,y,dep-1,xs,ys,nr,nc,xf,yf,depf,typef);
			else
				GEFindNeighbour(x+w2,y+w2,dep-1,xs,ys,nr,nc,xf,yf,depf,typef);
			break;
		case 1:
			xf=x;
			yf=y;
			depf=dep;
			typef=1;
			break;
		case 2:
			xf=x;
			yf=y;
			depf=dep;
			typef=2;
			break;
	}
}

void GEMultiIntersect(int xm, int ym, int depm, int thetam, BinaryStream &bs, int curvem, int xd, int yd, int depd, int nr, int nc, int ver, int &ncover1, int &ncover2, int &thetad)
{
	int r,c,oor,ooc;
	int mindist,maxdist;
	double dr,dc,munder,thetarc,sind,cosd,b,r2,c2;

	int w = (1<<depm);
	int rb = xm;
	int re = min(xm + w-1, nr-1);
	int cb = ym;
	int ce = min(ym + w-1, nc-1);

	int wd = (1<<depd);
	int rbd = xd;
	int red = min(xd + wd-1, nr-1);
	int cbd = yd;
	int ced = min(yd + wd-1, nc-1);

	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	if (thetam<re - rb)
	{
		oor = thetam + rb;
		ooc = cb;
	}
	else
	{
		oor = re;
		ooc = thetam - (re - rb) + cb;
	}
	
	ncover1 = ncover2 = 0;

	// calculate distance to determine lines
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr),abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
		for (r = rbd; r <= red; r++)
		{
			for (c = cbd; c <= ced; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c]>=mindist && Dist[r][c]<=maxdist)
				{
					if (bs.State(Dist[r][c]-mindist)==true)
						ncover1++;
					else
						ncover2++;
				}
			}
		}
	}
	else if (ver == 3)
	{		
		thetarc = atan2(dc,dr);
		sind = sin(-thetarc);
		cosd = cos(-thetarc);
		munder = max(abs(sind),abs(cosd));
		b = max(re-rb+1,ce-cb+1);
		b = curvem / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r-oor)+sind*(c-ooc);
				c2 = -sind*(r-oor)+cosd*(c-ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder)-0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	
	double drd,dcd,drd2,dcd2;

	int crd = rbd + (red - rbd) / 2;
	int ccd = cbd + (ced - cbd + 1) / 2;
	thetad = 0;
	drd = crd - rbd;
	dcd = ccd - cbd;
	for (r = rbd+1; r < red; r++)
	{
		drd2 = crd-r;
		dcd2 = ccd-cbd;
		if (abs(dcd2*crd*drd-ccd*drd*drd2) < abs(dcd*crd*drd2-ccd*drd*drd2))
		{
			thetad = r-rb;
			drd = crd - r;
			dcd = ccd - cbd;
		}
	}
	for (c = cbd; c <= ced; c++)
	{
		drd2 = crd-red;
		dcd2 = ccd-c;
		if (abs(dcd2*crd*drd-ccd*drd*drd2) < abs(dcd*crd*drd2-ccd*drd*drd2))
		{
			thetad = red - rbd + c - cbd;
			drd = crd - red;
			dcd = ccd - c;
		}
	}
}
void GEncoderPrediction1Sequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int ver, int graybit, MemoryFile mems[])
{
	int w2, wr, wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	int predict, predict1, predict2, px01, px10, px11;
	int x10,y10,x01,y01,x11,y11;
	int dep01,dep10,dep11;
	int type01,type10,type11;
	int ncover011,ncover012,ncover101,ncover102,ncover111,ncover112;
	int thetap01,thetap10,thetap11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		GEncoderPrediction1Sequence(x, y, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		GEncoderPrediction1Sequence(x, y + w2, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		GEncoderPrediction1Sequence(x + w2, y, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		GEncoderPrediction1Sequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x-1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y-1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x-1, y-1, nr, nc, x11, y11, dep11, type11);

		if (type10==2)
			GEMultiIntersect(x10,y10,dep10,DepWj[x10][y10][dep10],DepWbs[x10][y10][dep10],0,x,y,dep,nr,nc,ver,ncover101,ncover102,thetap10);
		if (type01==2)
			GEMultiIntersect(x01,y01,dep01,DepWj[x01][y01][dep01],DepWbs[x01][y01][dep01],0,x,y,dep,nr,nc,ver,ncover011,ncover012,thetap01);
		if (type11==2)
			GEMultiIntersect(x11,y11,dep11,DepWj[x11][y11][dep11],DepWbs[x11][y11][dep11],0,x,y,dep,nr,nc,ver,ncover111,ncover112,thetap11);

		//encoding gray
		predict = 0;
		if (dep==0)
		{
			if (x>0 && y>0)
			{
				px10 = imgwrd[x - 1][y];
				px01 = imgwrd[x][y - 1];
				px11 = imgwrd[x - 1][y - 1];
				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;
			}
			else if (x>0)
			{
				predict = imgwrd[x-1][y];
			}
			else if (y>0)
			{
				predict = imgwrd[x][y-1];
			}
		}
		else
		{
			if (type01==0 && type10==0)
			{
				predict = 0;
			}
			else if (type01==0 && type10==1)
			{
				// DONE
				predict = DepCm[x10][y10][dep10];
			}
			else if (type01 == 0 && type10 == 2 && ncover101 + ncover102>0)
			{
				predict = (int)round((DepWm1[x10][y10][dep10]*ncover101 + DepWm2[x10][y10][dep10]*ncover102)/((double)(ncover101+ncover102)));
			}
			else if (type01==1 && type10==0)
			{
				// DONE
				predict = DepCm[x01][y01][dep01];
			}
			else if (type01 == 2 && type10 == 0 && ncover011 + ncover012>0)
			{
				predict = (int)round((DepWm1[x01][y01][dep01]*ncover011 + DepWm2[x01][y01][dep01]*ncover012)/((double)(ncover011+ncover012)));
			}
			//else if (type01==1 && type10==1 && type11==1)
			//{
			//	px10 = DepCm[x10][y10][dep10];
			//	px01 = DepCm[x01][y01][dep01];
			//	px11 = DepCm[x11][y11][dep11];
			//	if (px11 >= max(px10, px01))
			//		predict = min(px10, px01);
			//	else if (px11 <= min(px10, px01))
			//		predict = max(px10, px01);
			//	else
			//		predict = px10 + px01 - px11;
			//}
			else
			{
				if (type01==1)
					px01 = DepCm[x01][y01][dep01];
				else if (type01==2 && ncover011 + ncover012>0)
					px01 = (int)round((DepWm1[x01][y01][dep01]*ncover011 + DepWm2[x01][y01][dep01]*ncover012)/((double)(ncover011+ncover012)));
				else
					px01 = imgwrd[x][y-1];

				if (type10==1)
					px10 = DepCm[x10][y10][dep10];
				else if (type10==2 && ncover101+ncover102>0)
					px10 = (int)round((DepWm1[x10][y10][dep10]*ncover101 + DepWm2[x10][y10][dep10]*ncover102)/((double)(ncover101+ncover102)));
				else
					px10 = imgwrd[x-1][y];
				
				if (type11==1)
					px11 = DepCm[x11][y11][dep11];
				else if (type11==2 && ncover111+ncover112>0)
					px11 = (int)round((DepWm1[x11][y11][dep11]*ncover111 + DepWm2[x11][y11][dep11]*ncover112)/((double)(ncover111+ncover112)));
				else
					px11 = imgwrd[x-1][y-1];

				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;				
			}
		}
		if (abs(DepCm[x][y][dep] - predict) > 100)
			predict1 = predict;
		NumSequence[0][NumSequenceCount[0]++] = (DepCm[x][y][dep] >> (8 - graybit)) - (predict >> (8 - graybit));
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x-1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y-1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x-1, y-1, nr, nc, x11, y11, dep11, type11);

		if (type10==2)
			GEMultiIntersect(x10,y10,dep10,DepWj[x10][y10][dep10],DepWbs[x10][y10][dep10],0,x,y,dep,nr,nc,ver,ncover101,ncover102,thetap10);
		if (type01==2)
			GEMultiIntersect(x01,y01,dep01,DepWj[x01][y01][dep01],DepWbs[x01][y01][dep01],0,x,y,dep,nr,nc,ver,ncover011,ncover012,thetap01);
		if (type11==2)
			GEMultiIntersect(x11,y11,dep11,DepWj[x11][y11][dep11],DepWbs[x11][y11][dep11],0,x,y,dep,nr,nc,ver,ncover111,ncover112,thetap11);

		int sm=0;
		if (type01==2 && ncover011+ncover012>sm)
		{
			if (ncover011==0)
				predict1 = predict2 = DepWm2[x01][y01][dep01];
			else if (ncover012==0)
				predict1 = predict2 = DepWm1[x01][y01][dep01];
			else
			{
				predict1 = DepWm1[x01][y01][dep01];
				predict2 = DepWm2[x01][y01][dep01];
			}
			sm = ncover011+ncover012;
		}
		if (type10==2 && ncover101+ncover102>sm)
		{
			if (ncover101==0)
				predict1 = predict2 = DepWm2[x10][y10][dep10];
			else if (ncover102==0)
				predict1 = predict2 = DepWm1[x10][y10][dep10];
			else
			{
				predict1 = DepWm1[x10][y10][dep10];
				predict2 = DepWm2[x10][y10][dep10];
			}
			sm = ncover101+ncover102;
		}
		if (type11==2 && ncover111+ncover112>sm)
		{
			if (ncover111==0)
				predict1 = predict2 = DepWm2[x11][y11][dep11];
			else if (ncover112==0)
				predict1 = predict2 = DepWm1[x11][y11][dep11];
			else
			{
				predict1 = DepWm1[x11][y11][dep11];
				predict2 = DepWm2[x11][y11][dep11];
			}
			sm = ncover111+ncover112;
		}		
		if (sm==0)
		{			
			if (type10==0 && type01==0)
			{
				predict1 = predict2 = 0;
			}
			else
			{
				int minp=255,maxp=0;
				if (type01>0)
				{
					minp = min(minp,imgwrd[x][y-1]);
					maxp = max(maxp,imgwrd[x][y-1]);
				}
				if (type10>0)
				{
					minp = min(minp,imgwrd[x-1][y]);
					maxp = max(maxp,imgwrd[x-1][y]);
				}
				if (type11>0)
				{
					minp = min(minp,imgwrd[x-1][y-1]);
					maxp = max(maxp,imgwrd[x-1][y-1]);
				}
				predict1 = minp;
				predict2 = maxp;
			}

		}		

		if (abs(DepWm1[x][y][dep] - predict1) + abs(DepWm2[x][y][dep] - predict2) <=
			abs(DepWm2[x][y][dep] - predict1) + abs(DepWm1[x][y][dep] - predict2))
		{
			cgrayswap = false;
			NumSequence[1][NumSequenceCount[1]++] = (DepWm1[x][y][dep] >> (8 - graybit)) - (predict1 >> (8 - graybit));
			NumSequence[2][NumSequenceCount[2]++] = (DepWm2[x][y][dep] >> (8 - graybit)) - (predict2 >> (8 - graybit));
		}
		else
		{
			cgrayswap = true;
			NumSequence[1][NumSequenceCount[1]++] = (DepWm2[x][y][dep] >> (8 - graybit)) - (predict1 >> (8 - graybit));
			NumSequence[2][NumSequenceCount[2]++] = (DepWm1[x][y][dep] >> (8 - graybit)) - (predict2 >> (8 - graybit));
		}

		//if (abs(DepWm1[x][y][dep] - predict1) + abs(DepWm2[x][y][dep] - predict2) <=
		//	abs(DepWm2[x][y][dep] - predict1) + abs(DepWm1[x][y][dep] - predict2))
		//{
		//	cgrayswap = false;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep] - predict1;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep] - predict2;
		//}
		//else
		//{
		//	cgrayswap = true;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep] - predict1;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep] - predict2;
		//}

		//encoding gray		
		//if (DepWm1[x][y][dep] > DepWm2[x][y][dep])
		//{
		//	cgrayswap = false;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm2[x][y][dep];
		//}
		//else
		//{
		//	cgrayswap = true;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm1[x][y][dep];
		//}

		//encoding theta & codebits
		wr = min(w, nr - x);
		wc = min(w, nc - y);

		thetabits = (int)ceil(log((double)(wr + wc - 1)) / log(2.0));
		codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

		mems[0].WriteBits(DepWj[x][y][dep], thetabits);
		for (i = 0; i<codedlinebits; i++)
			mems[0].WriteBit((DepWbs[x][y][dep].State(i) ^ cgrayswap) ? '1' : '0');

		break;
	}
}
int GEncoderPrediction1Sequence(int nr, int nc, int ver, int graybit, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			GEncoderPrediction1Sequence(i, j, nr, nc, indepthw, indepth, dep, ver, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0; i<4; i++)
		raten += GCodeSequence(i);
	return raten;
}

void GEncoderPrediction2Sequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int ver, MemoryFile mems[])
{
	int w2, wr, wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	int predict, predict1, predict2, px01, px10, px11;
	int x10, y10, x01, y01, x11, y11;
	int dep01, dep10, dep11;
	int type01, type10, type11;
	int ncover011, ncover012, ncover101, ncover102, ncover111, ncover112;
	int thetap01, thetap10, thetap11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		GEncoderPrediction2Sequence(x, y, nr, nc, w2, dep - 1, deporg, ver, mems);
		GEncoderPrediction2Sequence(x, y + w2, nr, nc, w2, dep - 1, deporg, ver, mems);
		GEncoderPrediction2Sequence(x + w2, y, nr, nc, w2, dep - 1, deporg, ver, mems);
		GEncoderPrediction2Sequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, ver, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x - 1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y - 1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x - 1, y - 1, nr, nc, x11, y11, dep11, type11);

		if (type10 == 2)
			GEMultiIntersect(x10, y10, dep10, DepWj[x10][y10][dep10], DepWbs[x10][y10][dep10], 0, x, y, dep, nr, nc, ver, ncover101, ncover102, thetap10);
		if (type01 == 2)
			GEMultiIntersect(x01, y01, dep01, DepWj[x01][y01][dep01], DepWbs[x01][y01][dep01], 0, x, y, dep, nr, nc, ver, ncover011, ncover012, thetap01);
		if (type11 == 2)
			GEMultiIntersect(x11, y11, dep11, DepWj[x11][y11][dep11], DepWbs[x11][y11][dep11], 0, x, y, dep, nr, nc, ver, ncover111, ncover112, thetap11);

		//encoding gray
		predict = 0;
		if (dep == 0)
		{
			if (x>0 && y>0)
			{
				px10 = imgwrd[x - 1][y];
				px01 = imgwrd[x][y - 1];
				px11 = imgwrd[x - 1][y - 1];
				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;
			}
			else if (x>0)
			{
				predict = imgwrd[x - 1][y];
			}
			else if (y>0)
			{
				predict = imgwrd[x][y - 1];
			}
		}
		else
		{
			if (type01 == 0 && type10 == 0)
			{
				predict = 0;
			}
			else if (type01 == 0 && type10 == 1)
			{
				// DONE
				predict = DepCm[x10][y10][dep10];
			}
			else if (type01 == 0 && type10 == 2)
			{
				predict = (int)round((DepWm1[x10][y10][dep10] * ncover101 + DepWm2[x10][y10][dep10] * ncover102) / ((double)(ncover101 + ncover102)));
			}
			else if (type01 == 1 && type10 == 0)
			{
				// DONE
				predict = DepCm[x01][y01][dep01];
			}
			else if (type01 == 2 && type10 == 0)
			{
				predict = (int)round((DepWm1[x01][y01][dep01] * ncover011 + DepWm2[x01][y01][dep01] * ncover012) / ((double)(ncover011 + ncover012)));
			}
			//else if (type01==1 && type10==1 && type11==1)
			//{
			//	px10 = DepCm[x10][y10][dep10];
			//	px01 = DepCm[x01][y01][dep01];
			//	px11 = DepCm[x11][y11][dep11];
			//	if (px11 >= max(px10, px01))
			//		predict = min(px10, px01);
			//	else if (px11 <= min(px10, px01))
			//		predict = max(px10, px01);
			//	else
			//		predict = px10 + px01 - px11;
			//}
			else
			{
				if (type01 == 1)
					px01 = DepCm[x01][y01][dep01];
				else if (ncover011 + ncover012>0)
					px01 = (int)round((DepWm1[x01][y01][dep01] * ncover011 + DepWm2[x01][y01][dep01] * ncover012) / ((double)(ncover011 + ncover012)));
				else
					px01 = imgwrd[x][y - 1];

				if (type10 == 1)
					px10 = DepCm[x10][y10][dep10];
				else if (ncover101 + ncover102>0)
					px10 = (int)round((DepWm1[x10][y10][dep10] * ncover101 + DepWm2[x10][y10][dep10] * ncover102) / ((double)(ncover101 + ncover102)));
				else
					px10 = imgwrd[x - 1][y];

				if (type11 == 1)
					px11 = DepCm[x11][y11][dep11];
				else if (ncover111 + ncover112>0)
					px11 = (int)round((DepWm1[x11][y11][dep11] * ncover111 + DepWm2[x11][y11][dep11] * ncover112) / ((double)(ncover111 + ncover112)));
				else
					px11 = imgwrd[x - 1][y - 1];

				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;
			}
		}
		NumSequence[0][NumSequenceCount[0]++] = DepCm[x][y][dep] - predict;
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x - 1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y - 1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x - 1, y - 1, nr, nc, x11, y11, dep11, type11);

		if (type10 == 2)
			GEMultiIntersect(x10, y10, dep10, DepWj[x10][y10][dep10], DepWbs[x10][y10][dep10], 0, x, y, dep, nr, nc, ver, ncover101, ncover102, thetap10);
		if (type01 == 2)
			GEMultiIntersect(x01, y01, dep01, DepWj[x01][y01][dep01], DepWbs[x01][y01][dep01], 0, x, y, dep, nr, nc, ver, ncover011, ncover012, thetap01);
		if (type11 == 2)
			GEMultiIntersect(x11, y11, dep11, DepWj[x11][y11][dep11], DepWbs[x11][y11][dep11], 0, x, y, dep, nr, nc, ver, ncover111, ncover112, thetap11);

		int sm = 0;
		if (type01 == 2 && ncover011 + ncover012>sm)
		{
			if (ncover011 == 0)
				predict1 = predict2 = DepWm2[x01][y01][dep01];
			else if (ncover012 == 0)
				predict1 = predict2 = DepWm1[x01][y01][dep01];
			else
			{
				predict1 = DepWm1[x01][y01][dep01];
				predict2 = DepWm2[x01][y01][dep01];
			}
			sm = ncover011 + ncover012;
		}
		if (type10 == 2 && ncover101 + ncover102>sm)
		{
			if (ncover101 == 0)
				predict1 = predict2 = DepWm2[x10][y10][dep10];
			else if (ncover102 == 0)
				predict1 = predict2 = DepWm1[x10][y10][dep10];
			else
			{
				predict1 = DepWm1[x10][y10][dep10];
				predict2 = DepWm2[x10][y10][dep10];
			}
			sm = ncover101 + ncover102;
		}
		if (type11 == 2 && ncover111 + ncover112>sm)
		{
			if (ncover111 == 0)
				predict1 = predict2 = DepWm2[x11][y11][dep11];
			else if (ncover112 == 0)
				predict1 = predict2 = DepWm1[x11][y11][dep11];
			else
			{
				predict1 = DepWm1[x11][y11][dep11];
				predict2 = DepWm2[x11][y11][dep11];
			}
			sm = ncover111 + ncover112;
		}
		if (sm == 0)
		{
			if (type10 == 0 && type01 == 0)
			{
				predict1 = predict2 = 0;
			}
			else
			{
				int minp = 255, maxp = 0;
				if (type01>0)
				{
					minp = min(minp, imgwrd[x][y - 1]);
					maxp = max(maxp, imgwrd[x][y - 1]);
				}
				if (type10>0)
				{
					minp = min(minp, imgwrd[x - 1][y]);
					maxp = max(maxp, imgwrd[x - 1][y]);
				}
				if (type11>0)
				{
					minp = min(minp, imgwrd[x - 1][y - 1]);
					maxp = max(maxp, imgwrd[x - 1][y - 1]);
				}
				predict1 = minp;
				predict2 = maxp;
			}

		}

		if (abs(DepWm1[x][y][dep] - predict1) + abs(DepWm2[x][y][dep] - predict2) <=
			abs(DepWm2[x][y][dep] - predict1) + abs(DepWm1[x][y][dep] - predict2))
		{
			cgrayswap = false;
			NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep] - predict1;
			NumSequence[2][NumSequenceCount[2]++] = DepWm2[x][y][dep] - predict2;
		}
		else
		{
			cgrayswap = true;
			NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep] - predict1;
			NumSequence[2][NumSequenceCount[2]++] = DepWm1[x][y][dep] - predict2;
		}

		//encoding gray		
		//if (DepWm1[x][y][dep] > DepWm2[x][y][dep])
		//{
		//	cgrayswap = false;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm2[x][y][dep];
		//}
		//else
		//{
		//	cgrayswap = true;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm1[x][y][dep];
		//}

		//encoding theta & codebits
		wr = min(w, nr - x);
		wc = min(w, nc - y);

		thetabits = (int)ceil(log((double)(wr + wc - 1)) / log(2.0));
		codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

		mems[0].WriteBits(DepWj[x][y][dep], thetabits);
		for (i = 0; i<codedlinebits; i++)
			mems[0].WriteBit((DepWbs[x][y][dep].State(i) ^ cgrayswap) ? '1' : '0');

		break;
	}
}
int GEncoderPrediction2Sequence(int nr, int nc, int ver, int graybit, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = 8; NumSequenceBit[1] = 8; NumSequenceBit[2] = 8; NumSequenceBit[3] = 8;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			GEncoderPrediction2Sequence(i, j, nr, nc, indepthw, indepth, dep, ver, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0; i<4; i++)
		raten += GCodeSequence(i);
	return raten;
}

int GeneralEncoder(int nr, int nc, int btver, int graybit, char filename[])
{
	int bestrate = (1<<30);
	int rate;

	if (ShowMessages) printf("\n==========================================\n\n");
	if (ShowMessages) printf("\n###########\n GEncoderNormalSequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount,0,sizeof(NumSequenceCount));
	rate = GEncoderNormalSequence(nr,nc,graybit,mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	if (ShowMessages) printf("\n###########\n GEncoderLocoISequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount,0,sizeof(NumSequenceCount));
	rate = GEncoderLocoISequence(nr,nc,graybit,mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	if (ShowMessages) printf("\n###########\n GEncoderPrediction1Sequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	rate = GEncoderPrediction1Sequence(nr, nc, btver, graybit, mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	//printf("\n###########\n GEncoderPrediction2Sequence \n");
	//mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	//memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	//rate = GEncoderPrediction2Sequence(nr, nc, btver, mems);
	//if (rate<bestrate)
	//	bestrate = rate;
	//printf("C-RATE : %d\n", rate);

	return bestrate+2;
}

void Decoder(char filename[])
{
	int version;
	BinaryFile bf(filename);
	bf.OpenRead();
	version = bf.Read1Byte();
	switch(version)
	{
		case 1:
			CodecV1Decode(bf);
			break;
		case 2:
			CodecV2Decode(bf);
			break;
		case 8:
			CodecV8Decode(bf);
	}
	bf.Close();
}

bool CheckEncodeDecode(int nr, int nc)
{
	int i,j;
	for (i=0;i<nr;i++)
		for (j=0;j<nc;j++)
			if (imgprd[i][j]!=rimg[i][j])
			{
				//printf("Check FAIL : (%d,%d)\n",i,j);
				return false;
			}
	//printf("Check Successfull\n");
	return true;
}

void GenerateRdType(int x, int y, int nr, int nc, int w2, int w, int dep)
{
	int i,j,wr,wc;
	int bestm1,bestm2;
	int xi,yi,xj,yj,xi1,yi1,xi2,yi2,xi3,yi3,xi4,yi4;
	bool sgn,sgn1,sgn2;
	if (x>=nr || y>=nc) return;
	wr = min(w,nr-x);
	wc = min(w,nc-y);
	switch(OptS[x][y][dep])
	{
		case 0:
			GenerateRdType(x,y,nr,nc,w2/2,w/2,dep-1);
			GenerateRdType(x,y+w2,nr,nc,w2/2,w/2,dep-1);
			GenerateRdType(x+w2,y,nr,nc,w2/2,w/2,dep-1);
			GenerateRdType(x+w2,y+w2,nr,nc,w2/2,w/2,dep-1);
			break;
		case 1:
			for (i=0;i<wr;i++)
				for (j=0;j<wc;j++)
					imgprdg[x+i][y+j]=1;
			break;
		case 2:
			wr = min(w,nr-x);
			wc = min(w,nc-y);			
			bestm1 = (DepWm1[x][y][dep]<DepWm2[x][y][dep])?2:3;
			bestm2 = 5-bestm1;
			GetXY(wr,wc,DepWi[x][y][dep],xi,yi);
			GetXY(wr,wc,DepWj[x][y][dep],xj,yj);
			for (i=0;i<wr;i++){
				for (j=0;j<wc;j++){
					sgn = ((j-yi)*(xj-xi)-(i-xi)*(yj-yi)) >= 0;
					if (sgn)
						imgprdg[x+i][y+j] = bestm1;
					else
						imgprdg[x+i][y+j] = bestm2;
				}
			}

			break;
		case 3:
			wr = min(w,nr-x);
			wc = min(w,nc-y);			
			bestm1 = (DepPm1[x][y][dep]<DepPm2[x][y][dep])?4:5;
			bestm2 = 9-bestm1;
			GetXY(wr,wc,DepPi1[x][y][dep],xi1,yi1);
			GetXY(wr,wc,DepPi2[x][y][dep],xi2,yi2);
			GetXY(wr,wc,DepPi3[x][y][dep],xi3,yi3);
			GetXY(wr,wc,DepPi4[x][y][dep],xi4,yi4);
			GetXY(wr,wc,DepWj[x][y][dep],xj,yj);
			for (i=0;i<wr;i++){
				for (j=0;j<wc;j++){
					sgn1 = ((j-yi1)*(xi4-xi1)-(i-xi1)*(yi4-yi1)) >= 0;
					sgn2 = ((j-yi2)*(xi3-xi2)-(i-xi2)*(yi3-yi2)) >= 0;
					if (sgn1 ^ sgn2)
						imgprdg[x+i][y+j] = bestm1;
					else
						imgprdg[x+i][y+j] = bestm2;
				}
			}
			break;
	}
}

void GenerateRdType(int nr, int nc)
{
	int dep,w,w2;
	for (dep=1,w=2,w2=1;w<=max(nr,nc);w*=2,w2*=2,dep++);
	GenerateRdType(0,0,nr,nc,w2,w,dep);
}

//-----------------------------------------------------

void WEncoderPrediction1Sequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int ver, int graybit, MemoryFile mems[])
{
	int w2, wr, wc;
	bool cgrayswap;
	int thetabits, codedlinebits;
	int i;
	int predict, predict1, predict2, px01, px10, px11;
	int x10, y10, x01, y01, x11, y11;
	int dep01, dep10, dep11;
	int type01, type10, type11;
	int ncover011, ncover012, ncover101, ncover102, ncover111, ncover112;
	int thetap01, thetap10, thetap11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		WEncoderPrediction1Sequence(x, y, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		WEncoderPrediction1Sequence(x, y + w2, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		WEncoderPrediction1Sequence(x + w2, y, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		WEncoderPrediction1Sequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, ver, graybit, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x - 1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y - 1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x - 1, y - 1, nr, nc, x11, y11, dep11, type11);

		if (type10 == 2)
			GEMultiIntersect(x10, y10, dep10, DepWj[x10][y10][dep10], DepWbs[x10][y10][dep10], 0, x, y, dep, nr, nc, ver, ncover101, ncover102, thetap10);
		if (type01 == 2)
			GEMultiIntersect(x01, y01, dep01, DepWj[x01][y01][dep01], DepWbs[x01][y01][dep01], 0, x, y, dep, nr, nc, ver, ncover011, ncover012, thetap01);
		if (type11 == 2)
			GEMultiIntersect(x11, y11, dep11, DepWj[x11][y11][dep11], DepWbs[x11][y11][dep11], 0, x, y, dep, nr, nc, ver, ncover111, ncover112, thetap11);

		//encoding gray
		predict = 0;
		if (dep == 0)
		{
			if (x>0 && y>0)
			{
				px10 = imgwrd[x - 1][y];
				px01 = imgwrd[x][y - 1];
				px11 = imgwrd[x - 1][y - 1];
				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;
			}
			else if (x>0)
			{
				predict = imgwrd[x - 1][y];
			}
			else if (y>0)
			{
				predict = imgwrd[x][y - 1];
			}
		}
		else
		{
			if (type01 == 0 && type10 == 0)
			{
				predict = 0;
			}
			else if (type01 == 0 && type10 == 1)
			{
				// DONE
				predict = DepCm[x10][y10][dep10];
			}
			else if (type01 == 0 && type10 == 2 && ncover101 + ncover102>0)
			{
				predict = (int)round((DepWm1[x10][y10][dep10] * ncover101 + DepWm2[x10][y10][dep10] * ncover102) / ((double)(ncover101 + ncover102)));
			}
			else if (type01 == 1 && type10 == 0)
			{
				// DONE
				predict = DepCm[x01][y01][dep01];
			}
			else if (type01 == 2 && type10 == 0 && ncover011 + ncover012>0)
			{
				predict = (int)round((DepWm1[x01][y01][dep01] * ncover011 + DepWm2[x01][y01][dep01] * ncover012) / ((double)(ncover011 + ncover012)));
			}
			//else if (type01==1 && type10==1 && type11==1)
			//{
			//	px10 = DepCm[x10][y10][dep10];
			//	px01 = DepCm[x01][y01][dep01];
			//	px11 = DepCm[x11][y11][dep11];
			//	if (px11 >= max(px10, px01))
			//		predict = min(px10, px01);
			//	else if (px11 <= min(px10, px01))
			//		predict = max(px10, px01);
			//	else
			//		predict = px10 + px01 - px11;
			//}
			else
			{
				if (type01 == 1)
					px01 = DepCm[x01][y01][dep01];
				else if (type01 == 2 && ncover011 + ncover012>0)
					px01 = (int)round((DepWm1[x01][y01][dep01] * ncover011 + DepWm2[x01][y01][dep01] * ncover012) / ((double)(ncover011 + ncover012)));
				else
					px01 = imgwrd[x][y - 1];

				if (type10 == 1)
					px10 = DepCm[x10][y10][dep10];
				else if (type10 == 2 && ncover101 + ncover102>0)
					px10 = (int)round((DepWm1[x10][y10][dep10] * ncover101 + DepWm2[x10][y10][dep10] * ncover102) / ((double)(ncover101 + ncover102)));
				else
					px10 = imgwrd[x - 1][y];

				if (type11 == 1)
					px11 = DepCm[x11][y11][dep11];
				else if (type11 == 2 && ncover111 + ncover112>0)
					px11 = (int)round((DepWm1[x11][y11][dep11] * ncover111 + DepWm2[x11][y11][dep11] * ncover112) / ((double)(ncover111 + ncover112)));
				else
					px11 = imgwrd[x - 1][y - 1];

				if (px11 >= max(px10, px01))
					predict = min(px10, px01);
				else if (px11 <= min(px10, px01))
					predict = max(px10, px01);
				else
					predict = px10 + px01 - px11;
			}
		}
		if (abs(DepCm[x][y][dep] - predict) > 100)
			predict1 = predict;
		NumSequence[0][NumSequenceCount[0]++] = (DepCm[x][y][dep] >> (8 - graybit)) - (predict >> (8 - graybit));
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		// find neighbours
		GEFindNeighbour(0, 0, deporg, x - 1, y, nr, nc, x10, y10, dep10, type10);
		GEFindNeighbour(0, 0, deporg, x, y - 1, nr, nc, x01, y01, dep01, type01);
		GEFindNeighbour(0, 0, deporg, x - 1, y - 1, nr, nc, x11, y11, dep11, type11);

		if (type10 == 2)
			GEMultiIntersect(x10, y10, dep10, DepWj[x10][y10][dep10], DepWbs[x10][y10][dep10], 0, x, y, dep, nr, nc, ver, ncover101, ncover102, thetap10);
		if (type01 == 2)
			GEMultiIntersect(x01, y01, dep01, DepWj[x01][y01][dep01], DepWbs[x01][y01][dep01], 0, x, y, dep, nr, nc, ver, ncover011, ncover012, thetap01);
		if (type11 == 2)
			GEMultiIntersect(x11, y11, dep11, DepWj[x11][y11][dep11], DepWbs[x11][y11][dep11], 0, x, y, dep, nr, nc, ver, ncover111, ncover112, thetap11);

		int sm = 0;
		if (type01 == 2 && ncover011 + ncover012>sm)
		{
			if (ncover011 == 0)
				predict1 = predict2 = DepWm2[x01][y01][dep01];
			else if (ncover012 == 0)
				predict1 = predict2 = DepWm1[x01][y01][dep01];
			else
			{
				predict1 = DepWm1[x01][y01][dep01];
				predict2 = DepWm2[x01][y01][dep01];
			}
			sm = ncover011 + ncover012;
		}
		if (type10 == 2 && ncover101 + ncover102>sm)
		{
			if (ncover101 == 0)
				predict1 = predict2 = DepWm2[x10][y10][dep10];
			else if (ncover102 == 0)
				predict1 = predict2 = DepWm1[x10][y10][dep10];
			else
			{
				predict1 = DepWm1[x10][y10][dep10];
				predict2 = DepWm2[x10][y10][dep10];
			}
			sm = ncover101 + ncover102;
		}
		if (type11 == 2 && ncover111 + ncover112>sm)
		{
			if (ncover111 == 0)
				predict1 = predict2 = DepWm2[x11][y11][dep11];
			else if (ncover112 == 0)
				predict1 = predict2 = DepWm1[x11][y11][dep11];
			else
			{
				predict1 = DepWm1[x11][y11][dep11];
				predict2 = DepWm2[x11][y11][dep11];
			}
			sm = ncover111 + ncover112;
		}
		if (sm == 0)
		{
			if (type10 == 0 && type01 == 0)
			{
				predict1 = predict2 = 0;
			}
			else
			{
				int minp = 255, maxp = 0;
				if (type01>0)
				{
					minp = min(minp, imgwrd[x][y - 1]);
					maxp = max(maxp, imgwrd[x][y - 1]);
				}
				if (type10>0)
				{
					minp = min(minp, imgwrd[x - 1][y]);
					maxp = max(maxp, imgwrd[x - 1][y]);
				}
				if (type11>0)
				{
					minp = min(minp, imgwrd[x - 1][y - 1]);
					maxp = max(maxp, imgwrd[x - 1][y - 1]);
				}
				predict1 = minp;
				predict2 = maxp;
			}

		}

		if (abs(DepWm1[x][y][dep] - predict1) + abs(DepWm2[x][y][dep] - predict2) <=
			abs(DepWm2[x][y][dep] - predict1) + abs(DepWm1[x][y][dep] - predict2))
		{
			cgrayswap = false;
			NumSequence[1][NumSequenceCount[1]++] = (DepWm1[x][y][dep] >> (8 - graybit)) - (predict1 >> (8 - graybit));
			NumSequence[2][NumSequenceCount[2]++] = (DepWm2[x][y][dep] >> (8 - graybit)) - (predict2 >> (8 - graybit));
		}
		else
		{
			cgrayswap = true;
			NumSequence[1][NumSequenceCount[1]++] = (DepWm2[x][y][dep] >> (8 - graybit)) - (predict1 >> (8 - graybit));
			NumSequence[2][NumSequenceCount[2]++] = (DepWm1[x][y][dep] >> (8 - graybit)) - (predict2 >> (8 - graybit));
		}

		//if (abs(DepWm1[x][y][dep] - predict1) + abs(DepWm2[x][y][dep] - predict2) <=
		//	abs(DepWm2[x][y][dep] - predict1) + abs(DepWm1[x][y][dep] - predict2))
		//{
		//	cgrayswap = false;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep] - predict1;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep] - predict2;
		//}
		//else
		//{
		//	cgrayswap = true;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep] - predict1;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep] - predict2;
		//}

		//encoding gray		
		//if (DepWm1[x][y][dep] > DepWm2[x][y][dep])
		//{
		//	cgrayswap = false;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm1[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm2[x][y][dep];
		//}
		//else
		//{
		//	cgrayswap = true;
		//	NumSequence[1][NumSequenceCount[1]++] = DepWm2[x][y][dep];
		//	NumSequence[2][NumSequenceCount[2]++] = DepWm1[x][y][dep];
		//}

		//encoding theta & codebits
		wr = min(w, nr - x);
		wc = min(w, nc - y);

		thetabits = (int)ceil(log((double)(wr + wc - 1)) / log(2.0));
		codedlinebits = DepWr[x][y][dep] - 2 * GRAYBIT - thetabits;

		mems[0].WriteBits(DepWj[x][y][dep], thetabits);
		for (i = 0; i<codedlinebits; i++)
			mems[0].WriteBit((DepWbs[x][y][dep].State(i) ^ cgrayswap) ? '1' : '0');

		break;
	}
}
int WEncoderPrediction1Sequence(int nr, int nc, int ver, int graybit, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			WEncoderPrediction1Sequence(i, j, nr, nc, indepthw, indepth, dep, ver, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0; i<4; i++)
		raten += GCodeSequence(i);
	return raten;
}


void WEncoderLocoISequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, MemoryFile mems[])
{
	int w2;
	bool cgrayswap;
	int bits;
	int predict, px01, px10, px11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		WEncoderLocoISequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderLocoISequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderLocoISequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderLocoISequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		//encoding gray
		if (x>0 && y>0)
		{
			px10 = imgwrd[x - 1][y];
			px01 = imgwrd[x][y - 1];
			px11 = imgwrd[x - 1][y - 1];
			if (px11 >= max(px10, px01))
				predict = min(px10, px01);
			else if (px11 <= min(px10, px01))
				predict = max(px10, px01);
			else
				predict = px10 + px01 - px11;
		}
		else
		{
			predict = 0;
		}
		NumSequence[0][NumSequenceCount[0]++] = (DepCm[x][y][dep] >> (8 - graybit)) - (predict >> (8 - graybit));
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		//encoding gray
		cgrayswap = false;
		NumSequence[1][NumSequenceCount[1]++] = (DepWm1[x][y][dep]) >> (8 - graybit);
		NumSequence[1][NumSequenceCount[1]++] = (DepWm2[x][y][dep]) >> (8 - graybit);

		//encoding theta & codebits
		bits = (DepWr[x][y][dep] - 2 * graybit) >> 1;
		mems[0].WriteBits(DepWi[x][y][dep], bits);
		mems[0].WriteBits(DepWj[x][y][dep], bits);

		break;
	}
}
int WEncoderLocoISequence(int nr, int nc, int graybit, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			WEncoderLocoISequence(i, j, nr, nc, indepthw, indepth, dep, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}


void WEncoderNormalSequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, MemoryFile mems[])
{
	int w2;
	bool cgrayswap;
	int bits;
	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		WEncoderNormalSequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderNormalSequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderNormalSequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit, mems);
		WEncoderNormalSequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		//encoding gray
		NumSequence[0][NumSequenceCount[0]++] = DepCm[x][y][dep] >> (8 - graybit);
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		//encoding gray
		cgrayswap = false;
		NumSequence[0][NumSequenceCount[0]++] = DepWm1[x][y][dep] >> (8 - graybit);
		NumSequence[0][NumSequenceCount[0]++] = DepWm2[x][y][dep] >> (8 - graybit);

		//encoding theta & codebits
		bits = (DepWr[x][y][dep] - 2 * graybit) >> 1;
		mems[0].WriteBits(DepWi[x][y][dep], bits);
		mems[0].WriteBits(DepWj[x][y][dep], bits);
		break;
	}
}

int WEncoderNormalSequence(int nr, int nc, int graybit, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			WEncoderNormalSequence(i, j, nr, nc, indepthw, indepth, dep, graybit, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}

int GeneralEncoderW(int nr, int nc, int btver, int graybit, char filename[])
{
	int bestrate = (1 << 30);
	int rate;

	if (ShowMessages) printf("\n==========================================\n\n");
	if (ShowMessages) printf("\n###########\n WEncoderNormalSequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	rate = WEncoderNormalSequence(nr, nc, graybit, mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	if (ShowMessages) printf("\n###########\n WEncoderLocoISequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	rate = WEncoderLocoISequence(nr, nc, graybit, mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	//if (ShowMessages) printf("\n###########\n WEncoderPrediction1Sequence \n");
	//mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	//memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	//rate = WEncoderPrediction1Sequence(nr, nc, btver, graybit, mems);
	//if (rate<bestrate)
	//	bestrate = rate;
	//if (ShowMessages) printf("C-RATE : %d\n", rate);

	//printf("\n###########\n GEncoderPrediction2Sequence \n");
	//mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	//memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	//rate = GEncoderPrediction2Sequence(nr, nc, btver, mems);
	//if (rate<bestrate)
	//	bestrate = rate;
	//printf("C-RATE : %d\n", rate);

	return bestrate + 2;
}

void StoreParametersW(ofstream &of, int x, int y, int nr, int nc, int w, int dep)
{
	int w2 = w / 2;

	if (x >= nr || y >= nc) return;

	if (OptS[x][y][dep] == 0)
	{
		StoreParametersW(of, x, y, nr, nc, w2, dep - 1);
		StoreParametersW(of, x, y + w2, nr, nc, w2, dep - 1);
		StoreParametersW(of, x + w2, y, nr, nc, w2, dep - 1);
		StoreParametersW(of, x + w2, y + w2, nr, nc, w2, dep - 1);
	}
	else
	{
		of << x << " " << y << " " << min(w, nr - x) << " " << min(w, nc - y) << " " << OptS[x][y][dep] << " ";
		switch (OptS[x][y][dep])
		{
		case 1:
			of << DepCm[x][y][dep];
			break;
		case 2:
			of << DepWm1[x][y][dep] << " " << DepWm1[x][y][dep] << " "
				<< DepWi[x][y][dep] << " " << DepWj[x][y][dep];
			break;
		}
		of << endl;
	}
}

void StoreParametersW(ofstream &of, int nr, int nc)
{
	int dep, w, w2;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	StoreParametersW(of, nr, nc, 0, 0, w, dep);
}

void ResizeImage(int x, int y, int nr, int nc, int downsz, int w, int dep)
{
	int w2 = w / 2;
	if (OptS[x][y][dep] == 0)
	{
		ResizeImage(x, y, nr, nc, downsz, w2, dep - 1);
		ResizeImage(x + w2, y, nr, nc, downsz, w2, dep - 1);
		ResizeImage(x, y + w2, nr, nc, downsz, w2, dep - 1);
		ResizeImage(x + w2, y + w2, nr, nc, downsz, w2, dep - 1);
	}
	else if (OptS[x][y][dep] == 1)
	{
	}
	else if (OptS[x][y][dep] == 2)
	{
	}
}

void ResizeImage(int nr, int nc, int downsz)
{
	int dep, w, w2;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	memset(rimg, 0, sizeof(rimg));
	ResizeImage(0, 0, nr, nc, downsz, w, dep);
}

//######################################################################
//################################## Bar ###############################

void RebuildBlockBar(int rb, int re, int cb, int ce, int dep, int ver, int sscale, int graybit, int theta, int ind1, int ind2, int curve, int g1, int g2)
{
	int wtheta;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	// second line
	int oor, ooc;
	if (theta<re - rb)
	{
		oor = theta + rb;
		ooc = cb;
	}
	else
	{
		oor = re;
		ooc = theta - (re - rb) + cb;
	}

	int woor, wooc;
	int ptheta1 = (re - rb) / sscale;
	if (theta <= ptheta1)
	{
		wtheta = theta*sscale;
		woor = wtheta + rb;
		wooc = cb;
	}
	else
	{
		wtheta = (theta - ptheta1)*sscale;
		woor = re;
		wooc = wtheta + cb;
	}

	// calculate distance to determine lines
	int r, c;
	double dr, dc, munder;
	int mindist, maxdist, wmindist, wmaxdist;
	double thetarc, sind, cosd, b, r2, c2;
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;

	double wdr, wdc;
	wdr = cr - woor;
	wdc = cc - wooc;
	wmindist = wmaxdist = 0;
	//int len = maxdist - mindist + 1;

	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr), abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		thetarc = atan2(dc, dr);
		sind = sin(-thetarc);
		cosd = cos(-thetarc);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 4)
	{
		munder = max(abs(dr), abs(dc))*sscale;
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}

		munder = max(abs(wdr), abs(wdc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				wDist[r][c] = (int)round(((c - wooc)*wdr - (r - woor)*wdc) / munder);
				if (wDist[r][c] < wmindist) wmindist = wDist[r][c];
				if (wDist[r][c] > wmaxdist) wmaxdist = wDist[r][c];
			}
		}
	}

	int wind1 = (int)round(ind1*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));
	int wind2 = (int)round(ind2*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));

	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if ((wDist[r][c] - wmindist) >= wind1 && (wDist[r][c] - wmindist) <= wind2)
				rimg[r][c] = g2;
			else
				rimg[r][c] = g1;
		}
	}

	DepGm1[rb][cb][dep] = 0;
	DepGm2[rb][cb][dep] = 0;

	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if ((Dist[r][c] - mindist) >= ind1 && (Dist[r][c] - mindist) <= ind2)
			{
				imgp[r][c][dep] = g2;
				DepGm2[rb][cb][dep]++;
			}
			else
			{
				imgp[r][c][dep] = g1;
				DepGm1[rb][cb][dep]++;
			}
		}
	}

}

void ComputeBestAssignmentBar(int len, int graybit, long long &bestd, int &bestind1, int &bestind2, int &g1, int &g2, int &area)
{
	int ind1, ind2;
	int sn, ss, sp;
	int n1, n2, s1, s2, p1, p2;
	int c1, c2;
	long long d;
	int pow2rem = 1 << (8 - graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	bestd = (1 << 30);

	sn = 0; ss = 0; sp = 0;
	for (ind1 = 0; ind1 < len; ind1++)
	{
		sn += StatVars[ind1][0];
		ss += StatVars[ind1][1];
		sp += StatVars[ind1][2];
	}

	for (ind1 = 0; ind1 < len; ind1++)
	{
		n1 = sn; s1 = ss; p1 = sp;
		n2 = 0; s2 = 0; p2 = 0;
		for (ind2 = ind1; ind2 < len; ind2++)
		{
			n1 -= StatVars[ind2][0];
			s1 -= StatVars[ind2][1];
			p1 -= StatVars[ind2][2];
			n2 += StatVars[ind2][0];
			s2 += StatVars[ind2][1];
			p2 += StatVars[ind2][2];

			if (n1 > 0)
			{
				c1 = ((int)((double)s1 / n1 / pow2rem))*pow2rem;
				if (c1 > maxgray) c1 = maxgray;
			}
			else
				c1 = 0;
			if (n2 > 0)
			{
				c2 = ((int)((double)s2 / n2 / pow2rem))*pow2rem;
				if (c2 > maxgray) c2 = maxgray;
			}
			else
				c2 = 0;
			d = p1 + n1*c1*c1 - 2 * c1*s1 + p2 + n2*c2*c2 - 2 * c2*s2;

			//cout << w << " , ";
			if (d < bestd)
			{
				bestd = d;
				bestind1 = ind1;
				bestind2 = ind2;
				g1 = c1;
				g2 = c2;
				area = n2;
			}

		}
	}
}

void ComputeBestAssignmentBar2(int len, int graybit, long long &bestd, int &bestind1, int &bestind2, int &g1, int &g2, int &area)
{
	int i;
	int ind1, ind2;
	int sn, ss, sp;
	int n1, n2, s1, s2, p1, p2;
	int c1, c2;
	long long d;
	int pow2rem = 1 << (8 - graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);
	double cent, cc2, sumg;
	int gv;

	bestd = (1 << 30);

	sn = 0; ss = 0; sp = 0;
	for (ind1 = 0; ind1 < len; ind1++)
	{
		sn += StatVars[ind1][0];
		ss += StatVars[ind1][1];
		sp += StatVars[ind1][2];
	}

	for (ind1 = 0; ind1 < len; ind1++)
	{
		n1 = sn; s1 = ss; p1 = sp;
		n2 = 0; s2 = 0; p2 = 0;
		for (ind2 = ind1; ind2 < len; ind2++)
		{
			n1 -= StatVars[ind2][0];
			s1 -= StatVars[ind2][1];
			p1 -= StatVars[ind2][2];
			n2 += StatVars[ind2][0];
			s2 += StatVars[ind2][1];
			p2 += StatVars[ind2][2];

			if (n1 > 0)
			{
				c1 = ((int)((double)s1 / n1 / pow2rem))*pow2rem;
				if (c1 > maxgray) c1 = maxgray;
			}
			else
				c1 = 0;
			if (n2 > 0)
			{
				c2 = ((int)((double)s2 / n2 / pow2rem))*pow2rem;
				if (c2 > maxgray) c2 = maxgray;
			}
			else
				c2 = 0;

			d = p1 + n1*c1*c1 - 2 * c1*s1 +p2 + n2*c2*c2 - 2 * c2*s2;
			if (d < bestd)
			{
				bestd = d;
				bestind1 = ind1;
				bestind2 = ind2;
				g1 = c1;
				g2 = c2;
			}

			d = p1 + n1*c1*c1 - 2 * c1*s1;// +p2 + n2*c2*c2 - 2 * c2*s2;

			cent = (ind1 + ind2) / 2.0;
			cc2 = -(ind2 - ind1 + 1)*(ind2 - ind1 + 1) / log(0.05);
			sumg = 0;
			for (i = ind1;i <= ind2;i++)
			{
				sumg += StatVars[i][0] * exp(-(i - cent)*(i - cent) / cc2);
			}
			c2 = (int)round((s2 - n2*c1) / sumg);			
			c2 = c2 > 255 ? 255 : (c2 < -255 ? -255 : 0);
			for (i = ind1;i <= ind2;i++)
			{
				gv = (int)round(c1 + c2*exp(-(i - cent)*(i - cent) / cc2));
				d += StatVars[i][2] + StatVars[i][0]*gv*gv - 2*StatVars[i][1]*gv;
			}

			//cout << w << " , ";
			if (d < bestd)
			{
				bestd = d;
				bestind1 = ind1;
				bestind2 = ind2;
				g1 = c1;
				g2 = c2;
				area = n2;
			}

		}
	}
}

void ComputeBlockBar(int rb, int re, int cb, int ce, int oor, int ooc, double curve, int ver, int sscale, int graybit, long long &bestd, int &bestind1, int &bestind2, int &g1, int &g2, int &codedlinelength)
{
	int r, c, i, j;
	double dr, dc, munder, b;
	double theta, sind, cosd, r2, c2;
	int mindist, maxdist;
	int area;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	//cout << "orientation [" << or << " : " << oc << "] to [" << cr << " : " << cc << "]";
	if (cr == oor && cc == ooc)
	{
		bestd = (1 << 30);
		//cout << endl;
		return;
	}

	// calculate distance to determine lines
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2 || ver == 12)
	{
		munder = max(abs(dr), abs(dc));		
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 4 || ver == 14)
	{
		munder = max(abs(dr), abs(dc))*sscale;
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		theta = atan2(dc, dr);
		sind = sin(-theta);
		cosd = cos(-theta);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}

	//if (maxdist-mindist>200)
	//	printf("D| %d \n",maxdist-mindist);

	// set statistical variables to zero
	for (i = 0; i <= maxdist - mindist; i++)
		for (j = 0; j < 3; j++)
			StatVars[i][j] = 0;

	// gathering line statistics n,s,p
	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			StatVars[Dist[r][c] - mindist][0]++; // n
			StatVars[Dist[r][c] - mindist][1] += img[r][c]; // s
			StatVars[Dist[r][c] - mindist][2] += img[r][c] * img[r][c]; // p
		}
	}

	codedlinelength = maxdist - mindist + 1;
	//cout << " L " << codedlinelength << endl;
	//for (i = 0; i < codedlinelength; i++)
	//	cout << "State " << i << " [" << StatVars[i][0] << "-" << StatVars[i][1] << "-" << StatVars[i][2] << "-" << (int)round(StatVars[i][1] / (double)StatVars[i][0]) << "]" << endl;
	if (ver == 12 || ver == 14)
		ComputeBestAssignmentBar2(codedlinelength, graybit, bestd, bestind1, bestind2, g1, g2, area);
	else
		ComputeBestAssignmentBar(codedlinelength, graybit, bestd, bestind1, bestind2, g1, g2, area);
	//cout << "Result : " << bestv << " (" << bestvw << ")" << endl;
	//cout << endl << "-------------------------" << endl;
}

void ComputeBlockBar(int rb, int re, int cb, int ce, int ver, int sscale, int graybit, long long &d, int &theta, int &ind1, int &ind2, int &curve, int &g1, int &g2, int &thetalength, int &codedlinelength, int &curvelength)
{
	int r, c;
	int gr1, gr2, codelen, bestind1, bestind2;
	int curveb, curvee, bcurve;
	long long bestd, seld;

	//cout << "ComputeBlock (" << rb << "," << re << "),(" << cb << "," << ce << ")  " << re-rb+1 << "-" << ce-cb+1 << endl;

	if (ver == 3 && max(re - rb + 1, ce - cb + 1) >= 8)
	{
		curvelength = (1 << DoTransformMaxCurveBit);
		curveb = -(curvelength / 2) + 1;
		curvee = curveb + curvelength - 1;
	}
	else
	{
		curveb = 0;
		curvee = 0;
		curvelength = 1;
	}

	seld = (1 << 30);
	for (bcurve = curveb; bcurve <= curvee;bcurve++)
	{
		int thetai = 0;
		for (r = rb; r < re; r+=sscale)
		{			
			ComputeBlockBar(rb, re, cb, ce, r, cb, bcurve, ver, sscale, graybit, bestd, bestind1, bestind2, gr1, gr2, codelen);
			if (bestd<seld)
			{
				seld = bestd;
				theta = thetai;
				curve = bcurve;
				ind1 = bestind1;
				ind2 = bestind2;
				g1 = gr1;
				g2 = gr2;
				codedlinelength = codelen;
			}
			thetai++;
		}
		for (c = cb; c <= ce; c+=sscale)
		{
			ComputeBlockBar(rb, re, cb, ce, re, c, bcurve, ver, sscale, graybit, bestd, bestind1, bestind2, gr1, gr2, codelen);
			if (bestd<seld)
			{
				seld = bestd;
				theta = thetai;
				curve = bcurve;
				ind1 = bestind1;
				ind2 = bestind2;
				g1 = gr1;
				g2 = gr2;
				codedlinelength = codelen;
			}
			thetai++;
		}
		thetalength = thetai;
	}

	d = seld;	
}

void CalcOneDepthBar(int nr, int nc, int w, int dep, int ver, int sscale, int graybit)
{
	cout << "CalcOneDepthCoded for dep : " << dep << endl;
	int bi, bj;
	int wr, wc;
	long long bestd;
	int bestm1, bestm2;
	int thetarate, codedlinerate, curverate;	
	int theta, ind1, ind2, curve, codedlinelength, curvelength, thetalength;

	for (bi = 0; bi<nr; bi += w)
	{
		for (bj = 0; bj<nc; bj += w)
		{
			// Calculate for one Square
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);

			ComputeBlockBar(bi, bi + wr - 1, bj, bj + wc - 1, ver, sscale, graybit, bestd, theta, ind1, ind2, curve, bestm1, bestm2, thetalength, codedlinelength, curvelength);

			thetarate = (int)ceil(log((double)(thetalength)) / log(2.0));
			codedlinerate = (int)ceil(log((double)(codedlinelength * (codedlinelength + 1) / 2)) / log(2.0));
			curverate = (int)ceil(log((double)curvelength) / log(2.0));

			DepPi1[bi][bj][dep] = ind1;
			DepPi2[bi][bj][dep] = ind2;
			DepPi3[bi][bj][dep] = theta;
			DepPi4[bi][bj][dep] = curve;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * graybit + thetarate + codedlinerate + curverate;
			if (ver == 12 || ver == 14)
				DepPr[bi][bj][dep]++;

			DepGi1[bi][bj][dep] = thetarate;
			DepGi2[bi][bj][dep] = codedlinerate;
			DepGi4[bi][bj][dep] = codedlinelength;
			DepGi3[bi][bj][dep] = curverate;

			RebuildBlockBar(bi, bi + wr - 1, bj, bj + wc - 1, dep, ver, sscale, graybit, theta, ind1, ind2, curve, bestm1, bestm2);
		}
	}

}

void DoTransformsBar(int nr, int nc, int ver, int sscale, int graybit)
{
	clock_t depTimeC;
	double depTime;
	int dep, w, ndep;

	cout << "Status : Bar Type" << endl;

	for (dep = 0, w = 1; (w >> 1)<max(nr, nc); dep++, w *= 2)
	{
		PreSetArray(nr, nc, dep);
		if (EnableConstantDep)
			CalcConstant(nr, nc, w, dep, graybit);
	}
	ndep = dep - 1;
	for (dep = 1; dep <= min(DoTransformMaxDepth, ndep); dep++)
	{
		int w = (int)pow(2.0, dep);
		depTimeC = clock();
		CalcOneDepthBar(nr, nc, w, dep, ver, sscale, graybit);
		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}

void RDBar(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{
	int dep, w, w2;
	int bi, bj, x, y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr, wc;

	for (bi = 0; bi<nr; bi++)
	{
		for (bj = 0; bj<nc; bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgprd[bi][bj] = imgc[bi][bj][0];
			imgprdg[bi][bj] = imgc[bi][bj][0];
			OptS[bi][bj][0] = 1;
		}
	}

	for (dep = 1, w = 2, w2 = 1;w2<max(nr, nc);w *= 2, w2 *= 2, dep++)
	{
		for (bi = 0; bi<nr; bi += w)
		{
			for (bj = 0; bj<nc; bj += w)
			{
				wr = min(w, nr - bi);
				wc = min(w, nc - bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];

				if (bi + w2<nr)
				{
					chD += OptD[bi + w2][bj];
					chR += OptR[bi + w2][bj];
				}
				if (bj + w2<nc)
				{
					chD += OptD[bi][bj + w2];
					chR += OptR[bi][bj + w2];
				}
				if (bi + w2<nr && bj + w2<nc)
				{
					chD += OptD[bi + w2][bj + w2];
					chR += OptR[bi + w2][bj + w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepPd[bi][bj][dep] + lambda * (2 + DepPr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind == 1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
						}
					}
					OptS[bi][bj][dep] = 1;
				}
				else if (ind == 2)
				{
					OptD[bi][bj] = DepPd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepPr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgp[bi + x][bj + y][dep];
						}
					}
					OptS[bi][bj][dep] = 2;
				}
				else if (ind == 3)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
					OptS[bi][bj][dep] = 0;
				}
			}
		}
	}

	rate = OptR[0][0];
	mse = (double)OptD[0][0] / (nr*nc);
	psnr = 10 * log(255.0*255.0 / mse) / log(10.0);
	bpp = (double)OptR[0][0] / (nr*nc);
}

void WriteToOutput(int graybit, int ind, double bppin, double psnr, double bpp, int rate, ofstream &out)
{
	printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin, bpp, psnr);
	out << "bppimg" << endl;
	if (psnr == std::numeric_limits<double>::infinity())
		out << "Inf";
	else
		out << psnr;
	out << " " << bpp << " " << ind << " " << bppin << " " << graybit << endl;
	Store(out, imgprd, "imgbar");
}

void BPPBar(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDBar(nr, nc, 0, psnr, bpp, rate);
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDBar(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPBar(int nr, int nc, int graybit, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[])
{
	double lambda;	
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDBar(nr, nc, lambda, psnr, bpp, rate);

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;			
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
		}
	}

	BPPBar(nr, nc, graybit, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r);
	BPPBar(nr, nc, graybit, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r);
}

void BPPBar(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[])
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDBar(nr, nc, 0, psnr, bpp, rate);
	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPBar(nr, nc, graybit, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r);
}

void BPPBar(int nr, int nc, int graybit, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDBar(nr, nc, lambda, psnr, bpp, rate);

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
			WriteToOutput(graybit, ind2, bppin[ind2], psnr, bpp, rate, out);
		}
	}

	BPPBar(nr, nc, graybit, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r, out);
	BPPBar(nr, nc, graybit, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r, out);
}

void BPPBar(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDBar(nr, nc, 0, psnr, bpp, rate);
	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPBar(nr, nc, graybit, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r, out);
}

void BPPBarGE(int nr, int nc, double bppin, int ver, int sscale, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDBar(nr, nc, 0, psnr, bpp, rate);
	rate = BarGeneralEncoder(nr, nc, ver, sscale, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDcoded(nr, nc, lambda, psnr, bpp, rate);
		rate = BarGeneralEncoder(nr, nc, ver, graybit, sscale, "v:\\inapp.BPPcodedGE");
		bpp = (double)rate / nr / nc;
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void RebuildBlockBar(int rb, int re, int cb, int ce, int ver, int sscale, int graybit, int theta, int ind1, int ind2, int curve, int g1, int g2, int wrb, int wre, int wcb, int wce)
{
	int wtheta;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	int wcr = wrb + (wre - wrb) / 2;
	int wcc = wcb + (wce - wcb + 1) / 2;

	// second line
	int oor, ooc;
	if (theta<re - rb)
	{
		oor = theta + rb;
		ooc = cb;
	}
	else
	{
		oor = re;
		ooc = theta - (re - rb) + cb;
	}

	int woor, wooc;
	int ptheta1 = (re - rb) / sscale;
	if (theta <= ptheta1)
	{
		wtheta = (int)round((theta*sscale)*(wre - wrb) / (double)(re - rb));
		woor = wtheta + wrb;
		wooc = wcb;
	}
	else
	{
		wtheta = (int)round(((theta - ptheta1)*sscale)*(wce - wcb) / (double)(ce - cb));
		woor = wre;
		wooc = wtheta + wcb;
	}

	// calculate distance to determine lines
	int r, c;
	double dr, dc, munder;
	int mindist, maxdist, wmindist, wmaxdist;
	double thetarc, sind, cosd, b, r2, c2;
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;

	double wdr, wdc;
	wdr = wcr - woor;
	wdc = wcc - wooc;
	wmindist = wmaxdist = 0;
	//int len = maxdist - mindist + 1;

	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr), abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		thetarc = atan2(dc, dr);
		sind = sin(-thetarc);
		cosd = cos(-thetarc);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 4)
	{
		munder = max(abs(dr), abs(dc))*sscale;
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}

		munder = max(abs(wdr), abs(wdc));
		for (r = wrb; r <= wre; r++)
		{
			for (c = wcb; c <= wce; c++)
			{
				wDist[r][c] = (int)round(((c - wooc)*wdr - (r - woor)*wdc) / munder);
				if (wDist[r][c] < wmindist) wmindist = wDist[r][c];
				if (wDist[r][c] > wmaxdist) wmaxdist = wDist[r][c];
			}
		}
	}

	int wind1 = (int)round(ind1*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));
	int wind2 = (int)round(ind2*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));

	for (r = wrb; r <= wre; r++)
	{
		for (c = wcb; c <= wce; c++)
		{
			if ((wDist[r][c] - wmindist)>=wind1 && (wDist[r][c] - wmindist)<=wind2)
				rimg[r][c] = g2;
			else
				rimg[r][c] = g1;
		}
	}

}

void BarImageCreate(int x, int y, int nr, int nc, int w, int dep, int ver, int sscale, int graybit, int wscale, int wx, int wy, int wxe, int wye)
{
	int i, j;
	int wx2pos, wy2pos;
	int w2 = w / 2;
	double wr = min(w, nr - x);
	double wc = min(w, nc - y);

	if (x >= nr || y >= nc) return;
	if (wxe < wx || wye < wy) return;
	switch (OptS[x][y][dep])
	{
	case 0:
		wx2pos = (int)round((wxe - wx + 1)*w2 / wr);
		wy2pos = (int)round((wye - wy + 1)*w2 / wc);
		BarImageCreate(x, y, nr, nc, w2, dep - 1, ver, sscale, graybit, wscale, wx, wy, min(wx+wx2pos-1,wxe), min(wy + wy2pos - 1, wye));
		BarImageCreate(x, y + w2, nr, nc, w2, dep - 1, ver, sscale, graybit, wscale, wx, wy+wy2pos, min(wx + wx2pos - 1, wxe), wye);
		BarImageCreate(x + w2, y, nr, nc, w2, dep - 1, ver, sscale, graybit, wscale, wx+wx2pos, wy, wxe, min(wy + wy2pos - 1, wye));
		BarImageCreate(x + w2, y + w2, nr, nc, w2, dep - 1, ver, sscale, graybit, wscale, wx+wx2pos, wy+wy2pos, wxe, wye);
		break;
	case 1:
		for (i = wx;i <= wxe;i++)
			for (j = wy;j <= wye;j++)
				rimg[i][j] = DepCm[x][y][dep];
		break;
	case 2:
		RebuildBlockBar(x, x + (int)wr - 1, y, y + (int)wc - 1, ver, sscale, graybit, DepPi3[x][y][dep], DepPi1[x][y][dep], DepPi2[x][y][dep], DepPi4[x][y][dep], DepPm1[x][y][dep], DepPm2[x][y][dep], wx, wxe, wy, wye);
		break;
	}
}

void BarImageCreate(int nr, int nc, int ver, int sscale, int graybit, int wscale, int &ww, int &wh)
{
	int dep, w, w2;	
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	ww = (int)round(nr / (double)wscale);
	wh = (int)round(nc / (double)wscale);
	memset(rimg, 255, sizeof(rimg));
	BarImageCreate(0, 0, nr, nc, w, dep, ver, sscale, graybit, wscale, 0, 0, ww-1, wh-1);
}

//############################## Bar Encoder ####################################

void BarEncoderLocoISequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, int sscale, MemoryFile mems[])
{
	int w2;
	bool cgrayswap;
	int predict, px01, px10, px11;

	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		BarEncoderLocoISequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderLocoISequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderLocoISequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderLocoISequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		//encoding gray
		if (x>0 && y>0)
		{
			px10 = imgprd[x - 1][y];
			px01 = imgprd[x][y - 1];
			px11 = imgprd[x - 1][y - 1];
			if (px11 >= max(px10, px01))
				predict = min(px10, px01);
			else if (px11 <= min(px10, px01))
				predict = max(px10, px01);
			else
				predict = px10 + px01 - px11;
		}
		else
		{
			predict = 0;
		}
		NumSequence[0][NumSequenceCount[0]++] = (DepCm[x][y][dep] >> (8 - graybit)) - (predict >> (8 - graybit));
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		//encoding gray
		cgrayswap = false;
		NumSequence[1][NumSequenceCount[1]++] = (DepPm1[x][y][dep]) >> (8 - graybit);
		NumSequence[1][NumSequenceCount[1]++] = (DepPm2[x][y][dep]) >> (8 - graybit);

		//encoding theta & codebits
		int indf;
		if (DepPi1[x][y][dep] <= DepPi2[x][y][dep])
			indf = DepPi1[x][y][dep] * (2 * DepGi4[x][y][dep] - DepPi1[x][y][dep] + 1) / 2 + DepPi2[x][y][dep] - DepPi1[x][y][dep] + 1;
		else
			indf = DepPi2[x][y][dep] * (2 * DepGi4[x][y][dep] - DepPi2[x][y][dep] + 1) / 2 + DepPi1[x][y][dep] - DepPi2[x][y][dep] + 1;
		mems[0].WriteBits(indf, DepGi2[x][y][dep]);
		mems[0].WriteBits(DepPi3[x][y][dep], DepGi1[x][y][dep]);

		break;
	}
}
int BarEncoderLocoISequence(int nr, int nc, int graybit, int sscale, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(sscale);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			BarEncoderLocoISequence(i, j, nr, nc, indepthw, indepth, dep, graybit, sscale, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}

void BarEncoderNormalSequence(int x, int y, int nr, int nc, int w, int dep, int deporg, int graybit, int sscale, MemoryFile mems[])
{
	int w2;
	bool cgrayswap;
	w2 = w / 2;

	if (x >= nr || y >= nc) return;

	switch (OptS[x][y][dep])
	{
	case 0:
		mems[0].WriteBit(0);
		BarEncoderNormalSequence(x, y, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderNormalSequence(x, y + w2, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderNormalSequence(x + w2, y, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		BarEncoderNormalSequence(x + w2, y + w2, nr, nc, w2, dep - 1, deporg, graybit, sscale, mems);
		break;
	case 1:
		//encoding structure
		if (dep > 0)
		{
			mems[0].WriteBit(1, 0);
		}

		//encoding gray
		NumSequence[0][NumSequenceCount[0]++] = DepCm[x][y][dep] >> (8 - graybit);
		break;
	case 2:
		//encoding structure
		mems[0].WriteBit(1, 1);

		//encoding gray
		cgrayswap = false;
		NumSequence[0][NumSequenceCount[0]++] = DepPm1[x][y][dep] >> (8 - graybit);
		NumSequence[0][NumSequenceCount[0]++] = DepPm2[x][y][dep] >> (8 - graybit);

		//encoding theta & codebits
		int indf;
		if (DepPi1[x][y][dep] <= DepPi2[x][y][dep])
			indf = DepPi1[x][y][dep] * (2 * DepGi4[x][y][dep] - DepPi1[x][y][dep] + 1) / 2 + DepPi2[x][y][dep] - DepPi1[x][y][dep] + 1;
		else
			indf = DepPi2[x][y][dep] * (2 * DepGi4[x][y][dep] - DepPi2[x][y][dep] + 1) / 2 + DepPi1[x][y][dep] - DepPi2[x][y][dep] + 1;
		mems[0].WriteBits(indf, DepGi2[x][y][dep]);
		mems[0].WriteBits(DepPi3[x][y][dep], DepGi1[x][y][dep]);

		break;
	}
}

int BarEncoderNormalSequence(int nr, int nc, int graybit, int sscale, MemoryFile mems[])
{
	int dep, w, w2, indepth;
	int i, j;
	NumSequenceBit[0] = graybit; NumSequenceBit[1] = graybit; NumSequenceBit[2] = graybit; NumSequenceBit[3] = graybit;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	indepth = DecideMaxInDepth(0, 0, nr, nc, w2, dep);
	int indepthw = (1 << indepth);

	mems[0].Write2Byte(nr);
	mems[0].Write2Byte(nc);
	mems[0].Write1Byte(sscale);
	mems[0].Write1Byte(indepthw);

	for (i = 0; i<nr; i += indepthw)
		for (j = 0; j<nc; j += indepthw)
			BarEncoderNormalSequence(i, j, nr, nc, indepthw, indepth, dep, graybit, sscale, mems);
	int raten = mems[0].GetLengthBit() + mems[1].GetLengthBit() + mems[2].GetLengthBit() + mems[3].GetLengthBit();
	for (i = 0;i<4;i++)
		raten += GCodeSequence(i);
	return raten;
}

int BarGeneralEncoder(int nr, int nc, int btver, int sscale, int graybit, char filename[])
{
	int bestrate = (1 << 30);
	int rate;

	if (ShowMessages) printf("\n==========================================\n\n");
	if (ShowMessages) printf("\n###########\n BarEncoderNormalSequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	rate = BarEncoderNormalSequence(nr, nc, graybit, sscale, mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	if (ShowMessages) printf("\n###########\n BarEncoderLocoISequence \n");
	mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	rate = BarEncoderLocoISequence(nr, nc, graybit, sscale, mems);
	if (rate<bestrate)
		bestrate = rate;
	if (ShowMessages) printf("C-RATE : %d\n", rate);

	//if (ShowMessages) printf("\n###########\n BarEncoderPrediction1Sequence \n");
	//mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	//memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	//rate = BarEncoderPrediction1Sequence(nr, nc, btver, graybit, mems);
	//if (rate<bestrate)
	//	bestrate = rate;
	//if (ShowMessages) printf("C-RATE : %d\n", rate);

	//printf("\n###########\n GEncoderPrediction2Sequence \n");
	//mems[0].Reset(); mems[1].Reset(); mems[2].Reset(); mems[3].Reset();
	//memset(NumSequenceCount, 0, sizeof(NumSequenceCount));
	//rate = GEncoderPrediction2Sequence(nr, nc, btver, mems);
	//if (rate<bestrate)
	//	bestrate = rate;
	//printf("C-RATE : %d\n", rate);

	return bestrate + 1;
}

//==============================================================
//==============================================================
//==============================================================
/// Image Code
//==============================================================
void RDBarT(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{
	int dep, w, w2;
	int bi, bj, x, y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr, wc;

	for (bi = 0; bi<nr; bi++)
	{
		for (bj = 0; bj<nc; bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgprd[bi][bj] = imgc[bi][bj][0];
			imgprdg[bi][bj] = imgc[bi][bj][0];
			OptS[bi][bj][0] = 1;
		}
	}

	for (dep = 1, w = 2, w2 = 1;w2<max(nr, nc);w *= 2, w2 *= 2, dep++)
	{
		for (bi = 0; bi<nr; bi += w)
		{
			for (bj = 0; bj<nc; bj += w)
			{
				wr = min(w, nr - bi);
				wc = min(w, nc - bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];

				if (bi + w2<nr)
				{
					chD += OptD[bi + w2][bj];
					chR += OptR[bi + w2][bj];
				}
				if (bj + w2<nc)
				{
					chD += OptD[bi][bj + w2];
					chR += OptR[bi][bj + w2];
				}
				if (bi + w2<nr && bj + w2<nc)
				{
					chD += OptD[bi + w2][bj + w2];
					chR += OptR[bi + w2][bj + w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepPd[bi][bj][dep] + lambda * (2 + DepPr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind == 1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
							imgwrd[bi + x][bj + y] = 0;
						}
					}
					OptS[bi][bj][dep] = 1;
				}
				else if (ind == 2)
				{
					OptD[bi][bj] = DepPd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepPr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgp[bi + x][bj + y][dep];
							imgwrd[bi + x][bj + y] = imgw[bi + x][bj + y][dep];
						}
					}
					OptS[bi][bj][dep] = 2;
				}
				else if (ind == 3)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
					OptS[bi][bj][dep] = 0;
				}
			}
		}
	}

	rate = OptR[0][0];
	mse = (double)OptD[0][0] / (nr*nc);
	psnr = 10 * log(255.0*255.0 / mse) / log(10.0);
	bpp = (double)OptR[0][0] / (nr*nc);
}

void BPPBarT(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDBarT(nr, nc, 0, psnr, bpp, rate);
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDBarT(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void RebuildBlockBarT(int rb, int re, int cb, int ce, int dep, int ver, int sscale, int graybit, int theta, int ind1, int ind2, int curve, int g1, int g2)
{
	int wtheta;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	// second line
	int oor, ooc;
	if (theta<re - rb)
	{
		oor = theta + rb;
		ooc = cb;
	}
	else
	{
		oor = re;
		ooc = theta - (re - rb) + cb;
	}

	int woor, wooc;
	int ptheta1 = (re - rb) / sscale;
	if (theta <= ptheta1)
	{
		wtheta = theta*sscale;
		woor = wtheta + rb;
		wooc = cb;
	}
	else
	{
		wtheta = (theta - ptheta1)*sscale;
		woor = re;
		wooc = wtheta + cb;
	}

	// calculate distance to determine lines
	int r, c;
	double dr, dc, munder;
	int mindist, maxdist, wmindist, wmaxdist;
	double thetarc, sind, cosd, b, r2, c2;
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;

	double wdr, wdc;
	wdr = cr - woor;
	wdc = cc - wooc;
	wmindist = wmaxdist = 0;
	//int len = maxdist - mindist + 1;

	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2)
	{
		munder = max(abs(dr), abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		thetarc = atan2(dc, dr);
		sind = sin(-thetarc);
		cosd = cos(-thetarc);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 4)
	{
		munder = max(abs(dr), abs(dc))*sscale;
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}

		munder = max(abs(wdr), abs(wdc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				wDist[r][c] = (int)round(((c - wooc)*wdr - (r - woor)*wdc) / munder);
				if (wDist[r][c] < wmindist) wmindist = wDist[r][c];
				if (wDist[r][c] > wmaxdist) wmaxdist = wDist[r][c];
			}
		}
	}

	int wind1 = (int)round(ind1*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));
	int wind2 = (int)round(ind2*(wmaxdist - wmindist + 1) / (double)(maxdist - mindist + 1));

	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if ((wDist[r][c] - wmindist) >= wind1 && (wDist[r][c] - wmindist) <= wind2)
				rimg[r][c] = g2;
			else
				rimg[r][c] = g1;
		}
	}

	DepGm1[rb][cb][dep] = 0;
	DepGm2[rb][cb][dep] = 0;

	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if ((Dist[r][c] - mindist) >= ind1 && (Dist[r][c] - mindist) <= ind2)
			{
				imgp[r][c][dep] = g2;
				DepGm2[rb][cb][dep]++;
				//imgw[r][c][dep] = abs(ind1 - ind2) + 1;
				imgw[r][c][dep] = abs(g1-g2);
			}
			else
			{
				imgp[r][c][dep] = g1;
				DepGm1[rb][cb][dep]++;
				imgw[r][c][dep] = 0;
			}			
		}
	}

}

void CalcOneDepthBarT(int nr, int nc, int w, int dep, int ver, int sscale, int graybit)
{
	cout << "CalcOneDepthCoded for dep : " << dep << endl;
	int bi, bj;
	int wr, wc;
	long long bestd;
	int bestm1, bestm2;
	int thetarate, codedlinerate, curverate;
	int theta, ind1, ind2, curve, codedlinelength, curvelength, thetalength;

	for (bi = 0; bi<nr; bi += w)
	{
		for (bj = 0; bj<nc; bj += w)
		{
			// Calculate for one Square
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);

			ComputeBlockBar(bi, bi + wr - 1, bj, bj + wc - 1, ver, sscale, graybit, bestd, theta, ind1, ind2, curve, bestm1, bestm2, thetalength, codedlinelength, curvelength);

			thetarate = (int)ceil(log((double)(thetalength)) / log(2.0));
			codedlinerate = (int)ceil(log((double)(codedlinelength * (codedlinelength + 1) / 2)) / log(2.0));
			curverate = (int)ceil(log((double)curvelength) / log(2.0));

			DepPi1[bi][bj][dep] = ind1;
			DepPi2[bi][bj][dep] = ind2;
			DepPi3[bi][bj][dep] = theta;
			DepPi4[bi][bj][dep] = curve;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * graybit + thetarate + codedlinerate + curverate;
			if (ver == 12 || ver == 14)
				DepPr[bi][bj][dep]++;

			DepGi1[bi][bj][dep] = thetarate;
			DepGi2[bi][bj][dep] = codedlinerate;
			DepGi4[bi][bj][dep] = codedlinelength;
			DepGi3[bi][bj][dep] = curverate;

			RebuildBlockBarT(bi, bi + wr - 1, bj, bj + wc - 1, dep, ver, sscale, graybit, theta, ind1, ind2, curve, bestm1, bestm2);
		}
	}

}

void DoTransformsBarT(int nr, int nc, int ver, int sscale, int graybit)
{
	clock_t depTimeC;
	double depTime;
	int dep, w, ndep;

	cout << "Status : Bar Type" << endl;

	for (dep = 0, w = 1; (w >> 1)<max(nr, nc); dep++, w *= 2)
	{
		PreSetArray(nr, nc, dep);
		if (EnableConstantDep)
			CalcConstant(nr, nc, w, dep, graybit);
	}
	ndep = dep - 1;
	for (dep = 1; dep <= min(DoTransformMaxDepth, ndep); dep++)
	{
		int w = (int)pow(2.0, dep);
		depTimeC = clock();
		CalcOneDepthBarT(nr, nc, w, dep, ver, sscale, graybit);
		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}

double DepSC[IMGSZR][IMGSZC][DEPMAX];

bool GetBarWedgeletsCodingStream(int x, int y, int nr, int nc, int w, int dep, double &cs, string &css)
{
	int w2 = w >> 1;
	double wr = min(w, nr - x);
	double wc = min(w, nc - y);
	double cs1, cs2, cs3, cs4;
	string css1, css2, css3, css4;

	cs = 0;
	css = "";
	if (x >= nr || y >= nc) return false;

	switch (OptS[x][y][dep])
	{
	case 0:
		GetBarWedgeletsCodingStream(x, y, nr, nc, w2, dep - 1, cs1, css1);
		GetBarWedgeletsCodingStream(x + w2, y, nr, nc, w2, dep - 1, cs2, css2);
		GetBarWedgeletsCodingStream(x, y + w2, nr, nc, w2, dep - 1, cs3, css3);
		GetBarWedgeletsCodingStream(x + w2, y + w2, nr, nc, w2, dep - 1, cs4, css4);
		cs = cs1 + cs2 + cs3 + cs4;
		break;
	case 1:
		cs = 0;
		break;
	case 2:
		//if (DepPm1[x][y][dep] >= DepPm2[x][y][dep])
		//{
			cs = (abs(DepPi1[x][y][dep] - DepPi2[x][y][dep]) + 1);
			cs = cs*cs*DepGm2[x][y][dep];
		//}
		break;
	}
	//cout << "(" << cs << ")";
	DepSC[x][y][dep] = cs/wr/wc;
	return false;
}

void GetBarWedgeletsCodingStream(int nr, int nc)
{
	int dep, w, w2;
	int x, y;
	double cs;
	string css;
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	bool csres = GetBarWedgeletsCodingStream(0, 0, nr, nc, w, dep, cs, css);

	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++)
	{
		cout << endl << "DEP " << dep << endl;
		for (x = 0;x < nr;x += w)
			for (y = 0;y < nc;y += w)
				cout << DepSC[x][y][dep] << " ";
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//---------------  Test Code for bar type --------------------------------------
//------------------------------------------------------------------------------

void TestBarCreateMultiLevelImage1(int nr, int nc, double bppin[], int bppinlen, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	int i,r,c;
	memset(rimg, 0, size(rimg));
	for (i = 0; i < bppinlen; i++)
	{
		BPPBarT(nr, nc, bppin[i], ver, graybit, psnr, bpp, rate);
		for (r = 0;r < nr;r++)
			for (c = 0;c < nc;c++)
				rimg[r][c] += imgprd[r][c];
	}
}

void CalcConstantMasked3(int nr, int nc, int w, int dep, int graybit)
{
	int bi, bj, x, y;
	int wr, wc;
	int m;
	int n1;
	long long s, p;
	long long d;
	int pow2rem = 1 << (8 - graybit);
	int maxgray = ((1 << graybit) - 1) << (8 - graybit);

	for (bi = 0;bi<nr;bi += w)
	{
		for (bj = 0;bj<nc;bj += w)
		{
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);
			n1 = 0;
			s = 0;
			p = 0;
			for (x = 0;x<wr;x++)
			{
				for (y = 0;y<wc;y++)
				{
					if (imgmask[bi + x][bj + y] == true)
					{
						n1++;
						s += img[bi + x][bj + y];
						p += (img[bi + x][bj + y] * img[bi + x][bj + y]);
					}
				}
			}
			if (n1 == 0)
			{
				m = 0;
				d = 0;
				DepCm[bi][bj][dep] = m;
				DepCd[bi][bj][dep] = d;
				DepCr[bi][bj][dep] = 0;
			}
			else
			{
				m = (int)round((double)s / n1);
				d = p + (long long)n1*m*m - 2 * m*s;
				DepCm[bi][bj][dep] = m;
				DepCd[bi][bj][dep] = d;
				DepCr[bi][bj][dep] = GRAYBIT;
			}

			for (x = 0;x<wr;x++)
			{
				for (y = 0;y<wc;y++)
				{
					if (imgmask[bi + x][bj + y] == false)
						imgc[bi + x][bj + y][dep] = 0;
						//imgc[bi + x][bj + y][dep] = m;
					else
						imgc[bi + x][bj + y][dep] = m;
				}
			}
		}
	}
}

//void DoTransformsBarMasked1(int nr, int nc, int ver, int sscale, int graybit)
//{
//	clock_t depTimeC;
//	double depTime;
//	int dep, w, ndep;
//
//	cout << "Status : Bar Type" << endl;
//
//	for (dep = 0, w = 1; (w >> 1)<max(nr, nc); dep++, w *= 2)
//	{
//		PreSetArray(nr, nc, dep);
//		if (EnableConstantDep)
//			CalcConstantMasked3(nr, nc, w, dep, graybit);
//	}
//	ndep = dep - 1;
//	for (dep = 1; dep <= min(DoTransformMaxDepth, ndep); dep++)
//	{
//		int w = (int)pow(2.0, dep);
//		depTimeC = clock();
//		CalcOneDepthBarMasked1(nr, nc, w, dep, ver, sscale, graybit);
//		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
//		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
//	}
//}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//---------------    Bar Masked version   --------------------------------------
//------------------------------------------------------------------------------

void RDBarMasked(int nr, int nc, double lambda, double &psnr, double &bpp, int &rate)
{
	int dep, w, w2;
	int bi, bj, x, y;
	int ind;
	double mse;
	long long chD;
	int chR;
	int wr, wc;

	for (bi = 0; bi<nr; bi++)
	{
		for (bj = 0; bj<nc; bj++)
		{
			OptR[bi][bj] = DepCr[bi][bj][0];
			OptD[bi][bj] = DepCd[bi][bj][0];
			imgprd[bi][bj] = imgc[bi][bj][0];
			imgprdg[bi][bj] = imgc[bi][bj][0];
			OptS[bi][bj][0] = 1;
		}
	}

	for (dep = 1, w = 2, w2 = 1;w2<max(nr, nc);w *= 2, w2 *= 2, dep++)
	{
		for (bi = 0; bi<nr; bi += w)
		{
			for (bj = 0; bj<nc; bj += w)
			{
				wr = min(w, nr - bi);
				wc = min(w, nc - bj);

				chD = OptD[bi][bj];
				chR = OptR[bi][bj];

				if (bi + w2<nr)
				{
					chD += OptD[bi + w2][bj];
					chR += OptR[bi + w2][bj];
				}
				if (bj + w2<nc)
				{
					chD += OptD[bi][bj + w2];
					chR += OptR[bi][bj + w2];
				}
				if (bi + w2<nr && bj + w2<nc)
				{
					chD += OptD[bi + w2][bj + w2];
					chR += OptR[bi + w2][bj + w2];
				}

				ind = mini(
					DepCd[bi][bj][dep] + lambda * (2 + DepCr[bi][bj][dep]),
					DepPd[bi][bj][dep] + lambda * (2 + DepPr[bi][bj][dep]),
					chD + lambda * (1 + chR));

				if (ind == 1)
				{
					OptD[bi][bj] = DepCd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepCr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgc[bi + x][bj + y][dep];
						}
					}
					OptS[bi][bj][dep] = 1;
				}
				else if (ind == 2)
				{
					OptD[bi][bj] = DepPd[bi][bj][dep];
					OptR[bi][bj] = 2 + DepPr[bi][bj][dep];
					for (x = 0; x<wr; x++)
					{
						for (y = 0; y<wc; y++)
						{
							imgprd[bi + x][bj + y] = imgp[bi + x][bj + y][dep];
						}
					}
					OptS[bi][bj][dep] = 2;
				}
				else if (ind == 3)
				{
					OptD[bi][bj] = chD;
					OptR[bi][bj] = 1 + chR;
					OptS[bi][bj][dep] = 0;
				}
			}
		}
	}

	//int sizeofmask = min(nr, nc) * 2 * 8;
	//int kk1=0; long long kk2=0;
	//for (x = 0; x<nr; x++)
	//{
	//	for (y = 0; y<nc; y++)
	//	{
	//		if (imgmask[x][y] == false)
	//		{
	//			imgprd[x][y] = 0;
	//			kk1++;
	//			kk2 += img[x][y];
	//		}
	//	}
	//}
	//int kkmean = round((double)kk2 / kk1);
	//for (x = 0; x<nr; x++)
	//{
	//	for (y = 0; y<nc; y++)
	//	{
	//		if (imgmask[x][y] == false)
	//		{
	//			OptD[0][0] += ((img[x][y] - kkmean) * (img[x][y] - kkmean));
	//		}
	//	}
	//}

	int sizeofmask = min(nr, nc) * 2 * 8;
	for (x = 0; x<nr; x++)
	{
		for (y = 0; y<nc; y++)
		{
			if (imgmask[x][y] == false)
			{
				imgprd[x][y] = 0;
			}
		}
	}

	rate = OptR[0][0] + sizeofmask;
	mse = (double)OptD[0][0] / (nr*nc);
	psnr = 10 * log(255.0*255.0 / mse) / log(10.0);
	bpp = (double)OptR[0][0] / (nr*nc);
}

void BPPBarMasked(int nr, int nc, double bppin, int ver, int graybit, double &psnr, double &bpp, int &rate)
{
	double lambda;
	double lambdamin = 0, lambdamax = 1e10;
	int iter = 0;
	int maxiter = 100;

	cout << "BPPcodedGE function called for " << bppin << " with " << graybit << " bits" << endl;

	RDBarMasked(nr, nc, 0, psnr, bpp, rate);
	if (bpp < bppin)
		return;

	bpp = -100;
	while (iter < maxiter && fabs(bppin - bpp) > 0.001)
	{
		lambda = (lambdamin + lambdamax) / 2;
		//cout << "lambda : " << lambdamin << " - " << lambda << " - " << lambdamax << " : ";
		RDBarMasked(nr, nc, lambda, psnr, bpp, rate);
		//cout << psnr << " , " << bpp << endl;
		if (bpp < bppin)
			lambdamax = lambda;
		else
			lambdamin = lambda;
		iter++;
	}
}

void BPPBarMasked(int nr, int nc, int graybit, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[])
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDBarMasked(nr, nc, lambda, psnr, bpp, rate);

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
		}
	}

	BPPBarMasked(nr, nc, graybit, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r);
	BPPBarMasked(nr, nc, graybit, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r);
}

void BPPBarMasked(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[])
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDBarMasked(nr, nc, 0, psnr, bpp, rate);
	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPBarMasked(nr, nc, graybit, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r);
}

void BPPBarMasked(int nr, int nc, int graybit, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDBarMasked(nr, nc, lambda, psnr, bpp, rate);

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
			WriteToOutput(graybit, ind2, bppin[ind2], psnr, bpp, rate, out);
		}
	}

	BPPBarMasked(nr, nc, graybit, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r, out);
	BPPBarMasked(nr, nc, graybit, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r, out);
}

void BPPBarMasked(int nr, int nc, int graybit, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDBarMasked(nr, nc, 0, psnr, bpp, rate);

	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPBarMasked(nr, nc, graybit, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r, out);
}

void BPPBarMaskedCmprs(int nr, int nc, int graybit, int ver, int iter, double lambdamin, double lambdamax, double bppin[], int bppinb, int bppine, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambda;
	double psnr, bpp;
	int rate;
	int i, ind1, ind2;
	int maxiter = 100;

	if (bppinb > bppine)
		return;

	lambda = (lambdamin + lambdamax) / 2;
	RDBarMasked(nr, nc, lambda, psnr, bpp, rate);
	rate = BarGeneralEncoder(nr, nc, ver, 1, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	if (iter > maxiter)
	{
		for (i = bppinb; i <= bppine; i++)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
		}
		return;
	}

	for (ind1 = bppinb; ind1 <= bppine;ind1++)
	{
		if (bppin[ind1] <= bpp + bppin[ind1] / 100.0)
			break;
	}
	for (ind2 = ind1; ind2 <= bppine;ind2++)
	{
		if (bppin[ind2] < bpp - bppin[ind2] / 100.0)
		{
			break;
		}
		else {
			pbr_pb[ind2][0] = psnr;
			pbr_pb[ind2][1] = bpp;
			pbr_r[ind2] = rate;
			WriteToOutput(graybit, ind2, bppin[ind2], psnr, bpp, rate, out);
		}
	}

	BPPBarMaskedCmprs(nr, nc, graybit, ver, iter + 1, lambdamin, lambda, bppin, bppinb, ind1 - 1, pbr_pb, pbr_r, out);
	BPPBarMaskedCmprs(nr, nc, graybit, ver, iter + 1, lambda, lambdamax, bppin, ind2, bppine, pbr_pb, pbr_r, out);
}

void BPPBarMaskedCmprs(int nr, int nc, int graybit, int ver, double bppin[], int bpplen, double pbr_pb[][2], int pbr_r[], ofstream &out)
{
	double lambdamin = 0, lambdamax = 1e10;
	double psnr, bpp;
	int rate;
	int i, begind = 0;;

	cout << "BPPcodedGE function called for array bpp (#" << bpplen << ") with " << graybit << " bits" << endl;

	RDBarMasked(nr, nc, 0, psnr, bpp, rate);
	rate = BarGeneralEncoder(nr, nc, ver, 1, graybit, "v:\\inapp.BPPcodedGE");
	bpp = (double)rate / nr / nc;

	for (i = 0; i < bpplen; i++)
	{
		if (bppin[i] >= bpp - bppin[i] / 100.0)
		{
			pbr_pb[i][0] = psnr;
			pbr_pb[i][1] = bpp;
			pbr_r[i] = rate;
			WriteToOutput(graybit, i, bppin[i], psnr, bpp, rate, out);
			begind = i + 1;
		}
		else
		{
			break;
		}
	}

	BPPBarMaskedCmprs(nr, nc, graybit, ver, 0, lambdamin, lambdamax, bppin, begind, bpplen - 1, pbr_pb, pbr_r, out);
}

void ComputeBlockBarMasked(int rb, int re, int cb, int ce, int oor, int ooc, double curve, int ver, int sscale, int graybit, long long &bestd, int &bestind1, int &bestind2, int &g1, int &g2, int &codedlinelength, int &area)
{
	int r, c, i, j;
	double dr, dc, munder, b;
	double theta, sind, cosd, r2, c2;
	int mindist, maxdist;

	// center
	int cr = rb + (re - rb) / 2;
	int cc = cb + (ce - cb + 1) / 2;

	//cout << "orientation [" << or << " : " << oc << "] to [" << cr << " : " << cc << "]";
	if (cr == oor && cc == ooc)
	{
		bestd = (1 << 30);
		//cout << endl;
		return;
	}

	// calculate distance to determine lines
	dr = cr - oor;
	dc = cc - ooc;
	mindist = maxdist = 0;
	if (ver == 1)
	{
		munder = sqrt(dr*dr + dc*dc);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 2 || ver == 12)
	{
		munder = max(abs(dr), abs(dc));
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 4 || ver == 14)
	{
		munder = max(abs(dr), abs(dc))*sscale;
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				Dist[r][c] = (int)round(((c - ooc)*dr - (r - oor)*dc) / munder);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}
	else if (ver == 3)
	{
		theta = atan2(dc, dr);
		sind = sin(-theta);
		cosd = cos(-theta);
		munder = max(abs(sind), abs(cosd));
		b = max(re - rb + 1, ce - cb + 1);
		b = curve / (b*b);
		for (r = rb; r <= re; r++)
		{
			for (c = cb; c <= ce; c++)
			{
				r2 = cosd*(r - oor) + sind*(c - ooc);
				c2 = -sind*(r - oor) + cosd*(c - ooc);
				Dist[r][c] = (int)ceil(((r2 - b*(c2*c2))*munder) - 0.5);
				if (Dist[r][c] < mindist) mindist = Dist[r][c];
				if (Dist[r][c] > maxdist) maxdist = Dist[r][c];
			}
		}
	}

	//if (maxdist-mindist>200)
	//	printf("D| %d \n",maxdist-mindist);

	// set statistical variables to zero
	for (i = 0; i <= maxdist - mindist; i++)
		for (j = 0; j < 3; j++)
			StatVars[i][j] = 0;

	// gathering line statistics n,s,p
	for (r = rb; r <= re; r++)
	{
		for (c = cb; c <= ce; c++)
		{
			if (imgmask[r][c] == true)
			{
				StatVars[Dist[r][c] - mindist][0]++; // n
				StatVars[Dist[r][c] - mindist][1] += img[r][c]; // s
				StatVars[Dist[r][c] - mindist][2] += img[r][c] * img[r][c]; // p
			}
		}
	}

	codedlinelength = maxdist - mindist + 1;
	//cout << " L " << codedlinelength << endl;
	//for (i = 0; i < codedlinelength; i++)
	//	cout << "State " << i << " [" << StatVars[i][0] << "-" << StatVars[i][1] << "-" << StatVars[i][2] << "-" << (int)round(StatVars[i][1] / (double)StatVars[i][0]) << "]" << endl;
	if (ver == 12 || ver == 14)
		ComputeBestAssignmentBar2(codedlinelength, graybit, bestd, bestind1, bestind2, g1, g2, area);
	else
		ComputeBestAssignmentBar(codedlinelength, graybit, bestd, bestind1, bestind2, g1, g2, area);
	//cout << "Result : " << bestv << " (" << bestvw << ")" << endl;
	//cout << endl << "-------------------------" << endl;
}

void ComputeBlockBarMasked(int rb, int re, int cb, int ce, int ver, int sscale, int graybit, long long &d, int &theta, int &ind1, int &ind2, int &curve, int &g1, int &g2, int &thetalength, int &codedlinelength, int &curvelength, int &area)
{
	int r, c;
	int gr1, gr2, codelen, bestind1, bestind2;
	int curveb, curvee, bcurve;
	long long bestd, seld;
	int selarea;

	//cout << "ComputeBlock (" << rb << "," << re << "),(" << cb << "," << ce << ")  " << re-rb+1 << "-" << ce-cb+1 << endl;

	if (ver == 3 && max(re - rb + 1, ce - cb + 1) >= 8)
	{
		curvelength = (1 << DoTransformMaxCurveBit);
		curveb = -(curvelength / 2) + 1;
		curvee = curveb + curvelength - 1;
	}
	else
	{
		curveb = 0;
		curvee = 0;
		curvelength = 1;
	}

	seld = (1 << 30);
	for (bcurve = curveb; bcurve <= curvee;bcurve++)
	{
		int thetai = 0;
		for (r = rb; r < re; r += sscale)
		{
			ComputeBlockBarMasked(rb, re, cb, ce, r, cb, bcurve, ver, sscale, graybit, bestd, bestind1, bestind2, gr1, gr2, codelen, selarea);
			if (bestd<seld)
			{
				seld = bestd;
				theta = thetai;
				curve = bcurve;
				ind1 = bestind1;
				ind2 = bestind2;
				g1 = gr1;
				g2 = gr2;
				codedlinelength = codelen;
				area = selarea;
			}
			thetai++;
		}
		for (c = cb; c <= ce; c += sscale)
		{
			ComputeBlockBarMasked(rb, re, cb, ce, re, c, bcurve, ver, sscale, graybit, bestd, bestind1, bestind2, gr1, gr2, codelen, selarea);
			if (bestd<seld)
			{
				seld = bestd;
				theta = thetai;
				curve = bcurve;
				ind1 = bestind1;
				ind2 = bestind2;
				g1 = gr1;
				g2 = gr2;
				codedlinelength = codelen;
				area = selarea;
			}
			thetai++;
		}
		thetalength = thetai;
	}

	d = seld;
}

void CalcOneDepthBarMasked(int nr, int nc, int w, int dep, int ver, int sscale, int graybit)
{
	cout << "CalcOneDepthCoded for dep : " << dep << endl;
	int bi, bj;
	int wr, wc;
	long long bestd;
	int bestm1, bestm2;
	int thetarate, codedlinerate, curverate;
	int theta, ind1, ind2, curve, codedlinelength, curvelength, thetalength;
	int area;

	for (bi = 0; bi<nr; bi += w)
	{
		for (bj = 0; bj<nc; bj += w)
		{
			// Calculate for one Square
			wr = min(w, nr - bi);
			wc = min(w, nc - bj);

			ComputeBlockBarMasked(bi, bi + wr - 1, bj, bj + wc - 1, ver, sscale, graybit, bestd, theta, ind1, ind2, curve, bestm1, bestm2, thetalength, codedlinelength, curvelength, area);

			thetarate = (int)ceil(log((double)(thetalength)) / log(2.0));
			codedlinerate = (int)ceil(log((double)(codedlinelength * (codedlinelength + 1) / 2)) / log(2.0));
			curverate = (int)ceil(log((double)curvelength) / log(2.0));

			DepPi1[bi][bj][dep] = ind1;
			DepPi2[bi][bj][dep] = ind2;
			DepPi3[bi][bj][dep] = theta;
			DepPi4[bi][bj][dep] = curve;
			DepPm1[bi][bj][dep] = bestm1;
			DepPm2[bi][bj][dep] = bestm2;
			DepPd[bi][bj][dep] = bestd;
			DepPr[bi][bj][dep] = 2 * graybit + thetarate + codedlinerate + curverate;
			if (ver == 12 || ver == 14)
				DepPr[bi][bj][dep]++;
			DepGr[bi][bj][dep] = area;

			DepGi1[bi][bj][dep] = thetarate;
			DepGi2[bi][bj][dep] = codedlinerate;
			DepGi4[bi][bj][dep] = codedlinelength;
			DepGi3[bi][bj][dep] = curverate;

			RebuildBlockBar(bi, bi + wr - 1, bj, bj + wc - 1, dep, ver, sscale, graybit, theta, ind1, ind2, curve, bestm1, bestm2);
		}
	}
}

void DoTransformsBarMasked(int nr, int nc, int ver, int sscale, int graybit)
{
	clock_t depTimeC;
	double depTime;
	int dep, w, ndep;

	cout << "Status : Bar Type" << endl;

	for (dep = 0, w = 1; (w >> 1)<max(nr, nc); dep++, w *= 2)
	{
		PreSetArray(nr, nc, dep);
		if (EnableConstantDep)
			CalcConstantMasked3(nr, nc, w, dep, graybit);
	}
	ndep = dep - 1;
	for (dep = 1; dep <= min(DoTransformMaxDepth, ndep); dep++)
	{
		int w = (int)pow(2.0, dep);
		depTimeC = clock();
		CalcOneDepthBarMasked(nr, nc, w, dep, ver, sscale, graybit);
		depTime = (clock() - depTimeC) / (double)CLOCKS_PER_SEC;
		cout << "Time elapsed for dep " << dep << " : " << depTime << " seconds" << endl;
	}
}

void ApplyZeroMaskonImg(int nr, int nc)
{
	int r, c;
	for (r = 0;r < nr;r++)
		for (c = 0;c < nc;c++)
			if (imgmask[r][c] == false)
				img[r][c] = 0;
}

void StoreBarmaskedStat1(ofstream &out, int x, int y, int nr, int nc, int w, int dep)
{
	int xx, yy;
	int w2 = w / 2;
	int wr, wc;

	if (x >= nr || y >= nc) return;

	wr = min(w, nr - x);
	wc = min(w, nc - y);

	switch (OptS[x][y][dep])
	{
	case 0:
		StoreBarmaskedStat1(out, x, y, nr, nc, w2, dep - 1);
		StoreBarmaskedStat1(out, x, y + w2, nr, nc, w2, dep - 1);
		StoreBarmaskedStat1(out, x + w2, y, nr, nc, w2, dep - 1);
		StoreBarmaskedStat1(out, x + w2, y + w2, nr, nc, w2, dep - 1);
		break;
	case 1:
		for (xx = x;xx < x+wr;xx++)
			for (yy = y;yy < y+wc;yy++)
			{
				imgk[xx][yy][0] = 0;
				imgk[xx][yy][1] = imgprd[xx][yy];;
				imgk[xx][yy][2] = 0;
			}
		break;
	case 2:
		for (xx = x;xx < x+wr;xx++)
			for (yy = y;yy < y+wc;yy++)
			{
				imgk[xx][yy][0] = 0;
				imgk[xx][yy][1] = 0;
				imgk[xx][yy][2] = imgprd[xx][yy];;
			}
		break;
	}

}

void StoreBarmaskedStat2(ofstream &out, int x, int y, int nr, int nc, int w, int dep)
{
	int w2 = w / 2;
	int wr, wc;

	if (x >= nr || y >= nc)
	{
		out << "-1" << endl;
		return;
	}

	wr = min(w, nr - x);
	wc = min(w, nc - y);

	switch (OptS[x][y][dep])
	{
	case 0:
		out << "0 ";
		out << x << " " << y << " " << dep << " " << wr << " " << wc << endl;
		StoreBarmaskedStat2(out, x, y, nr, nc, w2, dep - 1);
		StoreBarmaskedStat2(out, x, y + w2, nr, nc, w2, dep - 1);
		StoreBarmaskedStat2(out, x + w2, y, nr, nc, w2, dep - 1);
		StoreBarmaskedStat2(out, x + w2, y + w2, nr, nc, w2, dep - 1);
		break;
	case 1:
		out << "1 " << DepCm[x][y][dep] << " ";
		out << x << " " << y << " " << dep << " " << wr << " " << wc << endl;
		break;
	case 2:
		out << "2 " << DepPm1[x][y][dep] << " " << DepPm2[x][y][dep] << " " << DepPr[x][y][dep] << " " << DepPd[x][y][dep] << " "
			<< DepPi1[x][y][dep] << " " << DepPi2[x][y][dep] << " " << DepPi3[x][y][dep] << " " << DepPi4[x][y][dep] << " "
			<< DepGi1[x][y][dep] << " " << DepGi2[x][y][dep] << " " << DepGi3[x][y][dep] << " " << DepGi4[x][y][dep] << " ";
		out << DepGr[x][y][dep] << " ";
		out << x << " " << y << " " << dep << " " << wr << " " << wc << endl;
		break;
	}

}

void StoreBarmaskedStat(ofstream &out, int nr, int nc)
{
	int i, r, c;
	int dep, w, w2;
	// store 3 channel image
	for (dep = 1, w = 2, w2 = 1; w <= max(nr, nc); w *= 2, w2 *= 2, dep++);
	StoreBarmaskedStat1(out, 0, 0, nr, nc, w, dep);
	out << nr << " " << nc << " " << endl;
	for (i = 0; i < 3; i++)
	{		
		for (r = 0;r < nr;r++)
			for (c = 0;c < nc;c++)
				out << imgk[r][c][i] << " ";
		out << endl;
	}

	out << nr << " " << nc << " " << endl;
	StoreBarmaskedStat2(out, 0, 0, nr, nc, w, dep);
}
