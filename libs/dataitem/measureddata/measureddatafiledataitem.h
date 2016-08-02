#ifndef MEASUREDDATAFILEDATAITEM_H
#define MEASUREDDATAFILEDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class MeasuredData;
class MeasuredDataPointGroupDataItem;
class MeasuredDataVectorGroupDataItem;
class QAction;

class MEASUREDDATA_API MeasuredDataFileDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	MeasuredDataFileDataItem(MeasuredData* md, GraphicsWindowDataItem* parent);
	~MeasuredDataFileDataItem();

	MeasuredData* measuredData() const;
	MeasuredDataPointGroupDataItem* pointGroupDataItem() const;
	MeasuredDataVectorGroupDataItem* vectorGroupDataItem() const;
	void addCustomMenuItems(QMenu* menu) override;

private slots:
	void exportToFile();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;

	MeasuredData* m_measuredData;

	MeasuredDataPointGroupDataItem* m_pointGroupDataItem;
	MeasuredDataVectorGroupDataItem* m_vectorGroupDataItem;

	QAction* m_exportAction;
};

#endif // MEASUREDDATAFILEDATAITEM_H
