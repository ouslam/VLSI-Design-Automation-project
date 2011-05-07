#include "FMpartition.h"

using namespace std;

int FMpartition::readANum(string s){

	int num=0;
	for (unsigned int i=0; i< s.length(); ++i ){
		if ( s[i]>= '0' && s[i] <='9') {
			num = (num * 10) + (s[i] - '0');
		} else {
			cout << "Not a all-digit string"<< endl;
			exit (315);
		}
	}
	return num;
}


int FMpartition::readConn(string s, int k){
	int num = 0;
	bool digcon = false;
	
//	netArray[k].connNum = 0;


	for (unsigned int i=0; i< s.length(); ++i ){
		if ( s[i]>= '0' && s[i] <='9') {
			num = (num * 10) + (s[i] - '0');
			digcon = true;
		} else {
			if (digcon) {
				digcon = false;
//				++ netArray[k].connNum;
				netArray[k].conn.push_back(num);
//				++ cellArray[num].connNum;
				cellArray[num].conn.push_back(k);
				num = 0;
			}
		}
	}
	if (digcon) {
//		++ netArray[k].connNum;
		netArray[k].conn.push_back(num);
//		++ cellArray[num].connNum;
		cellArray[num].conn.push_back(k);
	}
	return 0;
}



int FMpartition::readfile(string fname){
	string line;

	ifstream inFile(fname.c_str());
	if (!inFile) {
		cout << "File open error" << endl;
		exit (301);
	}
	getline(inFile, line);
	cellNum = readANum(line);
//	if (cellNum > 500 ) {
//		segNum = (cellNum >> 3) + 1;
//	} else {
		segNum = cellNum+1;
//	}
	cellArray = new cell[cellNum+1];
//	cout << line << endl;
//	cout << "There are " << cellNum << " cells in the file" << endl;


	getline(inFile, line);
	netNum = readANum(line);
//	cout << "There are " << netNum << " nets in the file" << endl;
	netArray = new net[netNum];

	// processing cell information
	// cell
	totalArea = 0;
	cell * p = cellArray;
	++p;

	for (int i=1; i<= cellNum; ++i) {
		cellLink.push_back(p);
		getline(inFile, line);
		p->area = readANum(line);
		//p->connNum = 0;
		p->gain = 0;
		totalArea += p->area;

//		cellArray[i].area = readANum(line);
//		cellArray[i].connNum = 0;
//		totalArea += cellArray[i].area;
		++p;
//		cout << i << " "<< newCell.area << endl;
	}

	stepArea = cellArray[1].area;
	for (int i=2; i<= cellNum; ++i) {
		if (cellArray[i].area > stepArea) {
			stepArea = cellArray[i].area;
		}
	}

//	initComputeGeneralArea( balFactor );
//	cout << "total area is: " << totalArea << endl;

//	cout << "net connection" << endl;
	net * q = netArray;
	for (int i=0; i< netNum; ++i) {
		netLink.push_back(q);
		getline(inFile, line);
		readConn(line, i);

//		netArray[i].te = false;
//		netArray[i].fsa = false;
//		netArray[i].fsb = false;
		
		++q;
//		cout << i << " " << netArray[i].connNum << endl;
	}


	unsigned int deg = cellArray[1].conn.size();
	for (int i=2; i<= cellNum; ++i) {
		if (cellArray[i].conn.size() > deg){
			deg = cellArray[i].conn.size();
		}
	}
	cellDeg = (int) deg;
	// create bucket structure
	cal = new bucket(cellDeg);


//	cout << "cell Connection"<< endl;
//	for (int i=0; i< cellNum; ++i) {
//		cout << i << " " << cellArray[i].connNum << endl;
//	}

	inFile.close();
	return 0;
}

void FMpartition::initComputeGeneralArea (int balFac ) {
	goldCutArea = (int) ((double) (balFac * totalArea) / (double) 100 );
	pointDeltaArea = 1;
	if (((balFac * (totalArea % 100))%100) == 0){
		// potential overflow problem
		pointDeltaArea = 0;
	}
	//otherCutArea = totalArea - goldCutArea - pointDeltaArea ;	
	//parLowArea = goldCutArea - stepArea;
//	parLowArea = (goldCutArea >> 1);
	//parHighArea = totalArea - parLowArea;

}
void FMpartition::updateGroup(){

	groupA.clear();
	groupB.clear();
	for (vector < cell * >::const_iterator p = cellLink.begin(); p!= cellLink.end(); ++p) {
		if ((*p)->group){
			groupA.push_back(*p);
		} else {
			groupB.push_back(*p);
		}
	}	 
/*	for (int i = 1; i<= cellNum ; ++i ) {
		if ( cellArray[i].group ) {
			groupA.push_back(i);
		} else {
			groupB.push_back(i);
		}
	}
*/	return;
}

void FMpartition::restoreInitPartition(string fname){
	string line;
    ifstream inFile(fname.c_str());
    if (!inFile) {
        cout << "File open error" << fname << endl;
        exit (302);
    }

    getline(inFile, line);
    cutSize = readANum(line);
	getline(inFile, line);
	groupAarea = readANum(line);
	getline(inFile, line);

	// groupA
	getline(inFile, line);
    int num = 0;
    bool digcon = false;

    for (unsigned int i=0; i< line.length(); ++i ){
        if ( line[i]>= '0' && line[i] <='9') {
            num = (num * 10) + (line[i] - '0');
            digcon = true;
        } else {
            if (digcon) {
                digcon = false;
				cellArray[num].group= true;
                groupA.push_back(& cellArray[num]);
                num = 0;
            }
        }
    }
    if (digcon) {
		cellArray[num].group= true;
        groupA.push_back(& cellArray[num]);
    }

    for (vector < cell * >::const_iterator p3 = cellLink.begin(); p3!= cellLink.end(); ++p3) {
    	if (!(*p3)->group){
		    groupB.push_back(*p3);
	    }
    }
	cellArray[0].lock = true;
	// groupB
	getline(inFile, line);
// LOCK SWITCH
/*
	// lock
	getline(inFile, line);
	num = 0;
	digcon = false;
    for (unsigned int i=0; i< line.length(); ++i ){
        if ( line[i]>= '0' && line[i] <='9') {
            num = (num * 10) + (line[i] - '0');
            digcon = true;
        } else {
            if (digcon) {
                digcon = false;
                cellArray[num].lock = true;
                num = 0;
            }
        }
    }
    if (digcon) {
        cellArray[num].lock= true;
    }
*/	
	inFile.close();
}


