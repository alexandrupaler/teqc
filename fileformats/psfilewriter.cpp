#include "fileformats/psfilewriter.h"
#include "gatenumbers.h"
#include "cnotcounter.h"

string psfilewriter::getPostScriptFileName(const char* basisfilename)
{
	string ret(basisfilename);
	ret += ".ps";
	return ret;
}

void psfilewriter::writePostScriptFile(FILE* pFile, circuitmatrix& circ)
{
	fprintf(pFile, "%%!PS\n");
	int height = (circ.size() + 1) * 30;
	int width = (circ.getMaxColumn() + 3) * 30;
	fprintf(pFile, "%%%%BoundingBox: 0 0 %d %d\n", width, height);

	appendFile(pFile, "templateh.ps");

	fprintf(pFile, "30 %d moveto\n", height - 15);

	//not as general as Qcircuit
	int inQubit = 0;
	int outQubit = 0;

	for(int i=0; i<circ.size(); i++)
	{
		for(int j=0; j<circ.at(i).size(); j++)
		{
			int cmd = circ.at(i).at(j);
			if(cnotcounter::isCnot(cmd))
				cmd = cnotcounter::getCnotPart(cmd);

			//if(j==0)
			//if(initialisations.find(cmd) != initialisations.end())
			if(circ.isInitialisation(i,j))
			{
				/*search measurement*/
				int length = 0;
				for(int k=j+1; k<circ.at(i).size(); k++)
				{
					//if(measurements.find(circ.at(i).at(k)) != measurements.end())
					if(circ.isMeasurement(i, k))
					{
						length = k - j;
						break;
					}
				}

				switch(cmd)
				{
					case AA:
						fprintf(pFile, "(A) ket ");
						break;
					case YY:
						fprintf(pFile, "(Y) ket ");
						break;
					case ZERO:
						fprintf(pFile, "(0) ket ");
						break;
					case PLUS:
						fprintf(pFile, "(+) ket ");
						break;
				}
				//fprintf(pFile, "n %d wire ", (int)(circ.at(i).size() - 1));
				fprintf(pFile, "n %d wire ", length);
			}
			else if(cmd == MZ)
			{
				fprintf(pFile, "(Z) measure ");
			}
			else if(cmd == MX)
			{
				fprintf(pFile, "(X) measure ");
			}
			else if(cmd == MXZ)
			{
				fprintf(pFile, "(X/Z) measure ");
			}
			else if(cmd == MZX)
			{
				fprintf(pFile, "(Z/X) measure ");
			}
			else if(cmd == MA)
			{
				fprintf(pFile, "(A) measure ");
			}
			else if(cmd == MYZ)
			{
				fprintf(pFile, "(Y/Z) measure ");
			}
			else if(cmd == MY)
			{
				fprintf(pFile, "(Y) measure ");
			}
			else if(cmd == CTRL)
			{
				vector<int> targets = circ.findTarget(i,j);
				fprintf(pFile, "control ");

				//search maxpos and maxneg
				int maxpos = 0;
				int maxneg = 0;

				for(vector<int>::iterator it=targets.begin(); it!=targets.end(); it++)
				{
					int dist = *it - i;
					if(dist < 0 && dist < maxneg)
						maxneg = dist;
					if(dist > 0 && dist > maxpos)
						maxpos = dist;
				}
				if(maxpos != 0)
					fprintf(pFile, "%d qwx ", maxpos);
				if(maxneg != 0)
					fprintf(pFile, "%d qwx ", maxneg);

				//fprintf(pFile, "\\qw");
			}
			else if(cmd == TGT)
				fprintf(pFile, "target ");
			else if(cmd == WIRE)
			{
				//fprintf(pFile, "\\qw");
			}

			fprintf(pFile, "n ");
		}
		fprintf(pFile, "back \n");
	}

	appendFile(pFile, "templatec.ps");
	//fclose(pFile);
}

void psfilewriter::appendFile(FILE* destFile, const char* name)
{
	FILE* sourceFile = fopen (name, "r");
	char mystring [4096];

	while(!feof(sourceFile))
	{
		if(fgets (mystring, sizeof(mystring), sourceFile) != NULL)
			fprintf(destFile, "%s", mystring);
	}
	fclose(sourceFile);
}
