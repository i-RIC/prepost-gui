#define NAME_MAXLENGTH 200
#include "fortran_macros.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <cgnslib.h>
#include <cgns_io.h>

#include "iriclib.h"

#define CCNODE "CalculationConditions"
#define GCNODE "GridConditions"
#define STRINGMAXLEN 2048



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *      Convert between Fortran and C strings                            *
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void string_2_C_string(char *string, int string_length,
		char *c_string, int max_len, int *ierr) {
		int i, iend;

		if (string == NULL || c_string == NULL) {
//				cgi_error ("NULL string pointer");
				*ierr = CG_ERROR;
				return;
		}

		/** Skip and trailing blanks **/
		for (iend = string_length-1; iend >= 0; iend--) {
				if (string[iend] != ' ') break;
		}
		if (iend >= max_len) iend = max_len - 1;

		/** Copy the non-trailing blank portion of the string **/
		for (i = 0; i <= iend; i++)
				c_string[i] = string[i];

		/** NULL terminate the C string **/
		c_string[i] = '\0';
		*ierr = CG_OK;
}

static void string_2_F_string(char *c_string, char *string,
		int string_length, int *ierr) {
		int i;
		size_t len;

		if (c_string == NULL || string == NULL) {
//				cgi_error ("NULL string pointer");
				*ierr = CG_ERROR;
				return;
		}
		len = strlen(c_string);
		if (len > string_length) len = string_length;

		for (i = 0; i < len; i++)
				string[i] = c_string[i];
		while (i < string_length)
				string[i++] = ' ';
		*ierr = CG_OK;
}

void IRICLIBDLL FMNAME(cg_iric_init_f, CG_IRIC_INIT_F) (int *fid, int *ier) {
	*ier = cg_iRIC_Init(*fid);
}

void IRICLIBDLL FMNAME(cg_iric_initread_base_f, CG_IRIC_INITREAD_BASE_F) (int *fid, STR_PSTR(basename), int *ier STR_PLEN(basename)) {
	char c_basename[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(basename), STR_LEN(basename),
		c_basename, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_InitRead_Base(*fid, c_basename);
}

void IRICLIBDLL FMNAME(cg_iric_initread_f, CG_IRIC_INITREAD_F) (int *fid, int *ier) {
	*ier = cg_iRIC_InitRead(*fid);
}

void IRICLIBDLL FMNAME(cg_iric_setfilename_f, CG_IRIC_SETFILENAME_F) (int *fid, STR_PSTR(fname) STR_PLEN(fname)) {
	int ier;
	char c_fname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(fname), STR_LEN(fname),
		c_fname, CGIO_MAX_NAME_LENGTH, &ier);
	if (ier) return;

	cg_iRIC_SetFilename(*fid, c_fname);
}

void IRICLIBDLL FMNAME(iric_initoption_f, IRIC_INITOPTION_F) (int *option, int *ier) {
	*ier = iRIC_InitOption(*option);
}

void IRICLIBDLL FMNAME(cg_iric_flush_f, CG_IRIC_FLUSH_F) (STR_PSTR(name), int *fid, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Flush(c_name, fid);
}

void IRICLIBDLL FMNAME(iric_write_sol_start_f, IRIC_WRITE_SOL_START_F) (STR_PSTR(fname), int *ier STR_PLEN(fname)) {
	char c_fname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(fname), STR_LEN(fname),
		c_fname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = iRIC_Write_Sol_Start(c_fname);
}

void IRICLIBDLL FMNAME(iric_write_sol_end_f, IRIC_WRITE_SOL_END_F) (STR_PSTR(fname), int *ier STR_PLEN(fname)) {
	char c_fname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(fname), STR_LEN(fname),
		c_fname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = iRIC_Write_Sol_End(c_fname);
}

void IRICLIBDLL FMNAME(iric_check_lock_f, IRIC_CHECK_LOCK_F) (STR_PSTR(fname), int *ier STR_PLEN(fname)) {
	char c_fname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(fname), STR_LEN(fname),
		c_fname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = iRIC_Check_Lock(c_fname);
}

void IRICLIBDLL FMNAME(iric_check_cancel_f, IRIC_CHECK_CANCEL_F) (int *canceled) {
	*canceled = iRIC_Check_Cancel();
}