int FMpartition::initPartition(int factor){
	// find max-connected cell put it into group true, lock it.
	// continue drad connected cell into group true until it reach the limit. 


	// computer GroupLimit area
	int groupLimit;
//	groupLimit = factor * totalArea / 100 - stepArea;
	groupLimit = factor * totalArea / 100 ;

	// find max connected cell
/*	unsigned int maxconn = cellArray[1].conn.size();
	int congestCell = 1;
	for (int i = 2; i<= cellNum ; ++i ) {
		if (cellArray[i].conn.size() > maxconn ){
			congestCell = i;
			maxconn = cellArray[i].conn.size();
		} else if (cellArray[i].conn.size() == maxconn ){
			if (cellArray[i].area > cellArray[congestCell].area){
				congestCell = i;
			}
		}
	}
*/
	for (vector < cell * >::const_iterator p1 = cellLink.begin(); p1!= cellLink.end(); ++p1) {
		(*p1)->group = false;
	}

	groupA.clear();
	groupB.clear();
	
	
	unsigned int maxconn = 0;
	cell * pCell = NULL; 
	for (vector < cell * >::const_iterator p1 = cellLink.begin(); p1!= cellLink.end(); ++p1) {
		if ((*p1)->conn.size() > maxconn){
			maxconn = (*p1)->conn.size();
			pCell = (*p1);
		} else if ((*p1)->conn.size() == maxconn){
			if ((*p1)->area > pCell->area) {
				pCell = (*p1);
			}
		}
	}
	//cout << "congest cell is: " << congestCell+1 << " with max connection of " << maxconn << endl;
	// put it into group A
	// group A is true
//	int groupAarea;
	pCell->group = true; 
	//groupA.push_back(congestCell);
	groupA.push_back(pCell);
	groupAarea = pCell->area;

	
	if (groupAarea >= groupLimit ) {
                        for (vector < cell * >::const_iterator p3 = cellLink.begin(); p3!= cellLink.end(); ++p3) {
                            if (!(*p3)->group){
                                groupB.push_back(*p3);
                            }
                        }

		return 0;
	}

	// drag cell
	list<cell *> l;
	l.push_back(pCell);
	while (!l.empty()) {
		pCell = l.front();
		l.pop_front();
		//cout << "ite:" << ite << endl;
		for(list<int>::const_iterator p = pCell->conn.begin(); p!= pCell->conn.end(); ++p ) {
			for (list<int>::const_iterator q = netArray[*p].conn.begin(); q != netArray[*p].conn.end(); ++q) {
				//cout << "p is: " << *p << "q is: " << *q << endl;
				// if *q is in group B
				if (! cellArray[*q].group){
					cell * p2 = & cellArray[*q];
					//cout << "groupAarea" << groupAarea << endl;
					groupAarea += p2->area;
					p2->group = true;
					groupA.push_back(p2);
					l.push_back(p2);
					if (groupAarea + p2->area > groupLimit) {
						for (vector < cell * >::const_iterator p3 = cellLink.begin(); p3!= cellLink.end(); ++p3) {	
							if (!(*p3)->group){
								groupB.push_back(*p3);
							}
						}
						//for (int i = 1; i<= cellNum ; ++i ) {
						//	if (!cellArray[i].group){
						//		groupB.push_back(i);
						//	}
						//}
						return 0;
					}
				}
			}
		}
	}

	return 0;
}

int FMpartition::calAreaLimit (int area){
	//int otherCutArea = totalArea - goldCutArea ;

//	int thre = ((area << 1) > totalArea) ? (otherCutArea ) : goldCutArea ;
//	return (area > thre) ? ( area - thre - pointDeltaArea ) : ( thre - area );

	return (area - goldCutArea)> 0 ? (area - goldCutArea): (goldCutArea - area);

/*	int l = area - goldCutArea;
	l = (l>0)? l : (0-l); 
	int h = totalArea - goldCutArea - area;
	h = (h>0)? h:(0-h);
	return (l>h)? h:l;
*/	
}

void FMpartition::storeRecord (int area, int cell, int cutsize){
//	cout << "Cell " << cell << " is moved into group " << cellArray[cell].group << ". cutsize is: "<< cutsize <<endl;
//	printGroup();
	histgram_area.push_back(area);
	histgram_cell.push_back(cell);
	histgram_cutsize.push_back(cutsize);
}

void FMpartition::clearRecord (){
	histgram_area.clear();
	histgram_cell.clear();
	histgram_cutsize.clear();
}

void FMpartition::initBucket(){
	cal->clear();
	cal->remLink.clear();
	cal->remLink.resize(2*cellNum+1);
	cal->size = cellNum;
//	cout << "Insert gain" <<endl;
//	printGain();
	int maxgain =  -1;
	for (int i = 1; i<= cellNum ; ++i ) {
// LOCK SWITCH
/*
		if (cellArray[i].lock) {
			--(cal->size);
			continue;
		}
*/
	//for (vector < cell * >::const_iterator p = cellLink.begin(); p!= cellLink.end(); ++p) {
		int tmp = cellArray[i].gain + cellDeg;
//		cout << "index is: " << tmp << endl;
		cal->link[tmp].push_back(i);
		list<int>::iterator pp = cal->link[tmp].end() ;
		--pp;
		cal->remLink[i] = pp;
		//cout << i << *pp << endl;
//		cout << "Step:"<< endl;
		if (tmp > maxgain) {
			maxgain = tmp;
			cal->indexCur = i;
		} else if (tmp == maxgain) {
			if (cellArray[i].area > cellArray[cal->indexCur].area) {
				cal->indexCur = i;
			}
		}
		//cout << "gain " << i << " : " << cellArray[i].gain << endl;
	}
	return;
}


