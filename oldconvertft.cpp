#include "gatenumbers.h"
#include "databasereader.h"
#include "decomposition.h"

#include "fileformats/infilereader.h"
#include "fileformats/geomfilewriter.h"
#include "fileformats/iofilewriter.h"
#include "fileformats/boundingboxfilewriter.h"

#include "fileformats/chpfilewriter.h"
#include "fileformats/psfilewriter.h"
#include "fileformats/qcircfilewriter.h"

#include "fileformats/generaldefines.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/time.h>

#include "cnotcounter.h"
#include "circuitgeometry.h"
#include "circuitmatrix.h"

#include <climits>

using namespace std;

databasereader dbReader;
cnotcounter cncounter;
circuitgeometry geom;
circuitmatrix circ;

//map<vector<int>, bool> existaCnotLaImpingere;

/**
 * Returns a random number
 * @param min minimum value of random number
 * @param max maximum value of random number
 * @return the random number
 */
int getRandomNumber(int min, int max)
{
	int q = lrand48() % max;
	while(q < min)
		q = lrand48() % max;
	return q;
}

/**
 * For a circuit matrix cell that represents a CNOT element, find
 * the other element, and return a vector containing both lines.
 * @param line
 * @param col
 * @return vector of line indices containing the CTRL and TGT of a CNOT
 */
vector<int> findCNOTEndpoints(int line, int col)
{
	int type = cncounter.getCnotPart(circ[line].at(col));
	int capat = -1;
	if(type == CTRL)
	{
		capat = circ.findTarget(line, col)[0];
	}
	else
	{
		capat = circ.findControl(line, col)[0];
	}

	vector<int> endpts;
	endpts.push_back(capat);
	endpts.push_back(col);

	return endpts;
}

/**
 * Checks if on a given line there are enough empty cells
 * @param line the matrix line representing a qubit to check
 * @param col the starting column
 * @param nrCols the number of columns required for free cells
 * @return the number of empty cells on the line, thus the number of columns having empty cells
 */
int isFreeSpace(int line, int col, int nrCols)
{
	bool isSpace = true;
	for(int j=1; j<(nrCols+1); j++)
	{
		int y = col + j;
		if(y<0 || y >= circ.at(line).size())
		{
			//isSpace = false;
			//break;
			return j-1;
		}
//		printf("check %d %d %d\n", line, y, circ.at(line).at(y));
		if(!circ.isWire(line, y))//|| intersectsCnot(line,y).size()!=0)/* && circ.at(line).at(y) != INPUT)*/
		{
			//isSpace = false;
			//break;
			return j-1;
		}
	}
	return nrCols;
}

/**
 * Checks if a given circuit matrix cell is between the control and the target
 * of a CNOT. For example, consider that the control has coordinate (1,5) and
 * the target has coordinate (4,5). The cell (2,5) intersects the CNOT, but the
 * cell (7,5) does not. In the previous example, the first coordinate represents
 * the qubit number (line) and the second coordinate the gate number (column).
 * @param line
 * @param col
 * @return true if it intersects, false otherwise
 */
vector<int> intersectsCnot(int line, int col)
{
	vector<int> ret;

	if(cnotcounter::isCnot(circ.at(line).at(col)))
	{
		ret.push_back(line);
		int celalt = findCNOTEndpoints(line, col)[0];
		ret.push_back(celalt);

		//printf("@ %d,%d iscnot\n", linie, col);

		return ret;//pure luck - directly a CNOT
	}

	for(int i=line; i>=0; i--)
	{
		//go up and search for a CNOT element
		if(col < circ.at(i).size() && cncounter.isCnot(circ.at(i).at(col)))
		{
			//printf("@ %d,%d iscnot\n", i, col);

			int celalt = findCNOTEndpoints(i, col)[0];
			if(celalt > line)
			{
				ret.push_back(i);
				ret.push_back(celalt);
			}
		}
	}

	return ret;
}

/**
 * Replaces the Hadamard gate with its non-ICM decoposition. The method should be
 * moved tp processraw
 * @param line
 * @param col
 */
void replaceH(int line, int col)
{
	string key = dbReader.intToName.at(HGATE);
	decomposition hada = dbReader.decomp[key];

	int maxCols = hada.getMaxCols();

	//0 hardcoded because hadamard
	for(int i=0; i<hada.gates[0].size(); i++)
	{
		circ.at(line).at(col + i) = hada.gates[0][i];
	}
}

