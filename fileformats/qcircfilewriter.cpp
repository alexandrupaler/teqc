#include <stdio.h>
#include "fileformats/qcircfilewriter.h"
#include "gatenumbers.h"
#include "cnotcounter.h"

std::string qcircfilewriter::getQCircFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".tex";
	return ret;
}

void qcircfilewriter::writeQCircFile(FILE* pFile, circuitmatrix& circ)
{
	fprintf(pFile, "\\documentclass{article}\n");
	fprintf(pFile, "\\usepackage[a4paper, portrait, margin=2in]{geometry}");

	fprintf(pFile, "\\include{Qcircuit}\n");
	fprintf(pFile, "\\begin{document}\n");

	fprintf(pFile, "\\begin{figure}\n\\centerline{\n");
	fprintf(pFile, "\\Qcircuit @C=.6em @R=.7em {\n");

	int inQubit = 0;
	int outQubit = 0;

	for(size_t i=0; i<circ.size(); i++)
	{
		bool wasmeasure = false;
		for(size_t j=0; j < circ.at(i).size(); j++)
		{
			int cmd = circ.at(i).at(j);
			if(cnotcounter::isCnot(cmd))
				cmd = cnotcounter::getCnotPart(cmd);

			if(j==0)
			{
				switch(cmd)
				{
					case AA:
						fprintf(pFile, "\\lstick{\\ket{A}}");
						break;
					case YY:
						fprintf(pFile, "\\lstick{\\ket{Y}}");
						break;
					case ZERO:
						fprintf(pFile, "\\lstick{\\ket{0}}");
						break;
					case PLUS:
						fprintf(pFile, "\\lstick{\\ket{+}}");
						break;
					case INPUT:
						fprintf(pFile, "\\lstick{\\ket{in_%d}}", inQubit++);
						break;
				}
			}
			else if(cmd == MZ)
			{
				fprintf(pFile, "\\measure{Z}");
				wasmeasure = true;
			}
			else if(cmd == MX)
			{
				fprintf(pFile, "\\measure{X}");
				wasmeasure = true;
			}
			else if(cmd == MXZ)
			{
				fprintf(pFile, "\\measure{X/Z}");
				wasmeasure = true;
			}
			else if(cmd == MZX)
			{
				fprintf(pFile, "\\measure{Z/X}");
				wasmeasure = true;
			}
			else if(cmd == 'h'/*HGATE*/)
			{
				fprintf(pFile, "\\gate{H}");
			}
			else if(cmd == 't'/*TGATE*/)
			{
				fprintf(pFile, "\\gate{T}");
			}
			else if(cmd == 'p'/*PGATE*/)
			{
				fprintf(pFile, "\\gate{P}");
			}
			else if(cmd == 'r'/*RGATE*/)
			{
				fprintf(pFile, "\\gate{R}");
			}
			else if(cmd == 'v'/*RGATE*/)
			{
				fprintf(pFile, "\\gate{V}");
			}
			else if(cmd == CTRL)
			{
				//search target -- take the nearest one
//				int d = circ.size();
//				for(int k=0; k<circ.size(); k++)
//					if(circ.at(k).at(j) == 2)
//						d = abs(k-i) < abs(d) ? k-i : d;
				std::vector<int> targets = circ.findTarget(i,j);
				//fprintf(pFile, "\\ctrl{%d}", findTarget(i, j));
				fprintf(pFile, "\\control");

				//search maxpos and maxneg
				int maxpos = 0;
				int maxneg = 0;

				for(std::vector<int>::iterator it=targets.begin(); it!=targets.end(); it++)
				{
					int dist = *it - i;
					if(dist < 0 && dist < maxneg)
						maxneg = dist;
					if(dist > 0 && dist > maxpos)
						maxpos = dist;
				}
				if(maxpos != 0)
					fprintf(pFile, "\\qwx[%d]", maxpos);
				if(maxneg != 0)
					fprintf(pFile, "\\qwx[%d]", maxneg);

				fprintf(pFile, "\\qw");
			}
			else if(cmd == TGT)
				fprintf(pFile, "\\targ");
			else if(cmd == WIRE)
			{
				if(!wasmeasure)
					fprintf(pFile, "\\qw");
			}

//			if((cmd <6 || cmd>9) && j==circ.at(i).size()-1)
			if((cmd == 0) && j == circ.at(i).size() - 1)
				fprintf(pFile, "&\\rstick{\\ket{out_%d}}", outQubit++);

			fprintf(pFile, "&");
		}
		fprintf(pFile, "\\\\\n");
	}

	fprintf(pFile, "	}\n");
	fprintf(pFile, "}\\end{figure}\n");

	fprintf(pFile, "\\end{document}\n");
}
