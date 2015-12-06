#include "vtkdatasetattributestool.h"

#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>

std::vector<std::string> vtkDataSetAttributesTool::getArrayNames(vtkDataSetAttributes* atts)
{
	std::vector<std::string> arrayNames;

	int num = atts->GetNumberOfArrays();
	for (int i = 0; i < num; ++i) {
		vtkDataArray* arr = atts->GetArray(i);
		if (arr == nullptr) {continue;}

		arrayNames.push_back(arr->GetName());
	}
	return arrayNames;
}

std::vector<std::string> vtkDataSetAttributesTool::getArrayNamesWithOneComponent(vtkDataSetAttributes* atts)
{
	std::vector<std::string> arrayNames;

	int num = atts->GetNumberOfArrays();
	for (int i = 0; i < num; ++i) {
		vtkDataArray* arr = atts->GetArray(i);
		if (arr == nullptr) {continue;}
		if (arr->GetNumberOfComponents() != 1) {continue;}

		arrayNames.push_back(arr->GetName());
	}
	return arrayNames;
}

std::vector<std::string> vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(vtkDataSetAttributes* atts)
{
	std::vector<std::string> arrayNames;

	int num = atts->GetNumberOfArrays();
	for (int i = 0; i < num; ++i) {
		vtkDataArray* arr = atts->GetArray(i);
		if (arr == nullptr) {continue;}
		if (arr->GetNumberOfComponents() <= 1) {continue;}

		arrayNames.push_back(arr->GetName());
	}
	return arrayNames;
}