void FMpartition::initComputeArea() {

	groupAarea = 0;
	for (list<cell *>::const_iterator p = groupA.begin(); p!= groupA.end(); ++ p){
		groupAarea += (*p)->area;
	}
	
	return;
	/*
	for (int i = 1; i<= cellNum ; ++i ) {
		if ( cellArray[i].group ) {
			groupAarea += cellArray[i].area;
		} 
	}
	return;
	*/
}

int FMpartition::initComputeGain(){
    for (vector < cell * >::const_iterator p = cellLink.begin(); p!= cellLink.end(); ++p) {
        (*p)->gain = 0;
    }
//	for (int i=1; i<= cellNum; ++i){
//		cellArray[i].gain = 0;
//	}
	// compute gain 
	// iteration of nets
	cutSize = netNum;
	for (int i = 0; i< netNum ; ++i ) {
		netArray[i].tag = 0;
//		netArray[i].fsa = false;
//		netArray[i].fsb = false;
//		netArray[i].te = false;

		int ga = 0;
		int gb = 0;
		// record the only cell that belongs to one group.
		int posA = 0 ;
		int posB = 0 ;
		for (list<int>::const_iterator p = netArray[i].conn.begin(); p!= netArray[i].conn.end(); ++p ) {
			if ( cellArray[*p].group ) {
				posA = *p;
				++ga;
			} else {
				posB = *p;
				++gb;
			}
			if ( ga> 1 && gb > 1) {
				break;
			}
		}
		if (ga>1 && gb>1){
			continue;
		} else if ((ga == 0) || (gb == 0)) {
			--cutSize;
			//netArray[i].te = true;
			netArray[i].set_te();
			for (list<int>::const_iterator p = netArray[i].conn.begin(); p!= netArray[i].conn.end(); ++p ) {
				-- cellArray[*p].gain ;
			}
		} else {
			if (ga == 1){
				//netArray[i].fsa = true;
				netArray[i].set_fsa();
				netArray[i].posa = posA;
				++ cellArray[posA].gain;
			} 
			if (gb == 1){
				//netArray[i].fsb = true;
				netArray[i].set_fsb();
				netArray[i].posb = posB;
				++ cellArray[posB].gain;
			}
		}
	}
	return 0;
}

bool FMpartition::areaMoveVioLimit(int cellOnMove){
	if (cellArray[cellOnMove].group) {
		return ( (groupAarea - cellArray[cellOnMove].area) < parLowArea );
	} else {
		return ( (groupAarea + cellArray[cellOnMove].area) > parHighArea );
	}
}

int FMpartition::selCell2Move (){

    if (! areaMoveVioLimit (cal->indexCur)) {
        return cal->indexCur;
    }

	// test area constraint
	int cellOnMove = cal->indexCur;

//	cout << "initial cell: "<< cellOnMove <<endl ;

	list < int > areaVio;

	do {
		areaVio.push_back(cellOnMove);
		cal->link[cellArray[cellOnMove].gain + cellDeg].erase (cal->remLink[cellOnMove] );
		//cal->link[cellArray[cellOnMove].gain + cellDeg].remove(cellOnMove);
		-- cal->size;

		//cout << "Cell "<< cellOnMove << " is not moved due to area constraint" << endl;

		if (cal->size != 0) {
			//if (cal->size<=2) {
			//cout <<"size: "<< cal->size << " OnMove: " << cellOnMove<< endl;
			//	printBucket();
			//}
			int g = cellArray[cellOnMove].gain + cellDeg;
			while (cal->link[g].empty()){
				--g;
			}

			cellOnMove = cal->link[g].front();
			//cout << "New cell: " << cellOnMove << g <<endl;
		} else {
			// stop 
			return 0;
		}
	} while ( areaMoveVioLimit(cellOnMove) ) ;
	//cout << "after" << endl;
	for (list<int>::const_iterator p = areaVio.begin(); p != areaVio.end(); ++p){
//		cout <<"gain: " <<cellArray[*p].gain + cellDeg <<" " << *p << " is pushed back" << endl;
		++ cal->size;
		cal->link[cellArray[*p].gain + cellDeg].push_back(*p);
		list<int>::iterator pp = cal->link[cellArray[*p].gain + cellDeg].end();
		-- pp;
		cal->remLink[*p] = pp;
	}
//	cout <<"cur: "<< cal->indexCur << endl;
	return cellOnMove;
}
/*
int FMpartition::moveCell (int cellOnMove){
	// remove the cellOnMove from bucket
	--cal->size;
	cal->link[cellArray[cellOnMove].gain + cellDeg].remove(cellOnMove);

	// lock the cell
	cellArray[cellOnMove].lock = true;

	// change the group 
	if (cellArray[cellOnMove].group) {
		groupAarea -= cellArray[cellOnMove].area;
	} else {
		groupAarea += cellArray[cellOnMove].area;
	}
	cellArray[cellOnMove].group = ! cellArray[cellOnMove].group;
	// update cutSize
	cutSize -= cellArray[cellOnMove].gain;
	cout << "Cell " << cellOnMove <<" is moved into group "<<cellArray[cellOnMove].group 
		<<" cut size is: " << cutSize << " groupAarea is: " << groupAarea << endl;
	return 0;
}
*/

