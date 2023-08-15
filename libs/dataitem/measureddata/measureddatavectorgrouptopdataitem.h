#ifndef MEASUREDDATAVECTORGROUPTOPDATAITEM_H
#define MEASUREDDATAVECTORGROUPTOPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

#include <unordered_set>

class ColorMapSettingContainerI;
class MeasuredDataFileDataItem;

class MeasuredDataVectorGroupTopDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	MeasuredDataVectorGroupTopDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void update();
	MeasuredDataFileDataItem* fileDataItem() const;
	std::unordered_set<ColorMapSettingContainerI*> activeColorMapSettingsWithVisibleLegend() const;

public slots:
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // MEASUREDDATAVECTORGROUPTOPDATAITEM_H
