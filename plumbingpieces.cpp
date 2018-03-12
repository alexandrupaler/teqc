#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <set>

#include "plumbingpieces.h"
#include "fileformats/generaldefines.h"
#include "utils/trim.h"

/*
.position = .P
.defects = .D
.defects.primal = .D.p
.defects.dual = .D.d
.corrs = .C
*/

std::string plumbingpiece::toString()
{
//	char mask[5] = "xxxx";

	std::string msg = position.toString(',');
	msg = "{\"P\":[" + msg + "],";
	msg += "\"D\":{";

//	mask[0] = (primal&8) == 8 ? '1' : '0';
//	mask[1] = (primal&4) == 4 ? '1' : '0';
//	mask[2] = (primal&2) == 2 ? '1' : '0';
//	mask[3] = (primal&1) == 1 ? '1' : '0';
//	msg += "\"p\": \"" + std::string(mask) + "\", ";
	msg += "\"p\":" + intToString(primal) + ",";

//	mask[0] = (dual&8) == 8 ? '1' : '0';
//	mask[1] = (dual&4) == 4 ? '1' : '0';
//	mask[2] = (dual&2) == 2 ? '1' : '0';
//	mask[3] = (dual&1) == 1 ? '1' : '0';
//	msg += "\"d\": \"" + std::string(mask) + "\"";
	msg += "\"d\":" + intToString(dual) + "";

	msg += "}";
	msg += "}";
//	msg += "},";
//	msg += "\"C\": {} }";

//	"position": [0, 0, 0],
//	"defects": {
//		"primal": [1, 1, 0, 0],
//		"dual": [0, 0, 0, 0]
//	}

	return msg;
}

plumbingpiece::plumbingpiece()
{
	primal = 0;
	dual = 0;
}

void plumbingpiece::setMask(bool isPrimal, unsigned int mask)
{
	if(isPrimal)
		primal |= mask;
	else
		dual |= mask;
}

bool plumbingpiecesgenerator::countKey(long x, long y, long z)
{
	if(plumbMap.count(x) > 0 && plumbMap[x].count(y) > 0 && plumbMap[x][y].count(z) > 0)
	{
		return true;
	}
	return false;
}

int plumbingpiecesgenerator::getPlumbingPieceIndex(convertcoordinate& coord)
{
//	std::string key = coord.toString(',');
	//if(plumbMap.find(key) == plumbMap.end())
	if(!countKey(coord[0], coord[1], coord[2]))
	{
		plumbingpiece piece;
		piece.position = coord;
		pieces.push_back(piece);

//		plumbMap[key] = pieces.size() - 1;
		plumbMap[coord[0]][coord[1]][coord[2]] = pieces.size() - 1;
	}

	return plumbMap[coord[0]][coord[1]][coord[2]];
}

void plumbingpiecesgenerator::removeWrongCorners()
{
	for (int i = pieces.size() - 1; i >= 0; i--)
	{
		if (potentialCornerCounts.find(i) != potentialCornerCounts.end())
		{
			bool isCorner = (potentialCornerCounts[i] > 1);
			bool hasPrimalOrigin = pieces[i].primal == 8;
			bool hasDualOrigin = pieces[i].dual == 8;
			if ((hasPrimalOrigin || hasDualOrigin) && !isCorner)
			{
//				printf("%s\n", pieces[i].position.toString(',').c_str());
				pieces[i] = pieces.back();
				pieces.pop_back();
			}
		}
	}
}

void plumbingpiecesgenerator::generateFromGeometry(geometry& geom)
{
	//1+8=9,2+8=10,4+8=12
	for(std::vector<std::pair<long, long> >::iterator it = geom.segs.begin();
			it != geom.segs.end(); it++)
	{
		bool isIO = false;
		isIO = isIO || std::find(geom.io.begin(), geom.io.end(), it->first) != geom.io.end();
		isIO = isIO || std::find(geom.io.begin(), geom.io.end(), it->second) != geom.io.end();

		if(isIO)
			continue;

		convertcoordinate c1 = geom.coords[it->first];
		convertcoordinate c2 = geom.coords[it->second];

		bool primal = c1.isPrimalCorner(DELTA);

		long dxd = c1.at(0) - c2.at(0);
		long dyd = c1.at(1) - c2.at(1);
		long dzd = c1.at(2) - c2.at(2);

		long dx = dxd / DELTA;
		long dy = dyd / DELTA;
		long dz = dzd / DELTA;

		long dt = 0;
		int valPiece = 0;
		int index = -1;

		if(dx != 0)
		{
			valPiece = 12;//1100
			index = 0;
			dt = dx;
		}
		else if (dy != 0)
		{
			valPiece = 10;//1010
			index = 1;
			dt = dy;
		}
		else if (dz != 0)
		{
			valPiece = 9;//1001
			index = 2;
			dt = dz;
		}

		long direction = dt/abs(dt);

		convertcoordinate startPrimalPieceCoordinate(c1);
		if(c1.at(index) > c2.at(index))//cauta inceputul segmentului
		{
			startPrimalPieceCoordinate = c2;
			direction *= -1;//inverseaza directia
		}

		//coordinate is taken from a Center
		//and needs to be corner
		for(int i = 0; i < 3; i++)
		{
			int sign = -1;
			startPrimalPieceCoordinate[i] += sign * abs(DELTA + startPrimalPieceCoordinate[i])%DELTA;
		}

		//imparte la DELTA ca sa obtii plumbingpiece
		for(int i=0; i<3; i++)
		{
			startPrimalPieceCoordinate[i] /= DELTA;
		}

		//indiferent ca e dual sau primal tre sa misc intai coordonata asta
		convertcoordinate iterCoord(startPrimalPieceCoordinate);

		for(int i=0; i <= abs(dt); i++)
		{
			int localValPiece = valPiece;

			iterCoord.at(index) = startPrimalPieceCoordinate.at(index) - i * direction;
			int ppIdx = getPlumbingPieceIndex( iterCoord );

			if(i == abs(dt))
			{
				//always add last origin
				localValPiece = 8;
			}

			if(potentialCornerCounts.find(ppIdx) == potentialCornerCounts.end())
			{
				potentialCornerCounts[ppIdx] = 0;
			}
			potentialCornerCounts[ppIdx]++;

			pieces[ppIdx].setMask(primal, localValPiece);
		}
	}
}