int FMpartition::moveCell (int cellOnMove, cell * p){
    // remove the cellOnMove from bucket
    --cal->size;
//    cell * p;
//    p = &cellArray[cellOnMove];

    //cal->link[cellArray[cellOnMove].gain + cellDeg].remove(cellOnMove);
//	cout <<"here" << endl ;
    //cal->link[p->gain + cellDeg].remove(cellOnMove);
	cal->link[p->gain + cellDeg].erase(cal->remLink[cellOnMove]);
    // lock the cell
    //cellArray[cellOnMove].lock = true;
    p->lock = true;

    // change the group
    //if (cellArray[cellOnMove].group) {
    //  groupAarea -= cellArray[cellOnMove].area;
    //} else {
    //  groupAarea += cellArray[cellOnMove].area;
    //}
    if (p->group) {
        groupAarea -= p->area;
    } else {
        groupAarea += p->area;
    }
    //cellArray[cellOnMove].group = ! cellArray[cellOnMove].group;
    p->group = ! p->group;
    // update cutSize
    //cutSize -= cellArray[cellOnMove].gain;
    cutSize -= p->gain;
//  cout << "Cell " << cellOnMove <<" moved to g "<<cellArray[cellOnMove].group
//      <<" cut: " << cutSize << " area: " << groupAarea << endl;
    return 0;
}
/*
int FMpartition::computeGainOnMove_slow (){
	// test area constraint
	int cellOnMove = selCell2Move();
	
	if (cellOnMove == 0){
		return 2;
	}
//	cout << cellOnMove <<" in group " << cellArray[cellOnMove].group << " is moved" <<endl; 

	moveCell(cellOnMove);


	if (cal->size == 0){
		return 2;
	}

	// update gain
//	int t = cutSize;
	initComputeGain();
//	cout <<"Different t: " << t << " cutSize: "<< cutSize <<endl;
//	cutSize = t;

	cal->clear();
	int maxgain =  -1;
	for (int i = 1; i<= cellNum ; ++i ) {
		if (cellArray[i].lock){
			continue;
		}
		int tmp = cellArray[i].gain + cellDeg;
		cal->link[tmp].push_back(i);
		if (tmp > maxgain) {
			maxgain = tmp;
			cal->indexCur = i;
		} else if (tmp == maxgain) {
			if (cellArray[i].area > cellArray[cal->indexCur].area) {
				cal->indexCur = i;
			}
		}
	}


//	cout <<"After move cell: "<< cellOnMove << " group: "<< cellArray[cellOnMove].group << " CutSize is: "<< cutSize  <<endl;

	storeRecord(groupAarea, cellOnMove, cutSize);

	return 0;
}
*/


/*
 * Function : Move a cell
 */
/*
int FMpartition::computeGainOnMove_rough (){
	// test area constraint
	int cellOnMove = selCell2Move();
	
	if (cellOnMove == 0){
		return 2;
	}
//	cout << cellOnMove <<" in group " << cellArray[cellOnMove].group << " is moved" <<endl; 

	moveCell(cellOnMove);


	if (cal->size == 0){
		return 2;
	}

	// update bucket according to the moved cell
	set<int> newEvent;

	for (list<int>::const_iterator p1 = cellArray[cellOnMove].conn.begin(); p1!= cellArray[cellOnMove].conn.end(); ++p1 ) {
		for (list<int>::const_iterator p2 = netArray[*p1].conn.begin(); p2 != netArray[*p1].conn.end(); ++ p2){
			if (! cellArray[*p2].lock) {
				if (newEvent.find(*p2) == newEvent.end()){
					newEvent.insert(*p2);
				}
			}
		}
	}
	if (0){
	cout << "impacted cell:"<<endl;
	for (set<int>::const_iterator p1 = newEvent.begin(); p1 != newEvent.end(); ++ p1){
		cout << *p1 << " ";
	}
	cout << endl;
	}

	for (set<int>::const_iterator p1 = newEvent.begin(); p1 != newEvent.end(); ++ p1){
		int g = 0;
		for (list<int>::const_iterator p2 = cellArray[*p1].conn.begin(); p2 != cellArray[*p1].conn.end(); ++p2){
			int ga=0;
			for ( list<int>::const_iterator p3 = netArray[*p2].conn.begin(); p3 != netArray[*p2].conn.end(); ++p3){
				if (cellArray[*p1].group == cellArray[*p3].group){
					++ga;
				}
			}
			if (ga == 1){
				++g;
			} else if (ga == (int)netArray[*p2].conn.size()){
				--g;
			}
		}
		// update bucket cal->indexCur
		if (g != cellArray[*p1].gain){
//			cout << "Remove Cell -- gain " <<cellArray[*p1].gain <<" cell: " << *p1 <<endl;
			cal->link[cellArray[*p1].gain + cellDeg].remove(*p1);
			cal->link[g + cellDeg].push_back(*p1);
			cellArray[*p1].gain = g;
		}
		if (g > cellArray[cal->indexCur].gain ){
			cal->indexCur = *p1;
		}
	}

	
	if ( cal->indexCur == cellOnMove) {
		int g = cellArray[cal->indexCur].gain + cellDeg;
		while (cal->link[g].empty()){
			--g;
		}
		cal->indexCur = cal->link[g].front();
	}

//	cout <<"After move cell: "<< cellOnMove << " group: "<< cellArray[cellOnMove].group << " CutSize is: "<< cutSize  <<endl;

	storeRecord(groupAarea, cellOnMove, cutSize);

	return 0;
}
*/

/*
 * Function:
 * move a cell from group B to group A
 */
