#ifndef POST2DWINDOWGEODATADATAITEMI_H
#define POST2DWINDOWGEODATADATAITEMI_H

class DelegatedColorMapSettingContainer;
class GraphicsWindowDataModel;

class Post2dWindowGeoDataDataItemI
{
public:
	virtual ~Post2dWindowGeoDataDataItemI() {};

	virtual DelegatedColorMapSettingContainer* colorMapSetting() const = 0;
	virtual GraphicsWindowDataModel* dataModel() const = 0;
};

#endif // POST2DWINDOWGEODATADATAITEMI_H
