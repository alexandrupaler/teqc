#include "databasereader.h"
#include "fileformats/generaldefines.h"

void databasereader::fillTranslations()
{
	FILE* self = fopen(GATENUMBERSDB, "r");

	int number = -100;
	char el[1024] = {0};

	while(!feof(self))
	{
		fscanf(self, "%s ", el);
		if(strcmp(el, "#define") == 0)
		{
			fscanf(self, "%s %d ", el, &number);
			string s(el);
			nameToInt[el] = number;
			intToName[number] = el;

			//printf("%s %d\n", el, number);
		}
	}

	fclose(self);
}

int databasereader::getNumber(char* key)
{
	return nameToInt.at(string(key));
}

void databasereader::readOperations(FILE* fp, vector<int>& list)
{
	char c = '?';
	char op[100];
	int pos = -1;
	while (!feof(fp) && (c!='\r') && (c!='\n'))
	{
		fscanf(fp, "%c", &c);

		if(feof(fp) || isspace(c))
		{
			if(pos != -1)
			{
				op[++pos] = '\0';
				int val = getNumber(op);
				list.push_back(val);
			}
			pos = -1;
		}
		else
		{
			op[++pos] = c;
		}
	}
	//printf("----\n");
}

bool databasereader::readCnot(FILE* fp, vector<int>& list, int valoffset)
{
	char c = '?';
	char op[100];
	int pos = -2;
	while (!feof(fp) && (c!='\r') && (c!='\n'))
	{
		fscanf(fp, "%c", &c);

		if(feof(fp) || isspace(c))
		{
			if(pos != -1)
			{
				op[++pos] = '\0';
				int val = atoi(op);
				list.push_back(val - valoffset);//indices from zero?
				//printf("%s %d| ", op, val);
			}
			pos = -1;
		}
		else
		{
			if(pos == -2)
			{
				if(c == 'c')//line starts with 'c'
				{
					pos = -1;
				}
				else
				{
					fseek(fp, -1, SEEK_CUR);
					return false;
				}
			}
			else
			{
				op[++pos] = c;
			}
		}
	}
	//printf("----\n");
	return true;
}

decomposition databasereader::readDecomposition(FILE* file, string name)
{
	decomposition el;
	el.name = name;

	char type[100];
	fscanf(file, "%s", type);

	if(strcmp(type, "icm") == 0)
	{
		el.type = DecompType::icm;
	}
	else if(strcmp(type, "nicm") == 0)
	{
		el.type = DecompType::nonicm;
	}
	else if(strcmp(type, "icmdist") == 0)
	{
		el.type = DecompType::disticm;
	}

	fscanf(file, "%d ", &(el.nrAncilla));

	if(!el.isicm())
	{
		//printf("nr ancilla %d\n", el.nrancilla);

		for(int i=0; i<el.nrAncilla + 1; i++)
		{
			vector<int> line;
			readOperations(file, line);
			el.gates.push_back(line);
		}
	}
	else
	{
		readOperations(file, el.inits);
		vector<int> cnot;
		bool cn = readCnot(file, cnot, 1/*offset to the back the indices by 1*/);
		while(cn)
		{
			el.cnots.push_back(cnot);
			cnot.clear();
			cn = readCnot(file, cnot, 1);
		}
		readOperations(file, el.meas);
	}

	return el;
}


string databasereader::findDecomposition(FILE* file)
{
	char line[1024];
	while(!feof(file))
	{
		fscanf(file, "%s", line);
		if(line[0] == '=')
		{
			return string(line + 1);
		}
	}
	return "";
}

void databasereader::readDatabase(map<string, decomposition>& decomp)
{
	FILE* file = fopen("database", "r");

	while(!feof(file))
	{
		//read until string starts with name
		string name = findDecomposition(file);
		if(!name.empty())
		{
			//printf("%s\n", name.c_str());
			decomposition el = readDecomposition(file, name);
			decomp[name] = el;
		}
	}

	fclose(file);
}

databasereader::databasereader()
{
	fillTranslations();
	readDatabase(decomp);
}