int FMpartition::computeGainOnMove(){
	if (cutSize > lastCutSize ) {
		return 2;
	}
	//	printBucket();
	// test area constraint
	int cellOnMove = selCell2Move();
	
	if (cellOnMove == 0){
		return 2;
	}
//	cout << "OnMove: " << cellOnMove << endl;
	cell * pCell = & cellArray[cellOnMove];
	moveCell(cellOnMove, pCell);


	if (cal->size == 0){
		return 2;
	}
//	cout << "after moving cell" << endl;

	// update bucket according to the moved cell
	// record incremental change
	map <int, int> changeRecord;
	set <int> mapKeys;


	for (list<int>::const_iterator p1 = pCell->conn.begin(); p1!= pCell->conn.end(); ++p1 ) {
		map <int, int> incrChangeRecord;
		net * pNet =  &netArray[*p1];
		unsigned int swth = pNet->conn.size() ;
		if (swth > 3) {
			//if (netArray[*p1].te){
			if ( pNet->is_te()){
				//te -> fs
				//netArray[*p1].te = false;
				pNet->clear_te();
				for (list<int>::const_iterator p2 = pNet->conn.begin(); p2 != pNet->conn.end(); ++p2){
					// cellOnMove has been locked.
					if ( ! cellArray[*p2].lock){
						incrChangeRecord[*p2] = 1;
					}
				}

				if (pCell->group){
					//netArray[*p1].fsa = true;
					pNet->set_fsa();
					pNet->posa = cellOnMove;
				} else {
					//netArray[*p1].fsb = true;
					pNet->set_fsb();
					pNet->posb = cellOnMove;
				}
			//} else if (netArray[*p1].fsb) {
			} else if (pNet->is_fsb()) {
				//netArray[*p1].fsb = false;
				pNet->clear_fsb ();
				if (pCell->group){
					// fsb -> te
					//netArray[*p1].te = true;
					pNet->set_te();
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2 != pNet->conn.end(); ++p2){
						// cellOnMove has been locked.
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = -1;
						}
					}
				} else {
					if ( ! cellArray[pNet->posb].lock){
						incrChangeRecord[pNet->posb] = -1;
					}
				}
			//} else if (netArray[*p1].fsa) {
			//	netArray[*p1].fsa = false;
			} else if (pNet->is_fsa()) {
				pNet->clear_fsa();
				if ( pCell->group ){
					if ( ! cellArray[pNet->posa].lock){
						incrChangeRecord[pNet->posa] = -1;
					}
				} else {
					//netArray[*p1].te = true;
					pNet->set_te();
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2 != pNet->conn.end(); ++p2){
						// cellOnMove has been locked.
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = -1;
						}
					}
				}
			} else {
				int ga = 0;
				int gb = 0;
				// record the only cell that belongs to one group.
				int posA = 0 ;
				int posB = 0 ;
				for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2 ) {
					if ( cellArray[*p2].group ) {
						posA = *p2;
						++ga;
					} else {
						posB = *p2;
						++gb;
					}
					if ( ga> 1 && gb > 1) {
						goto cont_label;
						//break;
					}
				}

				if (ga == 1){
					//netArray[*p1].fsa = true;
					pNet->set_fsa();
					pNet->posa = posA;
					if ( ! cellArray[posA].lock){
						incrChangeRecord[posA] = 1;
					}
				} else if (gb == 1){
					//netArray[*p1].fsb = true;
					pNet->set_fsb();
					pNet->posb = posB;
					if ( ! cellArray[posB].lock){
						incrChangeRecord[posB] = 1;
					}
				}
			}
		} else if (swth == 3){
			//if (netArray[*p1].te){
			//	netArray[*p1].te = false;
			if (pNet->is_te()){
				pNet->clear_te();
				if ( pCell->group ){
					// cell has already been moved
					//netArray[*p1].fsa = true;
					pNet->set_fsa();
					pNet->posa = cellOnMove;
				} else {
					//netArray[*p1].fsb = true;
					pNet->set_fsb();
					pNet->posb = cellOnMove;
				}
				for (list<int>::const_iterator p2 = pNet->conn.begin(); p2 != pNet->conn.end(); ++p2){
					// cellOnMove has been locked.
					if ( ! cellArray[*p2].lock){
						incrChangeRecord[*p2] = 1;
					}
				}

			//} else if (netArray[*p1].fsa){
			//	netArray[*p1].fsa = false;
			} else if (pNet->is_fsa()){
				pNet->clear_fsa();
				if ( pCell->group ) {
					//netArray[*p1].fsb = true;
					pNet->set_fsb();
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2){
						if ( *p2 != pNet->posa && *p2 != cellOnMove ) {
							pNet->posb = *p2;
							break;
						} 
					}
					// fsa -> ##
					if ( ! cellArray[pNet->posa].lock){
						incrChangeRecord[pNet->posa] = -1;
					}
					// ## -> fsb
					if ( ! cellArray[pNet->posb].lock){
						incrChangeRecord[pNet->posb] = 1;
					}
				} else {
					//netArray[*p1].te = true;
					pNet->set_te();
					// ## -> te
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2){
						// cellOnMove has been locked.
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = -1;
						}
					}
				}
			} else {
				//if (netArray[*p1].fsb){
				//netArray[*p1].fsb = false;
				pNet->clear_fsb();
				if ( pCell->group ) {
					//netArray[*p1].te = true;
					pNet->set_te();
					// ## -> te
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2){
						// cellOnMove has been locked.
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = -1;
						}
					}
				} else {
					//netArray[*p1].fsa = true;
					pNet->set_fsa();
					for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2){
						if ( *p2 != pNet->posb && *p2 != cellOnMove ) {
							pNet->posa = *p2;
							break;
						}
					}
					// fsb -> ##
					if ( ! cellArray[pNet->posb].lock){
						incrChangeRecord[pNet->posb] = -1;
					}
					// ## -> fsa
					if ( ! cellArray[pNet->posa].lock){
						incrChangeRecord[pNet->posa] = 1;
					}
				}
			}
		} else {
//		} else if (netArray[*p1].connNum == 2){
//			if (netArray[*p1].te){
//				netArray[*p1].te = false;
//				netArray[*p1].fsa = true;
//				netArray[*p1].fsb = true;
			if (pNet->is_te()){
				//netArray[*p1].te = false;
				//netArray[*p1].fsa = true;
				//netArray[*p1].fsb = true;
				pNet->tag = 0x03;
				for (list<int>::const_iterator p2 = pNet->conn.begin(); p2!= pNet->conn.end(); ++p2){
					if (cellArray[*p2].group){
						pNet->posa = *p2;
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = 2;
						}
					} else {
						pNet->posb = *p2;
						if ( ! cellArray[*p2].lock){
							incrChangeRecord[*p2] = 2;
						}
					}
				}
			} else {
				// if netArray[*p1].fsa == true;
				// if netArray[*p1].fsb == true;

				//netArray[*p1].fsa = false;
				//netArray[*p1].fsb = false;
				//netArray[*p1].te = true;
				pNet->tag = 0x04;
				// fsa -> te
				if ( ! cellArray[pNet->posa].lock){
					incrChangeRecord[pNet->posa] = -2;
				}
				// fsb -> te
				if ( ! cellArray[pNet->posb].lock){
					incrChangeRecord[pNet->posb] = -2;
				}
			}
		}
