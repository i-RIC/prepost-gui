#ifndef DISTANCEMEASUREGROUPDATAITEM_H
#define DISTANCEMEASUREGROUPDATAITEM_H

#include "../guicore_global.h"

#include "../datamodel/graphicswindowdataitem.h"

class GUICOREDLL_EXPORT DistanceMeasureGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent);
	/// Destructor
	~DistanceMeasureGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void addMeasure();

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
