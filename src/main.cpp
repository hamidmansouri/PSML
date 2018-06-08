#include "PipeletsTransform.h"

int imgrow, imgcol;

void Store(ofstream &out, int mat[][IMGSZC], char *name)
{
	out << name << endl;
	out << imgrow << " " << imgcol << endl;
	for (int r=0;r<imgrow;r++)
	{
		for (int c=0;c<imgcol;c++)
			out << mat[r][c] << " ";
		out << endl;
	}
}

void Store(ofstream &out, long long mat[][IMGSZC], char *name)
{out << name << endl;
	out << imgrow << " " << imgcol << endl;
	for (int r=0;r<imgrow;r++)
	{
		for (int c=0;c<imgcol;c++)
			out << mat[r][c] << " ";
		out << endl;
	}
}

void Store(ofstream &out, int mat[][IMGSZC][DEPMAX], char *name, int dep)
{
	out << name << endl;
	out << dep << " " << imgrow << " " << imgcol << endl;
	for (int d=0;d<=dep;d++){
		for (int r=0;r<imgrow;r++)
		{
			for (int c=0;c<imgcol;c++)
				out << mat[r][c][d] << " ";
			out << endl;
		}
		out << endl;
	}
}

void Store(ofstream &out, int mat[][IMGSZC][DEPMAX], char *name)
{
	int dep,w;
	for (dep=0,w=1;w<=imgrow;w*=2,dep++);
	dep--;
	
	Store(out,mat,name,dep);
}

void Store(ofstream &out, long long mat[][IMGSZC][DEPMAX], char *name, int dep)
{
	out << name << endl;
	out << dep << " " << imgrow << " " << imgcol << endl;
	for (int d=0;d<=dep;d++){
		for (int r=0;r<imgrow;r++)
		{
			for (int c=0;c<imgcol;c++)
				out << mat[r][c][d] << " ";
			out << endl;
		}
		out << endl;
	}
}

void Store(ofstream &out, long long mat[][IMGSZC][DEPMAX], char *name)
{
	int dep,w;
	for (dep=0,w=1;w<=imgrow;w*=2,dep++);
	dep--;

	Store(out,mat,name,dep);
}

void Store(ofstream &out, double mat[][IMGSZC][DEPMAX], char *name, int dep)
{
	out << name << endl;
	out << dep << " " << imgrow << " " << imgcol << endl;
	for (int d=0;d<=dep;d++){
		for (int r=0;r<imgrow;r++)
		{
			for (int c=0;c<imgcol;c++)
				out << mat[r][c][d] << " ";
			out << endl;
		}
		out << endl;
	}
}

void Store(ofstream &out, double mat[][IMGSZC][DEPMAX], char *name)
{
	int dep,w;
	for (dep=0,w=1;w<=imgrow;w*=2,dep++);
	dep--;

	Store(out,mat,name,dep);
}

