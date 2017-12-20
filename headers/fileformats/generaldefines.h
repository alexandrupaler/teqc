#ifndef GENERALDEFINES_H_
#define GENERALDEFINES_H_

#include <vector>

//the underlying datastructure for qubitlines and filelines is the same
//but, the main difference is their interpretation
typedef std::vector<int> qubitline;
typedef std::vector<long> fileline;

//ugly
#define GATENUMBERSDB "headers/gatenumbers.h"

#define DELTA 6

#endif /* GENERALDEFINES_H_ */