cont_label:
//		cout << "Incre table "<<endl;
		for (map <int, int>::const_iterator p2=incrChangeRecord.begin(); p2!= incrChangeRecord.end() ; ++ p2 ){
//			cout << "Cell: "<< p2->first << " Increment: "<< p2->second << endl;
			if ( mapKeys.find(p2->first) != mapKeys.end()) {
				changeRecord[p2->first] += p2->second;
			} else {
				mapKeys.insert(p2->first);
				changeRecord[p2->first] = p2->second;
			}
		}
	}

	// update bucket cal->indexCur
	for (map<int,int>::const_iterator p1 =changeRecord.begin(); p1!= changeRecord.end() ; ++ p1){
		if (p1->second != 0) {
			//int maxgain = cellArray[cal->indexCur].gain;

			//cout << p1->first << " changed with: " << p1->second << endl;
			// update link
			int g = cellArray[p1->first].gain + cellDeg;
			//cal->link[g].remove(p1->first);
			cal->link[g].erase(cal->remLink[p1->first]);
			g += p1->second;
			cal->link[g].push_back(p1->first);
			list<int>::iterator pp = cal->link[g].end();
			--pp;
			cal->remLink[p1->first] = pp;
			cellArray[p1->first].gain += p1->second;
			// update indexCur
			//if (p1->first == cal->indexCur) {
			//	if (p1->second <0) {
			//		int g = maxgain + cellDeg;
			//		while (cal->link[g].empty()){
			//			--g;
			//		}
			//		cal->indexCur = cal->link[g].front();
			//	}
			//} else if (cellArray[p1->first].gain > maxgain){
				//cout << "exchange: "<< p1->first <<" with " << cal->indexCur << endl;
			if ( cellArray[p1->first].gain > cellArray[cal->indexCur].gain ){
				cal->indexCur = p1->first;
			}
		}
	}
    if (cellArray[cal->indexCur].lock) {
        int g = cellArray[cal->indexCur].gain + cellDeg;
        while (cal->link[g].empty()){
            --g;
        }
        cal->indexCur = cal->link[g].front();
    }

//		cout << "after adjust"<< endl;
//		printBucket();


	storeRecord(groupAarea, cellOnMove, cutSize);
//	histgram.push_back (make_pair (cellOnMove, cutSize));
	return 0;
}



int FMpartition::restorePartition(){
	// pick up last best solution
	int exitVal = 0; 
	int lowDeltaArea = totalArea;
	int tokenArea = totalArea;
	int lowCutSize = netNum;
	list<int>::const_iterator p1,p2,p3;
	for (p1 = histgram_area.begin(),p2 = histgram_cutsize.begin() ; p1 != histgram_area.end();  ++p1, ++p2){
		int a = calAreaLimit(*p1) ;
//		cout <<"lowDeltaArea: "<< lowDeltaArea <<" tokenArea: "<< tokenArea << " lowCutSize: "<< lowCutSize <<endl;
//		cout << "Delta: "<< a <<" " << stepArea << endl;
		if (a <= stepArea){
			if (*p2 < lowCutSize ){
				lowDeltaArea = a;
				tokenArea = *p1;
				lowCutSize = *p2;
			} else if (*p2 == lowCutSize){
				if (a<lowDeltaArea){
					lowDeltaArea = a;
					tokenArea = *p1;
				}
			}
		} else if (a<lowDeltaArea){
			lowDeltaArea = a;
			tokenArea = *p1;
			lowCutSize = *p2;
		}
	}
//	cout << "stepArea: " << stepArea << " lastArea: " <<lastArea << " lastCutSize: " << lastCutSize <<endl;
//	cout << "lowDeltaArea: "<< lowDeltaArea <<" tokenArea: "<< tokenArea << " lowCutSize: "<< lowCutSize <<endl;

	//if ((lowDeltaArea <= stepArea) && (lastCutSize == lowCutSize) && ( (lastArea == tokenArea) || ((lastArea + tokenArea) == totalArea))){
	if ((lastCutSize == lowCutSize) && ( lastArea == tokenArea )){
//		cout << "Final Solution is" << endl;

		if (lowDeltaArea <= stepArea) {
			exitVal = 2;
		} else {
			cout << "Doesn't get a solution " <<endl;
			cutSize = lowCutSize;
			return 3;
			//exit (2);
		}
	} 
   
	//else if (lastCutSize <= lowCutSize) {
	//	cout << "Doesn't get a solution " <<endl;
	//	exit (2);
	//}
	lastArea = tokenArea;
	lastCutSize = lowCutSize;

	// restore initial condition
//	cout << "restore last initial condition"<<endl;
	for (list<cell *>::const_iterator p1 = groupA.begin(); p1 != groupA.end(); ++ p1){
		(*p1)->group = true;
	}
	for (list<cell *>::const_iterator p1 = groupB.begin(); p1 != groupB.end(); ++ p1){
		(*p1)->group = false;
	}

//	printGroup();
	for ( p1 = histgram_area.begin(), 
		 p2 = histgram_cutsize.begin(), 
		 p3 = histgram_cell.begin(); p1 != histgram_area.end();  ++p1, ++p2, ++p3){
			cellArray[*p3].group = ! cellArray[*p3].group;
			//cout << *p3 << endl;
			if ((*p1 == tokenArea) && (*p2 == lowCutSize)){
			//	cout << "initial condition restored."<<endl;
				break;
			}
	}

	// store initial condition into groupA and groupB
	updateGroup();
	groupAarea = tokenArea;
	cutSize = lowCutSize;
//	cout << "ending restore partition function" << endl;
	
	return exitVal;
}


