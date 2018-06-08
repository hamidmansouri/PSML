#ifndef __HM_HUFFMANCODER__
#define __HM_HUFFMANCODER__

#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

struct HuffNode
{
	int code;
	int w;
	int left,right;
	int huffcodelen;
	bool operator < (const HuffNode &other)
	{
		return (*this).w < other.w;
	}
};

class HuffmanCoder
{
public:
	HuffmanCoder()
	{
		FillHuffTable();
	}
	int CodeRate(int hlen, unsigned char bits[], int begin, int end)
	{
		int hlen2 = (1<<hlen);
		int bestr = (1<<30);
		int r,i;
		for (i=0;i<hlen2;i++)
		{
			r = CodeRate(hlen,i,bits,begin,end);
			if (r<bestr)
				bestr = r;
		}
		return bestr;
	}
	int CodeRate(int hlen, int method, unsigned char bits[], int begin, int end)
	{
		int i,j,num;
		if (method == 0)
		{
			int z[2]; z[0]=0; z[1]=0;
			for (i=begin;i<end;i++)
				z[bits[i]]++;
			if (z[0]==0 || z[1]==0)
				return 1;
			return (1<<30);
		}		
		int rate=0;
		for(i=begin;i<end;i+=hlen)
		{
			num=0;
			for (j=0;j<hlen;j++)
				num = num*2+bits[i+j];
			rate += hufftable[hlen][method][num];
		}
		return rate;
	}
private:
	int hufftable[10][1024][1024];
	
	void FillHuffTable()
	{
		int i;
		for (i=2;i<6;i++)
			FillHuffTable(i);
	}

	void FillHuffTable(int hlen)
	{
		int i;
		int hlen2 = (1<<hlen);
		for(i=1;i<hlen2;i++)
			FillHuffTable(hlen,i);
	}

	void FillHuffTable(int hlen, int method)
	{
		int i,j;
		int hlen2 = (1<<hlen);
		int p0 = method;
		int p1 = hlen2-method;
		vector<HuffNode> nodes;
		for (i=0;i<hlen2;i++)
		{
			HuffNode node;
			node.code=i;
			node.left = node.right = NULL;
			node.w = 1;
			for (j=0;j<hlen;j++)
				if (((1<<j) & i)>0)
					node.w*=p1;
				else
					node.w*=p0;			
			nodes.push_back(node);
		}
		
		int cnt=0;
		vector<HuffNode>::iterator beg;
		while (nodes.size()-cnt>1)
		{			
			beg = nodes.begin()+cnt;
			sort(beg,nodes.end());
			HuffNode *n1 = &(*beg);
			HuffNode *n2 = &(*(beg+1));
			HuffNode node;
			node.code = -1;
			node.w = n1->w+n2->w;
			node.left = cnt;
			node.right = cnt+1;
			nodes.push_back(node);
			cnt+=2;
		}
		nodes.back().huffcodelen = 0;
		for(vector<HuffNode>::reverse_iterator it = nodes.rbegin(); it!=nodes.rend();it++)
		{
			if ((*it).code == -1)
			{				
				nodes[(*it).left].huffcodelen = (*it).huffcodelen+1;
				nodes[(*it).right].huffcodelen = (*it).huffcodelen+1;
			}
			else
			{
				hufftable[hlen][method][(*it).code] = (*it).huffcodelen;
			}
		}
	}
};

#endif