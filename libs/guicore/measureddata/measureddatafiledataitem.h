#ifndef MEASUREDDATAFILEDATAITEM_H
#define MEASUREDDATAFILEDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"

class MeasuredData;
class MeasuredDataPointGroupDataItem;
class MeasuredDataVectorGroupDataItem;
class QAction;

class GUICOREDLL_EXPORT MeasuredDataFileDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	MeasuredDataFileDataItem(MeasuredData* md, GraphicsWindowDataItem* parent);
	/// Destructor
	~MeasuredDataFileDataItem();
	MeasuredData* measuredData() const {return m_measuredData;}
	MeasuredDataPointGroupDataItem* pointGroupDataItem() const {return m_pointGroupDataItem;}
	MeasuredDataVectorGroupDataItem* vectorGroupDataItem() const {return m_vectorGroupDataItem;}
	void addCustomMenuItems(QMenu* menu) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;

private slots:
	void exportToFile();

protected:

	MeasuredData* m_measuredData;

	MeasuredDataPointGroupDataItem* m_pointGroupDataItem;
	MeasuredDataVectorGroupDataItem* m_vectorGroupDataItem;

	QAction* m_exportAction;
};

#endif // MEASUREDDATAFILEDATAITEM_H