void FMpartition::computeGainLoop () {


    int i = segNum;
	int delta = ((totalArea - goldCutArea ) < goldCutArea ) ?  (totalArea - goldCutArea): (goldCutArea) ;
//	delta = (delta >> 5);
	int kMax =  (delta ) / (segNum ) ;
	//int lo = goldCutArea - 2;
	//int hi = goldCutArea + 2;
	//int lo = goldCutArea - stepArea ;
	//int hi = goldCutArea + stepArea ;
	int lo = goldCutArea - stepArea ;
	int hi = goldCutArea + stepArea ;
	parLowArea = lo - kMax * i;
	parHighArea = hi +  kMax * i ;
	while (--i){
		//cout << "init Pass: " << i  << " cutSize:" << cutSize << endl;
		//if ( i == 14066 ) {
		//	outputStatus("saveme.core");
		//}
//		printGroup();
//		printGain();
//		printCellGain(0);
		//long int k = goldCutArea * i / (segNum * stepArea) + 1 ;
		//int k = (int) (kMax * (double) ( i ) ) ;
		//parLowArea = lo - k ;
		//parHighArea = hi + k ;
		parLowArea += kMax ;
		parHighArea -= kMax ;
		//cout << parLowArea << " " << parHighArea << endl;
		//cout << k << " " << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl;
		cout << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl;
		//parHighArea = totalArea - parLowArea;
        if ( computeGainOnMove()) {
            break;
        }
		//if (cutSize == 0 ) {
			//printGroup();
		//	exit (123);
		//}
    }

}

int FMpartition::initPass(int factor,string fname){
	clearRecord();
	initComputeGeneralArea(balFactor);

	if (factor == 0) {
		restoreInitPartition(fname);
	} else {
		initPartition(factor);
	}
	
//	printGroup();

	initComputeGain();
//	printGain();
	initBucket();
	lastArea = groupAarea;
	lastCutSize = cutSize;
	storeRecord(groupAarea, 0, cutSize);

	computeGainLoop();
/*    int i = segNum;
	int delta = ((goldCutArea << 1) > totalArea ) ?  (totalArea - goldCutArea): (goldCutArea) ;
	double kMax = (double) (delta ) / (double) (segNum ) ;
	
	while (--i){
		//cout << "init Pass: " << i  << " cutSize:" << cutSize << endl;
		//if ( i == 14066 ) {
		//	outputStatus("saveme.core");
		//}
//		printGroup();
//		printGain();
//		printCellGain(0);
		//long int k = goldCutArea * i / (segNum * stepArea) + 1 ;
		int k = (int) (kMax * (double) ( i ) ) ;
		parLowArea = goldCutArea - stepArea - k ;
		parHighArea = goldCutArea + stepArea + k ;
		//cout << parLowArea << " " << parHighArea << endl;
		//cout << k << " " << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl;
		cout << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl;
		//parHighArea = totalArea - parLowArea;
        if ( computeGainOnMove()) {
            break;
        }
		//if (cutSize == 0 ) {
			//printGroup();
		//	exit (123);
		//}
    }
	*/
//	cout << endl;
/*
	// continue current iteration.
	for (int i=1; i<= segNum; ++i) {
//		cout << "initPass i: " << i << endl;
		int exitCode;
//#ifdef _SLOW_
//		exitCode = computeGainOnMove_slow();
//#endif

//#ifdef _ROUGH_
//		exitCode = computeGainOnMove_rough();
//#endif

//#ifdef _FAST_
		exitCode = computeGainOnMove();
//#endif
//		printGain();
		if (exitCode != 0) {
			break;
		}
//		if (i == 2) break;
	}
*/
	// unlock all cells. 
//	for (int i=1; i<= cellNum; ++i) {
//		cellArray[i].lock = false;
//	}
    for (vector < cell * >::const_iterator p = cellLink.begin(); p!= cellLink.end(); ++p) {
        (*p)->lock = false;
    }

	return restorePartition();
//	printGroup();
//	printGain();

}

int FMpartition::onePass(int stage){
	clearRecord();
//	printGroup();
	initComputeGain();
	initComputeArea();
//	printGain();
//	cout << "Before init bucket "<<endl;
	
	initBucket();
//	cout << "After init bucket "<<endl;
//	lastArea = groupAarea;
//	lastCutSize = cutSize;
	storeRecord(groupAarea, 0, cutSize);

//	cout << "After compute gain "<<endl;
	computeGainLoop();
/*
	// continue current iteration.
	int i = segNum;
//	int kMax =  goldCutArea / stepArea - 1;
	int delta = ((goldCutArea << 1) > totalArea ) ?  (totalArea - goldCutArea): (goldCutArea) ;
	double kMax = (double) (delta) / (double) (segNum ) ;
	while (--i){
//		cout << "one Pass: " << i << " cutSize:" << cutSize << endl;
		//int k = kMax * i / segNum + 1;
		//long int k = goldCutArea * i / (segNum * stepArea) + 1 ;
		int k = (int) (kMax * (double) ( i) )  ;
		parLowArea = goldCutArea - stepArea -k ;
		parHighArea = goldCutArea + stepArea + k ;
		//cout << parLowArea << " " << parHighArea << endl;
		//cout << k << " " << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl; 
		cout << segNum - i << " " << cutSize << " " << calAreaLimit (groupAarea) << endl; 
		//parLowArea = (goldCutArea - stage * stepArea) ;
		//if (parLowArea <= 0 ) {
		//	cout << "Error " << endl;
		//	parLowArea = stepArea;
		//}	
		//parHighArea = totalArea - parLowArea;
		if ( computeGainOnMove()) {
			break;
		}
	}
	*/
//	cout << endl;
	/*
	for (int i=1; i<= segNum; ++i) {
//		cout << "onePass i: " << i << endl;
		int exitCode;
//#ifdef _SLOW_
//		exitCode = computeGainOnMove_slow();
//#endif

//#ifdef _ROUGH_
//		exitCode = computeGainOnMove_rough();
//#endif

//#ifdef _FAST_
		exitCode = computeGainOnMove();
//#endif

		if (exitCode != 0) {
			break;
		}
	}
	*/
//	cout << "After iterate "<<endl;


	// unlock all cells. 
//	for (int i=1; i<= cellNum; ++i) {
//		cellArray[i].lock = false;
//	}
    for (vector < cell * >::const_iterator p = cellLink.begin(); p!= cellLink.end(); ++p) {
        (*p)->lock = false;
    }

	return restorePartition();
}



