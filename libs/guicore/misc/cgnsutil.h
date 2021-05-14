#ifndef CGNSUTIL_H
#define CGNSUTIL_H

#include <cgnslib.h>

#include <string>
#include <vector>

namespace iRICLib {
	class H5CgnsSolutionI;
} // namespace iRICLib

class vtkDataSetAttributes;

class CgnsUtil
{
public:
	static bool isScalarName(const std::string& name);
	static bool isGridLocation(const std::string& name);
	static int loadScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName = "");
	static int loadEdgeIScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName = "");
	static int loadEdgeJScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName = "");
	static int loadVectorData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts);

	template<class T, class DA>
	static int addScalarDataT(const std::vector<T>& values, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName = "");

	template<class T, class DA>
	static int loadScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName = "");

	template<class T, class DA>
	static int loadEdgeIScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName = "");

	template<class T, class DA>
	static int loadEdgeJScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName = "");

	template<class T, class DA>
	static int loadVectorDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts);

private:
	CgnsUtil();
};

#include "private/cgnsutil_detail.h"

#endif // CGNSUTIL_H
