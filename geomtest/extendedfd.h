#ifndef EXTENDEDFD_H_
#define EXTENDEDFD_H_

#include <vector>

using namespace std;

/**
 *  the extendedFD class is used to simulate the text files
 *  used as inputs to the faildistillations class
 *
 *  the major parameters necessary for simulation
 *  	nrio: the number of circuit inputs
 *  	distboxes: the total number of distillation boxes in the geometry
 *
 *  for each of the above parameters the number of pins will be given by multiplication with 2
 *
 *	the list of distillation boxes is assumed to have the following format
 *	a. CIRC IO BOXES
 *	b. ATYPE BOXES
 *	c. YTYPE BOXES
 *
 *	The length of part a. will be 2*nrio, and the boxes are of mixed types
 *	The total length of a+b+c is 2*distboxes, and is computed in the computeadditional class
 */
class extendedFD : public faildistillations
{
public:
	extendedFD(int nrio, int distboxes)
	{
		EXPECT_GE(distboxes, nrio);

		int ios = addIO(nrio);
		EXPECT_EQ(ios, nrio);

		int pints = addDistPins(nrio, distboxes);
		EXPECT_EQ(pints, 2*distboxes);
	}

	int addIO(int maxnrio)
	{
		for(int nrio=0; nrio<maxnrio; nrio++)
		{
			//format of the entry expected by numberandcoordinate class
			//A 25 2 2 0 2 2 2

			vector<int> typeio(8);

			//even indices are ATYPE, the odd indices are YTYPE
			typeio[TYPE] = nrio % 2;
			//the id of the injection io plays no role
			typeio[INJNR] = -nrio;

			for(int i=0; i<6; i++)
			{
				//coordinates will play no role
				typeio[OFFSETNONCOORD + i] = nrio;
			}

			circIOs.addEntry(typeio);
		}

		return circIOs.size();
	}

	int addDistPins(int maxnrio, int maxdistboxes)
	{
		int nrdist = 0;

		//first: add distillation boxes corresponding to the IOs added in addIO
		//second: add additional boxes where
		//	- first half ATYPE
		//	- second half YTYPE
		for(int nrBox=0; nrBox<maxdistboxes; nrBox++)
		{
			int type = -1;

			if(nrBox < maxnrio)
			{
				type = (nrBox) % 2;
			}
			else
			{
				int position = nrBox - maxnrio;
				int length = maxdistboxes - maxnrio;

				int halfnr = (2*position)/length;

				type = halfnr;
			}

			//for each box there are two pins
			for(int i=0; i<2; i++)
			{
				vector<int> pin;
				//pin type
				pin.push_back(type);
				//pin coordinates
				for(int i=0; i<3; i++)
				{
					//coordinates will play no role, for the moment
					pin.push_back(nrBox);
				}
				//add pin
				distpins.push_back(pin);
			}
		}
		return distpins.size();
	}

	void makeHorizontalPin(int idx)
	{
		//modify the first coordinate between two neighbouring pins
		distpins[2*idx + 1][1] = distpins[2*idx + 0][1] + 1;
	}
};


#endif /* EXTENDEDFD_H_ */
