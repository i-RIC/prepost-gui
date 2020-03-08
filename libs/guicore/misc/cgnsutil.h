#ifndef CGNSUTIL_H
#define CGNSUTIL_H

#include <cgnslib.h>

#include <string>
#include <vector>

class QString;

class vtkDataSetAttributes;

class CgnsUtil
{
public:
	static int arrayIdWithName(const std::string& name);

	static int findArray(const QString& name, DataType_t dt, int dim, int narrays);
	static int findArray(const QString& name, DataType_t* dt, int* dim, cgsize_t* dimVec, int narrays);

	template<typename V>
	static int loadArrayWithName(const std::string& name, std::vector<V>* vals);

	template<typename V>
	static int loadArrayWithName(const std::string& name, const std::string& suffix, std::vector<V>* vals);

	template<class T, class DA>
	static void loadScalarDataT(const QString& name, vtkDataSetAttributes* atts, int index, int datalen, const QString& IBCName = "");

	template<class T, class DA>
	static void loadEdgeIScalarDataT(const QString& name, vtkDataSetAttributes* atts, int index, int datalen, cgsize_t dims[3], const QString& IBCName = "");

	template<class T, class DA>
	static void loadEdgeJScalarDataT(const QString& name, vtkDataSetAttributes* atts, int index, int datalen, cgsize_t dims[3], const QString& IBCName = "");

	template<class T, class DA>
	static void loadVectorDataT(const QString& name, vtkDataSetAttributes* atts, int iX, int iY, int iZ, int datalen);

private:
	CgnsUtil();
};

#include "private/cgnsutil_detail.h"

#endif // CGNSUTIL_H
