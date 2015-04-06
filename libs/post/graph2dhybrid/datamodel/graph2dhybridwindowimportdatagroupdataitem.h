#ifndef GRAPH2DHYBRIDWINDOWIMPORTDATAGROUPDATAITEM_H
#define GRAPH2DHYBRIDWINDOWIMPORTDATAGROUPDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include <QList>

class Graph2dHybridWindowImportDataDataItem;

class Graph2dHybridWindowImportDataGroupDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT
public:
	Graph2dHybridWindowImportDataGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowImportDataGroupDataItem();
	void setData(QList<Graph2dHybridWindowImportDataDataItem*> data);
	QList<Graph2dHybridWindowImportDataDataItem*> data();
	void addItem(Graph2dHybridWindowImportDataDataItem* item);
	void importCsv(const QString& filename);
	void updateData();
	void clear();
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
};

#endif // GRAPH2DHYBRIDWINDOWIMPORTDATAGROUPDATAITEM_H
