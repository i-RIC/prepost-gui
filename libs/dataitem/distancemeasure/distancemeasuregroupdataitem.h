#ifndef DISTANCEMEASUREGROUPDATAITEM_H
#define DISTANCEMEASUREGROUPDATAITEM_H

#include "distancemeasure_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class DISTANCEMEASURE_API DistanceMeasureGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent);
	~DistanceMeasureGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void addCustomMenuItems(QMenu* menu) override;

private slots:
	void addMeasure();
	void deleteSelected();
	void deleteAll();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/distancemeasuregroupdataitem_impl.h"
#endif // _DEBUG

#endif // DISTANCEMEASUREGROUPDATAITEM_H
