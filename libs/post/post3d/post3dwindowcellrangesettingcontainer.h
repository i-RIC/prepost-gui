#ifndef POST3DWINDOWCELLRANGESETTINGCONTAINER_H
#define POST3DWINDOWCELLRANGESETTINGCONTAINER_H

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>

class vtkStructuredGrid;

class Post3dWindowCellRangeSettingContainer : public CompositeContainer
{
public:
	Post3dWindowCellRangeSettingContainer();
	Post3dWindowCellRangeSettingContainer(const Post3dWindowCellRangeSettingContainer& c);
	~Post3dWindowCellRangeSettingContainer();

	vtkStructuredGrid* extractRegion(vtkStructuredGrid* grid);

	Post3dWindowCellRangeSettingContainer& operator=(const Post3dWindowCellRangeSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer enabled;
	IntContainer iMin;
	IntContainer iMax;
	IntContainer jMin;
	IntContainer jMax;
	IntContainer kMin;
	IntContainer kMax;
};

#endif // POST3DWINDOWCELLRANGESETTINGCONTAINER_H
