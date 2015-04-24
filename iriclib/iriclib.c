#define NAME_MAXLENGTH 200
#define ZONESIZE_LENGTH 9
#define FILEID_MAX 100
#define ARRAYINCREMENTSTEP 3
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cgnslib.h>
#include "iriclib.h"
#define IRICBASE "iRIC"
#define IRICZONE "iRICZone"
#define iRICBASE1D "iRIC1D"
#define iRICBASE2D "iRIC2D"
#define iRICBASE3D "iRIC3D"
#define BINAME "BaseIterativeData"
#define ZINAME "ZoneIterativeData"

#define CCNODE "CalculationConditions"
#define RDNODE "GeographicData"
#define GCNODE "GridConditions"
#define GCCNODE "GridComplexConditions"
#define ECNODE "ErrorCode"

// File index for the next cgns file
static int fileindexnext = 0;
// Length of arrays for cgns file properties
static int filebufferlen = 0;
// Array that relates a cgns file id to an iriclib's file index
static int* fileindex = 0;
// length of fileindex
static int fileindexlength = 0;

static int lastfileid;
/// Currently active base id
static int* baseid = 0;
/// Base id that contains calculation condition
static int* ccbaseid = 0;
/// Currently active zone id (that has name "iRICZone").
static int* zoneid = 0;
/// Currently active solution id
static int* solid = 0;

static int* celldim = 0;
static int* physdim = 0;
static cgsize_t** zonesize = 0;
static int* bccount = 0;
static int* bcbufferlen = 0;
static char** bcnames = 0;

static int* solmemsize = 0;
static char** solpointers = 0;

static int* timememsize = 0;
static double** timearray = 0;

static int* iterationmemsize = 0;
static int** indexarray = 0;

static char*** baseiterintnamearray = 0;
static int*** baseiterintdataarray = 0;
static int* baseiterintdatacount = 0;
static int* baseiterintmemsize = 0;

static char*** baseiterrealnamearray = 0;
static double*** baseiterrealdataarray = 0;
static int* baseiterrealdatacount = 0;
static int* baseiterrealmemsize = 0;

static int* gridcoordmemsize = 0;
static char** coordpointers = 0;

static int* particlememsize = 0;
static char** particlepointers = 0;

static int local_gotozone_Mul(int fid){
	return cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "end");
}

static int local_gotobase_Mul(int fid){
	return cg_goto(fid, *(baseid + fileindex[fid]), "end");
}

static int local_gotoccbase_Mul(int fid){
	return cg_goto(fid, *(ccbaseid + fileindex[fid]), "end");
}

static int local_gotobaseiterative_Mul(int fid){
	return cg_goto(fid, *(baseid + fileindex[fid]), BINAME, 0, "end");
}

static int local_gotoccbaseiterative_Mul(int fid){
	return cg_goto(fid, *(ccbaseid + fileindex[fid]), BINAME, 0, "end");
}

static int local_gotozoneiterative_Mul(int fid){
	return cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), ZINAME, 0, "end");
}

static int local_gotoccb_Mul(int fid, const char* path){
	int ier;
	ier = local_gotoccbase_Mul(fid);
	if (ier != 0){return ier;}
	ier = cg_gopath(fid, path);
	if (ier != 0){return ier;}
	return 0;
}

static int local_gotoz_Mul(int fid, const char* path){
	int ier;
	ier = local_gotozone_Mul(fid);
	if (ier != 0){return ier;}
	ier = cg_gopath(fid, path);
	if (ier != 0){return ier;}
	return 0;
}

static int local_gotocc_Mul(int fid){
	return local_gotoccb_Mul(fid, CCNODE);
}

static int local_gotord_Mul(int fid){
	return local_gotoccb_Mul(fid, RDNODE);
}

static int local_gotogc_Mul(int fid){
	return local_gotoz_Mul(fid, GCNODE);
}

static int local_gotoccchild_Mul(int fid, const char* path){
	char tmpstr[NAME_MAXLENGTH];
	size_t slen;
	strcpy(tmpstr, CCNODE);
	slen = strlen(CCNODE);
	strcpy(tmpstr + slen, "/");
	strcpy(tmpstr + slen + 1, path);
	return local_gotoccb_Mul(fid, tmpstr);
}

static int local_gotogcchild_Mul(int fid, const char* path){
	char tmpstr[NAME_MAXLENGTH];
	size_t slen;
	strcpy(tmpstr, GCNODE);
	slen = strlen(GCNODE);
	strcpy(tmpstr + slen, "/");
	strcpy(tmpstr + slen + 1, path);
	return local_gotoz_Mul(fid, tmpstr);
}

static int local_gotogcnewchild_Mul(int fid, char* name){
	int ier;
	ier = local_gotogc_Mul(fid);
	if (ier != 0){return ier;}
	// Try to delete the user defined node with the same name. It succeeds only when it already exist.
	ier = cg_delete_node(name);
	// Add UserDefiniedNode name
	ier = cg_user_data_write(name);
	if (ier != 0){return ier;}
	// Goto the new created node.
	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	return 0;
}

static int local_find_array(char* name, int* index, DataType_t* datatype, int* dim, cgsize_t* dimVec){
	int ier;
	int i, narrays;
	char tmparrayname[NAME_MAXLENGTH];

	ier = cg_narrays(&narrays);
	if (ier != 0){return ier;} // error handling

	for (i = 1; i <= narrays; ++i){
		ier = cg_array_info(i, tmparrayname, datatype, dim, dimVec);
		if (ier != 0){return ier;} // error handling
		if (strcmp(tmparrayname, name) == 0){
			*index = i;
			return 0;
		}
	}
	// not found.
	return -1;
}

static int local_read_array(char* arrayname, DataType_t datatype, cgsize_t length, void* memory)
{
	int ier;
	int index;
	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];

	ier = local_find_array(arrayname, &index, &dt, &dim, &(dimVec[0]));
	if (ier != 0){return ier;} // error handling
	// check datatype
	if (datatype != dt){return -2;}
	// check datalength if needed
	if (length != -1){
		if (dim != 1 || dimVec[0] != length){return -3;}
	}
	// found. load data.
	ier = cg_array_read(index, memory);
	if (ier != 0){return ier;} // error handling
	return 0;
}

static int local_read_array_as(char* arrayname, DataType_t datatype, size_t length, void* memory)
{
	int ier;
	int index;
	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];

	ier = local_find_array(arrayname, &index, &dt, &dim, &(dimVec[0]));
	if (ier != 0){return ier;} // error handling
	// check datalength if needed
	if (length != -1){
		if (dim != 1 || dimVec[0] != length){return -3;}
	}
	ier = cg_array_read_as(index, datatype, memory);
	if (ier != 0){return ier;} // error handling
	return 0;
}

static int local_read_string(char* arrayname, size_t bufferlen, char* buffer)
{
	int ier;
	int index;
	DataType_t datatype;
	int dim;
	cgsize_t dimVec[3];

	ier = local_find_array(arrayname, &index, &datatype, &dim, &(dimVec[0]));

	// check datatype
	if (datatype != Character){return -1;}
	if (bufferlen != -1){
		if (dim != 1 || dimVec[0] >= bufferlen){return -2;}
	}
	// found. load data.
	ier = cg_array_read(index, buffer);
	if (ier != 0){return ier;} // error handling
	*(buffer + dimVec[0]) = '\0';
	return 0;
}

static int local_write_array(char* arrayname, DataType_t datatype, size_t length, void* memory)
{
	int ier, dim;
	cgsize_t dimvec;
	dim = 1;
	dimvec = length;
	// If array named arrayname exists, delete it first. It succeeds only when the node exist.
	cg_delete_node(arrayname);
	if (length == 0){
		// If the length of the array is 0, cg_array_write fails.
		return 0;
	}
	ier = cg_array_write(arrayname, datatype, dim, &dimvec, memory);
	return ier;
}

static int local_write_string(char* arrayname, char* value)
{
	int ier, dim;
	cgsize_t dimvec;
	dim = 1;
	dimvec = strlen(value);
	// If array named arrayname exists, delete it first. It succeeds only when the node exist.
	cg_delete_node(arrayname);
	if (dimvec == 0){
		// If the length of the string is 0, cg_array_write fails.
		return 0;
	}
	ier = cg_array_write(arrayname, Character, dim, &dimvec, value);
	return ier;
}

static void local_get_complex_name(int num, char* name){
	sprintf(name, "Item%d", num);
}

static int local_gotocomplexgroup_Mul(int fid, char*groupname){
	return cg_goto(fid, *(baseid + fileindex[fid]), GCCNODE, 0, groupname, 0, "end");
}

static int local_gotocomplex_Mul(int fid, char* groupname, int num){
	return cg_goto(fid, *(baseid + fileindex[fid]), GCCNODE, 0, groupname, 0, "UserDefinedData_t", num, "end");
}

static int local_gotocomplexchild_Mul(int fid, char* groupname, int num, char* name){
	return cg_goto(fid, *(baseid + fileindex[fid]), GCCNODE, 0, groupname, 0, "UserDefinedData_t", num, name, 0, "end");
}

static int local_complex_add_gccnode_Mul(int fid)
{
	int ret;
	int usize;
	char tmpname[NAME_MAXLENGTH];
	int U;

	ret = local_gotobase_Mul(fid);
	if (ret != 0){return ret;}
	// try to find GridComplexCondition node.
	ret = cg_nuser_data(&usize);
	if (ret != 0){return ret;}
	for (U = 1; U <= usize; ++U){
		cg_user_data_read(U, tmpname);
		if (strcmp(tmpname, GCCNODE) == 0){return 0;}
	}
	// not exists. try to add node.
	ret = cg_user_data_write(GCCNODE);
	return ret;
}

static int local_gotocomplexchild_create_Mul(int fid, char* groupname, int num, char* name){
	char tmpname[NAME_MAXLENGTH];
	int ier;
	ier = local_gotocomplexgroup_Mul(fid, groupname);
	if (ier != 0){
		// group node does not exist. create.
		ier = cg_goto(fid, *(baseid + fileindex[fid]), GCCNODE, 0, "end");
		if (ier != 0){return ier;} // error handling.
		ier = cg_user_data_write(groupname);
		if (ier != 0){return ier;} // error handling.
	}
	ier = local_gotocomplex_Mul(fid, groupname, num);
	if (ier != 0){
		// node does not exist. create.
		ier = local_gotocomplexgroup_Mul(fid, groupname);
		if (ier != 0){return ier;} // error handling.
		local_get_complex_name(num, tmpname);
		ier = cg_user_data_write(tmpname);
		if (ier != 0){return ier;} // error handling.
	}
	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){
		// child node does not exist. create.
		ier = local_gotocomplex_Mul(fid, groupname, num);
		if (ier != 0){return ier;} // error handling.
		ier = cg_user_data_write(name);
		if (ier != 0){return ier;} // error handling.
	}
	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	return ier;
}

void local_init_fileindex(int fid){
	// extend arrays if necessary.
	if (! (fileindex && fid < fileindexlength)){
		oldlength = fileindexlength;
		fileindexlength = (fid / FILEID_MAX + 1) * FILEID_MAX;
		fileindex = realloc(fileindex, sizeof(int) * fileindexlength);
		for (i = oldlength; i < fileindexlength; i++){
			fileindex[i] = -1;
		}
	}
}