void IRICLIBDLL FMNAME(cg_iric_read_integer_mul_f, CG_IRIC_READ_INTEGER_MUL_F) (int *fid, STR_PSTR(name), int *intvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Integer_Mul(*fid, c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_real_mul_f, CG_IRIC_READ_REAL_MUL_F) (int *fid, STR_PSTR(name), double *realvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Real_Mul(*fid, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_realsingle_mul_f, CG_IRIC_READ_REALSINGLE_MUL_F) (int *fid, STR_PSTR(name), float *realvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_RealSingle_Mul(*fid, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_stringlen_mul_f, CG_IRIC_READ_STRINGLEN_MUL_F) (int *fid, STR_PSTR(name), int *length, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_StringLen_Mul(*fid, c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_string_mul_f, CG_IRIC_READ_STRING_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_String_Mul(*fid, c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalsize_mul_f, CG_IRIC_READ_FUNCTIONALSIZE_MUL_F) (int *fid, STR_PSTR(name), int *size, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalSize_Mul(*fid, c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_functional_mul_f, CG_IRIC_READ_FUNCTIONAL_MUL_F) (int *fid, STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Functional_Mul(*fid, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalwithname_mul_f, CG_IRIC_READ_FUNCTIONALWITHNAME_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(name) STR_PLEN(paramname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalWithName_Mul(*fid, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_functional_realsingle_mul_f, CG_IRIC_READ_FUNCTIONAL_REALSINGLE_MUL_F) (int *fid, STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Functional_RealSingle_Mul(*fid, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalwithname_realsingle_mul_f, CG_IRIC_READ_FUNCTIONALWITHNAME_REALSINGLE_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(paramname), float *data, int *ier STR_PLEN(name) STR_PLEN(paramname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalWithName_RealSingle_Mul(*fid, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_count_mul_f, CG_IRIC_READ_COMPLEX_COUNT_MUL_F) (int *fid, STR_PSTR(groupname), int *num, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Count_Mul(*fid, c_groupname, num);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_integer_mul_f, CG_IRIC_READ_COMPLEX_INTEGER_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), int *intvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Integer_Mul(*fid, c_groupname, *num, c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_real_mul_f, CG_IRIC_READ_COMPLEX_REAL_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), double *realvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Real_Mul(*fid, c_groupname, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_realsingle_mul_f, CG_IRIC_READ_COMPLEX_REALSINGLE_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), float *realvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_RealSingle_Mul(*fid, c_groupname, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_stringlen_mul_f, CG_IRIC_READ_COMPLEX_STRINGLEN_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), int *length, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_StringLen_Mul(*fid, c_groupname, *num, c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_string_mul_f, CG_IRIC_READ_COMPLEX_STRING_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(groupname) STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_String_Mul(*fid, c_groupname, *num, c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functionalsize_mul_f, CG_IRIC_READ_COMPLEX_FUNCTIONALSIZE_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), int *size, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_FunctionalSize_Mul(*fid, c_groupname, *num, c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functional_mul_f, CG_IRIC_READ_COMPLEX_FUNCTIONAL_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Functional_Mul(*fid, c_groupname, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functionalwithname_mul_f, CG_IRIC_READ_COMPLEX_FUNCTIONALWITHNAME_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(groupname) STR_PLEN(name) STR_PLEN(paramname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_FunctionalWithName_Mul(*fid, c_groupname, *num, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functional_realsingle_mul_f, CG_IRIC_READ_COMPLEX_FUNCTIONAL_REALSINGLE_MUL_F) (int *fid, STR_PSTR(groupname), int *num, STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Functional_RealSingle_Mul(*fid, c_groupname, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_complex_node_mul_f, CG_IRIC_READ_GRID_COMPLEX_NODE_MUL_F) (int *fid, STR_PSTR(groupname), int *values, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Complex_Node_Mul(*fid, c_groupname, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_complex_cell_mul_f, CG_IRIC_READ_GRID_COMPLEX_CELL_MUL_F) (int *fid, STR_PSTR(groupname), int *values, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Complex_Cell_Mul(*fid, c_groupname, values);
}

void IRICLIBDLL FMNAME(cg_iric_gotogridcoord2d_mul_f, CG_IRIC_GOTOGRIDCOORD2D_MUL_F) (int *fid, int *isize, int *jsize, int *ier) {
	cgsize_t c_isize;
	cgsize_t c_jsize;
	*ier = cg_iRIC_GotoGridCoord2d_Mul(*fid, &c_isize, &c_jsize);

	*isize = (int)(c_isize);
	*jsize = (int)(c_jsize);
}

void IRICLIBDLL FMNAME(cg_iric_getgridcoord2d_mul_f, CG_IRIC_GETGRIDCOORD2D_MUL_F) (int *fid, double *x, double *y, int *ier) {
	*ier = cg_iRIC_GetGridCoord2d_Mul(*fid, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_gotogridcoord3d_mul_f, CG_IRIC_GOTOGRIDCOORD3D_MUL_F) (int *fid, int *isize, int *jsize, int *ksize, int *ier) {
	cgsize_t c_isize;
	cgsize_t c_jsize;
	cgsize_t c_ksize;
	*ier = cg_iRIC_GotoGridCoord3d_Mul(*fid, &c_isize, &c_jsize, &c_ksize);

	*isize = (int)(c_isize);
	*jsize = (int)(c_jsize);
	*ksize = (int)(c_ksize);
}

void IRICLIBDLL FMNAME(cg_iric_getgridcoord3d_mul_f, CG_IRIC_GETGRIDCOORD3D_MUL_F) (int *fid, double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_GetGridCoord3d_Mul(*fid, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_real_node_mul_f, CG_IRIC_READ_GRID_REAL_NODE_MUL_F) (int *fid, STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Real_Node_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_integer_node_mul_f, CG_IRIC_READ_GRID_INTEGER_NODE_MUL_F) (int *fid, STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Integer_Node_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_real_cell_mul_f, CG_IRIC_READ_GRID_REAL_CELL_MUL_F) (int *fid, STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Real_Cell_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_integer_cell_mul_f, CG_IRIC_READ_GRID_INTEGER_CELL_MUL_F) (int *fid, STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Integer_Cell_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimensionsize_mul_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSIONSIZE_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(dimname), int *count, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_count;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimensionSize_Mul(*fid, c_name, c_dimname, &c_count);

	*count = (int)(c_count);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimension_integer_mul_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSION_INTEGER_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(dimname), int *value, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimension_Integer_Mul(*fid, c_name, c_dimname, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimension_real_mul_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSION_REAL_MUL_F) (int *fid, STR_PSTR(name), STR_PSTR(dimname), double *value, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimension_Real_Mul(*fid, c_name, c_dimname, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaltimesize_mul_f, CG_IRIC_READ_GRID_FUNCTIONALTIMESIZE_MUL_F) (int *fid, STR_PSTR(name), int *count, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_count;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalTimeSize_Mul(*fid, c_name, &c_count);

	*count = (int)(c_count);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaltime_mul_f, CG_IRIC_READ_GRID_FUNCTIONALTIME_MUL_F) (int *fid, STR_PSTR(name), double *time, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalTime_Mul(*fid, c_name, time);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_integer_node_mul_f, CG_IRIC_READ_GRID_FUNCTIONAL_INTEGER_NODE_MUL_F) (int *fid, STR_PSTR(name), int *dimid, int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Integer_Node_Mul(*fid, c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_real_node_mul_f, CG_IRIC_READ_GRID_FUNCTIONAL_REAL_NODE_MUL_F) (int *fid, STR_PSTR(name), int *dimid, double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Real_Node_Mul(*fid, c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_integer_cell_mul_f, CG_IRIC_READ_GRID_FUNCTIONAL_INTEGER_CELL_MUL_F) (int *fid, STR_PSTR(name), int *dimid, int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Integer_Cell_Mul(*fid, c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_real_cell_mul_f, CG_IRIC_READ_GRID_FUNCTIONAL_REAL_CELL_MUL_F) (int *fid, STR_PSTR(name), int *dimid, double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Real_Cell_Mul(*fid, c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord1d_mul_f, CG_IRIC_WRITEGRIDCOORD1D_MUL_F) (int *fid, int *isize, double *x, int *ier) {
	int c_isize;
	c_isize = (cgsize_t)(*isize);
	*ier = cg_iRIC_WriteGridCoord1d_Mul(*fid, c_isize, x);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord2d_mul_f, CG_IRIC_WRITEGRIDCOORD2D_MUL_F) (int *fid, int *isize, int *jsize, double *x, double *y, int *ier) {
	int c_isize;
	int c_jsize;
	c_isize = (cgsize_t)(*isize);
	c_jsize = (cgsize_t)(*jsize);
	*ier = cg_iRIC_WriteGridCoord2d_Mul(*fid, c_isize, c_jsize, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord3d_mul_f, CG_IRIC_WRITEGRIDCOORD3D_MUL_F) (int *fid, int *isize, int *jsize, int *ksize, double *x, double *y, double *z, int *ier) {
	int c_isize;
	int c_jsize;
	int c_ksize;
	c_isize = (cgsize_t)(*isize);
	c_jsize = (cgsize_t)(*jsize);
	c_ksize = (cgsize_t)(*ksize);
	*ier = cg_iRIC_WriteGridCoord3d_Mul(*fid, c_isize, c_jsize, c_ksize, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_initgrid_mul_f, CG_IRIC_INITGRID_MUL_F) (int *fid, int *zoneId, int *ier) {
	*ier = cg_iRIC_InitGrid_Mul(*fid, *zoneId);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_real_node_mul_f, CG_IRIC_WRITE_GRID_REAL_NODE_MUL_F) (int *fid, STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Real_Node_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_integer_node_mul_f, CG_IRIC_WRITE_GRID_INTEGER_NODE_MUL_F) (int *fid, STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Integer_Node_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_real_cell_mul_f, CG_IRIC_WRITE_GRID_REAL_CELL_MUL_F) (int *fid, STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Real_Cell_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_integer_cell_mul_f, CG_IRIC_WRITE_GRID_INTEGER_CELL_MUL_F) (int *fid, STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Integer_Cell_Mul(*fid, c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_count_mul_f, CG_IRIC_READ_SOL_COUNT_MUL_F) (int *fid, int *count, int *ier) {
	*ier = cg_iRIC_Read_Sol_Count_Mul(*fid, count);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_time_mul_f, CG_IRIC_READ_SOL_TIME_MUL_F) (int *fid, int *step, double *time, int *ier) {
	*ier = cg_iRIC_Read_Sol_Time_Mul(*fid, *step, time);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_iteration_mul_f, CG_IRIC_READ_SOL_ITERATION_MUL_F) (int *fid, int *step, int *index, int *ier) {
	*ier = cg_iRIC_Read_Sol_Iteration_Mul(*fid, *step, index);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_baseiterative_integer_mul_f, CG_IRIC_READ_SOL_BASEITERATIVE_INTEGER_MUL_F) (int *fid, int *step, STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_BaseIterative_Integer_Mul(*fid, *step, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_baseiterative_real_mul_f, CG_IRIC_READ_SOL_BASEITERATIVE_REAL_MUL_F) (int *fid, int *step, STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_BaseIterative_Real_Mul(*fid, *step, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_gridcoord2d_mul_f, CG_IRIC_READ_SOL_GRIDCOORD2D_MUL_F) (int *fid, int *step, double *x, double *y, int *ier) {
	*ier = cg_iRIC_Read_Sol_GridCoord2d_Mul(*fid, *step, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_gridcoord3d_mul_f, CG_IRIC_READ_SOL_GRIDCOORD3D_MUL_F) (int *fid, int *step, double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_Read_Sol_GridCoord3d_Mul(*fid, *step, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_integer_mul_f, CG_IRIC_READ_SOL_INTEGER_MUL_F) (int *fid, int *step, STR_PSTR(name), int *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_Integer_Mul(*fid, *step, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_real_mul_f, CG_IRIC_READ_SOL_REAL_MUL_F) (int *fid, int *step, STR_PSTR(name), double *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_Real_Mul(*fid, *step, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_time_mul_f, CG_IRIC_WRITE_SOL_TIME_MUL_F) (int *fid, double *time, int *ier) {
	*ier = cg_iRIC_Write_Sol_Time_Mul(*fid, *time);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_iteration_mul_f, CG_IRIC_WRITE_SOL_ITERATION_MUL_F) (int *fid, int *index, int *ier) {
	*ier = cg_iRIC_Write_Sol_Iteration_Mul(*fid, *index);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_baseiterative_integer_mul_f, CG_IRIC_WRITE_SOL_BASEITERATIVE_INTEGER_MUL_F) (int *fid, STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(*fid, c_name, *value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_baseiterative_real_mul_f, CG_IRIC_WRITE_SOL_BASEITERATIVE_REAL_MUL_F) (int *fid, STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_BaseIterative_Real_Mul(*fid, c_name, *value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_gridcoord2d_mul_f, CG_IRIC_WRITE_SOL_GRIDCOORD2D_MUL_F) (int *fid, double *x, double *y, int *ier) {
	*ier = cg_iRIC_Write_Sol_GridCoord2d_Mul(*fid, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_gridcoord3d_mul_f, CG_IRIC_WRITE_SOL_GRIDCOORD3D_MUL_F) (int *fid, double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_Write_Sol_GridCoord3d_Mul(*fid, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_integer_mul_f, CG_IRIC_WRITE_SOL_INTEGER_MUL_F) (int *fid, STR_PSTR(name), int *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Integer_Mul(*fid, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_real_mul_f, CG_IRIC_WRITE_SOL_REAL_MUL_F) (int *fid, STR_PSTR(name), double *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Real_Mul(*fid, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_errorcode_mul_f, CG_IRIC_WRITE_ERRORCODE_MUL_F) (int *fid, int *errorcode, int *ier) {
	*ier = cg_iRIC_Write_ErrorCode_Mul(*fid, *errorcode);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_count_mul_f, CG_IRIC_READ_BC_COUNT_MUL_F) (int *fid, STR_PSTR(type), int *num STR_PLEN(type)) {
	int ier;
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, &ier);
	if (ier) return;

	cg_iRIC_Read_BC_Count_Mul(*fid, c_type, num);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_indicessize_mul_f, CG_IRIC_READ_BC_INDICESSIZE_MUL_F) (int *fid, STR_PSTR(type), int *num, int *size, int *ier STR_PLEN(type)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_IndicesSize_Mul(*fid, c_type, *num, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_indices_mul_f, CG_IRIC_READ_BC_INDICES_MUL_F) (int *fid, STR_PSTR(type), int *num, int *indices, int *ier STR_PLEN(type)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Indices_Mul(*fid, c_type, *num, (cgsize_t*)(indices));
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_integer_mul_f, CG_IRIC_READ_BC_INTEGER_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), int *intvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Integer_Mul(*fid, c_type, *num, c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_real_mul_f, CG_IRIC_READ_BC_REAL_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), double *realvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Real_Mul(*fid, c_type, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_realsingle_mul_f, CG_IRIC_READ_BC_REALSINGLE_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), float *realvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_RealSingle_Mul(*fid, c_type, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_stringlen_mul_f, CG_IRIC_READ_BC_STRINGLEN_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), int *length, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_StringLen_Mul(*fid, c_type, *num, c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_string_mul_f, CG_IRIC_READ_BC_STRING_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(type) STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_String_Mul(*fid, c_type, *num, c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functionalsize_mul_f, CG_IRIC_READ_BC_FUNCTIONALSIZE_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), int *size, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_FunctionalSize_Mul(*fid, c_type, *num, c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functional_mul_f, CG_IRIC_READ_BC_FUNCTIONAL_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Functional_Mul(*fid, c_type, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functionalwithname_mul_f, CG_IRIC_READ_BC_FUNCTIONALWITHNAME_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(type) STR_PLEN(name) STR_PLEN(paramname)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_FunctionalWithName_Mul(*fid, c_type, *num, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functional_realsingle_mul_f, CG_IRIC_READ_BC_FUNCTIONAL_REALSINGLE_MUL_F) (int *fid, STR_PSTR(type), int *num, STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Functional_RealSingle_Mul(*fid, c_type, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_pos2d_mul_f, CG_IRIC_WRITE_SOL_PARTICLE_POS2D_MUL_F) (int *fid, int *count, double *x, double *y, int *ier) {
	int c_count;
	c_count = (cgsize_t)(*count);
	*ier = cg_iRIC_Write_Sol_Particle_Pos2d_Mul(*fid, c_count, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_pos3d_mul_f, CG_IRIC_WRITE_SOL_PARTICLE_POS3D_MUL_F) (int *fid, int *count, double *x, double *y, double *z, int *ier) {
	int c_count;
	c_count = (cgsize_t)(*count);
	*ier = cg_iRIC_Write_Sol_Particle_Pos3d_Mul(*fid, c_count, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_real_mul_f, CG_IRIC_WRITE_SOL_PARTICLE_REAL_MUL_F) (int *fid, STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Particle_Real_Mul(*fid, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_integer_mul_f, CG_IRIC_WRITE_SOL_PARTICLE_INTEGER_MUL_F) (int *fid, STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Particle_Integer_Mul(*fid, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_geo_count_mul_f, CG_IRIC_READ_GEO_COUNT_MUL_F) (int *fid, STR_PSTR(name), int *count, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Geo_Count_Mul(*fid, c_name, count);
}

void IRICLIBDLL FMNAME(cg_iric_read_geo_filename_mul_f, CG_IRIC_READ_GEO_FILENAME_MUL_F) (int *fid, STR_PSTR(name), int *geoid, STR_PSTR(filename), int *type, int *ier STR_PLEN(name) STR_PLEN(filename)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_filename[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Geo_Filename_Mul(*fid, c_name, *geoid, c_filename, type);

	if (*ier) return;
	string_2_F_string(c_filename, STR_PTR(filename), STR_LEN(filename), ier);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_open_f, IRIC_GEO_POLYGON_OPEN_F) (STR_PSTR(filename), int *id, int *ier STR_PLEN(filename)) {
	char c_filename[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(filename), STR_LEN(filename),
		c_filename, STRINGMAXLEN, ier);
	if (*ier) return;

	*ier = iRIC_Geo_Polygon_Open(c_filename, id);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_integervalue_f, IRIC_GEO_POLYGON_READ_INTEGERVALUE_F) (int *id, int *value, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_IntegerValue(*id, value);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_realvalue_f, IRIC_GEO_POLYGON_READ_REALVALUE_F) (int *id, double *value, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_RealValue(*id, value);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_pointcount_f, IRIC_GEO_POLYGON_READ_POINTCOUNT_F) (int *id, int *size, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_PointCount(*id, size);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_points_f, IRIC_GEO_POLYGON_READ_POINTS_F) (int *id, double *x, double *y, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_Points(*id, x, y);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_holecount_f, IRIC_GEO_POLYGON_READ_HOLECOUNT_F) (int *id, int *count, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_HoleCount(*id, count);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_holepointcount_f, IRIC_GEO_POLYGON_READ_HOLEPOINTCOUNT_F) (int *id, int *holeid, int *count, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_HolePointCount(*id, *holeid, count);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_read_holepoints_f, IRIC_GEO_POLYGON_READ_HOLEPOINTS_F) (int *id, int *holeid, double *x, double *y, int *ier) {
	*ier = iRIC_Geo_Polygon_Read_HolePoints(*id, *holeid, x, y);
}

void IRICLIBDLL FMNAME(iric_geo_polygon_close_f, IRIC_GEO_POLYGON_CLOSE_F) (int *id, int *ier) {
	*ier = iRIC_Geo_Polygon_Close(*id);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_open_f, IRIC_GEO_RIVERSURVEY_OPEN_F) (STR_PSTR(filename), int *id, int *ier STR_PLEN(filename)) {
	char c_filename[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(filename), STR_LEN(filename),
		c_filename, STRINGMAXLEN, ier);
	if (*ier) return;

	*ier = iRIC_Geo_RiverSurvey_Open(c_filename, id);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_count_f, IRIC_GEO_RIVERSURVEY_READ_COUNT_F) (int *id, int *count, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_Count(*id, count);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_position_f, IRIC_GEO_RIVERSURVEY_READ_POSITION_F) (int *id, int *pointid, double *x, double *y, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_Position(*id, *pointid, x, y);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_direction_f, IRIC_GEO_RIVERSURVEY_READ_DIRECTION_F) (int *id, int *pointid, double *vx, double *vy, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_Direction(*id, *pointid, vx, vy);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_name_f, IRIC_GEO_RIVERSURVEY_READ_NAME_F) (int *id, int* pointid, STR_PSTR(name), int *ier STR_PLEN(name)){
	char c_name[STRINGMAXLEN+1];
	const char* nameptr;
	*ier = iRIC_Geo_RiverSurvey_Read_Name(*id, *pointid, &nameptr);
	if (*ier) return;
	strncpy(c_name, nameptr, STRINGMAXLEN);

	string_2_F_string(c_name, STR_PTR(name), STR_LEN(name), ier);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_realname_f, IRIC_GEO_RIVERSURVEY_READ_REALNAME_F) (int *id, int *pointid, double *name, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_RealName(*id, *pointid, name);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_leftshift_f, IRIC_GEO_RIVERSURVEY_READ_LEFTSHIFT_F) (int *id, int *pointid, double *shift, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_LeftShift(*id, *pointid, shift);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_altitudecount_f, IRIC_GEO_RIVERSURVEY_READ_ALTITUDECOUNT_F) (int *id, int *pointid, int *count, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_AltitudeCount(*id, *pointid, count);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_altitudes_f, IRIC_GEO_RIVERSURVEY_READ_ALTITUDES_F) (int *id, int *pointid, double *position, double *height, int *active, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_Altitudes(*id, *pointid, position, height, active);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_fixedpointl_f, IRIC_GEO_RIVERSURVEY_READ_FIXEDPOINTL_F) (int *id, int *pointid, int *set, double *directionX, double *directionY, int *index, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_FixedPointL(*id, *pointid, set, directionX, directionY, index);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_fixedpointr_f, IRIC_GEO_RIVERSURVEY_READ_FIXEDPOINTR_F) (int *id, int *pointid, int *set, double *directionX, double *directionY, int *index, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_FixedPointR(*id, *pointid, set, directionX, directionY, index);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_read_watersurfaceelevation_f, IRIC_GEO_RIVERSURVEY_READ_WATERSURFACEELEVATION_F) (int *id, int *pointid, int *set, double *value, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Read_WaterSurfaceElevation(*id, *pointid, set, value);
}

void IRICLIBDLL FMNAME(iric_geo_riversurvey_close_f, IRIC_GEO_RIVERSURVEY_CLOSE_F) (int *id, int *ier) {
	*ier = iRIC_Geo_RiverSurvey_Close(*id);
}

void IRICLIBDLL FMNAME(cg_iric_read_integer_f, CG_IRIC_READ_INTEGER_F) (STR_PSTR(name), int *intvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Integer(c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_real_f, CG_IRIC_READ_REAL_F) (STR_PSTR(name), double *realvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Real(c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_realsingle_f, CG_IRIC_READ_REALSINGLE_F) (STR_PSTR(name), float *realvalue, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_RealSingle(c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_stringlen_f, CG_IRIC_READ_STRINGLEN_F) (STR_PSTR(name), int *length, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_StringLen(c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_string_f, CG_IRIC_READ_STRING_F) (STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_String(c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalsize_f, CG_IRIC_READ_FUNCTIONALSIZE_F) (STR_PSTR(name), int *size, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalSize(c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_functional_f, CG_IRIC_READ_FUNCTIONAL_F) (STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Functional(c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalwithname_f, CG_IRIC_READ_FUNCTIONALWITHNAME_F) (STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(name) STR_PLEN(paramname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalWithName(c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_functional_realsingle_f, CG_IRIC_READ_FUNCTIONAL_REALSINGLE_F) (STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Functional_RealSingle(c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_functionalwithname_realsingle_f, CG_IRIC_READ_FUNCTIONALWITHNAME_REALSINGLE_F) (STR_PSTR(name), STR_PSTR(paramname), float *data, int *ier STR_PLEN(name) STR_PLEN(paramname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_FunctionalWithName_RealSingle(c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_count_f, CG_IRIC_READ_COMPLEX_COUNT_F) (STR_PSTR(groupname), int *num, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Count(c_groupname, num);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_integer_f, CG_IRIC_READ_COMPLEX_INTEGER_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), int *intvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Integer(c_groupname, *num, c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_real_f, CG_IRIC_READ_COMPLEX_REAL_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), double *realvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Real(c_groupname, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_realsingle_f, CG_IRIC_READ_COMPLEX_REALSINGLE_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), float *realvalue, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_RealSingle(c_groupname, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_stringlen_f, CG_IRIC_READ_COMPLEX_STRINGLEN_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), int *length, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_StringLen(c_groupname, *num, c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_string_f, CG_IRIC_READ_COMPLEX_STRING_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(groupname) STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_String(c_groupname, *num, c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functionalsize_f, CG_IRIC_READ_COMPLEX_FUNCTIONALSIZE_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), int *size, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_FunctionalSize(c_groupname, *num, c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functional_f, CG_IRIC_READ_COMPLEX_FUNCTIONAL_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Functional(c_groupname, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functionalwithname_f, CG_IRIC_READ_COMPLEX_FUNCTIONALWITHNAME_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(groupname) STR_PLEN(name) STR_PLEN(paramname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_FunctionalWithName(c_groupname, *num, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_complex_functional_realsingle_f, CG_IRIC_READ_COMPLEX_FUNCTIONAL_REALSINGLE_F) (STR_PSTR(groupname), int *num, STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(groupname) STR_PLEN(name)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Complex_Functional_RealSingle(c_groupname, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_complex_node_f, CG_IRIC_READ_GRID_COMPLEX_NODE_F) (STR_PSTR(groupname), int *values, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Complex_Node(c_groupname, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_complex_cell_f, CG_IRIC_READ_GRID_COMPLEX_CELL_F) (STR_PSTR(groupname), int *values, int *ier STR_PLEN(groupname)) {
	char c_groupname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(groupname), STR_LEN(groupname),
		c_groupname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Complex_Cell(c_groupname, values);
}

void IRICLIBDLL FMNAME(cg_iric_gotogridcoord2d_f, CG_IRIC_GOTOGRIDCOORD2D_F) (int *isize, int *jsize, int *ier) {
	cgsize_t c_isize;
	cgsize_t c_jsize;
	*ier = cg_iRIC_GotoGridCoord2d(&c_isize, &c_jsize);

	*isize = (int)(c_isize);
	*jsize = (int)(c_jsize);
}

void IRICLIBDLL FMNAME(cg_iric_getgridcoord2d_f, CG_IRIC_GETGRIDCOORD2D_F) (double *x, double *y, int *ier) {
	*ier = cg_iRIC_GetGridCoord2d(x, y);
}

void IRICLIBDLL FMNAME(cg_iric_gotogridcoord3d_f, CG_IRIC_GOTOGRIDCOORD3D_F) (int *isize, int *jsize, int *ksize, int *ier) {
	cgsize_t c_isize;
	cgsize_t c_jsize;
	cgsize_t c_ksize;
	*ier = cg_iRIC_GotoGridCoord3d(&c_isize, &c_jsize, &c_ksize);

	*isize = (int)(c_isize);
	*jsize = (int)(c_jsize);
	*ksize = (int)(c_ksize);
}

void IRICLIBDLL FMNAME(cg_iric_getgridcoord3d_f, CG_IRIC_GETGRIDCOORD3D_F) (double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_GetGridCoord3d(x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_real_node_f, CG_IRIC_READ_GRID_REAL_NODE_F) (STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Real_Node(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_integer_node_f, CG_IRIC_READ_GRID_INTEGER_NODE_F) (STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Integer_Node(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_real_cell_f, CG_IRIC_READ_GRID_REAL_CELL_F) (STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Real_Cell(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_integer_cell_f, CG_IRIC_READ_GRID_INTEGER_CELL_F) (STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Integer_Cell(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimensionsize_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSIONSIZE_F) (STR_PSTR(name), STR_PSTR(dimname), int *count, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_count;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimensionSize(c_name, c_dimname, &c_count);

	*count = (int)(c_count);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimension_integer_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSION_INTEGER_F) (STR_PSTR(name), STR_PSTR(dimname), int *value, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimension_Integer(c_name, c_dimname, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaldimension_real_f, CG_IRIC_READ_GRID_FUNCTIONALDIMENSION_REAL_F) (STR_PSTR(name), STR_PSTR(dimname), double *value, int *ier STR_PLEN(name) STR_PLEN(dimname)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_dimname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(dimname), STR_LEN(dimname),
		c_dimname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalDimension_Real(c_name, c_dimname, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaltimesize_f, CG_IRIC_READ_GRID_FUNCTIONALTIMESIZE_F) (STR_PSTR(name), int *count, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_count;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalTimeSize(c_name, &c_count);

	*count = (int)(c_count);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functionaltime_f, CG_IRIC_READ_GRID_FUNCTIONALTIME_F) (STR_PSTR(name), double *time, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_FunctionalTime(c_name, time);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_integer_node_f, CG_IRIC_READ_GRID_FUNCTIONAL_INTEGER_NODE_F) (STR_PSTR(name), int *dimid, int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Integer_Node(c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_real_node_f, CG_IRIC_READ_GRID_FUNCTIONAL_REAL_NODE_F) (STR_PSTR(name), int *dimid, double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Real_Node(c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_integer_cell_f, CG_IRIC_READ_GRID_FUNCTIONAL_INTEGER_CELL_F) (STR_PSTR(name), int *dimid, int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Integer_Cell(c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_grid_functional_real_cell_f, CG_IRIC_READ_GRID_FUNCTIONAL_REAL_CELL_F) (STR_PSTR(name), int *dimid, double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Grid_Functional_Real_Cell(c_name, *dimid, value);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord1d_f, CG_IRIC_WRITEGRIDCOORD1D_F) (int *isize, double *x, int *ier) {
	int c_isize;
	c_isize = (cgsize_t)(*isize);
	*ier = cg_iRIC_WriteGridCoord1d(c_isize, x);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord2d_f, CG_IRIC_WRITEGRIDCOORD2D_F) (int *isize, int *jsize, double *x, double *y, int *ier) {
	int c_isize;
	int c_jsize;
	c_isize = (cgsize_t)(*isize);
	c_jsize = (cgsize_t)(*jsize);
	*ier = cg_iRIC_WriteGridCoord2d(c_isize, c_jsize, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_writegridcoord3d_f, CG_IRIC_WRITEGRIDCOORD3D_F) (int *isize, int *jsize, int *ksize, double *x, double *y, double *z, int *ier) {
	int c_isize;
	int c_jsize;
	int c_ksize;
	c_isize = (cgsize_t)(*isize);
	c_jsize = (cgsize_t)(*jsize);
	c_ksize = (cgsize_t)(*ksize);
	*ier = cg_iRIC_WriteGridCoord3d(c_isize, c_jsize, c_ksize, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_initgrid_f, CG_IRIC_INITGRID_F) (int *zoneId, int *ier) {
	*ier = cg_iRIC_InitGrid(*zoneId);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_real_node_f, CG_IRIC_WRITE_GRID_REAL_NODE_F) (STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Real_Node(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_integer_node_f, CG_IRIC_WRITE_GRID_INTEGER_NODE_F) (STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Integer_Node(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_real_cell_f, CG_IRIC_WRITE_GRID_REAL_CELL_F) (STR_PSTR(name), double *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Real_Cell(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_write_grid_integer_cell_f, CG_IRIC_WRITE_GRID_INTEGER_CELL_F) (STR_PSTR(name), int *values, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Grid_Integer_Cell(c_name, values);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_count_f, CG_IRIC_READ_SOL_COUNT_F) (int *count, int *ier) {
	*ier = cg_iRIC_Read_Sol_Count(count);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_time_f, CG_IRIC_READ_SOL_TIME_F) (int *step, double *time, int *ier) {
	*ier = cg_iRIC_Read_Sol_Time(*step, time);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_iteration_f, CG_IRIC_READ_SOL_ITERATION_F) (int *step, int *index, int *ier) {
	*ier = cg_iRIC_Read_Sol_Iteration(*step, index);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_baseiterative_integer_f, CG_IRIC_READ_SOL_BASEITERATIVE_INTEGER_F) (int *step, STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_BaseIterative_Integer(*step, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_baseiterative_real_f, CG_IRIC_READ_SOL_BASEITERATIVE_REAL_F) (int *step, STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_BaseIterative_Real(*step, c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_gridcoord2d_f, CG_IRIC_READ_SOL_GRIDCOORD2D_F) (int *step, double *x, double *y, int *ier) {
	*ier = cg_iRIC_Read_Sol_GridCoord2d(*step, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_gridcoord3d_f, CG_IRIC_READ_SOL_GRIDCOORD3D_F) (int *step, double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_Read_Sol_GridCoord3d(*step, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_integer_f, CG_IRIC_READ_SOL_INTEGER_F) (int *step, STR_PSTR(name), int *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_Integer(*step, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_sol_real_f, CG_IRIC_READ_SOL_REAL_F) (int *step, STR_PSTR(name), double *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Sol_Real(*step, c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_time_f, CG_IRIC_WRITE_SOL_TIME_F) (double *time, int *ier) {
	*ier = cg_iRIC_Write_Sol_Time(*time);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_iteration_f, CG_IRIC_WRITE_SOL_ITERATION_F) (int *index, int *ier) {
	*ier = cg_iRIC_Write_Sol_Iteration(*index);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_baseiterative_integer_f, CG_IRIC_WRITE_SOL_BASEITERATIVE_INTEGER_F) (STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_BaseIterative_Integer(c_name, *value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_baseiterative_real_f, CG_IRIC_WRITE_SOL_BASEITERATIVE_REAL_F) (STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_BaseIterative_Real(c_name, *value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_gridcoord2d_f, CG_IRIC_WRITE_SOL_GRIDCOORD2D_F) (double *x, double *y, int *ier) {
	*ier = cg_iRIC_Write_Sol_GridCoord2d(x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_gridcoord3d_f, CG_IRIC_WRITE_SOL_GRIDCOORD3D_F) (double *x, double *y, double *z, int *ier) {
	*ier = cg_iRIC_Write_Sol_GridCoord3d(x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_integer_f, CG_IRIC_WRITE_SOL_INTEGER_F) (STR_PSTR(name), int *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Integer(c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_real_f, CG_IRIC_WRITE_SOL_REAL_F) (STR_PSTR(name), double *data, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Real(c_name, data);
}

void IRICLIBDLL FMNAME(cg_iric_write_errorcode_f, CG_IRIC_WRITE_ERRORCODE_F) (int *errorcode, int *ier) {
	*ier = cg_iRIC_Write_ErrorCode(*errorcode);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_count_f, CG_IRIC_READ_BC_COUNT_F) (STR_PSTR(type), int *num STR_PLEN(type)) {
	int ier;
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, &ier);
	if (ier) return;

	cg_iRIC_Read_BC_Count(c_type, num);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_indicessize_f, CG_IRIC_READ_BC_INDICESSIZE_F) (STR_PSTR(type), int *num, int *size, int *ier STR_PLEN(type)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_IndicesSize(c_type, *num, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_indices_f, CG_IRIC_READ_BC_INDICES_F) (STR_PSTR(type), int *num, int *indices, int *ier STR_PLEN(type)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Indices(c_type, *num, (cgsize_t*)(indices));
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_integer_f, CG_IRIC_READ_BC_INTEGER_F) (STR_PSTR(type), int *num, STR_PSTR(name), int *intvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Integer(c_type, *num, c_name, intvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_real_f, CG_IRIC_READ_BC_REAL_F) (STR_PSTR(type), int *num, STR_PSTR(name), double *realvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Real(c_type, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_realsingle_f, CG_IRIC_READ_BC_REALSINGLE_F) (STR_PSTR(type), int *num, STR_PSTR(name), float *realvalue, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_RealSingle(c_type, *num, c_name, realvalue);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_stringlen_f, CG_IRIC_READ_BC_STRINGLEN_F) (STR_PSTR(type), int *num, STR_PSTR(name), int *length, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_StringLen(c_type, *num, c_name, length);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_string_f, CG_IRIC_READ_BC_STRING_F) (STR_PSTR(type), int *num, STR_PSTR(name), STR_PSTR(strvalue), int *ier STR_PLEN(type) STR_PLEN(name) STR_PLEN(strvalue)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_strvalue[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_String(c_type, *num, c_name, c_strvalue);

	if (*ier) return;
	string_2_F_string(c_strvalue, STR_PTR(strvalue), STR_LEN(strvalue), ier);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functionalsize_f, CG_IRIC_READ_BC_FUNCTIONALSIZE_F) (STR_PSTR(type), int *num, STR_PSTR(name), int *size, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	cgsize_t c_size;
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_FunctionalSize(c_type, *num, c_name, &c_size);

	*size = (int)(c_size);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functional_f, CG_IRIC_READ_BC_FUNCTIONAL_F) (STR_PSTR(type), int *num, STR_PSTR(name), double *x, double *y, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Functional(c_type, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functionalwithname_f, CG_IRIC_READ_BC_FUNCTIONALWITHNAME_F) (STR_PSTR(type), int *num, STR_PSTR(name), STR_PSTR(paramname), double *data, int *ier STR_PLEN(type) STR_PLEN(name) STR_PLEN(paramname)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_paramname[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(paramname), STR_LEN(paramname),
		c_paramname, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_FunctionalWithName(c_type, *num, c_name, c_paramname, data);
}

void IRICLIBDLL FMNAME(cg_iric_read_bc_functional_realsingle_f, CG_IRIC_READ_BC_FUNCTIONAL_REALSINGLE_F) (STR_PSTR(type), int *num, STR_PSTR(name), float *x, float *y, int *ier STR_PLEN(type) STR_PLEN(name)) {
	char c_type[CGIO_MAX_NAME_LENGTH+1];
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(type), STR_LEN(type),
		c_type, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_BC_Functional_RealSingle(c_type, *num, c_name, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_pos2d_f, CG_IRIC_WRITE_SOL_PARTICLE_POS2D_F) (int *count, double *x, double *y, int *ier) {
	int c_count;
	c_count = (cgsize_t)(*count);
	*ier = cg_iRIC_Write_Sol_Particle_Pos2d(c_count, x, y);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_pos3d_f, CG_IRIC_WRITE_SOL_PARTICLE_POS3D_F) (int *count, double *x, double *y, double *z, int *ier) {
	int c_count;
	c_count = (cgsize_t)(*count);
	*ier = cg_iRIC_Write_Sol_Particle_Pos3d(c_count, x, y, z);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_real_f, CG_IRIC_WRITE_SOL_PARTICLE_REAL_F) (STR_PSTR(name), double *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Particle_Real(c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_write_sol_particle_integer_f, CG_IRIC_WRITE_SOL_PARTICLE_INTEGER_F) (STR_PSTR(name), int *value, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Write_Sol_Particle_Integer(c_name, value);
}

void IRICLIBDLL FMNAME(cg_iric_read_geo_count_f, CG_IRIC_READ_GEO_COUNT_F) (STR_PSTR(name), int *count, int *ier STR_PLEN(name)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Geo_Count(c_name, count);
}

void IRICLIBDLL FMNAME(cg_iric_read_geo_filename_f, CG_IRIC_READ_GEO_FILENAME_F) (STR_PSTR(name), int *geoid, STR_PSTR(filename), int *type, int *ier STR_PLEN(name) STR_PLEN(filename)) {
	char c_name[CGIO_MAX_NAME_LENGTH+1];
	char c_filename[STRINGMAXLEN+1];
	string_2_C_string(STR_PTR(name), STR_LEN(name),
		c_name, CGIO_MAX_NAME_LENGTH, ier);
	if (*ier) return;

	*ier = cg_iRIC_Read_Geo_Filename(c_name, *geoid, c_filename, type);

	if (*ier) return;
	string_2_F_string(c_filename, STR_PTR(filename), STR_LEN(filename), ier);
}