/**
 *
 * @param lines
 * @param line
 * @param col
 * @param nrCols
 */
void replaceT(vector<vector<int> >& lines, int line, int col, int nrCols)
{
	circ[line].at(col + /*nrCols*/1) = MZ;//measure Z

	//first move is to connect the ancillas to the circ
	int cnotctrlid = cncounter.getNextCnotNumber();
	int cnottrgtid =  cnotctrlid + 1;

	lines.at(0).at(col) = (cnotctrlid);//make control?

	circ[line].at(col) = cnottrgtid;//make target?
	//initialize the first ancilla
	lines.at(0).at(0) = AA;

	//add selective target and destination subcircuit
	int cnots[5];
	for(int i=0; i<5; i++)
	{
		cnots[i] = cncounter.getNextCnotNumber();
	}

	lines.at(0).at(col + 1) = cnots[0];//control
	lines.at(0).at(col + 2) = cnots[1] + 1;//target
	lines.at(0).at(col + 4) = MZX;// 9;//measure Z/X

	lines.at(1).at(0) = -3;//zero
	lines.at(1).at(col + 1) = cnots[0] + 1;//target
	lines.at(1).at(col + 2) = WIRE;//0;//wire
	lines.at(1).at(col + 3) = cnots[2] + 1;//target
	lines.at(1).at(col + 4) = MXZ;//  8;//measure X/Z

	lines.at(2).at(0) = YY;// -2;//y
	lines.at(2).at(col + 1) = WIRE;//0;//wire
	lines.at(2).at(col + 2) = cnots[1];//control
	lines.at(2).at(col + 3) = WIRE;//0;//wire
	lines.at(2).at(col + 4) = cnots[3];//control
	lines.at(2).at(col + 5) = WIRE;//0;//wire
	lines.at(2).at(col + 6) = MXZ;//8;//measure X/Z

	lines.at(3).at(0) = PLUS;//-4;//+
	lines.at(3).at(col + 1) = WIRE;//0;//wire
	lines.at(3).at(col + 2) = WIRE;//0;//wire
	lines.at(3).at(col + 3) = cnots[2];//control
	lines.at(3).at(col + 4) = WIRE;//0;//wire
	lines.at(3).at(col + 5) = cnots[4];//control
	lines.at(3).at(col + 6) = MZX;//9;//measure Z/X

	lines.at(4).at(0) = ZERO;//-3;//+
	lines.at(4).at(col + 1) = WIRE;//0;//wire
	lines.at(4).at(col + 2) = WIRE;//0;//wire
	lines.at(4).at(col + 3) = WIRE;//0;//wire
	lines.at(4).at(col + 4) = cnots[3] + 1;//target
	lines.at(4).at(col + 5) = cnots[4] + 1;//target
	//lines.at(4).at(col + 6) = WIRE;//0;//wire
}

/**
 *
 * @param lines
 * @param line
 * @param col
 * @param nrCols
 */
void replaceP(vector<vector<int> >& lines, int line, int col, int nrCols)
{
	circ.at(line).at(col + nrCols) = MZ;// 6;//measure Z

	//first move is to connect the ancillas to the circ
	int cnotctrlid = cncounter.getNextCnotNumber();
	int cnottrgtid =  cnotctrlid + 1;

	lines.at(0).at(col) = cnotctrlid;//make control?
	circ.at(line).at(col) = cnottrgtid;//make target?
	//initialize the first ancilla
	lines.at(0).at(0) = YY/* -2*/;
}

/**
 *
 * @param lines
 * @param line
 * @param col
 * @param nrCols
 */
void replaceV(vector<vector<int> >& lines, int line, int col, int nrCols)
{
	circ.at(line).at(col + nrCols) = MX;//7;//measure X

	//first move is to connect the ancillas to the circ
	int cnotctrlid = cncounter.getNextCnotNumber();
	int cnottrgtid =  cnotctrlid + 1;

	lines.at(0).at(col) = cnottrgtid;//make control?
	circ.at(line).at(col) = cnotctrlid;//make target?

	//initialize the first ancilla
	lines.at(0).at(0) = YY/*-2*/;
}

/**
 * Prepares the circuit matrix representation for replacing a non-ICM gate
 * with its ICM decomposition. It introduces additional lines (qubit ancillae)
 * required by the ICM representation.
 * @param type
 * @param line
 * @param col
 */
