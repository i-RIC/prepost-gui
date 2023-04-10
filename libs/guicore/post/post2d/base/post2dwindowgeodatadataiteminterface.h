#ifndef POST2DWINDOWGEODATADATAITEMINTERFACE_H
#define POST2DWINDOWGEODATADATAITEMINTERFACE_H

class DelegatedColorMapSettingContainer;
class GraphicsWindowDataModel;

class Post2dWindowGeoDataDataItemInterface
{
public:
	virtual ~Post2dWindowGeoDataDataItemInterface() {};
	virtual DelegatedColorMapSettingContainer* colorMapSetting() const = 0;
	virtual GraphicsWindowDataModel* dataModel() const = 0;
};

#endif // POST2DWINDOWGEODATADATAITEMINTERFACE_H