void local_init_properties(int fid){
	int i, j, oldlength;

	local_init_fileindex(fid);

	if (fileindex[fid] != -1){
		// this fid is already used. reuse it.

		*(baseid + fileindex[fid]) = 0;
		*(ccbaseid + fileindex[fid]) = 0;
		*(zoneid + fileindex[fid]) = 0;
		*(solid + fileindex[fid]) = 0;
		*(celldim + fileindex[fid]) = 0;
		*(physdim + fileindex[fid]) = 0;
		// the memory for zone size is allocated last time.
		// *(zonesize + fileindex[fid]) = malloc(sizeof(cgsize_t) * ZONESIZE_LENGTH);
		for (j = 0; j < ZONESIZE_LENGTH; ++j){
			(*(zonesize + fileindex[fid]))[j] = 0;
		}
		*(bccount + fileindex[fid]) = 0;
		*(bcbufferlen + fileindex[fid]) = 10;
		if (*(bcnames + fileindex[fid]) != 0){
			free(*(bcnames + fileindex[fid]));
		}
		*(bcnames + fileindex[fid]) = (char*)malloc(sizeof(char) * *(bcbufferlen + fileindex[fid]) * 32);
		*(solmemsize + fileindex[fid]) = 0;
		if (*(solpointers + fileindex[fid]) != 0){
			free(*(solpointers + fileindex[fid]));
			*(solpointers + fileindex[fid]) = 0;
		}
		*(timememsize + fileindex[fid]) = 0;
		if (*(timearray + fileindex[fid]) != 0){
			free(*(timearray + fileindex[fid]));
			*(timearray + fileindex[fid]) = 0;
		}
		*(iterationmemsize + fileindex[fid]) = 0;
		if (*(indexarray + fileindex[fid]) != 0){
			free(*(indexarray + fileindex[fid]));
			*(indexarray + fileindex[fid]) = 0;
		}
		*(baseiterintdatacount + fileindex[fid]) = 0;
		*(baseiterintmemsize + fileindex[fid]) = 10;
		if (*(baseiterintnamearray + fileindex[fid]) != 0){
			free(*(baseiterintnamearray + fileindex[fid]));
			*(baseiterintnamearray + fileindex[fid]) = 0;
		}
		if (*(baseiterintdataarray + fileindex[fid]) != 0){
			free(*(baseiterintdataarray + fileindex[fid]));
			*(baseiterintdataarray + fileindex[fid]) = 0;
		}

		*(baseiterrealdatacount + fileindex[fid]) = 0;
		*(baseiterrealmemsize + fileindex[fid]) = 10;
		if (*(baseiterrealnamearray + fileindex[fid]) != 0){
			free(*(baseiterrealnamearray + fileindex[fid]));
			*(baseiterrealnamearray + fileindex[fid]) = 0;
		}
		if (*(baseiterrealdataarray + fileindex[fid]) != 0){
			free(*(baseiterrealdataarray + fileindex[fid]));
			*(baseiterrealdataarray + fileindex[fid]) = 0;
		}
		*(gridcoordmemsize + fileindex[fid]) = 0;
		if (*(coordpointers + fileindex[fid]) != 0){
			free(*(coordpointers + fileindex[fid]));
			*(coordpointers + fileindex[fid]) = 0;
		}
		*(particlememsize + fileindex[fid]) = 0;
		if (*(particlepointers + fileindex[fid]) != 0){
			free(*(particlepointers + fileindex[fid]));
			*(particlepointers + fileindex[fid]) = 0;
		}
	} else {
		// this fid is used for the first time.
		fileindex[fid] = fileindexnext++;
	}

	// initialize file-local variables.
	if (! (fileindex[fid] < filebufferlen)){
		filebufferlen += ARRAYINCREMENTSTEP;
		baseid = realloc(baseid, sizeof(int*) * filebufferlen);
		ccbaseid = realloc(ccbaseid, sizeof(int*) * filebufferlen);
		zoneid = realloc(zoneid, sizeof(int*) * filebufferlen);
		solid = realloc(solid, sizeof(int*) * filebufferlen);
		celldim = realloc(celldim, sizeof(int*) * filebufferlen);
		physdim = realloc(physdim, sizeof(int*) * filebufferlen);
		zonesize = realloc(zonesize, sizeof(cgsize_t*) * filebufferlen);
		bccount = realloc(bccount, sizeof(int) * filebufferlen);
		bcbufferlen = realloc(bcbufferlen, sizeof(int) * filebufferlen);
		bcnames = realloc(bcnames, sizeof(char*) * filebufferlen);

		solmemsize = realloc(solmemsize, sizeof(int) * filebufferlen);
		solpointers = realloc(solpointers, sizeof(char*) * filebufferlen);

		timememsize = realloc(timememsize, sizeof(int) * filebufferlen);
		timearray = realloc(timearray, sizeof(double*) * filebufferlen);

		iterationmemsize = realloc(iterationmemsize, sizeof(int) * filebufferlen);
		indexarray = realloc(indexarray, sizeof(int*) * filebufferlen);

		baseiterintnamearray = realloc(baseiterintnamearray, sizeof(char**) * filebufferlen);
		baseiterintdataarray = realloc(baseiterintdataarray, sizeof(int**) * filebufferlen);
		baseiterintdatacount = realloc(baseiterintdatacount, sizeof(int) * filebufferlen);
		baseiterintmemsize = realloc(baseiterintmemsize, sizeof(int) * filebufferlen);

		baseiterrealnamearray = realloc(baseiterrealnamearray, sizeof(char**) * filebufferlen);
		baseiterrealdataarray = realloc(baseiterrealdataarray, sizeof(double**) * filebufferlen);
		baseiterrealdatacount = realloc(baseiterrealdatacount, sizeof(int) * filebufferlen);
		baseiterrealmemsize = realloc(baseiterrealmemsize, sizeof(int) * filebufferlen);

		gridcoordmemsize = realloc(gridcoordmemsize, sizeof(int) * filebufferlen);
		coordpointers = realloc(coordpointers, sizeof(char*) * filebufferlen);

		particlememsize = realloc(particlememsize, sizeof(int) * filebufferlen);
		particlepointers = realloc(particlepointers, sizeof(char*) * filebufferlen);

		for (i = filebufferlen - ARRAYINCREMENTSTEP; i < filebufferlen; ++i){
			*(baseid + i) = 0;
			*(ccbaseid + i) = 0;
			*(zoneid + i) = 0;
			*(solid + i) = 0;
			*(celldim + i) = 0;
			*(physdim + i) = 0;
			*(zonesize + i) = malloc(sizeof(cgsize_t) * ZONESIZE_LENGTH);
			for (j = 0; j < ZONESIZE_LENGTH; ++j){
				(*(zonesize + i))[j] = 0;
			}
			*(bccount + i) = 0;
			*(bcbufferlen + i) = 0;
			*(bcnames + i) = 0;

			*(solmemsize + i) = 0;
			*(solpointers + i) = 0;

			*(timememsize + i) = 0;
			*(timearray + i) = 0;
			*(iterationmemsize + i) = 0;
			*(indexarray + i) = 0;

			*(baseiterintdatacount + i) = 0;
			*(baseiterintmemsize + i) = 10;
			*(baseiterintnamearray + i) = 0;
			*(baseiterintdataarray + i) = 0;

			*(baseiterrealdatacount + i) = 0;
			*(baseiterrealmemsize + i) = 10;
			*(baseiterrealnamearray + i) = 0;
			*(baseiterrealdataarray + i) = 0;

			*(gridcoordmemsize + i) = 0;
			*(coordpointers + i) = 0;

			*(particlememsize + i) = 0;
			*(particlepointers + i) = 0;
		}
	}
}

static int local_init_biter_Mul(int fid){
	int ier;
	ier = local_gotobase_Mul(fid);
	if (ier != 0){return ier;}
	// delete old base iterative data if exists.
	cg_delete_node(BINAME);
	// the number of base iterative data is 0 first, but when we give 0 as the
	// fourth argument it returns error. so, we use value 1.
	ier = cg_biter_write(fid, *(baseid + fileindex[fid]), BINAME, 1);
	if (ier != 0){return ier;}
	return 0;
}

static int local_init_ziter_Mul(int fid){
	int ier;
	ier = local_gotozone_Mul(fid);
	if (ier != 0){return ier;}
	cg_delete_node(ZINAME);
	ier = cg_ziter_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), ZINAME);
	if (ier != 0){return ier;}
	return 0;
}

static int local_load_sol_Mul(fid){
	char arrayname[NAME_MAXLENGTH];
	DataType_t datatype;
	int arrays;
	int dim;
	cgsize_t dimvec;
	int ier;
	int i;
	int intBiIndex;
	int intBiCount;
	int realBiIndex;
	int realBiCount;

	intBiIndex = 0;
	intBiCount = 0;
	realBiIndex = 0;
	realBiCount = 0;
	ier = local_gotobase_Mul(fid);
	if (ier != 0){return ier;}
	// load solution count.
	ier = cg_nsols(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), solid + fileindex[fid]);
	if (ier != 0){return ier;}
	// load TimeValues, IterationValues, etc.
	local_gotobaseiterative_Mul(fid);
	ier = cg_narrays(&arrays);
	if (ier != 0){return ier;}

	for (i = 1; i <= arrays; ++i){
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		if (strcmp(arrayname, "TimeValues") == 0){
			*(timememsize + fileindex[fid]) = dimvec;
			*(timearray + fileindex[fid]) =
				(double*)malloc(sizeof(double) * dimvec);
			ier = cg_array_read(i, *(timearray + fileindex[fid]));
			if (ier != 0){return ier;}
		} else if (strcmp(arrayname, "IterationValues") == 0){
			*(iterationmemsize + fileindex[fid]) = dimvec;
			*(indexarray + fileindex[fid]) =
				(int*)malloc(sizeof(int) * dimvec);
			ier = cg_array_read(i, *(indexarray + fileindex[fid]));
			if (ier != 0){return ier;}
		} else {
			if (datatype == RealDouble){
				++ realBiCount;
			} else if (datatype == RealSingle){
				++ realBiCount;
			} else if (datatype == Integer){
				++ intBiCount;
			}
		}
	}
	*(baseiterintdatacount + fileindex[fid]) = intBiCount;
	if (intBiCount > 0){
		*(baseiterintnamearray + fileindex[fid]) = (char**)malloc(sizeof(char*) * intBiCount);
		*(baseiterintdataarray + fileindex[fid]) = (int**)malloc(sizeof(int*) * intBiCount);
	}

	*(baseiterrealdatacount + fileindex[fid]) = realBiCount;
	if (realBiCount > 0){
		*(baseiterrealnamearray + fileindex[fid]) = (char**)malloc(sizeof(char*) * realBiCount);
		*(baseiterrealdataarray + fileindex[fid]) = (double**)malloc(sizeof(double*) * realBiCount);
	}

	for (i = 1; i <= arrays; ++i){
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		if (strcmp(arrayname, "TimeValues") == 0){
			// it is already read. ignore.
		} else if (strcmp(arrayname, "IterationValues") == 0){
			// it is already read. ignore.
		} else {
			if (datatype == RealDouble || datatype == RealSingle){
				// copy the name.
				*(*(baseiterrealnamearray + fileindex[fid]) + realBiIndex) = (char*)malloc(sizeof(char) * (strlen(arrayname) + 1));
				if (*(*(baseiterrealnamearray + fileindex[fid]) + realBiIndex) == NULL){return 1;}
				strcpy(*(*(baseiterrealnamearray + fileindex[fid]) + realBiIndex), arrayname);
				// copy the data.
				*(*(baseiterrealdataarray + fileindex[fid]) + realBiIndex) = (double*)malloc(sizeof(double) * dimvec);
				if (*(*(baseiterrealdataarray + fileindex[fid]) + realBiIndex) == NULL){return 1;}
				ier = cg_array_read_as(i, RealDouble, *(*(baseiterrealdataarray + fileindex[fid]) + realBiIndex));
				++ realBiIndex;
			} else if (datatype == Integer){
				// copy the name.
				*(*(baseiterintnamearray + fileindex[fid]) + intBiIndex) = (char*)malloc(sizeof(char) * (strlen(arrayname) + 1));
				if (*(*(baseiterintnamearray + fileindex[fid]) + intBiIndex) == NULL){return 1;}
				strcpy(*(*(baseiterintnamearray + fileindex[fid]) + intBiIndex), arrayname);
				// copy the data.
				*(*(baseiterintdataarray + fileindex[fid]) + intBiIndex) = (int*)malloc(sizeof(int) * dimvec);
				if (*(*(baseiterintdataarray + fileindex[fid]) + intBiIndex) == NULL){return 1;}
				ier = cg_array_read_as(i, Integer, *(*(baseiterintdataarray + fileindex[fid]) + intBiIndex));
				++ intBiIndex;
			}
		}
	}

	return 0;
}

static int local_init_bc_names_Mul(int fid){
	// get the number of boundary condition items.
	int ier;
	int tmpbccount;
	int BC;
	char tmpbcname[32];
	BCType_t bocotype;
	PointSetType_t ptset_type;
	cgsize_t npnts;
	int normalIndex;
	cgsize_t normalListFlag;
	DataType_t normalDataType;
	int ndataset;

	ier = cg_nbocos(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &tmpbccount);
	if (ier != 0){return ier;}
	*(bccount + fileindex[fid]) = tmpbccount;
	*(bcbufferlen + fileindex[fid]) = 10;
	while (*(bccount + fileindex[fid]) >= *(bcbufferlen + fileindex[fid])){
		*(bcbufferlen + fileindex[fid]) *= 2;
	}
	*(bcnames + fileindex[fid]) = (char*)malloc(sizeof(char) * *(bcbufferlen + fileindex[fid]) * 32);
	for (BC = 1; BC <= tmpbccount; ++ BC){
		cg_boco_info(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), BC, tmpbcname, &bocotype, &ptset_type, &npnts, &normalIndex, &normalListFlag, &normalDataType, &ndataset);
		// copy tmpbcname into bcnames
		memcpy(*(bcnames + fileindex[fid]) + 32 * (BC - 1), tmpbcname, sizeof(char) * 32);
	}
	return 0;
}

static int local_cg_init_sol_Mul(int fid){
	int ier;
	char solname[32];
	int i, u, nsols;
	char gridname[32];
	char tmpname[32];
	int ngrids, usize;
	GridLocation_t location;
	// initializes base iterative data.
	ier = local_init_biter_Mul(fid);
	if (ier != 0){return ier;}
	// initializes zone iterativedata.
	ier = local_init_ziter_Mul(fid);
	if (ier != 0){return ier;}
	// delete solution data under iRICZone node.
	ier = cg_nsols(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &nsols);
	if (ier != 0){return ier;}
	for (i = 0; i < nsols; ++i){
		ier = cg_sol_info(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), 1, solname, &location);
		if (ier != 0){return ier;}
		ier = cg_delete_node(solname);
		if (ier != 0){return ier;}
	}
	// delete grid data with index >= 2 under iRICZone node.
	ier = cg_ngrids(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &ngrids);
	for (i = 0; i < ngrids - 1; ++i){
		ier = cg_grid_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), 2, gridname);
		if (ier != 0){return ier;}
		ier = cg_delete_node(gridname);
		if (ier != 0){return ier;}
	}
	// delete particle data under iRICZone node.
	ier = local_gotozone_Mul(fid);
	if (ier != 0){return ier;}
	// try to find particle node.
	ier = cg_nuser_data(&usize);
	if (ier != 0){return ier;}
	for (u = usize; u >= 1; --u){
		ier = cg_user_data_read(u, tmpname);
		if (strncmp(tmpname, "ParticleSolution", 16) == 0){
			ier = cg_delete_node(tmpname);
			if (ier != 0){return ier;}
		}
	}

	*(solid + fileindex[fid]) = 0;
	return 0;
}

static int local_gotoccchild_create_Mul(int fid, char* name){
	int ier;
	const char* error;
	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){
		// not exist yet. create it.
		local_gotocc_Mul(fid);
		ier = cg_user_data_write(name);
		error = cg_get_error();

		if (ier != 0){return ier;}
		ier = local_gotoccchild_Mul(fid, name);
		if (ier != 0){return ier;}
	}
	return 0;
}

static int local_gc_add_gcnode_Mul(int fid)
{
	int ret;
	int usize;
	char tmpname[NAME_MAXLENGTH];
	int U;
	ret = local_gotozone_Mul(fid);
	if (ret != 0){return ret;}
	// try to find GridCondition node.
	ret = cg_nuser_data(&usize);
	if (ret != 0){return ret;}
	for (U = 1; U <= usize; ++U){
		cg_user_data_read(U, tmpname);
		if (strcmp(tmpname, GCNODE) == 0){return 0;}
	}
	// not exists. try to add node.
	ret = cg_user_data_write(GCNODE);
	return ret;
}

