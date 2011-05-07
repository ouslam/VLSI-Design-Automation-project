#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include "FMpartition.h"

using namespace std;
//#define _SLOW_
//#define _ROUGH_
//#define _FAST_
#define PARTITION_FACTOR 42 

int main(int argc, char * argv[]) {
//	cout << "Size of Int:  " << sizeof (int) << endl;
//	cout << "Size of Char: " << sizeof (char) << endl;
	
	string infile;
	int balFactor = 50;
	time_t start,end;
	double timePass;


	switch (argc){
		case 3 :
			balFactor = atoi(argv[2]);
		case 2 : 
			infile = argv[1];
			break;
		default : 
			cout << "Usage : <fileName> [<balFactor>] " << endl;
			exit (1);
	}
	
	//cout << balFactor << infile << endl;
//	exit (32);
//	int exitVal;
//	cout << "size of int is: " << sizeof (int) << endl;
	time (&start);

	FMpartition ex;
	string outFile, balOutFile, revBalOutFile;
	outFile = infile + ".result";
//	ex.readfile("FMbench2.net");
	ex.readfile(infile);
	if (balFactor != 0) {
		ex.balFactor = PARTITION_FACTOR ;
		ex.solutionPass(balFactor, "no such file");

		int balSolCutSize = ex.cutSize;
		cout << "cutSize is " << ex.cutSize << endl;
		ex.outputResult(outFile);

		ex.balFactor = 100 - PARTITION_FACTOR ;
		ex.solutionPass( 100 - balFactor, "no such file");
		cout << "cutSize is " << ex.cutSize << endl;
		int revBalSolCutSize = ex.cutSize;
		if (balSolCutSize > revBalSolCutSize) {
			remove (outFile.c_str());
			ex.outputResult(outFile);
		}

//		ex.outputResult(outFile);
//		cout << "cutSize is " << ex.cutSize << endl;
	} else {
		ex.solutionPass(0, "backup.res");
	}
	time (&end);


//	ex.printQuality();
//	ex.printGroup();
//	string outfile = infile;
//	outfile += ".result";
//	ex.outputResult(outfile);
	timePass = difftime (end,start);

	// calculate memory usage. 
	int mypid = getpid();
	FILE * cmdStream;
	char buf[20];
	//char memCmd[100] = "pmap 1 | grep total | awk '{print $3}'";
	char memCmd[100] ;
   	sprintf (memCmd, "pmap %d  | grep total | awk '{print $2}' ", mypid);
	cmdStream = popen (memCmd,"r" );
	if (fgets(buf, 20, cmdStream) != NULL){
		cout<<"mem usage: " << buf << endl;
	}
	pclose (cmdStream);
	cout << "The Program runs "<< timePass << " seconds on benchmark file: " << infile << endl;
	return 0;
}

