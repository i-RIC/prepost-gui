#ifndef MEASUREDDATAFILEDATAITEM_H
#define MEASUREDDATAFILEDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class MeasuredData;
class MeasuredDataPointGroupDataItem;
class MeasuredDataVectorGroupDataItem;

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

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/measureddatafiledataitem_impl.h"
#endif // _DEBUG

#endif // MEASUREDDATAFILEDATAITEM_H