static int local_cg_iRIC_Add_Solution_Node_Mul(int fid){
	int ier, S;
	cgsize_t dimVec[2];
	char solname[32];
	size_t namelen;
	if (*(solmemsize + fileindex[fid]) == 0){
		// first time to call this function.
		*(solmemsize + fileindex[fid]) = 10;
		*(solpointers + fileindex[fid]) = (char*)malloc(sizeof(char) * *(solmemsize + fileindex[fid]) * 32);
	}
	if ((*(solid + fileindex[fid]) - 1) == *(solmemsize + fileindex[fid])){
		// the timearray is full of data.
		*(solmemsize + fileindex[fid]) *= 2;
		*(solpointers + fileindex[fid]) = realloc(*(solpointers + fileindex[fid]), sizeof(char) * *(solmemsize + fileindex[fid]) * 32);
		if (*(solpointers + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	sprintf(solname, "FlowSolution%d", *(solid + fileindex[fid]));
	namelen = strlen(solname);
	strcpy(*(solpointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1), solname);
	memset(*(solpointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1) + namelen, ' ', 32 - namelen);
	ier = local_gotozoneiterative_Mul(fid);
	if (ier != 0){return ier;}
	// write solution data.
	ier = cg_sol_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), solname, Vertex, &S);
	if (ier != 0){return ier;}
	// write solpointer data.
	dimVec[0] = 32;
	dimVec[1] = *(solid + fileindex[fid]);
	ier = cg_array_write("FlowSolutionPointers", Character, 2, dimVec, *(solpointers + fileindex[fid]));
	return ier;
}

static int local_cg_iRIC_Add_Sol_GridCoordNode_Mul(int fid){
	int G, ier;
	cgsize_t dimVec[2];
	char coordname[32];
	size_t coordnamelen;
	if (*(gridcoordmemsize + fileindex[fid]) == 0){
		// first time to call this function.
		*(gridcoordmemsize + fileindex[fid]) = 10;
		*(coordpointers + fileindex[fid]) = (char*)malloc(sizeof(char) * *(gridcoordmemsize + fileindex[fid]) * 32);
	}
	if ((*(solid + fileindex[fid]) - 1) == *(gridcoordmemsize + fileindex[fid])){
		// the timearray is full of data.
		*(gridcoordmemsize + fileindex[fid]) *= 2;
		*(coordpointers + fileindex[fid]) = realloc(*(coordpointers + fileindex[fid]), sizeof(char) * *(gridcoordmemsize + fileindex[fid]) * 32);
		if (*(coordpointers + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	sprintf(coordname, "GridCoordinatesForSolution%d", *(solid + fileindex[fid]));
	coordnamelen = strlen(coordname);
	strcpy(*(coordpointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1), coordname);
	memset(*(coordpointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1) + coordnamelen, ' ', 32 - coordnamelen);
	// write GridCoordinates_t node.
	ier = cg_grid_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), coordname, &G);
	if (ier != 0){return ier;}
	// write coordpointers data.
	local_gotozoneiterative_Mul(fid);
	dimVec[0] = 32;
	dimVec[1] = *(solid + fileindex[fid]);
	ier = cg_array_write("GridCoordinatesPointers", Character, 2, dimVec, *(coordpointers + fileindex[fid]));
	return ier;
}

static void local_get_particlesol_name(int num, char* name){
	sprintf(name, "ParticleSolution%d", num);
}

static int local_cg_iRIC_Add_Sol_ParticleNode_Mul(int fid){
	int G, ier;
	cgsize_t dimVec[2];
	char particlename[32];
	size_t particlenamelen;
	if (*(particlememsize + fileindex[fid]) == 0){
		// first time to call this function.
		*(particlememsize + fileindex[fid]) = 10;
		*(particlepointers + fileindex[fid]) = (char*)malloc(sizeof(char) * *(particlememsize + fileindex[fid]) * 32);
	}
	if ((*(solid + fileindex[fid]) - 1) == *(particlememsize + fileindex[fid])){
		// the timearray is full of data.
		*(particlememsize + fileindex[fid]) *= 2;
		*(particlepointers + fileindex[fid]) = realloc(*(particlepointers + fileindex[fid]), sizeof(char) * *(particlememsize + fileindex[fid]) * 32);
		if (*(particlepointers + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	local_get_particlesol_name(*(solid + fileindex[fid]), particlename);
	particlenamelen = strlen(particlename);
	strcpy(*(particlepointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1), particlename);
	memset(*(particlepointers + fileindex[fid]) + 32 * (*(solid + fileindex[fid]) - 1) + particlenamelen, ' ', 32 - particlenamelen);
	// write UserDefined_t node.
	ier = local_gotozone_Mul(fid);
	if (ier != 0){return ier;} // error handling.
	ier = cg_user_data_write(particlename);
	if (ier != 0){return ier;} // error handling.
	return 0;
}

static void local_get_bc_name(char* type, int num, char* name){
	sprintf(name, "%s_%d",type, num);
}

static void local_get_bc_index_Mul(int fid, char* type, int num, int* BC){
	char tmpname[32];
	int index;
	local_get_bc_name(type, num, tmpname);
	for (index = 1; index <= *(bccount + fileindex[fid]); ++index){
		if (strcmp(tmpname, (*(bcnames + fileindex[fid]) + 32 * (index - 1))) == 0){
			*BC = index;
			return;
		}
	}
	// not found.
	*BC = 0;
}

static int local_gotobc_Mul(int fid, char* type, int num){
	int BC;
	int ier;
	local_get_bc_index_Mul(fid, type, num, &BC);
	if (BC == 0){
		// it does not exists.
		return 1;
	}
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "ZoneBC_t", 1, "BC_t", BC, "end");
	return ier;
}

static int local_gotobcchild_Mul(int fid, char* type, int num, char* name){
	int BC;
	int ier;
	local_get_bc_index_Mul(fid, type, num, &BC);
	if (BC == 0){
		// it does not exists.
		return 1;
	}
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "ZoneBC_t", 1, "BC_t", BC, name, 0, "end");
	return ier;
}

static int local_gotobcchild_create_Mul(int fid, char* type, int num, char* name){
	int ier;
	const char* error;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){
		// not exist yet. create it.
		local_gotobc_Mul(fid, type, num);
		ier = cg_user_data_write(name);
		error = cg_get_error();

		if (ier != 0){return ier;}
		ier = local_gotobcchild_Mul(fid, type, num, name);
		if (ier != 0){return ier;}
	}
	return 0;
}

int cg_iRIC_Init(int fid){
	char tmpname[NAME_MAXLENGTH];
	int i, j, ier, nbases, nzones;
	cgsize_t tmpzonesize[9];

	// set CG_CONFIG_COMPRESS to 0.
	cg_configure(CG_CONFIG_COMPRESS, 0);

	// set file id first.
	lastfileid = fid;

	// initialize properties for fid.
	local_init_properties(fid);

	ier = cg_nbases(fid, &nbases);
	if (ier != 0){return ier;} // error handling
	for (i = 1; i <= nbases; ++i){
		ier = cg_base_read(fid, i, tmpname, celldim + fileindex[fid], physdim + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
		if (strcmp(IRICBASE, tmpname) == 0){
			// Base found!
			*(baseid + fileindex[fid]) = i;
			*(ccbaseid + fileindex[fid]) = i;
			ier = local_gotobase_Mul(fid);
			// Find "iRICZone" zone.
			ier = cg_nzones(fid, *(baseid + fileindex[fid]), &nzones);
			if (ier != 0){return ier;} // error handling
			for (j = 1; j <= nzones; ++j){
				ier = cg_zone_read(fid, *(baseid + fileindex[fid]), j, tmpname, tmpzonesize);
				if (ier != 0){return ier;} // error handling
				if (strcmp(IRICZONE, tmpname) == 0){
					// Zone found!
					*(zoneid + fileindex[fid]) = j;
					// initializes solution.
					ier = local_cg_init_sol_Mul(fid);
					if (ier != 0){return ier;} // error handling
					// initializes boundary condition name list.
					ier = local_init_bc_names_Mul(fid);
					if (ier != 0){return ier;} // error handling
					return ier;
				}
			}
		}
	}
	// not found.
	return 1;
}

int cg_iRIC_InitRead_Base(int fid, char* bname)
{
	char* basename = bname;
	if (basename == NULL){
		basename = IRICBASE;
	}

	char tmpname[NAME_MAXLENGTH];
	int i, j, ier, nbases, nzones;
	cgsize_t tmpzonesize[9];

	// set CG_CONFIG_COMPRESS to 0.
	cg_configure(CG_CONFIG_COMPRESS, 0);

	// set file id first.
	lastfileid = fid;

	// initialize properties for fid.
	local_init_properties(fid);

	ier = cg_nbases(fid, &nbases);
	if (ier != 0){return ier;} // error handling
	for (i = 1; i <= nbases; ++i){
		ier = cg_base_read(fid, i, tmpname, celldim + fileindex[fid], physdim + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
		if (strcmp(basename, tmpname) == 0){
			// Base found!
			*(baseid + fileindex[fid]) = i;
			*(ccbaseid + fileindex[fid]) = i;
			ier = local_gotobase_Mul(fid);
			// Find "iRICZone" zone.
			ier = cg_nzones(fid, *(baseid + fileindex[fid]), &nzones);
			if (ier != 0){return ier;} // error handling
			for (j = 1; j <= nzones; ++j){
				ier = cg_zone_read(fid, *(baseid + fileindex[fid]), j, tmpname, tmpzonesize);
				if (ier != 0){return ier;} // error handling
				if (strcmp(IRICZONE, tmpname) == 0){
					// Zone found!
					*(zoneid + fileindex[fid]) = j;
					// load solution time, iteration, and base iterative values.
					ier = local_load_sol_Mul(fid);
					if (ier != 0){return ier;} // error handling
					// initializes boundary condition name list.
					ier = local_init_bc_names_Mul(fid);
					if (ier != 0){return ier;} // error handling
					return ier;
				}
			}
		}
	}
	// not found.
	return 1;
}

int cg_iRIC_InitRead(int fid)
{
	return cg_iRIC_InitRead_Base(fid, NULL);
}

int cg_iRIC_Flush(char* filename, int* fid){
	int ier;
	// close the CGNS fie first.
	ier = cg_close(*fid);
	if (ier != 0){return ier;}

	int findex = fileindex[*fid];

	// clear the dictionary
	fileindex[*fid] = -1;

	// open the file again
	ier = cg_open(filename, CG_MODE_MODIFY, fid);
	if (ier != 0){return ier;}

	local_init_fileindex(*fid);
	fileindex[*fid] = findex;
	return 0;
}

int cg_iRIC_GotoBase(int fid, int* B){
	char tmpbasename[NAME_MAXLENGTH];
	int i, ier, nbases;

	// set file id first.
	lastfileid = fid;

	// initialize properties for fid.
	local_init_properties(fid);

	ier = cg_nbases(fid, &nbases);
	if (ier != 0){return ier;} // error handling
	for (i = 1; i <= nbases; ++i){
		ier = cg_base_read(fid, i, tmpbasename, celldim + fileindex[fid], physdim + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
		if (strcmp(IRICBASE, tmpbasename) == 0){
			// Base found!
			*(baseid + fileindex[fid]) = i;
			*(ccbaseid + fileindex[fid]) = i;
			*B = *(baseid + fileindex[fid]);
			ier = local_gotobase_Mul(fid);
			return 0;
		}
	}
	// not found.
	return 1;
}

int cg_iRIC_GotoCC(int fid){
	char tmpbasename[NAME_MAXLENGTH];
	int i, ier, nbases;

	// set file id first.
	lastfileid = fid;

	// initialize properties for fid.
	local_init_properties(fid);

	ier = cg_nbases(fid, &nbases);
	if (ier != 0){return ier;} // error handling
	for (i = 1; i <= nbases; ++i){
		ier = cg_base_read(fid, i, tmpbasename, celldim + fileindex[fid], physdim + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
		if (strcmp(IRICBASE, tmpbasename) == 0){
			// Base found!
			*(baseid + fileindex[fid]) = i;
			*(ccbaseid + fileindex[fid]) = i;
			// successful!
			// try to goto "CalculationCondition" node.
			ier = local_gotocc_Mul(fid);
			if (ier != 0){
				// this node doesn't exist yet.
				ier = local_gotoccbase_Mul(fid);
				ier = cg_user_data_write(CCNODE);
				if (ier != 0){return ier;} // error handling
				ier = local_gotocc_Mul(fid);
				if (ier != 0){return ier;} // error handling
			}
			return 0;
		}
	}
	// not found.
	return 1;
}

int cg_iRIC_GotoRawDataTop(int fid)
{
	char tmpbasename[NAME_MAXLENGTH];
	int i, ier, nbases;

	// set file id first.
	lastfileid = fid;

	// initialize properties for fid.
	local_init_properties(fid);

	ier = cg_nbases(fid, &nbases);
	if (ier != 0){return ier;} // error handling
	for (i = 1; i <= nbases; ++i){
		ier = cg_base_read(fid, i, tmpbasename, celldim + fileindex[fid], physdim + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
		if (strcmp(IRICBASE, tmpbasename) == 0){
			// Base found!
			*(baseid + fileindex[fid]) = i;
			*(ccbaseid + fileindex[fid]) = i;
			// this base node
			ier = local_gotoccbase_Mul(fid);
			// delete "RawData" node forst.
			cg_delete_node(RDNODE);
			// create "RawData" node again.
			ier = cg_user_data_write(RDNODE);
			if (ier != 0){return ier;} // error handling
			ier = local_gotord_Mul(fid);
			if (ier != 0){return ier;} // error handling
			return 0;
		}
	}
	// not found.
	return 1;
}

int cg_iRIC_Set_ZoneId_Mul(int fid, int zid)
{
	*(zoneid + fileindex[fid]) = zid;
	return 0;
}

int cg_iRIC_Read_Integer_Mul(int fid, char* name, int* intvalue){
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", Integer, 1, intvalue);
	return ier;
}

int cg_iRIC_Read_Real_Mul(int fid, char* name, double* realvalue){
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", RealDouble, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_RealSingle_Mul(int fid, char* name, float* realvalue){
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array_as("Value", RealSingle, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_String_Mul(int fid, char* name, char* strvalue){
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_string("Value", -1, strvalue);
	return ier;
}

int cg_iRIC_Read_FunctionalSize_Mul(int fid, char* name, cgsize_t* size)
{
	DataType_t datatype;
	int arrays;
	int dim;
	cgsize_t dimvec;
	int ier;
	int i;
	int paramfound;
	char arrayname[NAME_MAXLENGTH];

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Param", "Value" will exists.
	ier = cg_narrays(&arrays);
	if (ier != 0){return ier;}
	paramfound = 0;
	for (i = 1; i <= arrays; ++i){
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		*size = dimvec;
		return 0;
	}
	return 1;
}

int cg_iRIC_Read_Functional_Mul(int fid, char* name, double* x, double* y){
	int ret;

	ret = cg_iRIC_Read_FunctionalWithName_Mul(fid, name, "Param", x);
	if (ret != 0){return ret;}
	ret = cg_iRIC_Read_FunctionalWithName_Mul(fid, name, "Value", y);
	if (ret != 0){return ret;}
	return 0;
}

int cg_iRIC_Read_FunctionalWithName_Mul(int fid, char* name, char* paramname, double* data)
{
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array(paramname, RealDouble, -1, data);
	return ier;
}

int cg_iRIC_Read_Functional_RealSingle_Mul(int fid, char* name, float* x, float* y){
	int ier;

	ier = local_gotoccchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array_as("Param", RealSingle, -1, x);
	if (ier != 0){return ier;}
	ier = local_read_array_as("Value", RealSingle, -1, y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Integer_Mul(int fid, char* name, int intvalue){
	int ier;

	ier = local_gotoccchild_create_Mul(fid, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", Integer, 1, &intvalue);
	return ier;
}

int cg_iRIC_Write_Real_Mul(int fid, char* name, double realvalue){
	int ier;

	ier = local_gotoccchild_create_Mul(fid, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", RealDouble, 1, &realvalue);
	return ier;
}

int cg_iRIC_Write_String_Mul(int fid, char* name, char* strvalue){
	int ier;

	ier = local_gotoccchild_create_Mul(fid, name);
	if (ier != 0){return ier;}
	ier = local_write_string("Value", strvalue);
	return ier;
}

int cg_iRIC_Write_Functional_Mul(int fid, char* name, int length, double* realarray_x, double* realarray_y){
	int ier;

	ier = cg_iRIC_Write_FunctionalWithName_Mul(fid, name, "Param", length, realarray_x);
	if (ier != 0){return ier;}
	ier = cg_iRIC_Write_FunctionalWithName_Mul(fid, name, "Value", length, realarray_y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_FunctionalWithName_Mul(int fid, char* name, char* paramname, int length, double* data)
{
	int ier;

	ier = local_gotoccchild_create_Mul(fid, name);
	if (ier != 0){return ier;}
	ier = local_write_array(paramname, RealDouble, length, data);
	return ier;
}

int cg_iRIC_Read_Complex_Count_Mul(int fid, char* groupname, int* count)
{
	int ier;

	ier = local_gotocomplexgroup_Mul(fid, groupname);
	if (ier != 0){return ier;}
	ier = cg_nuser_data(count);
	return ier;
}

int cg_iRIC_Read_Complex_Integer_Mul(int fid, char* groupname, int num, char* name, int* intvalue)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", Integer, 1, intvalue);
	return ier;
}

int cg_iRIC_Read_Complex_Real_Mul(int fid, char* groupname, int num, char* name, double* realvalue)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", RealDouble, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_Complex_RealSingle_Mul(int fid, char* groupname, int num, char* name, float* realvalue)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array_as("Value", RealSingle, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_Complex_String_Mul(int fid, char* groupname, int num, char* name, char* strvalue)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_string("Value", -1, strvalue);
	return ier;
}

int cg_iRIC_Read_Complex_FunctionalSize_Mul(int fid, char* groupname, int num, char* name, cgsize_t* size)
{
	DataType_t datatype;
	int arrays;
	int dim;
	cgsize_t dimvec;
	int ier;
	int i;
	int paramfound;
	char arrayname[NAME_MAXLENGTH];

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Param", "Value" will exists.
	ier = cg_narrays(&arrays);
	if (ier != 0){return ier;}
	paramfound = 0;
	for (i = 1; i <= arrays; ++i){
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		*size = dimvec;
		return 0;
	}
	return 1;
}

int cg_iRIC_Read_Complex_Functional_Mul(int fid, char* groupname, int num, char* name, double* x, double* y)
{
	int ret;

	ret = cg_iRIC_Read_Complex_FunctionalWithName_Mul(fid, groupname, num, name, "Param", x);
	if (ret != 0){return ret;}
	ret = cg_iRIC_Read_Complex_FunctionalWithName_Mul(fid, groupname, num, name, "Value", y);
	if (ret != 0){return ret;}
	return 0;
}

int cg_iRIC_Read_Complex_FunctionalWithName_Mul(int fid, char* groupname, int num, char* name, char* paramname, double* data)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array(paramname, RealDouble, -1, data);
	return ier;
}

int cg_iRIC_Read_Complex_Functional_RealSingle_Mul(int fid, char* groupname, int num, char* name, float* x, float* y)
{
	int ier;

	ier = local_gotocomplexchild_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array_as("Param", RealSingle, -1, x);
	if (ier != 0){return ier;}
	ier = local_read_array_as("Value", RealSingle, -1, y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Read_Grid_Complex_Node_Mul(int fid, char* groupname, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Node_Mul(fid, groupname, values);
}

int cg_iRIC_Read_Grid_Complex_Cell_Mul(int fid, char *groupname, int* values)
{
	return cg_iRIC_Read_Grid_Complex_Node_Mul(fid, groupname, values);
}

int cg_iRIC_Clear_Complex_Mul(int fid)
{
	int ier;
	// delete GridComplexConditions node.
	local_gotobase_Mul(fid);
	// it succeeds only when ZoneBC node exists.
	ier = cg_delete_node(GCCNODE);

	ier = local_complex_add_gccnode_Mul(fid);
	if (ier != 0 ){return 0;}
	// always return 0.
	return 0;
}

int cg_iRIC_Write_Complex_Integer_Mul(int fid, char* groupname, int num, char* name, int intvalue)
{
	int ier;

	ier = local_gotocomplexchild_create_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", Integer, 1, &intvalue);
	return ier;
}

int cg_iRIC_Write_Complex_Real_Mul(int fid, char* groupname, int num, char* name, double realvalue)
{
	int ier;

	ier = local_gotocomplexchild_create_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", RealDouble, 1, &realvalue);
	return ier;
}

int cg_iRIC_Write_Complex_String_Mul(int fid, char* groupname, int num, char* name, char* strvalue)
{
	int ier;

	ier = local_gotocomplexchild_create_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	ier = local_write_string("Value", strvalue);
	return ier;
}

int cg_iRIC_Write_Complex_Functional_Mul(int fid, char* groupname, int num, char* name, int length, double* realarray_x, double* realarray_y)
{
	int ier;

	ier = cg_iRIC_Write_Complex_FunctionalWithName_Mul(fid, groupname, num, name, "Param", length, realarray_x);
	if (ier != 0){return ier;}
	ier = cg_iRIC_Write_Complex_FunctionalWithName_Mul(fid, groupname, num, name, "Value", length, realarray_y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Complex_FunctionalWithName_Mul(int fid, char* groupname, int num, char* name, char* paramname, int length, double* data)
{
	int ier;

	ier = local_gotocomplexchild_create_Mul(fid, groupname, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array(paramname, RealDouble, length, data);
	return ier;
}

int cg_iRIC_Write_Grid_Complex_Node_Mul(int fid, char* groupname, int* values)
{
	return cg_iRIC_Write_Grid_Integer_Node_Mul(fid, groupname, values);
}

int cg_iRIC_Write_Grid_Complex_Cell_Mul(int fid, char *groupname, int* values)
{
	return cg_iRIC_Write_Grid_Complex_Node_Mul(fid, groupname, values);
}

int cg_iRIC_GotoGridCoord2d_Mul(int fid, cgsize_t* isize, cgsize_t* jsize)
{
	char tmpzonename[NAME_MAXLENGTH];
	int ier;
	ZoneType_t zonetype;

	ier = cg_zone_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), tmpzonename, *(zonesize + fileindex[fid]));
	if (ier != 0){return ier;} // error handling
	// get zonetype
	cg_zone_type(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &zonetype);
	// this function supports only Structured grids.
	if (zonetype != Structured){return 1;}
	// (*(zonesize + fileindex[fid]))[0] == NVertexI, (*(zonesize + fileindex[fid]))[1] == NVertexJ.
	*isize = (*(zonesize + fileindex[fid]))[0];
	*jsize = (*(zonesize + fileindex[fid]))[1];
	// goto "GridCondition" node.
	ier = local_gotogc_Mul(fid);
	return ier;
}

int cg_iRIC_GetGridCoord2d_Mul(int fid, double *x, double *y)
{
	int ier, ncoords, i;
	cgsize_t rangemin[3], rangemax[3];

	ier = cg_ncoords(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &ncoords);
	if (ier != 0){return ier;} // error handling
	for (i = 0; i < 3; ++i){rangemin[i] = 1;}
	for (i = 0; i < 2; ++i){rangemax[i] = (*(zonesize + fileindex[fid]))[i];}
	rangemax[2] = 1;
	// read x coordinates.
	ier = cg_coord_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "CoordinateX", RealDouble, rangemin, rangemax, x);
	if (ier != 0){return ier;} // error handling
	// read y coordinates.
	ier = cg_coord_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "CoordinateY", RealDouble, rangemin, rangemax, y);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_GotoGridCoord3d_Mul(int fid, cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize)
{
	char tmpzonename[NAME_MAXLENGTH];
	int ier;
	ZoneType_t zonetype;

	ier = cg_zone_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), tmpzonename, *(zonesize + fileindex[fid]));
	if (ier != 0){return ier;} // error handling
	// get zonetype
	cg_zone_type(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &zonetype);
	// this function supports only Structured grids.
	if (zonetype != Structured){return 1;}
	// (*(zonesize + fileindex[fid]))[0] == NVertexI, (*(zonesize + fileindex[fid]))[1] == NVertexJ, (*(zonesize + fileindex[fid]))[2] = NVertexK.
	*isize = (*(zonesize + fileindex[fid]))[0];
	*jsize = (*(zonesize + fileindex[fid]))[1];
	*ksize = (*(zonesize + fileindex[fid]))[2];
	// goto "GridCondition" node.
	ier = local_gotogc_Mul(fid);
	return ier;
}

int cg_iRIC_GetGridCoord3d_Mul(int fid, double *x, double *y, double *z)
{
	int ier, ncoords, i;
	cgsize_t rangemin[3], rangemax[3];

	ier = cg_ncoords(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), &ncoords);
	if (ier != 0){return ier;} // error handling
	for (i = 0; i < 3; ++i){rangemin[i] = 1;}
	for (i = 0; i < 3; ++i){rangemax[i] = (*(zonesize + fileindex[fid]))[i];}
	// read x coordinates.
	ier = cg_coord_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "CoordinateX", RealDouble, rangemin, rangemax, x);
	if (ier != 0){return ier;} // error handling
	// read y coordinates.
	ier = cg_coord_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "CoordinateY", RealDouble, rangemin, rangemax, y);
	if (ier != 0){return ier;} // error handling
	// read z coordinates.
	ier = cg_coord_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "CoordinateZ", RealDouble, rangemin, rangemax, z);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Read_Grid_Real_Node_Mul(int fid, char* name, double* values)
{
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}

	// load data from "Value" array.
	ier = local_read_array_as("Value", RealDouble, -1, values);
	return ier;
}

int cg_iRIC_Read_Grid_Integer_Node_Mul(int fid, char* name, int* values)
{
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}

	// load data from "Value" array.
	ier = local_read_array_as("Value", Integer, -1, values);
	return ier;
}

int cg_iRIC_Read_Grid_Real_Cell_Mul(int fid, char *name, double* values)
{
	return cg_iRIC_Read_Grid_Real_Node_Mul(fid, name, values);
}

int cg_iRIC_Read_Grid_Integer_Cell_Mul(int fid, char *name, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Node_Mul(fid, name, values);
}

static void local_set_dimension_array_name(char* buffer, char* dimname)
{
	sprintf(buffer, "%s_%s", "Dimension", dimname);
}

int cg_iRIC_Read_Grid_FunctionalDimensionSize_Mul(int fid, char* name, char* dimname, cgsize_t* count)
{
	char dimarrayname[NAME_MAXLENGTH];
	int index;
	DataType_t datatype;
	int dim;
	cgsize_t dimvec[3];
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	local_set_dimension_array_name(&(dimarrayname[0]), dimname);

	ier = local_find_array(dimarrayname, &index, &datatype, &dim, &(dimvec[0]));

	if (ier != 0){return ier;}
	*count = dimvec[0];
	return 0;
}

int cg_iRIC_Read_Grid_FunctionalDimension_Integer_Mul(int fid, char* name, char* dimname, int* value)
{
	char dimarrayname[NAME_MAXLENGTH];
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	local_set_dimension_array_name(&(dimarrayname[0]), dimname);
	ier = local_read_array_as(dimarrayname, Integer, -1, value);
	return ier;
}

int cg_iRIC_Read_Grid_FunctionalDimension_Real_Mul(int fid, char* name, char* dimname, double* value)
{
	char dimarrayname[NAME_MAXLENGTH];
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	local_set_dimension_array_name(&(dimarrayname[0]), dimname);
	ier = local_read_array_as(dimarrayname, RealDouble, -1, value);
	return ier;
}

int cg_iRIC_Read_Grid_FunctionalTimeSize_Mul(int fid, char* name, cgsize_t* count)
{
	return cg_iRIC_Read_Grid_FunctionalDimensionSize_Mul(fid, name, "time", count);
}

int cg_iRIC_Read_Grid_FunctionalTime_Mul(int fid, char* name, double* time)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Real_Mul(fid, name, "time", time);
}

static void local_get_gridfunctionaldata_name(int num, char* name){
	if (num == 1){
		strcpy(name, "Value");
	} else {
		sprintf(name, "Value%d", num - 1);
	}
}

int cg_iRIC_Read_Grid_Functional_Integer_Node_Mul(int fid, char* name, int dimid, int* value)
{
	char arrayname[NAME_MAXLENGTH];
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	local_get_gridfunctionaldata_name(dimid, &(arrayname[0]));

	// load data from "Value", "Value1", "Value2" ... array.
	ier = local_read_array_as(arrayname, Integer, -1, value);
	return ier;
}

int cg_iRIC_Read_Grid_Functional_Real_Node_Mul(int fid, char* name, int dimid, double* value)
{
	char arrayname[NAME_MAXLENGTH];
	int ier;

	ier = local_gotogcchild_Mul(fid, name);
	if (ier != 0){return ier;}
	local_get_gridfunctionaldata_name(dimid, &(arrayname[0]));

	// load data from "Value", "Value1", "Value2" ... array.
	ier = local_read_array_as(arrayname, RealDouble, -1, value);
	return ier;
}

int cg_iRIC_Read_Grid_Functional_Integer_Cell_Mul(int fid, char* name, int dimid, int* value)
{
	return cg_iRIC_Read_Grid_Functional_Integer_Node_Mul(fid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Real_Cell_Mul(int fid, char* name, int dimid, double* value)
{
	return cg_iRIC_Read_Grid_Functional_Real_Node_Mul(fid, name, dimid, value);
}

int cg_iRIC_WriteGridCoord1d_Mul(int fid, cgsize_t isize, double* x)
{
	int ier, G, C;
	if (*(celldim + fileindex[fid]) != 1){
		// current cell dimension is not 1. create new 1D base!
		*(celldim + fileindex[fid]) = 1;
		*(physdim + fileindex[fid]) = 1;
		ier = cg_base_write(fid, iRICBASE1D, *(celldim + fileindex[fid]), *(physdim + fileindex[fid]), baseid + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
	}
	// add new zone.
	(*(zonesize + fileindex[fid]))[0] = isize;
	(*(zonesize + fileindex[fid]))[1] = isize - 1;
	ier = cg_zone_write(fid, *(baseid + fileindex[fid]), IRICZONE, *(zonesize + fileindex[fid]), Structured, zoneid + fileindex[fid]);
	if (ier != 0){return ier;} // error handling
	ier = cg_grid_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "GridCoordinates", &G);
	if (ier != 0){return ier;} // error handling
	// write X coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateX", x, &C);
	if (ier != 0){return ier;} // error handling
	ier = local_cg_init_sol_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// initializes gridconditions
	ier = local_gc_add_gcnode_Mul(fid);
	if (ier != 0){return ier;}
	// initialized gridcomplexconditions
	ier = local_complex_add_gccnode_Mul(fid);
	if (ier != 0){return ier;}
	// initializes base iterative data.
	ier = local_init_biter_Mul(fid);
	if (ier != 0){return ier;}
	// initializes zone iterativedata.
	ier = local_init_ziter_Mul(fid);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_WriteGridCoord2d_Mul(int fid, cgsize_t isize, cgsize_t jsize, double* x, double* y)
{
	int ier, G, C;
	if (*(celldim + fileindex[fid]) != 2){
		// current cell dimension is not 2. create new 2D base!
		*(celldim + fileindex[fid]) = 2;
		*(physdim + fileindex[fid]) = 2;
		ier = cg_base_write(fid, iRICBASE2D, *(celldim + fileindex[fid]), *(physdim + fileindex[fid]), baseid + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
	}
	// add new zone.
	(*(zonesize + fileindex[fid]))[0] = isize;
	(*(zonesize + fileindex[fid]))[1] = jsize;
	(*(zonesize + fileindex[fid]))[2] = isize - 1;
	(*(zonesize + fileindex[fid]))[3] = jsize - 1;
	ier = cg_zone_write(fid, *(baseid + fileindex[fid]), IRICZONE, *(zonesize + fileindex[fid]), Structured, zoneid + fileindex[fid]);
	if (ier != 0){return ier;} // error handling
	ier = cg_grid_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "GridCoordinates", &G);
	if (ier != 0){return ier;} // error handling
	// write X coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateX", x, &C);
	if (ier != 0){return ier;} // error handling
	// write Y coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateY", y, &C);
	if (ier != 0){return ier;} // error handling
	ier = local_cg_init_sol_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// initializes gridconditions
	ier = local_gc_add_gcnode_Mul(fid);
	if (ier != 0){return ier;}
	// initialized gridcomplexconditions
	ier = local_complex_add_gccnode_Mul(fid);
	if (ier != 0){return ier;}
	// initializes base iterative data.
	ier = local_init_biter_Mul(fid);
	if (ier != 0){return ier;}
	// initializes zone iterativedata.
	ier = local_init_ziter_Mul(fid);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_WriteGridCoord3d_Mul(int fid, cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z)
{
	int ier, G, C;
	if (*(celldim + fileindex[fid]) != 3){
		// current cell dimension is not 3. create new 3D base!
		*(celldim + fileindex[fid]) = 3;
		*(physdim + fileindex[fid]) = 3;
		ier = cg_base_write(fid, iRICBASE3D, *(celldim + fileindex[fid]), *(physdim + fileindex[fid]), baseid + fileindex[fid]);
		if (ier != 0){return ier;} // error handling
	}
	// add new zone.
	(*(zonesize + fileindex[fid]))[0] = isize;
	(*(zonesize + fileindex[fid]))[1] = jsize;
	(*(zonesize + fileindex[fid]))[2] = ksize;
	(*(zonesize + fileindex[fid]))[3] = isize - 1;
	(*(zonesize + fileindex[fid]))[4] = jsize - 1;
	(*(zonesize + fileindex[fid]))[5] = ksize - 1;
	ier = cg_zone_write(fid, *(baseid + fileindex[fid]), IRICZONE, *(zonesize + fileindex[fid]), Structured, zoneid + fileindex[fid]);
	if (ier != 0){return ier;} // error handling
	ier = cg_grid_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), "GridCoordinates", &G);
	if (ier != 0){return ier;} // error handling
	// write X coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateX", x, &C);
	if (ier != 0){return ier;} // error handling
	// write Y coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateY", y, &C);
	if (ier != 0){return ier;} // error handling
	// write Z coordinates.
	ier = cg_coord_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), RealDouble, "CoordinateZ", z, &C);
	if (ier != 0){return ier;} // error handling
	ier = local_cg_init_sol_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// initializes gridconditions
	ier = local_gc_add_gcnode_Mul(fid);
	if (ier != 0){return ier;}
	// initialized gridcomplexconditions
	ier = local_complex_add_gccnode_Mul(fid);
	if (ier != 0){return ier;}
	// initializes base iterative data.
	ier = local_init_biter_Mul(fid);
	if (ier != 0){return ier;}
	// initializes zone iterativedata.
	ier = local_init_ziter_Mul(fid);
	if (ier != 0){return ier;}

	return 0;
}

int cg_iRIC_Write_Grid_Real_Node_Mul(int fid, char* name, double* values)
{
	int ier;
	cgsize_t dimVec[3];

	if (*(celldim + fileindex[fid]) == 1){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0];
	} else if (*(celldim + fileindex[fid]) == 2){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0] * (*(zonesize + fileindex[fid]))[1];
	} else if (*(celldim + fileindex[fid]) == 3){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0] * (*(zonesize + fileindex[fid]))[1] * (*(zonesize + fileindex[fid]))[2];
	}
	dimVec[1] = 0;
	dimVec[2] = 0;

	ier = local_gotogcnewchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// Add Value node.
	ier = cg_array_write("Value", RealDouble, 1, dimVec, values);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Grid_Integer_Node_Mul(int fid, char* name, int* values)
{
	int ier;
	cgsize_t dimVec[3];

	if (*(celldim + fileindex[fid]) == 1){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0];
	} else if (*(celldim + fileindex[fid]) == 2){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0] * (*(zonesize + fileindex[fid]))[1];
	} else if (*(celldim + fileindex[fid]) == 3){
		dimVec[0] = (*(zonesize + fileindex[fid]))[0] * (*(zonesize + fileindex[fid]))[1] * (*(zonesize + fileindex[fid]))[2];
	}
	dimVec[1] = 0;
	dimVec[2] = 0;

	ier = local_gotogcnewchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// add Value node.
	ier = cg_array_write("Value", Integer, 1, dimVec, values);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Grid_Real_Cell_Mul(int fid, char* name, double* values)
{
	int ier;
	cgsize_t dimVec[3];

	if (*(celldim + fileindex[fid]) == 1){
		dimVec[0] = (*(zonesize + fileindex[fid]))[1];
	} else if (*(celldim + fileindex[fid]) == 2){
		dimVec[0] = (*(zonesize + fileindex[fid]))[2] * (*(zonesize + fileindex[fid]))[3];
	} else if (*(celldim + fileindex[fid]) == 3){
		dimVec[0] = (*(zonesize + fileindex[fid]))[3] * (*(zonesize + fileindex[fid]))[4] * (*(zonesize + fileindex[fid]))[5];
	}
	dimVec[1] = 0;
	dimVec[2] = 0;

	ier = local_gotogcnewchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// add Value node.
	ier = cg_array_write("Value", RealDouble, 1, dimVec, values);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Grid_Integer_Cell_Mul(int fid, char* name, int* values)
{
	int ier;
	cgsize_t dimVec[3];

	if (*(celldim + fileindex[fid]) == 1){
		dimVec[0] = (*(zonesize + fileindex[fid]))[1];
	} else if (*(celldim + fileindex[fid]) == 2){
		dimVec[0] = (*(zonesize + fileindex[fid]))[2] * (*(zonesize + fileindex[fid]))[3];
	} else if (*(celldim + fileindex[fid]) == 3){
		dimVec[0] = (*(zonesize + fileindex[fid]))[3] * (*(zonesize + fileindex[fid]))[4] * (*(zonesize + fileindex[fid]))[5];
	}
	dimVec[1] = 0;
	dimVec[2] = 0;

	ier = local_gotogcnewchild_Mul(fid, name);
	if (ier != 0){return ier;}
	// add Value node.
	ier = cg_array_write("Value", Integer, 1, dimVec, values);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Read_Sol_Count_Mul(int fid, int* count)
{
	*count = *(solid + fileindex[fid]);
	return 0;
}

int cg_iRIC_Read_Sol_Time_Mul(int fid, int step, double* time){
	// TODO make sure that step is not larger then the number of steps.
	if (step > *(solid + fileindex[fid])){
		// step out of range.
		return 1;
	}

	*time = *(*(timearray + fileindex[fid]) + step - 1);
	return 0;
}

int cg_iRIC_Read_Sol_Iteration_Mul(int fid, int step, int* index){
	// TODO make sure that step is not larger then the number of steps.
	if (step > *(solid + fileindex[fid])){
		// step out of range.
		return 1;
	}

	*index = *(*(indexarray + fileindex[fid]) + step - 1);
	return 0;
}

int cg_iRIC_Read_Sol_BaseIterative_Integer_Mul(int fid, int step, char* name, int* value)
{
	int i;
	int dataindex;

	if (step > *(solid + fileindex[fid])){
		// step out of range.
		return 1;
	}

	// find data that corresponds to the name.
	dataindex = -1;
	for (i = 0; i < *(baseiterintdatacount + fileindex[fid]); ++i){
		if (strcmp(name, *(*(baseiterintnamearray + fileindex[fid]) + i)) == 0){
			dataindex = i;
			break;
		}
	}
	if (dataindex == -1){
		// not found.
		return 1;
	}

	*value = *(*(*(baseiterintdataarray + fileindex[fid]) + dataindex) + step - 1);
	return 0;
}

int cg_iRIC_Read_Sol_BaseIterative_Real_Mul(int fid, int step, char* name, double* value)
{
	int i;
	int dataindex;

	if (step > *(solid + fileindex[fid])){
		// step out of range.
		return 1;
	}

	// find data that corresponds to the name.
	dataindex = -1;
	for (i = 0; i < *(baseiterrealdatacount + fileindex[fid]); ++i){
		if (strcmp(name, *(*(baseiterrealnamearray + fileindex[fid]) + i)) == 0){
			dataindex = i;
			break;
		}
	}
	if (dataindex == -1){
		// not found.
		return 1;
	}

	*value = *(*(*(baseiterrealdataarray + fileindex[fid]) + dataindex) + step - 1);
	return 0;
}

int cg_iRIC_Read_Sol_GridCoord2d_Mul(int fid, int step, double* x, double* y)
{
	int ier;

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "GridCoordinates_t", step + 1, "end");
	if (ier != 0){return ier;} // error handling

	ier = local_read_array("CoordinateX", RealDouble, -1, x);
	if (ier != 0){return ier;} // error handling

	ier = local_read_array("CoordinateY", RealDouble, -1, y);
	if (ier != 0){return ier;} // error handling

	return 0;
}

int cg_iRIC_Read_Sol_GridCoord3d_Mul(int fid, int step, double* x, double* y, double* z)
{
	int ier;

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "GridCoordinates_t", step + 1, "end");
	if (ier != 0){return ier;} // error handling

	ier = local_read_array("CoordinateX", RealDouble, -1, x);
	if (ier != 0){return ier;} // error handling

	ier = local_read_array("CoordinateY", RealDouble, -1, y);
	if (ier != 0){return ier;} // error handling

	ier = local_read_array("CoordinateZ", RealDouble, -1, z);
	if (ier != 0){return ier;} // error handling

	return 0;
}

int cg_iRIC_Read_Sol_Integer_Mul(int fid, int step, char *name, int* data)
{
	int ier;

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "FlowSolution_t", step, "end");
	if (ier != 0){return ier;} // error handling

	ier = local_read_array(name, Integer, -1, data);
	if (ier != 0){return ier;} // error handling

	return 0;
}

int cg_iRIC_Read_Sol_Real_Mul(int fid, int step, char *name, double* data)
{
	int ier;

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "FlowSolution_t", step, "end");
	if (ier != 0){return ier;} // error handling

	ier = local_read_array(name, RealDouble, -1, data);
	if (ier != 0){return ier;} // error handling

	return 0;
}

int cg_iRIC_Write_Sol_Time_Mul(int fid, double time){
	int ier;
	cgsize_t dimVec[1];

	if (*(timememsize + fileindex[fid]) == 0){
		// first time to call this function.
		*(timememsize + fileindex[fid]) = 10;
		*(timearray + fileindex[fid]) =
			(double*)malloc(sizeof(double) * *(timememsize + fileindex[fid]));
	}
	if (*(solid + fileindex[fid]) == *(timememsize + fileindex[fid])){
		// the timearray is full of data.
		*(timememsize + fileindex[fid]) *= 2;
		*(timearray + fileindex[fid]) = realloc(*(timearray + fileindex[fid]), sizeof(double) *
			*(timememsize + fileindex[fid]));
		if (*(timearray + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	*(*(timearray + fileindex[fid]) + *(solid + fileindex[fid])) = time;
	++ (*(solid + fileindex[fid]));

	// write base iterative data, with new solid to the default base (with calculation condition).
	ier = cg_biter_write(fid, *(ccbaseid + fileindex[fid]), BINAME, *(solid + fileindex[fid]));
	if (ier != 0){return ier;}
	ier = local_gotoccbaseiterative_Mul(fid);
	if (ier != 0){return ier;}
	// write time.
	dimVec[0] = *(solid + fileindex[fid]);
	ier = cg_array_write("TimeValues", RealDouble, 1, dimVec, *(timearray + fileindex[fid]));

	// write base iterative data, with new solid to the current base.
	ier = cg_biter_write(fid, *(baseid + fileindex[fid]), BINAME, *(solid + fileindex[fid]));
	if (ier != 0){return ier;}
	ier = local_gotobaseiterative_Mul(fid);
	if (ier != 0){return ier;}
	// write time.
	dimVec[0] = *(solid + fileindex[fid]);
	ier = cg_array_write("TimeValues", RealDouble, 1, dimVec, *(timearray + fileindex[fid]));

	if (ier != 0){return ier;}
	// add solution node.
	ier = local_cg_iRIC_Add_Solution_Node_Mul(fid);
	return ier;
}

int cg_iRIC_Write_Sol_Iteration_Mul(int fid, int index){
	int ier;
	cgsize_t dimVec[1];
	if (*(iterationmemsize + fileindex[fid]) == 0){
		// first time to call this function.
		*(iterationmemsize + fileindex[fid]) = 10;
		*(indexarray + fileindex[fid]) = (int*)malloc(sizeof(int) *
			*(iterationmemsize + fileindex[fid]));
	}
	if (*(solid + fileindex[fid]) == *(iterationmemsize + fileindex[fid])){
		// the timearray is full of data.
		*(iterationmemsize + fileindex[fid]) *= 2;
		*(indexarray + fileindex[fid]) = realloc(
			*(indexarray + fileindex[fid]), sizeof(int) *
			*(iterationmemsize + fileindex[fid]));
		if (*(indexarray + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	*(*(indexarray + fileindex[fid]) + *(solid + fileindex[fid])) = index;
	++ (*(solid + fileindex[fid]));

	// write base iterative data, with new solid to the default base (with calculation condition).
	ier = cg_biter_write(fid, *(ccbaseid + fileindex[fid]), BINAME,
		*(solid + fileindex[fid]));
	if (ier != 0){return ier;}
	ier = local_gotoccbaseiterative_Mul(fid);
	if (ier != 0){return ier;}
	// write iteration data.
	dimVec[0] = *(solid + fileindex[fid]);
	ier = cg_array_write("IterationValues", Integer, 1, dimVec,
		*(indexarray + fileindex[fid]));
	if (ier != 0){return ier;}

	// write base iterative data, with new solid to the current base.
	ier = cg_biter_write(fid, *(baseid + fileindex[fid]), BINAME,
		*(solid + fileindex[fid]));
	if (ier != 0){return ier;}
	ier = local_gotobaseiterative_Mul(fid);
	if (ier != 0){return ier;}
	// write iteration data.
	dimVec[0] = *(solid + fileindex[fid]);
	ier = cg_array_write("IterationValues", Integer, 1, dimVec,
		*(indexarray + fileindex[fid]));
	if (ier != 0){return ier;}

	// add solution node.
	ier = local_cg_iRIC_Add_Solution_Node_Mul(fid);
	return ier;
}

int cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(int fid, char *name, int value){
	int i;
	int ier;
	cgsize_t dimVec[1];
	int dataindex;

	if (*(baseiterintdatacount + fileindex[fid]) == 0){
		// first time to call this function.
		*(baseiterintdatacount + fileindex[fid]) = 1;
		*(baseiterintnamearray + fileindex[fid]) = (char**)malloc(sizeof(char*));
		*(baseiterintdataarray + fileindex[fid]) = (int**)malloc(sizeof(int*));
		// copy the name.
		**(baseiterintnamearray + fileindex[fid]) = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(**(baseiterintnamearray + fileindex[fid]), name);
		// copy data.
		**(baseiterintdataarray + fileindex[fid]) = (int*)malloc(sizeof(int) * *(baseiterintmemsize + fileindex[fid]));
	}
	if (*(solid + fileindex[fid]) == *(baseiterintmemsize + fileindex[fid])){
		// dataaray is full of data.
		*(baseiterintmemsize + fileindex[fid]) *= 2;
		for (i = 0; i < *(baseiterintdatacount + fileindex[fid]); ++i){
			*(*(baseiterintdataarray + fileindex[fid]) + i) = realloc(*(*(baseiterintdataarray + fileindex[fid]) + i), sizeof(int) * *(baseiterintmemsize + fileindex[fid]));
			if (*(*(baseiterintdataarray + fileindex[fid]) + i) == 0){return 1;}
		}
	}
	// find data corresponds to the name already exists here.
	dataindex = -1;
	for (i = 0; i < *(baseiterintdatacount + fileindex[fid]); ++i){
		if (strcmp(name, *(*(baseiterintnamearray + fileindex[fid]) + i)) == 0){
			dataindex = i;
			break;
		}
	}
	if (dataindex == -1){
		// not found. new data.
		dataindex = *(baseiterintdatacount + fileindex[fid]);
		++ (*(baseiterintdatacount + fileindex[fid]));
		*(baseiterintnamearray + fileindex[fid]) = realloc(*(baseiterintnamearray + fileindex[fid]), sizeof(char*) * *(baseiterintdatacount + fileindex[fid]));
		if (*(baseiterintnamearray + fileindex[fid]) == NULL){return 1;}
		*(baseiterintdataarray + fileindex[fid]) = realloc(*(baseiterintdataarray + fileindex[fid]), sizeof(int*) * *(baseiterintdatacount + fileindex[fid]));
		if (*(baseiterintdataarray + fileindex[fid]) == NULL){return 1;}
		*(*(baseiterintdataarray + fileindex[fid]) + dataindex) = (int*)malloc(sizeof(int) * *(baseiterintmemsize + fileindex[fid]));
		if (*(*(baseiterintdataarray + fileindex[fid]) + dataindex) == NULL){return 1;}
		// copy the name.
		*(*(baseiterintnamearray + fileindex[fid]) + dataindex) = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		if (*(*(baseiterintnamearray + fileindex[fid]) + dataindex) == NULL){return 1;}
		strcpy(*(*(baseiterintnamearray + fileindex[fid]) + dataindex), name);
	}
	// write the value.
	*(*(*(baseiterintdataarray + fileindex[fid]) + dataindex) + *(solid + fileindex[fid]) - 1) = value;
	dimVec[0] = *(solid + fileindex[fid]);
	local_gotobaseiterative_Mul(fid);
	ier = cg_array_write(*(*(baseiterintnamearray + fileindex[fid]) + dataindex), Integer, 1, dimVec, *(*(baseiterintdataarray + fileindex[fid]) + dataindex));
	return ier;
}

int cg_iRIC_Write_Sol_BaseIterative_Real_Mul(int fid, char *name, double value){
	int i;
	int ier;
	cgsize_t dimVec[1];
	int dataindex;

	if (*(baseiterrealdatacount + fileindex[fid]) == 0){
		// first time to call this function.
		*(baseiterrealdatacount + fileindex[fid]) = 1;
		*(baseiterrealnamearray + fileindex[fid]) = (char**)malloc(sizeof(char*));
		*(baseiterrealdataarray + fileindex[fid]) = (double**)malloc(sizeof(double*));
		// copy the name.
		**(baseiterrealnamearray + fileindex[fid]) = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(**(baseiterrealnamearray + fileindex[fid]), name);
		// copy data.
		**(baseiterrealdataarray + fileindex[fid]) = (double*)malloc(sizeof(double) * *(baseiterrealmemsize + fileindex[fid]));
	}
	if (*(solid + fileindex[fid]) == *(baseiterrealmemsize + fileindex[fid])){
		// dataaray is full of data.
		*(baseiterrealmemsize + fileindex[fid]) *= 2;
		for (i = 0; i < *(baseiterrealdatacount + fileindex[fid]); ++i){
			*(*(baseiterrealdataarray + fileindex[fid]) + i) = realloc(*(*(baseiterrealdataarray + fileindex[fid]) + i), sizeof(double) * *(baseiterrealmemsize + fileindex[fid]));
			if (*(*(baseiterrealdataarray + fileindex[fid]) + i) == 0){return 1;}
		}
	}
	// find data corresponds to the name already exists here.
	dataindex = -1;
	for (i = 0; i < *(baseiterrealdatacount + fileindex[fid]); ++i){
		if (strcmp(name, *(*(baseiterrealnamearray + fileindex[fid]) + i)) == 0){
			dataindex = i;
			break;
		}
	}
	if (dataindex == -1){
		// not found. new data.
		dataindex = *(baseiterrealdatacount + fileindex[fid]);
		++ (*(baseiterrealdatacount + fileindex[fid]));
		*(baseiterrealnamearray + fileindex[fid]) = realloc(*(baseiterrealnamearray + fileindex[fid]), sizeof(char*) * *(baseiterrealdatacount + fileindex[fid]));
		if (*(baseiterrealnamearray + fileindex[fid]) == NULL){return 1;}
		*(baseiterrealdataarray + fileindex[fid]) = realloc(*(baseiterrealdataarray + fileindex[fid]), sizeof(int*) * *(baseiterrealdatacount + fileindex[fid]));
		if (*(baseiterrealdataarray + fileindex[fid]) == NULL){return 1;}
		*(*(baseiterrealdataarray + fileindex[fid]) + dataindex) = (double*)malloc(sizeof(double) * *(baseiterrealmemsize + fileindex[fid]));
		if (*(*(baseiterrealdataarray + fileindex[fid]) + dataindex) == NULL){return 1;}
		// copy the name.
		*(*(baseiterrealnamearray + fileindex[fid]) + dataindex) = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		if (*(*(baseiterrealnamearray + fileindex[fid]) + dataindex) == NULL){return 1;}
		strcpy(*(*(baseiterrealnamearray + fileindex[fid]) + dataindex), name);
	}
	// write the value.
	*(*(*(baseiterrealdataarray + fileindex[fid]) + dataindex) + *(solid + fileindex[fid]) - 1) = value;
	dimVec[0] = *(solid + fileindex[fid]);
	local_gotobaseiterative_Mul(fid);
	ier = cg_array_write(*(*(baseiterrealnamearray + fileindex[fid]) + dataindex), RealDouble, 1, dimVec, *(*(baseiterrealdataarray + fileindex[fid]) + dataindex));
	return ier;
}

int cg_iRIC_Write_Sol_GridCoord2d_Mul(int fid, double *x, double *y)
{
	int ier;
	cgsize_t dimVec[2];

	ier = local_cg_iRIC_Add_Sol_GridCoordNode_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "GridCoordinates_t", *(solid + fileindex[fid]) + 1, "end");

	dimVec[0] = (*(zonesize + fileindex[fid]))[0];
	dimVec[1] = (*(zonesize + fileindex[fid]))[1];

	// write x coordinates.
	ier = cg_array_write("CoordinateX", RealDouble, 2, dimVec, x);
	if (ier != 0){return ier;} // error handling
	// write y coordinates.
	ier = cg_array_write("CoordinateY", RealDouble, 2, dimVec, y);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Write_Sol_GridCoord3d_Mul(int fid, double *x, double *y, double *z)
{
	int ier;
	cgsize_t dimVec[3];

	ier = local_cg_iRIC_Add_Sol_GridCoordNode_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// goto the GridCoordinates_t node created.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), "GridCoordinates_t", *(solid + fileindex[fid]) + 1, "end");

	dimVec[0] = (*(zonesize + fileindex[fid]))[0];
	dimVec[1] = (*(zonesize + fileindex[fid]))[1];
	dimVec[2] = (*(zonesize + fileindex[fid]))[2];

	// write x coordinates.
	ier = cg_array_write("CoordinateX", RealDouble, 3, dimVec, x);
	if (ier != 0){return ier;} // error handling
	// write y coordinates.
	ier = cg_array_write("CoordinateY", RealDouble, 3, dimVec, y);
	if (ier != 0){return ier;} // error handling
	// write z coordinates.
	ier = cg_array_write("CoordinateZ", RealDouble, 3, dimVec, z);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Write_Sol_Integer_Mul(int fid, char *name, int* data){
	int F;
	return cg_field_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), *(solid + fileindex[fid]), Integer, name, data, &F);
}

int cg_iRIC_Write_Sol_Real_Mul(int fid, char *name, double* data){
	int F;
	return cg_field_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), *(solid + fileindex[fid]), RealDouble, name, data, &F);
}

int cg_iRIC_Write_ErrorCode_Mul(int fid, int errorcode){
	int ret;
	cgsize_t dimVec[] = {1, 0, 0};
	ret = local_gotobase_Mul(fid);
	if (ret != 0){return ret;}
	// delete ErrorCode node first.
	cg_delete_node(ECNODE);
	// add ErrorCode node.
	ret = cg_user_data_write(ECNODE);
	if (ret != 0){return ret;}
	ret = cg_goto(fid, *(baseid + fileindex[fid]), ECNODE, 0, "end");
	if (ret != 0){return ret;}
	// add Value array
	ret = cg_array_write("Value", Integer, 1, dimVec, &errorcode);
	if (ret != 0){return ret;}
	return 0;
}

void cg_iRIC_Init_BC_Names_Mul(int fid){
	local_init_bc_names_Mul(fid);
}

void cg_iRIC_Read_BC_Count_Mul(int fid, char* type, int* num){
	int count;
	int BC;

	count = 0;
	while (1){
		// try to get BC
		local_get_bc_index_Mul(fid, type, count + 1, &BC);
		if (BC == 0){
			// not found.
			*num = count;
			return;
		}
		++ count;
	}
}

int cg_iRIC_Read_BC_IndicesSize_Mul(int fid, char* type, int num, cgsize_t* size){
	int ier;
	int BC;
	char bcname[32];
	BCType_t bctype;
	PointSetType_t pstype;
	cgsize_t npnts;
	int nindex[3];
	cgsize_t nlistflag;
	DataType_t ndtype;
	int ndataset;

	local_get_bc_index_Mul(fid, type, num, &BC);
	if (BC == 0){
		// not found.
		return 1;
	}
	ier = cg_boco_info(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), BC, bcname, &bctype, &pstype,
		&npnts, &(nindex[0]), &nlistflag, &ndtype, &ndataset);
	if (ier != 0){return ier;}
	*size = npnts;
	return 0;
}

int cg_iRIC_Read_BC_Indices_Mul(int fid, char* type, int num, cgsize_t* indices){
	int ier;
	int BC;
	int normallist;

	local_get_bc_index_Mul(fid, type, num, &BC);
	if (BC == 0){
		// not found.
		return 1;
	}
	ier = cg_boco_read(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), BC, indices, &normallist);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Read_BC_Integer_Mul(int fid, char* type, int num, char* name, int* intvalue)
{
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", Integer, 1, intvalue);
	return ier;
}

int cg_iRIC_Read_BC_Real_Mul(int fid, char* type, int num, char* name, double* realvalue){
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array("Value", RealDouble, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_BC_RealSingle_Mul(int fid, char* type, int num, char* name, float* realvalue){
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_array_as("Value", RealSingle, 1, realvalue);
	return ier;
}

int cg_iRIC_Read_BC_String_Mul(int fid, char* type, int num, char* name, char* strvalue){
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Value" will exists.
	ier = local_read_string("Value", -1, strvalue);
	return ier;
}

int cg_iRIC_Read_BC_FunctionalSize_Mul(int fid, char* type, int num, char* name, cgsize_t* size){
	DataType_t datatype;
	int arrays;
	int dim;
	cgsize_t dimvec;
	int ier;
	int i;
	int paramfound;
	char arrayname[NAME_MAXLENGTH];

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name "Param", "Value" will exists.
	ier = cg_narrays(&arrays);
	if (ier != 0){return ier;}
	paramfound = 0;
	for (i = 1; i <= arrays; ++i){
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		*size = dimvec;
		return 0;
	}
	return 1;
}

int cg_iRIC_Read_BC_Functional_Mul(int fid, char* type, int num, char* name, double* x, double* y){
	int ret;

	ret = cg_iRIC_Read_BC_FunctionalWithName_Mul(fid, type, num, name, "Param", x);
	if (ret != 0){return ret;}
	ret = cg_iRIC_Read_BC_FunctionalWithName_Mul(fid, type, num, name, "Value", y);
	if (ret != 0){return ret;}
	return 0;
}

int cg_iRIC_Read_BC_FunctionalWithName_Mul(int fid, char* type, int num, char* name, char* paramname, double* data){
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array(paramname, RealDouble, -1, data);
	return ier;
}

int cg_iRIC_Read_BC_Functional_RealSingle_Mul(int fid, char* type, int num, char* name, float* x, float* y){
	int ier;

	ier = local_gotobcchild_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	// under this UserDefinedData_t, array with name paramname will exists.
	ier = local_read_array_as("Param", RealSingle, -1, x);
	if (ier != 0){return ier;}
	ier = local_read_array_as("Value", RealSingle, -1, y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Clear_BC_Mul(int fid){
	int ier;
	// delete ZoneBC_t node.
	local_gotozone_Mul(fid);
	// it succeeds only when ZoneBC node exists.
	ier = cg_delete_node("ZoneBC");
	// clear bcnames
	free(*(bcnames + fileindex[fid]));
	*(bccount + fileindex[fid]) = 0;
	*(bcbufferlen + fileindex[fid]) = 10;
	*(bcnames + fileindex[fid]) = (char*)malloc(sizeof(char) * *(bcbufferlen + fileindex[fid]) * 32);

	// always return 0.
	return 0;
}

int cg_iRIC_Write_BC_Indices_Mul(int fid, char* type, int num, cgsize_t size, cgsize_t* indices){
	char tmpname[32];
	int BC;
	BCType_t bctype;
	PointSetType_t pointsettype;

	local_get_bc_name(type, num, tmpname);
	bctype = BCTypeUserDefined;
	pointsettype = PointList;

	// write BC_t node into CGNS file.
	cg_boco_write(fid, *(baseid + fileindex[fid]), *(zoneid + fileindex[fid]), tmpname, bctype, pointsettype, size, indices, &BC);

	if (BC - 1 == *(bcbufferlen + fileindex[fid])){
		// bcbufferlen is full of data.
		*(bcbufferlen + fileindex[fid]) *= 2;
		*(bcnames + fileindex[fid]) = realloc(*(bcnames + fileindex[fid]), sizeof(char) * *(bcbufferlen + fileindex[fid]) * 32);
		if (*(bcnames + fileindex[fid]) == NULL){
			// lack of memory. returns error.
			return 1;
		}
	}
	memcpy(*(bcnames + fileindex[fid]) + 32 * (BC - 1), tmpname, 32);
	++ (*(bccount + fileindex[fid]));
	return 0;
}

int cg_iRIC_Write_BC_Integer_Mul(int fid, char* type, int num, char* name, int intvalue){
	int ier;

	ier = local_gotobcchild_create_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", Integer, 1, &intvalue);
	return ier;
}

int cg_iRIC_Write_BC_Real_Mul(int fid, char* type, int num, char* name, double realvalue)
{
	int ier;

	ier = local_gotobcchild_create_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array("Value", RealDouble, 1, &realvalue);
	return ier;
}

int cg_iRIC_Write_BC_String_Mul(int fid, char* type, int num, char* name, char* strvalue)
{
	int ier;

	ier = local_gotobcchild_create_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	ier = local_write_string("Value", strvalue);
	return ier;
}

int cg_iRIC_Write_BC_Functional_Mul(int fid, char* type, int num, char* name, int length, double* realarray_x, double* realarray_y){
	int ier;
	ier = cg_iRIC_Write_BC_FunctionalWithName_Mul(fid, type, num, name, "Param", length, realarray_x);
	if (ier != 0){return ier;}
	ier = cg_iRIC_Write_BC_FunctionalWithName_Mul(fid, type, num, name, "Value", length, realarray_y);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_BC_FunctionalWithName_Mul(int fid, char* type, int num, char* name, char* paramname, int length, double* data){
	int ier;

	ier = local_gotobcchild_create_Mul(fid, type, num, name);
	if (ier != 0){return ier;}
	ier = local_write_array(paramname, RealDouble, length, data);
	return ier;
}

int cg_iRIC_Read_Geo_Count_Mul(int fid, char* name, int* count)
{
	int ier;
	// goto RawData node.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), RDNODE, 0, name, 0, "end");
	if (ier != 0){return ier;} // error handling
	ier = cg_nuser_data(count);
	return ier;
}

int cg_iRIC_Read_Geo_Filename_Mul(int fid, char* name, int geoid, char* filename, int* type)
{
	int ier;

	// goto RawData item node.
	ier = cg_goto(fid, *(baseid + fileindex[fid]), RDNODE, 0, name, 0, "UserDefinedData_t", geoid, "end");
	if (ier != 0){return ier;} // error handling

	ier = local_read_string("filename", -1, filename);
	if (ier != 0){return ier;}
	ier = local_read_array("type", Integer, 1, type);
	if (ier != 0){return ier;}
	return 0;
}

int cg_iRIC_Write_Sol_Particle_Pos2d_Mul(int fid, cgsize_t count, double* x, double* y)
{
	int ier;
	cgsize_t dimVec;
	char particlenodename[32];

	ier = local_cg_iRIC_Add_Sol_ParticleNode_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// goto the UserDefined_t node created.
	local_get_particlesol_name(*(solid + fileindex[fid]), particlenodename);
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), particlenodename, 0, "end");

	dimVec = count;
	// write x coordinates.
	ier = cg_array_write("CoordinateX", RealDouble, 1, &dimVec, x);
	if (ier != 0){return ier;} // error handling
	// write y coordinates.
	ier = cg_array_write("CoordinateY", RealDouble, 1, &dimVec, y);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Write_Sol_Particle_Pos3d_Mul(int fid, cgsize_t count, double* x, double* y, double* z)
{
	int ier;
	cgsize_t dimVec;
	char particlenodename[32];

	ier = local_cg_iRIC_Add_Sol_ParticleNode_Mul(fid);
	if (ier != 0){return ier;} // error handling

	// goto the UserDefined_t node created.
	local_get_particlesol_name(*(solid + fileindex[fid]), particlenodename);
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), particlenodename, 0, "end");

	dimVec = count;
	// write x coordinates.
	ier = cg_array_write("CoordinateX", RealDouble, 1, &dimVec, x);
	if (ier != 0){return ier;} // error handling
	// write y coordinates.
	ier = cg_array_write("CoordinateY", RealDouble, 1, &dimVec, y);
	if (ier != 0){return ier;} // error handling
	// write z coordinates.
	ier = cg_array_write("CoordinateZ", RealDouble, 1, &dimVec, z);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Write_Sol_Particle_Real_Mul(int fid, char* name, double* value)
{
	int ier;
	cgsize_t dimVec;
	char particlenodename[32];

	char arrayname[32];
	DataType_t datatype;
	int dim;

	// goto the UserDefined_t node created.
	local_get_particlesol_name(*(solid + fileindex[fid]), particlenodename);
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), particlenodename, 0, "end");

	// get array info for the first array. that will be CoordinateX.
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);

	// write values coordinates.
	ier = cg_array_write(name, RealDouble, 1, &dimVec, value);
	if (ier != 0){return ier;} // error handling
	return 0;
}

int cg_iRIC_Write_Sol_Particle_Integer_Mul(int fid, char* name, int* value)
{
	int ier;
	cgsize_t dimVec;
	char particlenodename[32];

	char arrayname[32];
	DataType_t datatype;
	int dim;

	// goto the UserDefined_t node created.
	local_get_particlesol_name(*(solid + fileindex[fid]), particlenodename);
	ier = cg_goto(fid, *(baseid + fileindex[fid]), "Zone_t", *(zoneid + fileindex[fid]), particlenodename, 0, "end");

	// get array info for the first array. that will be CoordinateX.
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);

	// write values coordinates.
	ier = cg_array_write(name, Integer, 1, &dimVec, value);
	if (ier != 0){return ier;} // error handling
	return 0;
}

/*********************************************************/
/* The following function implementations are generated  */
/* using make_iriclib_singlefuncs.php                    */
/*********************************************************/

int cg_iRIC_Set_ZoneId(int zoneid)
{
	return cg_iRIC_Set_ZoneId_Mul(lastfileid, zoneid);
}

int cg_iRIC_Read_Integer(char* name, int* intvalue)
{
	return cg_iRIC_Read_Integer_Mul(lastfileid, name, intvalue);
}

int cg_iRIC_Read_Real(char* name, double* realvalue)
{
	return cg_iRIC_Read_Real_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Read_RealSingle(char* name, float* realvalue)
{
	return cg_iRIC_Read_RealSingle_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Read_String(char* name, char* strvalue)
{
	return cg_iRIC_Read_String_Mul(lastfileid, name, strvalue);
}

int cg_iRIC_Read_FunctionalSize(char* name, cgsize_t* size)
{
	return cg_iRIC_Read_FunctionalSize_Mul(lastfileid, name, size);
}

int cg_iRIC_Read_Functional(char* name, double* x, double* y)
{
	return cg_iRIC_Read_Functional_Mul(lastfileid, name, x, y);
}

int cg_iRIC_Read_FunctionalWithName(char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_FunctionalWithName_Mul(lastfileid, name, paramname, data);
}

int cg_iRIC_Read_Functional_RealSingle(char* name, float* x, float* y)
{
	return cg_iRIC_Read_Functional_RealSingle_Mul(lastfileid, name, x, y);
}

int cg_iRIC_Write_Integer(char* name, int intvalue)
{
	return cg_iRIC_Write_Integer_Mul(lastfileid, name, intvalue);
}

int cg_iRIC_Write_Real(char* name, double realvalue)
{
	return cg_iRIC_Write_Real_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Write_String(char* name, char* strvalue)
{
	return cg_iRIC_Write_String_Mul(lastfileid, name, strvalue);
}

int cg_iRIC_Write_Functional(char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_Functional_Mul(lastfileid, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_FunctionalWithName(char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_FunctionalWithName_Mul(lastfileid, name, paramname, length, data);
}

int cg_iRIC_Read_Complex_Count(char* groupname, int* num)
{
	return cg_iRIC_Read_Complex_Count_Mul(lastfileid, groupname, num);
}

int cg_iRIC_Read_Complex_Integer(char* groupname, int num, char* name, int* intvalue)
{
	return cg_iRIC_Read_Complex_Integer_Mul(lastfileid, groupname, num, name, intvalue);
}

int cg_iRIC_Read_Complex_Real(char* groupname, int num, char* name, double* realvalue)
{
	return cg_iRIC_Read_Complex_Real_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Read_Complex_RealSingle(char* groupname, int num, char* name, float* realvalue)
{
	return cg_iRIC_Read_Complex_RealSingle_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Read_Complex_String(char* groupname, int num, char* name, char* strvalue)
{
	return cg_iRIC_Read_Complex_String_Mul(lastfileid, groupname, num, name, strvalue);
}

int cg_iRIC_Read_Complex_FunctionalSize(char* groupname, int num, char* name, cgsize_t* size)
{
	return cg_iRIC_Read_Complex_FunctionalSize_Mul(lastfileid, groupname, num, name, size);
}

int cg_iRIC_Read_Complex_Functional(char* groupname, int num, char* name, double* x, double* y)
{
	return cg_iRIC_Read_Complex_Functional_Mul(lastfileid, groupname, num, name, x, y);
}

int cg_iRIC_Read_Complex_FunctionalWithName(char* groupname, int num, char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_Complex_FunctionalWithName_Mul(lastfileid, groupname, num, name, paramname, data);
}

int cg_iRIC_Read_Complex_Functional_RealSingle(char* groupname, int num, char* name, float* x, float* y)
{
	return cg_iRIC_Read_Complex_Functional_RealSingle_Mul(lastfileid, groupname, num, name, x, y);
}

int cg_iRIC_Read_Grid_Complex_Node(char* groupname, int* values)
{
	return cg_iRIC_Read_Grid_Complex_Node_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Read_Grid_Complex_Cell(char* groupname, int* values)
{
	return cg_iRIC_Read_Grid_Complex_Cell_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Clear_Complex()
{
	return cg_iRIC_Clear_Complex_Mul(lastfileid);
}

int cg_iRIC_Write_Complex_Integer(char* groupname, int num, char* name, int intvalue)
{
	return cg_iRIC_Write_Complex_Integer_Mul(lastfileid, groupname, num, name, intvalue);
}

int cg_iRIC_Write_Complex_Real(char* groupname, int num, char* name, double realvalue)
{
	return cg_iRIC_Write_Complex_Real_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Write_Complex_String(char* groupname, int num, char* name, char* strvalue)
{
	return cg_iRIC_Write_Complex_String_Mul(lastfileid, groupname, num, name, strvalue);
}

int cg_iRIC_Write_Complex_Functional(char* groupname, int num, char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_Complex_Functional_Mul(lastfileid, groupname, num, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_Complex_FunctionalWithName(char* groupname, int num, char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_Complex_FunctionalWithName_Mul(lastfileid, groupname, num, name, paramname, length, data);
}

int cg_iRIC_Write_Grid_Complex_Node(char* groupname, int* values)
{
	return cg_iRIC_Write_Grid_Complex_Node_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Write_Grid_Complex_Cell(char* groupname, int* values)
{
	return cg_iRIC_Write_Grid_Complex_Cell_Mul(lastfileid, groupname, values);
}

int cg_iRIC_GotoGridCoord2d(cgsize_t* isize, cgsize_t* jsize)
{
	return cg_iRIC_GotoGridCoord2d_Mul(lastfileid, isize, jsize);
}

int cg_iRIC_GetGridCoord2d(double* x, double* y)
{
	return cg_iRIC_GetGridCoord2d_Mul(lastfileid, x, y);
}

int cg_iRIC_GotoGridCoord3d(cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize)
{
	return cg_iRIC_GotoGridCoord3d_Mul(lastfileid, isize, jsize, ksize);
}

int cg_iRIC_GetGridCoord3d(double* x, double* y, double* z)
{
	return cg_iRIC_GetGridCoord3d_Mul(lastfileid, x, y, z);
}

int cg_iRIC_Read_Grid_Real_Node(char* name, double* values)
{
	return cg_iRIC_Read_Grid_Real_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Integer_Node(char* name, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Real_Cell(char* name, double* values)
{
	return cg_iRIC_Read_Grid_Real_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Integer_Cell(char* name, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_FunctionalDimensionSize(char* name, char* dimname, cgsize_t* count)
{
	return cg_iRIC_Read_Grid_FunctionalDimensionSize_Mul(lastfileid, name, dimname, count);
}

int cg_iRIC_Read_Grid_FunctionalDimension_Integer(char* name, char* dimname, int* value)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Integer_Mul(lastfileid, name, dimname, value);
}

int cg_iRIC_Read_Grid_FunctionalDimension_Real(char* name, char* dimname, double* value)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Real_Mul(lastfileid, name, dimname, value);
}

int cg_iRIC_Read_Grid_FunctionalTimeSize(char* name, cgsize_t* count)
{
	return cg_iRIC_Read_Grid_FunctionalTimeSize_Mul(lastfileid, name, count);
}

int cg_iRIC_Read_Grid_FunctionalTime(char* name, double* time)
{
	return cg_iRIC_Read_Grid_FunctionalTime_Mul(lastfileid, name, time);
}

int cg_iRIC_Read_Grid_Functional_Integer_Node(char* name, int dimid, int* value)
{
	return cg_iRIC_Read_Grid_Functional_Integer_Node_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Real_Node(char* name, int dimid, double* value)
{
	return cg_iRIC_Read_Grid_Functional_Real_Node_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Integer_Cell(char* name, int dimid, int* value)
{
	return cg_iRIC_Read_Grid_Functional_Integer_Cell_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Real_Cell(char* name, int dimid, double* value)
{
	return cg_iRIC_Read_Grid_Functional_Real_Cell_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_WriteGridCoord1d(cgsize_t isize, double* x)
{
	return cg_iRIC_WriteGridCoord1d_Mul(lastfileid, isize, x);
}

int cg_iRIC_WriteGridCoord2d(cgsize_t isize, cgsize_t jsize, double* x, double* y)
{
	return cg_iRIC_WriteGridCoord2d_Mul(lastfileid, isize, jsize, x, y);
}

int cg_iRIC_WriteGridCoord3d(cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z)
{
	return cg_iRIC_WriteGridCoord3d_Mul(lastfileid, isize, jsize, ksize, x, y, z);
}

int cg_iRIC_Write_Grid_Real_Node(char* name, double* values)
{
	return cg_iRIC_Write_Grid_Real_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Integer_Node(char* name, int* values)
{
	return cg_iRIC_Write_Grid_Integer_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Real_Cell(char* name, double* values)
{
	return cg_iRIC_Write_Grid_Real_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Integer_Cell(char* name, int* values)
{
	return cg_iRIC_Write_Grid_Integer_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Sol_Count(int* count)
{
	return cg_iRIC_Read_Sol_Count_Mul(lastfileid, count);
}

int cg_iRIC_Read_Sol_Time(int step, double* time)
{
	return cg_iRIC_Read_Sol_Time_Mul(lastfileid, step, time);
}

int cg_iRIC_Read_Sol_Iteration(int step, int* index)
{
	return cg_iRIC_Read_Sol_Iteration_Mul(lastfileid, step, index);
}

int cg_iRIC_Read_Sol_BaseIterative_Integer(int step, char* name, int* value)
{
	return cg_iRIC_Read_Sol_BaseIterative_Integer_Mul(lastfileid, step, name, value);
}

int cg_iRIC_Read_Sol_BaseIterative_Real(int step, char* name, double* value)
{
	return cg_iRIC_Read_Sol_BaseIterative_Real_Mul(lastfileid, step, name, value);
}

int cg_iRIC_Read_Sol_GridCoord2d(int step, double* x, double* y)
{
	return cg_iRIC_Read_Sol_GridCoord2d_Mul(lastfileid, step, x, y);
}

int cg_iRIC_Read_Sol_GridCoord3d(int step, double* x, double* y, double* z)
{
	return cg_iRIC_Read_Sol_GridCoord3d_Mul(lastfileid, step, x, y, z);
}

int cg_iRIC_Read_Sol_Integer(int step, char* name, int* data)
{
	return cg_iRIC_Read_Sol_Integer_Mul(lastfileid, step, name, data);
}

int cg_iRIC_Read_Sol_Real(int step, char* name, double* data)
{
	return cg_iRIC_Read_Sol_Real_Mul(lastfileid, step, name, data);
}

int cg_iRIC_Write_Sol_Time(double time)
{
	return cg_iRIC_Write_Sol_Time_Mul(lastfileid, time);
}

int cg_iRIC_Write_Sol_Iteration(int index)
{
	return cg_iRIC_Write_Sol_Iteration_Mul(lastfileid, index);
}

int cg_iRIC_Write_Sol_BaseIterative_Integer(char* name, int value)
{
	return cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_BaseIterative_Real(char* name, double value)
{
	return cg_iRIC_Write_Sol_BaseIterative_Real_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_GridCoord2d(double* x, double* y)
{
	return cg_iRIC_Write_Sol_GridCoord2d_Mul(lastfileid, x, y);
}

int cg_iRIC_Write_Sol_GridCoord3d(double* x, double* y, double* z)
{
	return cg_iRIC_Write_Sol_GridCoord3d_Mul(lastfileid, x, y, z);
}

int cg_iRIC_Write_Sol_Integer(char* name, int* data)
{
	return cg_iRIC_Write_Sol_Integer_Mul(lastfileid, name, data);
}

int cg_iRIC_Write_Sol_Real(char* name, double* data)
{
	return cg_iRIC_Write_Sol_Real_Mul(lastfileid, name, data);
}

int cg_iRIC_Write_ErrorCode(int errorcode)
{
	return cg_iRIC_Write_ErrorCode_Mul(lastfileid, errorcode);
}

void cg_iRIC_Init_BC_Names()
{
	cg_iRIC_Init_BC_Names_Mul(lastfileid);
}

void cg_iRIC_Read_BC_Count(char* type, int* num)
{
	cg_iRIC_Read_BC_Count_Mul(lastfileid, type, num);
}

int cg_iRIC_Read_BC_IndicesSize(char* type, int num, cgsize_t* size)
{
	return cg_iRIC_Read_BC_IndicesSize_Mul(lastfileid, type, num, size);
}

int cg_iRIC_Read_BC_Indices(char* type, int num, cgsize_t* indices)
{
	return cg_iRIC_Read_BC_Indices_Mul(lastfileid, type, num, indices);
}

int cg_iRIC_Read_BC_Integer(char* type, int num, char* name, int* intvalue)
{
	return cg_iRIC_Read_BC_Integer_Mul(lastfileid, type, num, name, intvalue);
}

int cg_iRIC_Read_BC_Real(char* type, int num, char* name, double* realvalue)
{
	return cg_iRIC_Read_BC_Real_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Read_BC_RealSingle(char* type, int num, char* name, float* realvalue)
{
	return cg_iRIC_Read_BC_RealSingle_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Read_BC_String(char* type, int num, char* name, char* strvalue)
{
	return cg_iRIC_Read_BC_String_Mul(lastfileid, type, num, name, strvalue);
}

int cg_iRIC_Read_BC_FunctionalSize(char* type, int num, char* name, cgsize_t* size)
{
	return cg_iRIC_Read_BC_FunctionalSize_Mul(lastfileid, type, num, name, size);
}

int cg_iRIC_Read_BC_Functional(char* type, int num, char* name, double* x, double* y)
{
	return cg_iRIC_Read_BC_Functional_Mul(lastfileid, type, num, name, x, y);
}

int cg_iRIC_Read_BC_FunctionalWithName(char* type, int num, char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_BC_FunctionalWithName_Mul(lastfileid, type, num, name, paramname, data);
}

int cg_iRIC_Read_BC_Functional_RealSingle(char* type, int num, char* name, float* x, float* y)
{
	return cg_iRIC_Read_BC_Functional_RealSingle_Mul(lastfileid, type, num, name, x, y);
}

int cg_iRIC_Clear_BC()
{
	return cg_iRIC_Clear_BC_Mul(lastfileid);
}

int cg_iRIC_Write_BC_Indices(char* type, int num, cgsize_t size, cgsize_t* indices)
{
	return cg_iRIC_Write_BC_Indices_Mul(lastfileid, type, num, size, indices);
}

int cg_iRIC_Write_BC_Integer(char* type, int num, char* name, int intvalue)
{
	return cg_iRIC_Write_BC_Integer_Mul(lastfileid, type, num, name, intvalue);
}

int cg_iRIC_Write_BC_Real(char* type, int num, char* name, double realvalue)
{
	return cg_iRIC_Write_BC_Real_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Write_BC_String(char* type, int num, char* name, char* strvalue)
{
	return cg_iRIC_Write_BC_String_Mul(lastfileid, type, num, name, strvalue);
}

int cg_iRIC_Write_BC_Functional(char* type, int num, char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_BC_Functional_Mul(lastfileid, type, num, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_BC_FunctionalWithName(char* type, int num, char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_BC_FunctionalWithName_Mul(lastfileid, type, num, name, paramname, length, data);
}

int cg_iRIC_Write_Sol_Particle_Pos2d(cgsize_t count, double* x, double* y)
{
	return cg_iRIC_Write_Sol_Particle_Pos2d_Mul(lastfileid, count, x, y);
}

int cg_iRIC_Write_Sol_Particle_Pos3d(cgsize_t count, double* x, double* y, double* z)
{
	return cg_iRIC_Write_Sol_Particle_Pos3d_Mul(lastfileid, count, x, y, z);
}

int cg_iRIC_Write_Sol_Particle_Real(char* name, double* value)
{
	return cg_iRIC_Write_Sol_Particle_Real_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_Particle_Integer(char* name, int* value)
{
	return cg_iRIC_Write_Sol_Particle_Integer_Mul(lastfileid, name, value);
}

int cg_iRIC_Read_Geo_Count(char* name, int* count)
{
	return cg_iRIC_Read_Geo_Count_Mul(lastfileid, name, count);
}

int cg_iRIC_Read_Geo_Filename(char* name, int geoid, char* filename, int* type)
{
	return cg_iRIC_Read_Geo_Filename_Mul(lastfileid, name, geoid, filename, type);
}

