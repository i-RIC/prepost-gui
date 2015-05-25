#ifndef DISTANCEMEASUREGROUPDATAITEM_H
#define DISTANCEMEASUREGROUPDATAITEM_H

#include "../guicore_global.h"

#include "../datamodel/graphicswindowdataitem.h"

class GUICOREDLL_EXPORT DistanceMeasureGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent);
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void addMeasure();

protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;

	QAction* m_addAction;
};

#endif // DISTANCEMEASUREGROUPDATAITEM_H
