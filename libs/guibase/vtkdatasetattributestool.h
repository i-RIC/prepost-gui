#ifndef VTKDATASETATTRIBUTESTOOL_H
#define VTKDATASETATTRIBUTESTOOL_H

#include "guibase_global.h"

#include <string>
#include <vector>

class vtkDataSetAttributes;

class GUIBASEDLL_EXPORT vtkDataSetAttributesTool
{
public:
	static std::vector<std::string> getArrayNames(vtkDataSetAttributes* atts);
	static std::vector<std::string> getArrayNamesWithOneComponent(vtkDataSetAttributes* atts);
	static std::vector<std::string> getArrayNamesWithMultipleComponents(vtkDataSetAttributes* atts);

private:
	vtkDataSetAttributesTool();
	~vtkDataSetAttributesTool();
};

#endif // VTKDATASETATTRIBUTESTOOL_H
