#ifndef IRICLIB_CGNSFILE_H
#define IRICLIB_CGNSFILE_H

#include "iriclib_global.h"

namespace iRICLib {

class CgnsFile
{
public:
	CgnsFile();
	~CgnsFile();

	void setFileId(int fileId);

	// -----------------------------------
	// Initialization and Basic functions
	// -----------------------------------
	int Init();
	int InitRead_Base(char* basename);
	int InitRead();

	int GotoBase(int* B);
	int GotoCC();
	int GotoRawDataTop();

	int Set_ZoneId(int zoneid);


private:
	Impl* impl;
};

} // iRICLib

#endif // IRICLIB_CGNSFILE_H
