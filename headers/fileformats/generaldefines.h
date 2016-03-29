#ifndef GENERALDEFINES_H_
#define GENERALDEFINES_H_

#include <vector>

using namespace std;

//the underlying datastructure for qubitlines and filelines is the same
//but, the main difference is their interpretation
typedef vector<int> qubitline;
typedef vector<long> fileline;

//ugly
#define GATENUMBERSDB "headers/gatenumbers.h"

#define DELTA 2

#endif /* GENERALDEFINES_H_ */
