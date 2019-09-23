#include "graph2dverificationwindowimportdatadataitem.h"
#include "graph2dverificationwindowimportdatagroupdataitem.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItem>
#include <QTextStream>

Graph2dVerificationWindowImportDataGroupDataItem::Graph2dVerificationWindowImportDataGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowDataItem(tr("External Data"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
}

Graph2dVerificationWindowImportDataGroupDataItem::~Graph2dVerificationWindowImportDataGroupDataItem()
{

}

void Graph2dVerificationWindowImportDataGroupDataItem::updateData()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dVerificationWindowImportDataDataItem* item = dynamic_cast<Graph2dVerificationWindowImportDataDataItem*>(*it);
		item->update();
	}
}

void Graph2dVerificationWindowImportDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		QDomNode child = list.at(i);
		Graph2dVerificationWindowImportDataDataItem* ditem = new Graph2dVerificationWindowImportDataDataItem(this);
		ditem->loadFromProjectMainFile(child);
		m_childItems.append(ditem);
	}
	updateData();
}

void Graph2dVerificationWindowImportDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* ditem = dynamic_cast<Graph2dVerificationWindowImportDataDataItem*>(m_childItems.at(i));
		writer.writeStartElement("Data");
		ditem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Graph2dVerificationWindowImportDataGroupDataItem::propertyDialog(QWidget* /*parent*/)
{
	return 0;
}

void Graph2dVerificationWindowImportDataGroupDataItem::handlePropertyDialogAccepted(QDialog* /*propDialog*/)
{
}


void Graph2dVerificationWindowImportDataGroupDataItem::clear()
{
	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		delete m_childItems.at(i);
	}
}

void Graph2dVerificationWindowImportDataGroupDataItem::addItem(Graph2dVerificationWindowImportDataDataItem* item)
{
	m_childItems.append(item);
}

void Graph2dVerificationWindowImportDataGroupDataItem::importCsv(const QString& filename)
{
	QFile csvFile(filename);
	csvFile.open(QFile::ReadOnly);
	QTextStream csvStream(&csvFile);

	QList<QString> titles = iRIC::parseCSVLine(csvStream.readLine());
	QList<QVector<double> > values;
	QVector<double> emptyVec;
	for (int i = 0; i < titles.count(); ++i) {
		values.append(emptyVec);
	}
	while (! csvStream.atEnd()) {
		QString line = csvStream.readLine();
		QList<QString> frags = iRIC::parseCSVLine(line);
		for (int i = 0; i < frags.count(); ++i) {
			QString f = frags[i].trimmed();
			if (! f.isEmpty()) {
				double val = f.toDouble();
				values[i].append(val);
			}
		}
	}
	csvFile.close();

	if (titles.at(0) != "X") {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("The title of the first column has to be \"X\""));
		return;
	}
	QVector<double> xVals;
	QVector<double> yVals;

	QFileInfo finfo(filename);
	QString fname = finfo.fileName();
	Graph2dVerificationWindowResultSetting::Setting setting;
	for (int i = 0; i < titles.count(); ++i) {
		QString title = titles.at(i);
		if (title == "X") {
			xVals = values.at(i);
			continue;
		}
		yVals = values.at(i);
		if (xVals.count() != yVals.count()) {
			// skip this data.
			QMessageBox::warning(mainWindow(), tr("Warning"), tr("The number of data of %1 and %2 mismatch. Data %1 is skipped."));
			continue;
		}
		Graph2dVerificationWindowImportDataDataItem* item = new Graph2dVerificationWindowImportDataDataItem(this);
		item->setData(xVals, yVals);
		item->setId(fname, title);
		// TODO change line color.
		item->setSetting(setting);
		m_childItems.append(item);
	}
	updateData();
}

void Graph2dVerificationWindowImportDataGroupDataItem::setData(QList<Graph2dVerificationWindowImportDataDataItem*> data)
{
	m_childItems.clear();
	for (int i = 0; i < data.count(); ++i) {
		m_childItems.append(data.at(i));
	}
	updateData();
}

QList<Graph2dVerificationWindowImportDataDataItem*> Graph2dVerificationWindowImportDataGroupDataItem::data()
{
	QList<Graph2dVerificationWindowImportDataDataItem*> ret;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* item = dynamic_cast<Graph2dVerificationWindowImportDataDataItem*>(m_childItems.at(i));
		ret.append(item);
	}
	return ret;
}

bool Graph2dVerificationWindowImportDataGroupDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* rItem = dynamic_cast<Graph2dVerificationWindowImportDataDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
