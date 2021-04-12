#ifndef POST3DFACESETTINGCONTAINER_H
#define POST3DFACESETTINGCONTAINER_H

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class Post3dFaceSettingContainer : public CompositeContainer
{
public:
	enum class Direction {dirI, dirJ, dirK};

	Post3dFaceSettingContainer();
	Post3dFaceSettingContainer(const Post3dFaceSettingContainer& c);
	~Post3dFaceSettingContainer();

	Post3dFaceSettingContainer& operator=(const Post3dFaceSettingContainer& c);
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

#endif // POST3DFACESETTINGCONTAINER_H
