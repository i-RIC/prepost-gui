#ifndef POST3DWINDOWFACESETTINGCONTAINER_H
#define POST3DWINDOWFACESETTINGCONTAINER_H

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class Post3dWindowFaceSettingContainer : public CompositeContainer
{
public:
	enum class Direction {dirI, dirJ, dirK};

	Post3dWindowFaceSettingContainer();
	Post3dWindowFaceSettingContainer(const Post3dWindowFaceSettingContainer& c);
	~Post3dWindowFaceSettingContainer();

	vtkStructuredGrid* extractFace(vtkStructuredGrid* grid) const;

	Post3dWindowFaceSettingContainer& operator=(const Post3dWindowFaceSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer enabled;
	EnumContainerT<Direction> direction;
	IntContainer iMin;
	IntContainer iMax;
	IntContainer jMin;
	IntContainer jMax;
	IntContainer kMin;
	IntContainer kMax;
};

#endif // POST3DWINDOWFACESETTINGCONTAINER_H