int FMpartition::solutionPass(int factor,string res){
	int exitCode = 0;
	//int stage = ((goldCutArea / stepArea) >> 1);
	//stage = 1; 
	initPass(factor,res);
	while(exitCode == 0){
		//stage = (stage >>2);
		//if (stage < 1) {
		//	stage = 1;
		//}
		//exitCode = onePass(stage);
		exitCode = onePass(0);
	}
	return 0;
}
/*
void FMpartition::printGroup(){
	cout << "Group A: " << endl;
	for (int i = 1; i<= cellNum ; ++i) {
		if (cellArray[i].group) {
			cout << " " << i ;
		}
	}
	cout << endl;
	cout << "Group B: " << endl;
	for (int i = 1; i<= cellNum ; ++i) {
		if (!cellArray[i].group) {
			cout << " " << i ;
		}
	}
	cout << endl;
	return ;
}

void FMpartition::printGain(){
	cout << "Print Gain"<<endl;
	for (int i=1; i<= cellNum; ++i) {
		if (!cellArray[i].lock){
			cout << "Cell " << i << " gain: " << cellArray[i].gain<<endl;
		}
	}
	cout << endl;
}

void FMpartition::printBucket(){
	cout <<"Print Bucket" <<endl;
	int e = (cellDeg<<1) +1;
	cout << "indexCur cell: "<< cal->indexCur << " gain: " << cellArray[cal->indexCur].gain <<endl;
	for (int i = 0; i < e; ++i){
		if (cal->link[i].empty()){
			continue;
		}
		cout << "gain group: " << i << ":";
		for (list<int>::const_iterator p = cal->link[i].begin(); p != cal->link[i].end(); ++p){
			cout << " " << *p ;
		}
		cout << endl;
	} 
	cout << endl;
	return ;
}

void FMpartition::printAreaPar(){
	cout << "parLowArea: " << parLowArea <<endl;
	cout << "parHighArea: " << parHighArea <<endl;
	cout << "totalArea: " << totalArea <<endl;
	cout << "stepArea: " << stepArea <<endl;
}

void FMpartition::printQuality(){
	cout << "Gold Cut Area: " << goldCutArea << endl;
	cout << "stepArea: " << stepArea <<endl;
	cout << "GroupA Area: " << groupAarea <<endl;
	cout << "GroupB Area: " << totalArea - groupAarea <<endl;
	cout << "CutSize: " << cutSize << endl;;
}
*/
void FMpartition::outputResult (string filename){
	ofstream result (filename.c_str());
	result << cutSize << endl;
	bool ev = groupAarea < ( totalArea - groupAarea);
	if (ev) {
		result << groupAarea << endl;
	}
	result << totalArea - groupAarea << endl;
	if (!ev) {
		result << groupAarea << endl;
	}
	if (ev) {
	for (list<cell*>::const_iterator p = groupA.begin(); p!= groupA.end(); ++p){
		result << (*p - cellArray) << " ";
	}
	result << endl;
	}
	for (list<cell*>::const_iterator p = groupB.begin(); p!= groupB.end(); ++p){
		result << (*p - cellArray) << " ";
	}
	result << endl;
    if (!ev) {
    for (list<cell*>::const_iterator p = groupA.begin(); p!= groupA.end(); ++p){
        result << (*p - cellArray) << " ";
    }
    result << endl;
    }


	result.close();
}
/*
void FMpartition::outputStatus (string filename){
	ofstream result (filename.c_str());
	result << cutSize << endl;
	result << groupAarea << endl;
	result << totalArea - groupAarea << endl;
	for (list<cell*>::const_iterator p = groupA.begin(); p!= groupA.end(); ++p){
		result << (*p - cellArray) << " ";
	}
	result << endl;
	for (list<cell*>::const_iterator p = groupB.begin(); p!= groupB.end(); ++p){
		result << (*p - cellArray) << " ";
	}
	result << endl;
    for (int i=1; i<= cellNum; ++i) {
		if (cellArray[i].lock){
			result  << i << " ";
		}
	}

	result << endl;
	result.close();
}

void FMpartition::printCellGain( int c){
	vector <int> cellGain;
	for (int i = 0; i<= cellNum ; ++i ) {
		cellGain.push_back(0);
	}
	for (int i = 0; i< netNum ; ++i ) {
		int ga = 0;
		int gb = 0;
		// record the only cell that belongs to one group.
		int posA = 0 ;
		int posB = 0 ;
		for (list<int>::const_iterator p = netArray[i].conn.begin(); p!= netArray[i].conn.end(); ++p ) {
			if ( cellArray[*p].group ) {
				posA = *p;
				++ga;
			} else {
				posB = *p;
				++gb;
			}
			if ( ga> 1 && gb > 1) {
				break;
			}
		}
		if (ga>1 && gb>1){
			continue;
		} else if ((ga == 0) || (gb == 0)) {
			for (list<int>::const_iterator p = netArray[i].conn.begin(); p!= netArray[i].conn.end(); ++p ) {
				-- cellGain[*p] ;
			}
		} else {
			if (ga == 1){
				++ cellGain[posA];
			} 
			if (gb == 1){
				++ cellGain[posB];
			}
		}
	}
	if (c==0) {
		for (int i = 1; i<= cellNum ; ++i ) {
			cout << "cell: " << i << " gain: " << cellGain[i] << endl;
		}
	} else {
		cout << "cell: " << c << " gain: " << cellGain[c] << endl;
	}
}
*/