void replace(int gateType, int line, int col)
{
	//check if there is space on the columns before the gate
	//if yes, then lines and columns should not be added,
	//in order to reduce the size of the circuit
	int nrCols = 1;
	int nrLines = 1;
	if(gateType == TGATE)//for corrected T gates add multiple lines and cols
	{
		nrCols = 6;
		nrLines = 5;
	}

	//for P introduce a row after line
	//for R introduce a row after line
	//for T introduce 5 rows after line
	vector<vector<int> > lines;
	for(int i=0; i< nrLines; i++)
	{
		vector<int> v(circ.at(line).size(), WIRE);
		lines.push_back(v);
	}

	//copy from prev line to last new line
	//for P this is line +1
	//for R this is line+1
	//for T this is line +5
	if(nrCols > 0)
	{
		//for(int cl = col + 1 + nrCols; cl < circ.at(line).size(); cl++)
		for(int cl = col + 1; cl < circ.at(line).size(); cl++)
		{
			lines.at(lines.size() - 1).at(cl) = circ.at(line).at(cl);
			circ.at(line).at(cl) = WIRE;
		}
	}

	if(gateType == PGATE)
	{
		replaceP(lines, line, col, nrCols);
	}
	else if (gateType == RGATE)
	{
		replaceV(lines, line, col, nrCols);
	}
	else if (gateType == TGATE)
	{
		replaceT(lines, line, col, nrCols);
	}
	else if(gateType == HGATE)
	{
		replaceH(line, col);
	}

	//insert the lines
	circ.insertRows(line + 1, lines);
}

void replaceAllHs()
{
	bool isH = true;
	while(isH)
	{
		isH = false;
		for(int i=0; i<circ.size(); i++)
		{
			for(int j=0; j<circ.at(i).size(); j++)
			{
				if(circ.at(i).at(j) == HGATE)
				{
					replaceH(i,j);
					isH = true;
				}
			}
		}
	}
}

/**
 * Searches for all the qubit measurements and moves them to the rightmost
 * possible position on the circuit matrix representation.
 */
void moveMeasurementsToRight()
{
	for(int i=0; i<circ.size(); i++)
	{
		vector<int> ms;
		for(int j=0; j<circ.at(i).size(); j++)
		{
			if(circ.isMeasurement(i, j))
			{
				ms.push_back(j);
			}
		}

		//find the next element which is not an EMPTY
		vector<int> firstNonEmpty;
		for(vector<int>::iterator it=ms.begin(); it!=ms.end(); it++)
		{
			int mindex = *it;
			int currindex = mindex + 1;
			while(circ.indexLessThanSize(i, currindex) && (circ.isEmpty(i, currindex) || circ.isWire(i, currindex)))
			{
				currindex++;
			}
			currindex--;
			firstNonEmpty.push_back(currindex);
		}

		//move the measurement to the right
		for(int j=0; j<ms.size(); j++)
		{
			int val = circ.at(i).at(ms[j]);
			for(int k=ms[j]; k<firstNonEmpty[j]; k++)
			{
				circ.at(i).at(k) = WIRE;
			}
			circ.at(i).at(firstNonEmpty[j]) = val;
		}
	}
}

/**
 * Searches for all the qubit measurements and moves them to the leftmost
 * possible position on the circuit matrix representation.
 */
void moveMeasurementsToLeft()
{
	for(int i=0; i<circ.size(); i++)
	{
		vector<int> measurementIndices;
		//save a list of all the measurements on a given qubitline
		for(int j=0; j<circ.at(i).size(); j++)
		{
			if(circ.isMeasurement(i, j))
			{
				measurementIndices.push_back(j);
			}
		}

		//find pair initialisation/cnot/etc
		vector<int> firstWireBeforeM;
		for(vector<int>::iterator it=measurementIndices.begin(); it!=measurementIndices.end(); it++)
		{
			int mindex = *it;
			int currindex = mindex - 1;
			while(circ.isWire(i, currindex))
			{
				currindex--;
			}
			currindex++;//ultimul index a fost inainte
			firstWireBeforeM.push_back(currindex);
		}

		//move the measurement to the left
		//resutl: deletes the wire between control/target and measurement
		for(int j=0; j<measurementIndices.size(); j++)
		{
			int val = circ.at(i).at(measurementIndices[j]);
			for(int k=firstWireBeforeM[j] + 1; k <= measurementIndices[j]; k++)
			{
				circ.at(i).at(k) = EMPTY;
			}
			circ.at(i).at(firstWireBeforeM[j]) = val;
		}

		//there may be wires even after the last measurement: due to possible negligence
		//it is not wrong, but it is not good for later when the geometry will be generated
		//remove wires starting from the last measurement
		if(measurementIndices.size() >= 1)
		{
			int lastm = measurementIndices.at(measurementIndices.size() - 1);
			for(int j=lastm + 1; j<circ.at(i).size(); j++)
			{
				circ.at(i).at(j) = EMPTY;
			}
		}
	}
}

