#ifndef __HM_BINARYFILE__
#define __HM_BINARYFILE__

#include <cstring>
#include <cstdlib>

#define MEMORYSPACESZ 100000

class BinaryFile
{
public:
	BinaryFile(char filename[])
	{
		strcpy_s(this->filename,filename);
		f = NULL;
		length=0;
		position=0;
		bitpos=0;
		buff=0;
		isWriteMode = false;
	}

	~BinaryFile()
	{
		if (f!=NULL)
		{
			Close();
		}
	}

	void OpenRead()
	{
		fopen_s(&f,filename,"rb");
		isWriteMode = false;
	}

	void OpenWrite()
	{
		fopen_s(&f,filename,"wb");
		isWriteMode = true;
	}

	void Close()
	{
		if (isWriteMode && bitpos!=0)
		{
			bitpos=0;
			fwrite(&buff,1,1,f);
			position++;
			length++;
		}
		fclose(f);
		f = NULL;
	}

	void WriteBit(int v)
	{
		unsigned char vpos = (1<<bitpos);
		if (v==1)
			buff = buff | vpos;
		bitpos++;
		if (bitpos==8)
		{
			bitpos=0;
			fwrite(&buff,1,1,f);
			position++;
			length++;
			buff=0;
		}
	}

	void WriteBit(int v1, int v2)
	{
		WriteBit(v1);
		WriteBit(v2);
	}

	void WriteBit(int v1, int v2, int v3)
	{
		WriteBit(v1);
		WriteBit(v2);
		WriteBit(v3);
	}

	void WriteBits(int v, int bits)
	{
		for (int i=0;i<bits;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	void Write1Byte(int v)
	{
		for (int i=0;i<8;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	void Write2Byte(int v)
	{
		for (int i=0;i<16;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	int ReadBit()
	{
		if (bitpos==0)
		{
			fread(&buff,1,1,f);
			position++;
		}
		int vpos = (1<<bitpos);
		bitpos++;
		if (bitpos==8)
			bitpos=0;
		if ((buff & vpos)>0)
			return 1;
		return 0;
	}

	int ReadBits(int bits)
	{
		int v=0;
		for (int i=0;i<bits;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int Read1Byte()
	{
		int v=0;
		for (int i=0;i<8;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int Read2Byte()
	{
		int v=0;
		for (int i=0;i<16;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int GetLength()
	{
		return length;
	}

private:
	char filename[300];
	FILE* f;
	int length;
	int position;
	int bitpos;
	unsigned char buff;
	bool isWriteMode;
};

class MemoryFile
{
public:
	unsigned char memoryspace[MEMORYSPACESZ];
	MemoryFile()
	{
		length=0;
		position = 0;
		memset(memoryspace,0,sizeof(memoryspace));
	}

	~MemoryFile()
	{
	}

	void OpenRead()
	{
		position=0;
	}

	void Reset()
	{
		position=0;
		length=0;
		memset(memoryspace,0,sizeof(memoryspace));
	}

	void WriteBit(int v)
	{
		if (v==1)
			memoryspace[position/8] = memoryspace[position/8] | (1<<(position%8));
		position++;
		if (length<position)
			length=position;
	}

	void WriteBit(int v1, int v2)
	{
		WriteBit(v1);
		WriteBit(v2);
	}

	void WriteBit(int v1, int v2, int v3)
	{
		WriteBit(v1);
		WriteBit(v2);
		WriteBit(v3);
	}

	void WriteBits(int v, int bits)
	{
		for (int i=0;i<bits;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	void Write1Byte(int v)
	{
		for (int i=0;i<8;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	void Write2Byte(int v)
	{
		for (int i=0;i<16;i++)
		{
			WriteBit(v%2);
			v>>=1;
		}
	}

	int ReadBit()
	{
		if ( (memoryspace[position/8] & (1<<(position%8))) > 0)
		{
			position++;
			return 1;
		}
		else
		{
			position++;
			return 0;
		}
	}

	int ReadBits(int bits)
	{
		int v=0;
		for (int i=0;i<bits;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int Read1Byte()
	{
		int v=0;
		for (int i=0;i<8;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int Read2Byte()
	{
		int v=0;
		for (int i=0;i<16;i++)
		{
			v += (ReadBit()<<i);
		}
		return v;
	}

	int GetLengthBit()
	{
		return length;
	}

	int GetLengthByte()
	{
		return (int)ceil((double)length/8);
	}
	
private:	
	int length;
	int position;
};

#endif