int main1(int argc, char**argv)
{
	char name[200], imgfilename[200], maskfilename[200], resfilename[400], res2filename[400];
	char tempstr[1000];
	int r,c;
	bool setLambda = false;
	ofstream fres,f2res;
	bool outputFileOpen = false;
	bool outputFile2Open = false;
	int btver = 0, graybit = 8;
	int sscale = 1;

	printf("Input Argument : %d \n",argc);
	for(int i=0;i<argc;i++)
		printf("arg[%d] = %s\n",i,argv[i]);

	for(int i=1;i<argc;i++)
	{
		if (strcmp(argv[i],"-name") == 0)
		{
			strcpy_s(name, argv[++i]);
			cout << "Read -name completed" << endl;
		}
		else if (strcmp(argv[i],"-img") == 0)
		{
			strcpy_s(imgfilename, argv[++i]);
			ifstream fimg = ifstream(imgfilename);
			fimg >> imgrow >> imgcol;			
			for(c=0;c<imgcol;c++)
				for(r=0;r<imgrow;r++)
					fimg >> img[r][c];
			fimg.close();
			cout << "read -img with " << imgrow << "x" << imgcol << " completed" << endl;
		}
		else if (strcmp(argv[i],"-imgDebug") == 0)
		{
			strcpy_s(imgfilename, argv[++i]);
			ifstream fimg = ifstream(imgfilename);
			fimg >> imgrow >> imgcol;			
			for(r=0;r<imgrow;r++)
				for(c=0;c<imgcol;c++)
					fimg >> img[r][c];
			fimg.close();
			cout << "read -img with " << imgrow << "x" << imgcol << " completed" << endl;
		}
		else if (strcmp(argv[i],"-resfilename") == 0)
		{
			strcpy_s(resfilename, argv[++i]);
			fres.open(resfilename);
			outputFileOpen = true;
		}
		else if (strcmp(argv[i],"-res2filename") == 0)
		{
			strcpy_s(resfilename, argv[++i]);
			fres.open(resfilename);
			outputFileOpen = true;
			strcpy_s(res2filename, argv[++i]);
			f2res.open(res2filename);
			outputFile2Open = true;
		}
		else if (strcmp(argv[i], "-dobuildtree") == 0)
		{
			DoTransforms(imgrow, imgcol);
			Store(fres, img, "img");		cout << "Store 'img' finished" << endl;
			Store(fres, imgc, "imgc");	cout << "Store 'imgc' finished" << endl;
			Store(fres, imgw, "imgw");	cout << "Store 'imgw' finished" << endl;
			Store(fres, imgp, "imgp");	cout << "Store 'imgp' finished" << endl;
			Store(fres, imgk, "imgk");	cout << "Store 'imgk' finished" << endl;
			Store(fres, DepCd, "DepCd");	cout << "Store 'DepCd' finished" << endl;
			Store(fres, DepWd, "DepWd");	cout << "Store 'DepWd' finished" << endl;
			Store(fres, DepPd, "DepPd");	cout << "Store 'DepPd' finished" << endl;
			Store(fres, DepKd, "DepKd");	cout << "Store 'DepKd' finished" << endl;
		}
		else if (strcmp(argv[i], "-dobuildtreecoded") == 0)
		{
			sscanf_s(argv[++i],"%d",&btver);
			DoTransformsCoded(imgrow, imgcol, btver, 8);
			Store(fres, img, "img");		cout << "Store 'img' finished" << endl;
			Store(fres, imgc, "imgc");	cout << "Store 'imgc' finished" << endl;
			Store(fres, imgw, "imgw");	cout << "Store 'imgw' finished" << endl;
			Store(fres, DepCd, "DepCd");	cout << "Store 'DepCd' finished" << endl;
			Store(fres, DepWd, "DepWd");	cout << "Store 'DepWd' finished" << endl;
		}
		else if (strcmp(argv[i], "-dobuildtreeonly") == 0)
		{
			cout << "Begin DoBuildTreeOnly" << endl;
			DoTransforms(imgrow, imgcol);			
		}
		else if (strcmp(argv[i], "-dobuildtreemaskedonly") == 0)
		{
			cout << "Begin DoBuildTreeMaskedOnly" << endl;
			DoTransformsMasked(imgrow, imgcol);
		}
		else if (strcmp(argv[i], "-dobuildtreecodedonly") == 0)
		{
			sscanf_s(argv[++i],"%d",&btver);
			cout << "Begin DoBuildTreeCodedOnly V " << btver << endl;
			DoTransformsCoded(imgrow, imgcol, btver, graybit);
		}
		else if (strcmp(argv[i], "-dobuildtreecodedonlyg") == 0)
		{
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &graybit);
			cout << "Begin DoBuildTreeCodedOnly V " << btver << " B " << graybit << endl;
			DoTransformsCoded(imgrow, imgcol, btver, graybit);
		}
		else if (strcmp(argv[i], "-dobuildtreebar") == 0)
		{			
			sscanf_s(argv[++i], "%d", &btver);			
			sscanf_s(argv[++i], "%d", &graybit);
			sscanf_s(argv[++i], "%d", &sscale);
			cout << "Begin DoBuildTreeBar V " << btver << " B " << graybit << " s-scale " << sscale << endl;
			DoTransformsBar(imgrow, imgcol, btver, sscale, graybit);
		}
		else if (strcmp(argv[i], "-constant") == 0)
		{
			EnableConstantDep = true;
			cout << "Enable Constant" << endl;
		}
		else if (strcmp(argv[i], "-wedgelet") == 0)
		{
			EnableWedgeletDep = true;
			cout << "Enable Wedgelet" << endl;
		}
		else if (strcmp(argv[i], "-pipelet") == 0)
		{
			EnablePipeletDep = true;
			cout << "Enable Pipelet" << endl;
		}
		else if (strcmp(argv[i],"-kleinlet") == 0)
		{
			EnableKleinletDep = true;
		}
		else if (strcmp(argv[i], "-pdepth") == 0)
		{
			strcpy_s(tempstr, argv[++i]);
			sscanf_s(tempstr, "%d", &DoTransformMaxDepth);
		}
		else if (strcmp(argv[i], "-storew") == 0)
		{
			Store(fres, imgw, "imgw");	cout << "Store 'imgw' finished" << endl;
		}
		else if (strcmp(argv[i], "-setlambda") == 0)
		{
			sscanf_s(argv[++i], "%d", &LambdaLength);
			for (int j = 0;j<LambdaLength;j++)
				sscanf_s(argv[++i], "%lf", &LambdaArray[j]);
			setLambda = true;
		}
		else if (strcmp(argv[i], "-dolambdaP") == 0)
		{
			double psnr, bpp;
			int rate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j=0;j<LambdaLength;j++)
			{
				cout << j+1 << " / " << LambdaLength << " : ";
				RDp(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				fres << "rdp_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgprd, "imgrd");
				Store(fres, imgprdg, "imgrdg");
				Store(fres, OptD, "optd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i],"-dolambdaPrd") == 0)
		{
			double psnr, bpp;
			int rate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j=0;j<LambdaLength;j++)
			{
				cout << j+1 << " / " << LambdaLength << " : ";
				RDp(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				fres << "rdp_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << rate << endl;
				Store(fres, imgprd, "imgrd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i],"-dolambdaPrd2") == 0)
		{
			double psnr, bpp, cbpp;
			int rate, crate, lb;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j=0;j<LambdaLength;j++)
			{
				cout << j+1 << " / " << LambdaLength << " : ";
				RDpDebug(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				lb = Encoder(imgrow,imgcol,2,"inapp.encoder");
				crate = lb*8;
				cbpp = (double)crate/(imgrow*imgcol);
				fres << "rdp_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << cbpp << " " << crate << endl;
				Store(fres, imgprd, "imgrd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i],"-dolambdaPmark") == 0)
		{
			double psnr, bpp, lambda;
			int rate;
			strcpy_s(tempstr, argv[++i]);
			printf("tempstr : %s \n",tempstr);
			sscanf_s(tempstr,"%lf",&lambda);
			printf("Lambda Mark : %f \n",lambda);
			RDpDebug(imgrow, imgcol, lambda, psnr, bpp, rate);
			fres << "rdp_param" << endl;
			fres << lambda << " ";
			if (psnr == std::numeric_limits<double>::infinity())
				fres << "Inf";
			else
				fres << psnr;
			fres << " " << bpp << " " << rate << endl;
			Store(fres, imgprd, "imgrd");
			GenerateRdType(imgrow,imgcol);
			Store(fres, imgprdg, "imgrdtype");
			fres << "end" << endl;
		}
		else if (strcmp(argv[i],"-dolambdaPDebug") == 0)
		{
			double psnr, bpp, cbpp;
			int rate,crate;
			++i; // to access file path
			if (setLambda == false)
				SetLambda();
			cout << "lambdaPlen : " << LambdaLength << endl;
			fres << "lambdaPlen" << endl << LambdaLength << " " << 6+1+8 << endl;
			for (int j=0;j<LambdaLength;j++)
			{			
				printf("%2d / %d : ",j+1,LambdaLength);
				fflush(stdout);
				RDpDebug(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				//Statistics1(imgrow,imgcol);
				int lb = Encoder(imgrow,imgcol,2,argv[i]);
				crate = lb*8;
				cbpp = (double)crate/(imgrow*imgcol);
				Decoder(argv[i]);
				bool res = CheckEncodeDecode(imgrow,imgcol);
				//printf("%7.4f %7.5f %10d %7.5f %10d %c\n",psnr,bpp,rate,cbpp,crate,res?'T':'F');
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << rate << " " << cbpp << " " << crate << " " << (res?'1':'0');
				for (int k=0;k<=8;k++)
					fres << " " << StatisticsVar[k];
				fres << endl;
				if (outputFile2Open)
				{
					Store(f2res,imgprd,"imgrd");
				}
			}
		}
		else if (strcmp(argv[i], "-dolambdaCoded") == 0)
		{
			double psnr, bpp;
			int rate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j = 0; j<LambdaLength; j++)
			{
				cout << j + 1 << " / " << LambdaLength << " : ";
				RDcoded(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				fres << "rd_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgwrd, "imgrd");
			}
		}
		else if (strcmp(argv[i], "-dolambdaCoded2") == 0)
		{
			double psnr, bpp;
			int rate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j = 0; j<LambdaLength; j++)
			{
				cout << j + 1 << " / " << LambdaLength << " : ";
				RDcoded(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				fres << "rd_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << rate << endl;
				Store(fres, imgwrd, "imgrd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i], "-dolambdaCoded2Compress") == 0)
		{
			double psnr, bpp, cbpp;
			int rate, crate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j = 0; j<LambdaLength; j++)
			{
				cout << j + 1 << " / " << LambdaLength << " : ";
				RDcoded(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				int lb = Encoder(imgrow, imgcol, 8, "v:\\inapp.dolambdaCoded2Compress");
				crate = lb * 8;
				cbpp = (double)crate / (imgrow*imgcol);
				fres << "rd_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << cbpp << " " << crate << endl;
				Store(fres, imgwrd, "imgrd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i], "-dolambdaCoded2GC") == 0)
		{
			double psnr, bpp;
			int rate;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;
			for (int j = 0; j<LambdaLength; j++)
			{
				cout << j + 1 << " / " << LambdaLength << " : ";
				RDcoded(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				rate = GeneralEncoder(imgrow, imgcol, btver, graybit, "inapp.dolambdaCoded2GC");
				fres << "rd_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << rate << endl;
				Store(fres, imgwrd, "imgrd");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i], "-dolambdaCodedCompress") == 0)
		{
			double psnr, bpp, cbpp;
			int rate, crate;
			int compressver;
			sscanf_s(argv[++i],"%d",&compressver);
			cout << "dolambdaCodedCompress : Compression Method = " << compressver << endl;
			strcpy_s(tempstr, argv[++i]); // compress file path
			cout << "Temp file path : " << tempstr << endl;
			if (setLambda == false)
				SetLambda();
			fres << "lambdaPlen" << endl << LambdaLength << endl;			
			for (int j = 0; j<LambdaLength; j++)
			{
				printf("%2d / %d : ",j+1,LambdaLength);
				fflush(stdout);
				RDcoded(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
				int lb = Encoder(imgrow,imgcol,compressver,tempstr);
				crate = lb*8;
				cbpp = (double)crate/(imgrow*imgcol);
				//Decoder(tempstr);
				//bool res = CheckEncodeDecode(imgrow,imgcol);
				fres << "rd_param" << endl;
				fres << LambdaArray[j] << " ";
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << cbpp << endl;
				Store(fres, imgwrd, "imgrd");				
			}
		}
		//else if (strcmp(argv[i], "-dolambdaCodedhm") == 0)
		//{
		//	double psnr, bpp;
		//	int rate;
		//	if (setLambda == false)
		//		SetLambda();
		//	fres << "lambdaPlen" << endl << LambdaLength << endl;
		//	for (int j = 0; j<LambdaLength; j++)
		//	{
		//		cout << j + 1 << " / " << LambdaLength << " : ";
		//		RDcodedhm(imgrow, imgcol, LambdaArray[j], psnr, bpp, rate);
		//		fres << "rd_param" << endl;
		//		fres << LambdaArray[j] << " ";
		//		if (psnr == std::numeric_limits<double>::infinity())
		//			fres << "Inf";
		//		else
		//			fres << psnr;
		//		fres << " " << bpp << endl;
		//		Store(fres, imgwrd, "imgrd");
		//	}
		//}
		else if (strcmp(argv[i],"-fPixel") == 0 || strcmp(argv[i],"-fPixel2") == 0)
		{
			int version = 0;
			if (strcmp(argv[i],"-fPixel") == 0)
				version = 1;
			if (strcmp(argv[i],"-fPixel2") == 0)
				version = 2;
			sscanf_s(argv[++i],"%d",&FeaturePixelCount);
			for (int j=0;j<FeaturePixelCount;j++)
				sscanf_s(argv[++i],"%d",&FeaturePixelWidth[j]);
			if (version==0 || version==1)
				DoFeaturePixel(imgrow,imgcol);
			if (version==2)
				DoFeaturePixel2(imgrow,imgcol);
			cout << "operation -fPixel" << version << " end." << endl;
			Store(fres,DepCm,"DepCm",FeaturePixelCount-1);		cout << "Store 'DepCm' finished" << endl;
			Store(fres,DepCd,"DepCd",FeaturePixelCount-1);		cout << "Store 'DepCd' finished" << endl;
			Store(fres,DepWm1,"DepWm1",FeaturePixelCount-1);	cout << "Store 'DepWm1' finished" << endl;
			Store(fres,DepWm2,"DepWm2",FeaturePixelCount-1);	cout << "Store 'DepWm2' finished" << endl;
			Store(fres,DepWd,"DepWd",FeaturePixelCount-1);		cout << "Store 'DepWd' finished" << endl;
			Store(fres,DepPm1,"DepPm1",FeaturePixelCount-1);	cout << "Store 'DepPm1' finished" << endl;
			Store(fres,DepPm2,"DepPm2",FeaturePixelCount-1);	cout << "Store 'DepPm2' finished" << endl;
			Store(fres,DepPd,"DepPd",FeaturePixelCount-1);		cout << "Store 'DepPd' finished" << endl;
			Store(fres,DepPh,"DepPh",FeaturePixelCount-1);		cout << "Store 'DepPh' finished" << endl;
		}
		else if (strcmp(argv[i], "-mask") == 0)
		{
			int R, C, v;
			strcpy_s(maskfilename, argv[++i]);
			ifstream fmask = ifstream(maskfilename);
			fmask >> R >> C;
			for (c = 0; c<C; c++)
				for (r = 0; r < R; r++){
					fmask >> v;
					imgmask[r][c] = (v == 1 || v == 255);
				}
			fmask.close();
			cout << "read -mask with " << R << "x" << C << " completed" << endl;
		}
		else if (strcmp(argv[i], "-bppimg") == 0)
		{
			double psnr, bpp;
			double bppin;
			sscanf_s(argv[++i], "%lf", &bppin);
			cout << "-bppimg for " << bppin << endl;
			DoTransforms(imgrow, imgcol);
			BPPp(imgrow, imgcol, bppin, psnr, bpp);
			fres << "bppimg" << endl;
			if (psnr == std::numeric_limits<double>::infinity())
				fres << "Inf";
			else
				fres << psnr;
			fres << " " << bpp << endl;
			Store(fres, imgprd, "imgrd");
			Store(fres, imgprdg, "imgrdg");
			Store(fres, OptD, "optd");
			Store(fres, OptR, "optr");
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppcodedimg") == 0)
		{
			double psnr, bpp;
			double bppin;
			int rate;
			sscanf_s(argv[++i], "%lf", &bppin);
			cout << "-bppimg for " << bppin << endl;
			BPPcoded(imgrow, imgcol, bppin, psnr, bpp, rate);
			fres << "bppimg" << endl;
			if (psnr == std::numeric_limits<double>::infinity())
				fres << "Inf";
			else
				fres << psnr;
			fres << " " << bpp << endl;
			Store(fres, imgwrd, "imgrd");
			Store(fres, imgwrdg, "imgrdg");
			Store(fres, OptD, "optd");
			Store(fres, OptR, "optr");
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppcodedimgnbis") == 0)
		{
			double psnr, bpp, bestpsnr, bestbpp;
			double bppin;
			int rate;
			int gbmin,gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%lf", &bppin);
			cout << "-bppimg for " << bppin << endl;
			ShowMessages = false;
			bestpsnr = 0;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				BPPcodedGE(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
				printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
				if (psnr > bestpsnr)
				{
					bestpsnr = psnr;
					bestbpp = bpp;
					for (int r = 0; r < imgrow; r++)
						for (int c = 0; c < imgcol; c++)
							imgkrd[r][c] = imgwrd[r][c];
				}
			}
			printf("Best result : %lf , %lf\n", bestbpp, bestpsnr);
			fres << "bppimg" << endl;
			if (bestpsnr == std::numeric_limits<double>::infinity())
				fres << "Inf";
			else
				fres << bestpsnr;
			fres << " " << bestbpp << endl;
			Store(fres, imgkrd, "imgrd");
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppcodedimgnbisgen") == 0)
		{
			double psnr, bpp;
			int bppinlen,j;
			double bppin[40];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedGE(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << endl;
					Store(fres, imgwrd, "imgrd");
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper1wedge") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransforms(imgrow, imgcol, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedWE(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
					Store(fres, imgwrd, "imgrd");
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper2wedge") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransforms(imgrow, imgcol, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedWE2(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
					Store(fres, imgwrd, "imgrd");
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper3wedge") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransforms(imgrow, imgcol, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedWE2(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4wedge") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBar(imgrow, imgcol, btver, 1, graybit);
				BPPBar(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{					
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4wedgeinm") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBar(imgrow, imgcol, btver, 1, graybit);
				BPPBar(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4barmasked") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4barmaskedinm") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4barmaskeds") == 0)
		{
			int gbmin, gbmax;
			double bppin, psnr, bpp;
			int rate;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
				printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin, bpp, psnr);
				fres << "bppimg" << endl;
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << bppin << " " << graybit << endl;
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper1psml") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedGE(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
					Store(fres, imgwrd, "imgrd");
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper2psml") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedGE2(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
					Store(fres, imgwrd, "imgrd");
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper3psml") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				for (j = 0; j < bppinlen; j++)
				{
					BPPcodedGE2(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
					printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
					fres << "bppimg" << endl;
					if (psnr == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << psnr;
					fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4psml") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				BPPcodedGE2(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4psmlcmprs") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				BPPcodedGE2Cmprs(imgrow, imgcol, graybit, btver, bppin, bppinlen, pbr_pb, pbr_r);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper5psmlcmprs") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			double pbr_pb[100][2];
			int pbr_r[100];
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsCoded(imgrow, imgcol, btver, graybit);
				BPPcodedGE2Cmprs(imgrow, imgcol, graybit, btver, bppin, bppinlen, pbr_pb, pbr_r, fres);
				for (j = 0; j < bppinlen; j++)
				{
					printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin[j], pbr_pb[j][1], pbr_pb[j][0]);
					fres << "bppimg" << endl;
					if (pbr_pb[j][0] == std::numeric_limits<double>::infinity())
						fres << "Inf";
					else
						fres << pbr_pb[j][0];
					fres << " " << pbr_pb[j][1] << " " << j << " " << bppin[j] << " " << graybit << endl;
				}
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bpppaper4barmaskedinm") == 0)
		{
			double bppin;
			double bpp, psnr;
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);

				printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin, bpp, psnr);

			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppbarmaskedstat") == 0)
		{
			double bppin;
			double bpp, psnr;
			int rate;
			int gbmin, gbmax;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
				printf("Coded for #G %d & bppin[%lf] results in bpp[%lf] , psnr [%lf] \n", graybit, bppin, bpp, psnr);
				StoreBarmaskedStat(fres, imgrow, imgcol);
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppbarpaper201") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			int pbr_r[100];
			double pbr_pb[100][2];
			int gbmin, gbmax;

			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;			
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBar(imgrow, imgcol, btver, 1, graybit);
				BPPBar(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r, fres);
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppbarmaskedpaper201") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			int pbr_r[100];
			double pbr_pb[100][2];
			int gbmin, gbmax;

			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMasked(imgrow, imgcol, graybit, bppin, bppinlen, pbr_pb, pbr_r, fres);
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppbarmaskedpaper201cmprs") == 0)
		{
			int bppinlen, j;
			double bppin[100];
			int pbr_r[100];
			double pbr_pb[100][2];
			int gbmin, gbmax;

			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &gbmax);
			sscanf_s(argv[++i], "%d", &gbmin);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			ApplyZeroMaskonImg(imgrow, imgcol);
			for (graybit = gbmax; graybit >= gbmin; graybit--)
			{
				DoTransformsBarMasked(imgrow, imgcol, btver, 1, graybit);
				BPPBarMaskedCmprs(imgrow, imgcol, graybit, btver, bppin, bppinlen, pbr_pb, pbr_r, fres);
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-bppbar") == 0)
		{
			double psnr, bpp, bppin;
			int rate;
			//sscanf_s(argv[++i], "%d", &btver);
			//sscanf_s(argv[++i], "%d", &graybit);			
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			//DoTransformsBar(imgrow, imgcol, btver, sscale, graybit);
			BPPBar(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
			printf("Coded for #G %d as bpp[%lf] in bpp[%lf] , psnr [%lf] (sz:%d)\n", graybit, bppin, bpp, psnr, rate);
		}
		else if (strcmp(argv[i], "-bppbarcompress") == 0)
		{
			double psnr, bpp, bppin;
			int rate;
			//sscanf_s(argv[++i], "%d", &btver);
			//sscanf_s(argv[++i], "%d", &graybit);			
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			//DoTransformsBar(imgrow, imgcol, btver, sscale, graybit);
			BPPBarGE(imgrow, imgcol, bppin, btver, sscale, graybit, psnr, bpp, rate);
			printf("Coded Compress for #G %d as bpp[%lf] in bpp[%lf] , psnr [%lf] (sz:%d)\n", graybit, bppin, bpp, psnr, rate);
		}
		else if (strcmp(argv[i], "-getbarimgsmask") == 0)
		{
			double psnr, bpp, bppin;
			int rate;
			int bppinlen, j;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &graybit);
			sscanf_s(argv[++i], "%d", &sscale);
			cout << "Begin DoBuildTreeBar V " << btver << " B " << graybit << " s-scale " << sscale << endl;
			DoTransformsBar(imgrow, imgcol, btver, sscale, graybit);
			sscanf_s(argv[++i], "%d", &bppinlen);
			ShowMessages = false;
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin);
				BPPBar(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
				printf("Coded for #G %d as bpp[%lf] in bpp[%lf] , psnr [%lf] (sz:%d)\n", graybit, bppin, bpp, psnr, rate);
				Store(fres, imgprd, "imgrd");
				Store(fres, imgprd, "rimg");
			}
		}
		else if (strcmp(argv[i], "-bppcodedimgcompress") == 0)
		{
			double psnr, bpp;
			double bppin;
			int rate;
			sscanf_s(argv[++i], "%lf", &bppin);
			BPPcoded(imgrow, imgcol, bppin, psnr, bpp, rate);
			printf("psnr : %lf , bpp : %lf \n",psnr,bpp);
			int r = GeneralEncoder(imgrow, imgcol, btver, graybit, "inapp.codedcompress");
			printf("rate : %d (%lf) => %d (%lf) \n",rate,(double)rate/imgrow/imgcol,r,(double)r/imgrow/imgcol);
			if (psnr == std::numeric_limits<double>::infinity())
				fres << "Inf";
			else
				fres << psnr;
			fres << " " << bpp << " " << rate << " " << (double)r / imgrow / imgcol << " " << r << endl;
		}

		else if (strcmp(argv[i], "-savetree") == 0)
		{
			SaveTree(argv[++i]);
		}
		else if (strcmp(argv[i], "-loadtree") == 0)
		{
			LoadTree(argv[++i]);
		}
		else if (strcmp(argv[i], "-bppimgs") == 0)
		{
			int num;
			double psnr, bpp;
			double bppin;

			cout << "bppimgs" << endl;			
			
			fres << "bppimgs" << endl;
			sscanf_s(argv[++i], "%d", &num);
			cout << "read " << num << " for bppimgs" << endl;
			fres << num << endl;
			for (int j = 0; j < num; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin);
				BPPp(imgrow, imgcol, bppin, psnr, bpp);
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgprd, "imgrd");
				Store(fres, imgprdg, "imgrdg");
				Store(fres, OptD, "optd");
				Store(fres, OptR, "optr");
				fres << "end" << endl;
			}
		}
		else if (strcmp(argv[i], "-bppimgs2") == 0)
		{
			int num;
			double psnr, bpp;
			double bppin;

			cout << "bppimgs" << endl;

			fres << "bppimgs" << endl;
			sscanf_s(argv[++i], "%d", &num);
			cout << "read " << num << " for bppimgs" << endl;
			fres << num << endl;
			for (int j = 0; j < num; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin);
				BPPp(imgrow, imgcol, bppin, psnr, bpp);
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgprd, "imgrd");
			}
		}
		else if (strcmp(argv[i], "-bppcodedimgs") == 0)
		{
			int num;
			double psnr, bpp;
			double bppin;
			int rate;

			cout << "bppimgs" << endl;

			fres << "bppimgs" << endl;
			sscanf_s(argv[++i], "%d", &num);
			cout << "read " << num << " for bppcodedimgs" << endl;
			fres << num << endl;
			for (int j = 0; j < num; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin);
				BPPcoded(imgrow, imgcol, bppin, psnr, bpp, rate);
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgwrd, "imgrd");
			}
		}
		else if (strcmp(argv[i], "-bppcodedimgsgc") == 0)
		{
			int num;
			double psnr, bpp;
			double bppin;
			int rate;

			cout << "bppimgs" << endl;

			fres << "bppimgs" << endl;
			sscanf_s(argv[++i], "%d", &num);
			cout << "read " << num << " for bppcodedimgs" << endl;
			fres << num << endl;
			for (int j = 0; j < num; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin);
				BPPcoded(imgrow, imgcol, bppin, psnr, bpp, rate);
				rate = GeneralEncoder(imgrow,imgcol,btver,graybit,"imapp.bppcodedimgsgc");
				bpp = (double)rate/imgrow/imgcol;
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << endl;
				Store(fres, imgwrd, "imgrd");
			}
		}
		else if (strcmp(argv[i], "-statistics2") == 0)
		{
			double psnr,bpp;
			int rate;
			BPPcoded(imgrow, imgcol, 2, psnr, bpp, rate);
			Statistics2(imgrow, imgcol);
		}
		else if (strcmp(argv[i], "-bppretrive1") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &graybit);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			DoTransformsCoded(imgrow, imgcol, btver, graybit);
			for (j = 0; j < bppinlen; j++)
			{
				BPPcodedGE(imgrow, imgcol, bppin[j], btver, graybit, psnr, bpp, rate);
				printf("Coded for #G %d in bpp[%lf] , psnr [%lf] \n", graybit, bpp, psnr);
				fres << "bppimg" << endl;
				if (psnr == std::numeric_limits<double>::infinity())
					fres << "Inf";
				else
					fres << psnr;
				fres << " " << bpp << " " << j << " " << bppin[j] << " " << graybit << endl;
				StoreParametersW(fres, imgrow, imgcol);
			}
			fres << "end" << endl;
		}
		else if (strcmp(argv[i], "-getbarimg") == 0)
		{
			int wscale, ww, wh;
			sscanf_s(argv[++i], "%d", &wscale);
			BarImageCreate(imgrow, imgcol, btver, sscale, graybit, wscale, ww, wh);
			fres << ww << " " << wh << endl;
			for (int r = 0;r<ww;r++)
			{
				for (int c = 0;c<wh;c++)
					fres << rimg[r][c] << " ";
				fres << endl;
			}
		}
		else if (strcmp(argv[i], "-barimgcmpstr") == 0)
		{
			double bppin,bpp,psnr;
			int rate;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &graybit);
			sscanf_s(argv[++i], "%lf", &bppin);
			ShowMessages = false;
			DoTransformsBarT(imgrow, imgcol, btver, 1, graybit);
			BPPBarT(imgrow, imgcol, bppin, btver, graybit, psnr, bpp, rate);
			Store(fres, imgprd, "codedimg");
			Store(fres, imgwrd, "barmaskimg");
			GetBarWedgeletsCodingStream(imgrow, imgcol);
		}
		else if (strcmp(argv[i], "-barimgmap1") == 0)
		{
			double psnr, bpp;
			int bppinlen, j;
			double bppin[100];
			int rate;
			sscanf_s(argv[++i], "%d", &btver);
			sscanf_s(argv[++i], "%d", &graybit);
			sscanf_s(argv[++i], "%d", &bppinlen);
			for (j = 0; j < bppinlen; j++)
			{
				sscanf_s(argv[++i], "%lf", &bppin[j]);
			}
			ShowMessages = false;
			//DoTransformsBarMasked1(imgrow, imgcol, btver,1, graybit);
			for (j = 0; j < bppinlen; j++)
			{
				
			}
			fres << "end" << endl;
		}
	}
		
	if (outputFileOpen)
	{
		fres << "end" << endl;
		fres.close();
	}
	if (outputFile2Open)
	{
		f2res << "end" << endl;
		f2res.close();
	}

	return 0;
}

void main(int argc, char *argv[])
{
	//char* args[] = {"pipeletstransform.exe","-imgDebug","Debug/img.b","-pdepth","2","-wedgelet","-pipelet","-dobuildtreeonly","-dolambdaPDebug","v:\\img.pipelet"};
	//main1(9,args);
	//char* args[] = {"pipeletstransform.exe","-resfilename","D:\\Projects\\Test\\pipeletsres\\compare\\inapp.res","-imgDebug","../compare.b","-pdepth","2","-wedgelet","-pipelet","-dobuildtreeonly","-dolambdaPDebug","v:\\img.pipelet"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\multilineimg.res","-img","v:\\multilineimg.b","-constant","-pdepth","4","-dobuildtreecodedonly","3","-dolambdaCodedCompress","8","v:\\inapp.multiline"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\multilineimg.res","-img","v:\\comp8.b","-constant","-pdepth","4","-dobuildtreecodedonly","2","-statistics2"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\multilineimg.res","-img","v:\\img.b","-constant","-pdepth","3","-dobuildtreecodedonly","2","-dolambdaCodedCompress","9","v:\\temp.compress"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\multilineimg.res","-img","v:\\img.b","-constant","-pdepth","5","-dobuildtreecodedonlyg","2","6","-bppcodedimgcompress","8","-bppcodedimgcompress","1","-bppcodedimgcompress","0.5"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\identification.res","-img","v:\\identification1.b","-constant","-pdepth","7","-bppcodedimgnbis","2","4","4","1"};
	//char* args[] = {"pipeletstransform.exe","-resfilename","v:\\distrib.res","-img","v:\\img.b","-constant","-pdepth","4","-dobuildtreecodedonlyg","2","8","-setlambda","1","10","-dolambdaCodedhm"};
	//char* args[] = { "pipeletstransform.exe","-resfilename","bar.res","-img","img.b","-constant","-pdepth","5","-dobuildtreebar","4","8","1","-bppbar","1","-bppbar","0.5","-bppbar","0.1","-dobuildtreebar","14","8","1","-bppbar","1","-bppbar","0.5","-bppbar","0.1"};
	//char* args[] = { "pipeletstransform.exe","-resfilename","v:\\genimg.res","-img","v:\\genimg.b","-constant","-wedgelet","-pdepth","4","-bpppaper4wedge","2","8","8","79","4","3.9","3.8","3.7","3.6","3.5","3.4","3.3","3.2","3.1","3","2.9","2.8","2.7","2.6","2.5","2.4","2.3","2.2","2.1","2","1.9","1.8","1.7","1.6","1.5","1.4","1.3","1.2","1.1","1","0.97","0.94","0.91","0.88","0.85","0.82","0.79","0.76","0.73","0.7","0.67","0.64","0.61","0.58","0.55","0.52","0.49","0.46","0.429999999999999","0.399999999999999","0.369999999999999","0.339999999999999","0.309999999999999","0.279999999999999","0.249999999999999","0.219999999999999","0.189999999999999","0.159999999999999","0.129999999999999","0.1","0.095","0.09","0.085","0.08","0.075","0.07","0.065","0.06","0.055","0.05","0.045","0.04","0.035","0.03","0.025","0.02","0.015","0.01" };
	//char* args[] = { "pipeletstransform.exe","-resfilename","v:\\genimg.res","-img","v:\\genimg.b","-constant","-wedgelet","-pdepth","5","-barimgcmpstr","2","8","0.1" };
	//char* args[] = { "pipeletstransform.exe","-resfilename","v:\\genimg.res","-img","v:\\genimg.b","-mask","v:\\maskimg.b","-constant","-pdepth","8","-bpppaper4wedgeinm","2","8","8","79","4","3.9","3.8","3.7","3.6","3.5","3.4","3.3","3.2","3.1","3","2.9","2.8","2.7","2.6","2.5","2.4","2.3","2.2","2.1","2","1.9","1.8","1.7","1.6","1.5","1.4","1.3","1.2","1.1","1","0.97","0.94","0.91","0.88","0.85","0.82","0.79","0.76","0.73","0.7","0.67","0.64","0.61","0.58","0.55","0.52","0.49","0.46","0.429999999999999","0.399999999999999","0.369999999999999","0.339999999999999","0.309999999999999","0.279999999999999","0.249999999999999","0.219999999999999","0.189999999999999","0.159999999999999","0.129999999999999","0.1","0.095","0.09","0.085","0.08","0.075","0.07","0.065","0.06","0.055","0.05","0.045","0.04","0.035","0.03","0.025","0.02","0.015","0.01" };
	char* args[] = { "pipeletstransform.exe","-resfilename","E:\\Mansouri\\Result\\paper1-4\\result.tmp","-img","v:\\genimg.b","-mask","v:\\maskimg.b","-constant","-pdepth","4","-bppbarmaskedstat","2","8","8","0.1" };
	if (argc>1){
		main1(argc,argv);
	}else{
		//main1(17,args);
		//main1(28,args);
		//main1(11,args);
		//main1(93, args);
		//main1(13, args);
		main1(14, args);
		getchar();
	}
}