void writeGeometry(char** argv)
{
	geomfilewriter geomwr;
	string fname = geomfilewriter::getGeomFileName(argv[1]);
	FILE* geomfile = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(geomfile, geom.simplegeom.io, geom.simplegeom.segs, geom.simplegeom.coords);
	fclose(geomfile);

	iofilewriter iowr;
	string iofname = iofilewriter::getIOFileName(fname.c_str());
	FILE* iofile = fopen(iofname.c_str(), "w");
	iowr.writeIOFile(iofile, geom.allpins.inputList);
	fclose(iofile);

	boundingboxfilewriter bboxwr;
	string bboxname = boundingboxfilewriter::getBoundingBoxFileName(fname.c_str());
	FILE* bboxfile = fopen(bboxname.c_str(), "w");
	bboxwr.writeBoundingBoxFile(bboxfile, geom.simplegeom.boundingbox);
	fclose(bboxfile);
}

void writeOtherFormats(const char* basisname)
{
	/*chpfilewriter chpwr;
	string chpfname = chpfilewriter::getCHPFileName(basisname);
	FILE* chpfile = fopen(chpfname.c_str(), "w");
	chpwr.writeCHPFile(chpfile, circ);
	fclose(chpfile);*/

	/*qcircfilewriter qcircwr;
	string fname = qcircfilewriter::getQCircFileName(basisname);
	FILE* file = fopen(fname.c_str(), "w");
	qcircwr.writeQCircFile(file, circ);
	fclose(file);*/

	psfilewriter pswr;
	string psfname = psfilewriter::getPostScriptFileName(basisname);
	FILE* psfile = fopen(psfname.c_str(), "w");
	pswr.writePostScriptFile(psfile, circ);
	fclose(psfile);
}

void arrangeInputsAndOutputs()
{
	//find outputs - move to absolute right - not like moveMeasurementsToRight
	for(int i=0; i<circ.size(); i++)
	{
		if(circ.at(i).size() == 0)
			continue;

		vector<int>::iterator it = circ.at(i).end();
		while(*(it-1) == EMPTY && it!=circ.at(i).begin())
		{
			circ.at(i).erase(it-1);
			it--;
		}

	}

	int max = circ.getMaxColumn();

	for(int i=0; i<circ.size(); i++)
	{
		for(int j=0; j<circ.at(i).size(); j++)
		{
			if(circ.isOutput(i, j))
			{
				if(j != (max-1))
				{
					int tointro = max - circ.at(i).size() + 1;//+1 ca sa fiu sigur ca e mutat la capat de tot
					circ.at(i).insert(circ.at(i).end(), tointro, WIRE);
					circ.at(i).at(j) = WIRE;
					circ.at(i).at(max) = OUTPUT;
				}
			}
		}
	}
}

/**
 * 	Insert necessary additional columns
 * 	for the moment, assume all lines have same length
 */
void extendCircuitWithColumns()
{
	for (int j = 0; j < circ.at(0).size(); j++)
	{
		int col = 0;
		for (int i = 0; i < circ.size(); i++)
		{
			int ccol = 0;
			int gtype = circ.at(i).at(j);

			if (gtype == TGATE)
				ccol = 8;
			if (gtype == PGATE)
				ccol = 2;
			if (gtype == RGATE)
				ccol = 2;
			if (gtype == HGATE)
				ccol = 6;

			if (col < ccol)
				col = ccol;
		}

		circ.insertColumns(j + 1, col);
	}
}

/**
 * Convert a non-ICM circuit into ICM.
 */
