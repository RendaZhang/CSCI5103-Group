#ifndef INDEXNODE_H
#define INDEXNODE_H
#include <stack>
#include <iostream>
#include <fstream>

using namespace std;

class IndexNode
{
	public:
		const static int INDEX_NODE_SIZE = 64 ;
		const static int MAX_DIRECT_BLOCKS = 10 ;

	private:
		unsigned short mode;
		unsigned short nlink;
		unsigned short uid;
		unsigned short gid;
		int size;
		int directBlocks[MAX_DIRECT_BLOCKS];
		int indirectBlock;
		int doubleIndirectBlock;//Not yet implemented.
		int tripleIndirectBlock;//Not yet implemented.
		int atime;//Not yet implemented.
		int mtime ;//Not yet implemented.
		int ctime ;//Not yet implemented.

	public:
		IndexNode();
		void getBlocks(stack<int> &s);
		void setMode(short newMode);
		short getMode();
		void setNlink(short newNlink);
		short getNlink();
		void setUid(short newUid);
		short getUid();
		short getGid();
		void setGid(short newGid);
		void setSize(int newSize);
		int getSize();
		int getBlockAddress(int block);
		void setBlockAddress(int block , int address);
		void setAtime(int newAtime);
		int getAtime();
		void setMtime(int newMtime);
		int getMtime();
		void setCtime(int newCtime);
		int getCtime();
		void write(char * buffer, int offset);
		void read(char * buffer, int offset);
		char * toString();
		void copy(IndexNode& indexNode);
};
#endif
