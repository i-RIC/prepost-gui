#ifndef CGNSUTIL_DETAIL_H
#define CGNSUTIL_DETAIL_H

#include "../cgnsutil.h"

#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>

#include <h5cgnsflowsolution.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

template<class T, class DA>
int CgnsUtil::addScalarDataT(const std::vector<T>& values, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	auto tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(name.c_str());

	tmpArray->Allocate(values.size());
	if (IBCName == name) {
		// for IBC values, special handling is done: 0 is inactive the others are all active.
		for (int i = 0; i < values.size(); ++i) {
			auto origV = values[i];
			int val = static_cast<int>(origV);
			if (val != 0) {origV = 1;}
			tmpArray->InsertNextValue(origV);
		}
	} else {
		for (int i = 0; i < values.size(); ++i) {
			tmpArray->InsertNextValue(values[i]);
		}
	}
	atts->AddArray(tmpArray);

	return IRIC_NO_ERROR;
}

template<class T, class DA>
int CgnsUtil::loadScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<T> data;
	int ier = sol->readValue(name, &data);
	if (ier != IRIC_NO_ERROR) {return ier;}

	return addScalarDataT<T, DA>(data, name, atts, IBCName);
}

template<class T, class DA>
int CgnsUtil::loadEdgeIScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<T> edgeIArray;
	int ier = sol->readValue(name, &edgeIArray);
	if (ier != IRIC_NO_ERROR) {return ier;}

	auto size = sol->zone()->size();

	int ni = size[0];
	int nj = size[1];

	vtkSmartPointer<DA> tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(name.c_str());
	tmpArray->Allocate(ni * nj);

	if (IBCName == name) {
		// for IBC values, special handling is done: 0 is inactive the others are all active.
		for (int i = 0; i < edgeIArray.size(); ++i) {
			int val = static_cast<int>(edgeIArray[i]);
			if (val != 0) { edgeIArray[i] = 1; }
		}
	}

	for (int j = 0; j < nj; ++j) {
		for (int i = 0; i < ni; ++i) {
			if (j == 0) {
				int vdx = i;
				tmpArray->InsertNextValue(edgeIArray[vdx]);
			}	else if (j == (nj - 1)) {
				int vdx = (j - 1) * ni + i;
				tmpArray->InsertNextValue(edgeIArray[vdx]);
			}	else {
				int i1 = (j - 1) * ni + i;
				int i2 = j * ni + i;
				T v = (edgeIArray[i1] + edgeIArray[i2]) * 0.5;
				tmpArray->InsertNextValue(v);
			}
		}
	}
	atts->AddArray(tmpArray);

	return IRIC_NO_ERROR;
}

template<class T, class DA>
int CgnsUtil::loadEdgeJScalarDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts, const std::string& IBCName)
{
	std::vector<T> edgeJArray;
	int ier = sol->readValue(name, &edgeJArray);
	if (ier != IRIC_NO_ERROR) {return ier;}

	auto size = sol->zone()->size();

	int ni = size[0];
	int nj = size[1];

	vtkSmartPointer<DA> tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(name.c_str());
	tmpArray->Allocate(ni * nj);

	if (IBCName == name) {
		// for IBC values, special handling is done: 0 is inactive the others are all active.
		for (int i = 0; i < edgeJArray.size(); ++i) {
			int val = static_cast<int>(edgeJArray[i]);
			if (val != 0) { edgeJArray[i] = 1; }
		}
	}

	for (int j = 0; j < nj; ++j) {
		for (int i = 0; i < ni; ++i) {
			if (i == 0) {
				int vdx = j * (ni - 1);
				tmpArray->InsertNextValue(edgeJArray[vdx]);
			} else if (i == (ni - 1)) {
				int vdx = j * (ni - 1) + i - 1;
				tmpArray->InsertNextValue(edgeJArray[vdx]);
			}	else {
				int i1 = j * (ni - 1) + i - 1;
				int i2 = j * (ni - 1) + i;
				T v = (edgeJArray[i1] + edgeJArray[i2]) * 0.5;
				tmpArray->InsertNextValue(v);
			}
		}
	}
	atts->AddArray(tmpArray);

	return IRIC_NO_ERROR;
}

template<class T, class DA>
int CgnsUtil::loadVectorDataT(iRICLib::H5CgnsSolutionI* sol, const std::string& name, vtkDataSetAttributes* atts)
{
	std::vector<T> dataX;
	std::vector<T> dataY;
	std::vector<T> dataZ;
	std::vector<double> dataMag;

	std::set<std::string> valueNames;
	int ier = sol->readValueNames(&valueNames);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// read x
	auto xName = name;
	xName.append("X");
	ier = sol->readValue(xName, &dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// read y
	auto yName = name;
	yName.append("Y");
	ier = sol->readValue(yName, &dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// read z
	auto zName = name;
	zName.append("Z");
	if (valueNames.find(zName) == valueNames.end()) {
		dataZ.assign(dataX.size(), 0);
	} else {
		ier = sol->readValue(zName, &dataZ);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	// calculate magnitude
	dataMag.assign(dataX.size(), 0);
	for (int i = 0; i < dataX.size(); ++i) {
		double sum = 0;
		sum += dataX[i] * dataX[i];
		sum += dataY[i] * dataY[i];
		sum += dataZ[i] * dataZ[i];
		dataMag[i] = std::sqrt(sum);
	}

	auto tmpArray = vtkSmartPointer<DA>::New();
	tmpArray->SetName(name.c_str());
	tmpArray->SetNumberOfComponents(3);
	tmpArray->Allocate(dataX.size());
	for (int i = 0; i < dataX.size(); ++i) {
		tmpArray->InsertNextTuple3(dataX[i], dataY[i], dataZ[i]);
	}
	atts->AddArray(tmpArray);

	auto magArray = vtkSmartPointer<vtkDoubleArray>::New();
	std::string magName = name;
	magName.append(" (magnitude)");
	magArray->SetName(magName.c_str());
	magArray->Allocate(dataMag.size());
	for (int i = 0; i < dataMag.size(); ++i) {
		magArray->InsertNextValue(dataMag[i]);
	}
	atts->AddArray(magArray);

	return IRIC_NO_ERROR;
}

#endif // CGNSUTIL_DETAIL_H
