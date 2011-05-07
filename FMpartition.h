
#include <iostream>
#include <fstream>
#include <string>

#include <utility>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

#define TE_SET_TAG 0x04
#define FSA_SET_TAG 0x02
#define FSB_SET_TAG 0x01
#define TE_CLR_TAG 0xfb
#define FSA_CLR_TAG 0xfd
#define FSB_CLR_TAG 0xfe

//#define BAL_FACTOR 42 
//#define BAL_FACTOR (100-42) 

typedef short cint ;

class net {
public:
//	int connNum;
	list<int> conn;
	char tag;
//	bool te;
//	bool fsa;
	int posa;
//	bool fsb;
	int posb;

	net(){
		tag = 0;
//		te = false;
//		fsa = false;
//		fsb = false;
		posa = 0;
		posb = 0;
//		connNum = 0;
	};
	~net(){};
	void set_te (){
		tag |= TE_SET_TAG;
	}
	void set_fsa (){
		tag |= FSA_SET_TAG;
	}
	void set_fsb (){
		tag |= FSB_SET_TAG;
	}
	void clear_te (){
		tag &= TE_CLR_TAG;
	}
	void clear_fsa (){
		tag &= FSA_CLR_TAG;
	}
	void clear_fsb (){
		tag &= FSB_CLR_TAG;
	}
	bool is_te (){
		return ((tag & TE_SET_TAG) != 0);
	}
	bool is_fsa (){
		return ((tag & FSA_SET_TAG) != 0);
	}
	bool is_fsb (){
		return ((tag & FSB_SET_TAG) != 0);
	}
};


class cell {
public :
	int area;
	bool group;
	bool lock;
//	int connNum;
	int gain;
	list<int> conn;
	cell(){
		lock = false;
		group = false;
		gain = 0;
//		connNum=0;
	};
	~cell(){};
};




class bucket {
public:
	list <int> *link;
	vector < list<int>::iterator > remLink; 
	int size;
	int indexCur ;
	int cellDeg;
	bucket () {
		size = 0;
		indexCur = 0;
	};
	bucket (int max) {
		indexCur = 0;
		size = 0;
		cellDeg = max;
		int size = (max << 1) +1;
		link = new list <int> [size];
	};

	~bucket() {
		delete [] link;
	};

	void updateIndex(int gain) {
		if (size != 0) {
			int g = gain + cellDeg;
			while (link[g].empty()){
				--g;
			}
			indexCur = link[g].front();
		}
		return;
	}

	void remove(int gain, int c){
		link[gain + cellDeg].remove(c);
		-- size;
		if (c == indexCur){
			updateIndex (gain);
		} 
		return;
	}

	void clear (){
        delete [] link;
//        indexCur = 0;
  //      size = 0;
        int s = (cellDeg << 1) +1;
        link = new list <int> [s];
//		int max = (cellDeg << 1) ;
//		for (int i=0; i<=max; ++i ){
//			link[i].clear();
//		}
		return ;
	}
/*
	void insert (int gain, int cell){
		link[gain+cellDeg].push_back(cell);
		if (gain > cellArray[indexCur].gain){
			indexCur = cell;
		}
		return;
	}
*/
};

class FMpartition {
private:
	int cellNum;
	int netNum;
	int segNum;
	net * netArray;
	cell * cellArray;
	vector<cell*> cellLink;
	vector<net*> netLink;

	int totalArea;
	int groupAarea;
	//int groupBarea;
	int stepArea;
	int parLowArea;
	int parHighArea;
	int goldCutArea;
//	int otherCutArea;
	int pointDeltaArea;

	int cellDeg; 
	int cutSizeMin;

	int lastArea;
	int lastCutSize;

	list<int> histgram_area;
	list<int> histgram_cell;
	list<int> histgram_cutsize;
	// area, cell, cutsize

	int readANum(string s);
	int readConn(string s, int k);
	int calAreaLimit (int area);
	bool areaMoveVioLimit(int cellOnMove);
	void clearRecord ();
	void storeRecord (int area, int cell, int cutsize);
	void updateGroup();
public:
	int cutSize;
	int balFactor;
	list< cell * > groupA;
	list< cell * > groupB;
	bucket *cal;

	FMpartition(){
		//balFactor = BAL_FACTOR;
	};
	~FMpartition(){ 
		delete [] netArray;
		delete [] cellArray;
		delete cal;
	};
	int readfile(string fname);
	void restoreInitPartition(string fname);
	void initComputeGeneralArea(int balFac);
	int initPartition(int factor);
	int restorePartition();
	int initComputeGain();
	void initComputeArea();
	void initBucket();
	int selCell2Move();
	int moveCell(int cellOnMove, cell* p);
	int computeGainOnMove_rough();
	int computeGainOnMove_slow();
	int computeGainOnMove();
	void computeGainLoop ();
	int initPass(int factor, string fname);
	int onePass(int stage);
	int solutionPass(int factor, string fname);

	void outputResult(string filename);
/*	void printGroup();
	void printBucket();
	void printGain();
	void printCellGain(int c);
	void printAreaPar();
	void printQuality();
	void outputStatus(string filename);
*/
};

