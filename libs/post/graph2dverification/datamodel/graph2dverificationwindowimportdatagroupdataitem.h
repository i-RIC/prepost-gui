#ifndef GRAPH2DVERIFICATIONWINDOWIMPORTDATAGROUPDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWIMPORTDATAGROUPDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include <QList>

class Graph2dVerificationWindowImportDataDataItem;

class Graph2dVerificationWindowImportDataGroupDataItem : public Graph2dVerificationWindowDataItem
{
	Q_OBJECT

public:
	Graph2dVerificationWindowImportDataGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowImportDataGroupDataItem();
	void setData(QList<Graph2dVerificationWindowImportDataDataItem*> data);
	QList<Graph2dVerificationWindowImportDataDataItem*> data();
	void addItem(Graph2dVerificationWindowImportDataDataItem* item);
	void importCsv(const QString& filename);
	void updateData();
	void clear();
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
};

#endif // GRAPH2DVERIFICATIONWINDOWIMPORTDATAGROUPDATAITEM_H