void convertCirc()
{
	int nrT = 0;
	int nrP = 0;
	int nrR = 0;
	int nrH = 0;

	//count the number of P and T gates
	for(int i=0; i<circ.size(); i++)
	{
		for(int j=0; j<circ.at(i).size(); j++)
		{
			int gtype = circ.at(i).at(j);
			if(gtype == TGATE)
				nrT++;
			if(gtype == PGATE)
				nrP++;
			if(gtype == RGATE)
				nrR++;
			if(gtype == HGATE)
				nrH+=3;
		}
	}

	//the total number of ancillas
	int nrAnc = nrT + nrP + nrR + nrH;
	//add the ancillas

	for(int i=0; i<nrAnc; i++)
	{
		int maxj = circ.size();
		for(int j=0; j<maxj; j++)
		{
			int maxk = circ.at(j).size();
			for(int k=0; k<maxk; k++)
			{
				if(circ.at(j).at(k) == PGATE || circ.at(j).at(k) == TGATE || circ.at(j).at(k) == RGATE || circ.at(j).at(k) == HGATE)
				{
					replace(circ.at(j).at(k), j, k);
					k = maxk; j = maxj;//get out of loops
				}
			}
		}
	}
}

/**
 * Returns if a column was moved or not to the leftmost possible empty column.
 */
bool moveColumnToTheLeft(int origColumnIndex, int start, int stop)
{
	int destination = origColumnIndex;

	//find the leftmost possible column where the current one can be moved
	for(int jj=origColumnIndex - 1; jj>=0; jj--)
	{
		bool isEmptyColumn = true;
		for(int i=start; i<=stop; i++)
		{
			if(!circ.indexLessThanSize(i, jj))
				continue;

			bool noOtherCnot = (intersectsCnot(i,jj).size() == 0);

			isEmptyColumn = (isEmptyColumn && (noOtherCnot && (circ.isEmpty(i, jj) || circ.isWire(i, jj))));
		}
		if(isEmptyColumn)
		{
			destination = jj;
		}
		else
		{
			//a column cannot move through walls: first encountered wall stops the search
			break;
		}
	}

	if(destination != origColumnIndex)
	{
		for(int i=start; i<=stop; i++)
		{
			//if(circ.at(i).size() > destination)
			if(circ.indexLessThanSize(i, destination))
			{
				circ.at(i).at(destination) = circ.at(i).at(origColumnIndex);

				if(!circ.isEmpty(i, origColumnIndex))
				{
					circ.at(i).at(origColumnIndex) = WIRE;
				}
			}
		}

		return true;
	}

	return false;
}

/**
 * The testris operation is just the movement of columns to the left.
 * @return if any column was moved to the left
 */
bool tetrisColumns()
{
	int maxcol = circ.getMaxColumn();

	bool moved = false;
	//take each column and try to move it to the left
	for(int j=0; j<maxcol; j++)
	{
		printf("*** %d %d\n", j, maxcol);
		int idxctrl = -1;
		int idxtgt = -1;
		for(int i=0; i<circ.size(); i++)//urmareste liniile
		{
			if(!circ.indexLessThanSize(i, j))
				continue;

			if(cncounter.isControl(circ.at(i).at(j)))
			{
				idxctrl = i;
				idxtgt = circ.findTarget(i, j)[0];
			}
			else if(cncounter.isTarget(circ.at(i).at(j)))
			{
				idxctrl = circ.findControl(i, j)[0];
				idxtgt = i;
			}

			//if both the control and the target where found
			//the cnot is defined between the minimum and the maximum index of both
			if(idxctrl!=-1 && idxtgt != -1)
			{
				int min = idxctrl < idxtgt ? idxctrl : idxtgt;
				int max = idxctrl > idxtgt ? idxctrl : idxtgt;

				moved = moveColumnToTheLeft(j, min, max) || moved;
				idxctrl = -1;
				idxtgt = -1;
			}
		}
	}

	return moved;
}

int main(int argc, char** argv)
{
	timeval tim;
	gettimeofday(&tim, NULL);
	srand48((unsigned int) tim.tv_usec);

	int distrounds = 0;//number of distillation rounds

	if(argc < 2)
	{
		printf("run with [random | fname] [distrounds]\n");
		return 2;
	}

	if(argc == 3)
	{
		distrounds = atoi(argv[2]);
	}

	//open file
	//readFile(argv[1]);
	infilereader inr;
	FILE* fin = fopen(argv[1], "r");
	//a lot of copying
	circ = circuitmatrix(inr.readInFile(fin));
	fclose(fin);

	extendCircuitWithColumns();

	//printf("\n== Replace H\n");
	replaceAllHs();
	circ.printCirc();

	convertCirc();

	moveMeasurementsToRight();
	tetrisColumns();
	moveMeasurementsToLeft();

	circ.removeEmptyColumns();
	circ.printCirc();
	arrangeInputsAndOutputs();
	circ.printCirc();

	geom.makeGeometryFromCircuit(circ);

	writeGeometry(argv);

	writeOtherFormats(argv[1]);

	return 1;
}
