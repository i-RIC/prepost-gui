#include "cgnsutil.h"
#include "../project/projectcgnsfile.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>

bool CgnsUtil::isScalarName(const std::string& name)
{
	char last = name.at(name.size() - 1);
	return ! (last == 'X' || last == 'Y' || last == 'Z');
}

bool CgnsUtil::isGridLocation(const std::string& name)
{
	return name == "GridLocation";
}

int CgnsUtil::loadScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<std::string> names;
	int ier = sol->readValueNames(&names);
	if (ier != 0) {return ier;}

	for (const auto& n : names) {
		if (! isScalarName(n)) {continue;}
		if (isGridLocation(n)) {continue;}

		iRICLib::H5Util::DataArrayValueType type;
		ier = sol->readValueType(n, &type);
		if (ier != 0) {return ier;}

		if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			ier = loadScalarDataT<double, vtkDoubleArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		} else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			ier = loadScalarDataT<int, vtkIntArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		}
	}
	return 0;
}

int CgnsUtil::loadEdgeIScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<std::string> names;
	int ier = sol->readValueNames(&names);
	if (ier != 0) {return ier;}

	for (const auto& n : names) {
		if (! isScalarName(n)) {continue;}
		if (isGridLocation(n)) {continue;}

		iRICLib::H5Util::DataArrayValueType type;
		ier = sol->readValueType(n, &type);
		if (ier != 0) {return ier;}

		if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			ier = loadEdgeIScalarDataT<double, vtkDoubleArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		} else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			ier = loadEdgeIScalarDataT<int, vtkIntArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		}
	}
	return 0;
}

int CgnsUtil::loadEdgeJScalarData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<std::string> names;
	int ier = sol->readValueNames(&names);
	if (ier != 0) {return ier;}

	for (const auto& n : names) {
		if (! isScalarName(n)) {continue;}
		if (isGridLocation(n)) {continue;}

		iRICLib::H5Util::DataArrayValueType type;
		ier = sol->readValueType(n, &type);
		if (ier != 0) {return ier;}

		if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			ier = loadEdgeJScalarDataT<double, vtkDoubleArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		} else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			ier = loadEdgeJScalarDataT<int, vtkIntArray> (sol, n, atts, IBCName);
			if (ier != 0) {return ier;}
		}
	}
	return 0;
}

int CgnsUtil::loadVectorData(iRICLib::H5CgnsSolutionI* sol, vtkDataSetAttributes* atts)
{
	std::vector<std::string> names;
	int ier = sol->readValueNames(&names);
	if (ier != 0) {return ier;}

	for (const auto& n : names) {
		if (isGridLocation(n)) {continue;}
		char last = n.at(n.size() - 1);
		if (last != 'X') {continue;}

		iRICLib::H5Util::DataArrayValueType type;
		ier = sol->readValueType(n, &type);
		if (ier != 0) {return ier;}

		auto vname = n.substr(0, n.size() - 1);

		if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			ier = loadVectorDataT<double, vtkDoubleArray> (sol, vname, atts);
			if (ier != 0) {return ier;}
		} else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			ier = loadVectorDataT<int, vtkIntArray> (sol, vname, atts);
			if (ier != 0) {return ier;}
		}
	}
	return 0;
}

CgnsUtil::CgnsUtil()
{}
