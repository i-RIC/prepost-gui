#include "graph2dhybridwindowimportdatadataitem.h"
#include "graph2dhybridwindowimportdatagroupdataitem.h"

#include <misc/stringtool.h>

#include <QFile>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItem>
#include <QTextStream>

Graph2dHybridWindowImportDataGroupDataItem::Graph2dHybridWindowImportDataGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowDataItem(tr("External Data"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
}

Graph2dHybridWindowImportDataGroupDataItem::~Graph2dHybridWindowImportDataGroupDataItem()
{

}

void Graph2dHybridWindowImportDataGroupDataItem::updateData()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dHybridWindowImportDataDataItem* item = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(*it);
		item->update();
	}
}

void Graph2dHybridWindowImportDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		QDomNode child = list.at(i);
		Graph2dHybridWindowImportDataDataItem* ditem = new Graph2dHybridWindowImportDataDataItem(this);
		ditem->loadFromProjectMainFile(child);
		m_childItems.append(ditem);
	}
	updateData();
}

void Graph2dHybridWindowImportDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* ditem = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(m_childItems.at(i));
		writer.writeStartElement("Data");
		ditem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Graph2dHybridWindowImportDataGroupDataItem::propertyDialog(QWidget* /*parent*/)
{
	return 0;
}

void Graph2dHybridWindowImportDataGroupDataItem::handlePropertyDialogAccepted(QDialog* /*propDialog*/)
{
}


void Graph2dHybridWindowImportDataGroupDataItem::clear()
{
	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		delete m_childItems.at(i);
	}
}

void Graph2dHybridWindowImportDataGroupDataItem::addItem(Graph2dHybridWindowImportDataDataItem* item)
{
	m_childItems.append(item);
}

void Graph2dHybridWindowImportDataGroupDataItem::importCsv(const QString& filename)
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
	Graph2dHybridWindowResultSetting::Setting setting;
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
		Graph2dHybridWindowImportDataDataItem* item = new Graph2dHybridWindowImportDataDataItem(this);
		item->setData(xVals, yVals);
		item->setId(fname, title);
		// TODO change line color.
		item->setSetting(setting);
		m_childItems.append(item);
	}
	updateData();
}

void Graph2dHybridWindowImportDataGroupDataItem::setData(QList<Graph2dHybridWindowImportDataDataItem*> data)
{
	m_childItems.clear();
	for (int i = 0; i < data.count(); ++i) {
		m_childItems.append(data.at(i));
	}
	updateData();
}

QList<Graph2dHybridWindowImportDataDataItem*> Graph2dHybridWindowImportDataGroupDataItem::data()
{
	QList<Graph2dHybridWindowImportDataDataItem*> ret;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* item = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(m_childItems.at(i));
		ret.append(item);
	}
	return ret;
}

bool Graph2dHybridWindowImportDataGroupDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* rItem = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
