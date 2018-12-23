#include "preprocessorhydraulicdatadataitem.h"
#include "preprocessorhydraulicdatagroupdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorhydraulicdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/hydraulicdata/hydraulicdata.h>
#include <guicore/pre/hydraulicdata/hydraulicdatacreator.h>
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationcreator.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>

#include <QAction>
#include <QDomNode>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include <set>

PreProcessorHydraulicDataGroupDataItem::PreProcessorHydraulicDataGroupDataItem(HydraulicDataCreator* creator, PreProcessorDataItem* parent) :
	PreProcessorHydraulicDataGroupDataItemInterface (parent),
	m_creator {creator}
{
	setSubPath(creator->typeName().c_str());

	auto wseCreator = dynamic_cast<HydraulicDataRiverSurveyWaterElevationCreator*>(creator);
	if (wseCreator != nullptr) {
		auto item = new PreProcessorHydraulicDataDataItem(this);
		HydraulicData* data = wseCreator->create(item);
		setUniqueNameAndCaption(data);
		data->setCaption("WSE");
		item->setHydraulicData(data);
		m_childItems.push_back(item);
	}
}

HydraulicDataCreator* PreProcessorHydraulicDataGroupDataItem::creator() const
{
	return m_creator;
}

const QList<PreProcessorHydraulicDataDataItemInterface*> PreProcessorHydraulicDataGroupDataItem::hydraulicDatas() const
{
	QList<PreProcessorHydraulicDataDataItemInterface*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<PreProcessorHydraulicDataDataItemInterface*>(child));
	}
	return ret;
}

void PreProcessorHydraulicDataGroupDataItem::moveUpItem(int index)
{
	auto item = m_childItems.at(index);
	m_childItems.erase(m_childItems.begin() + index);
	m_childItems.insert(m_childItems.begin() + index - 1, item);
	iricMainWindow()->updateCrosssectionWindows();
}

void PreProcessorHydraulicDataGroupDataItem::moveDownItem(int index)
{
	auto item = m_childItems.at(index);
	m_childItems.erase(m_childItems.begin() + index);
	m_childItems.insert(m_childItems.begin() + index + 1, item);
	iricMainWindow()->updateCrosssectionWindows();
}

void PreProcessorHydraulicDataGroupDataItem::deleteItem(int index)
{
	auto item = childItems().at(index);
	delete item;
	iricMainWindow()->updateCrosssectionWindows();
}

void PreProcessorHydraulicDataGroupDataItem::addImportAction(QMenu* menu)
{
	QString cap = m_creator->caption();
	cap.append("...");
	auto a = new QAction(cap, menu);
	menu->addAction(a);

	connect(a, SIGNAL(triggered()), this, SLOT(import()));
}

void PreProcessorHydraulicDataGroupDataItem::import()
{
	QStringList filters;
	QStringList extensions;
	std::map<QString, HydraulicDataImporter*> extMap;
	std::vector<HydraulicDataImporter*> filterList;
	QWidget* pre = preProcessorWindow();

	for (HydraulicDataImporter* importer : m_creator->importers()) {
		for (const auto& filter : importer->fileDialogFilters()) {
			filters.push_back(filter);
			filterList.push_back(importer);
		}
		for (const auto& ext : importer->acceptableExtensions()) {
			extensions << QString("*.").append(ext);
			extMap.insert({ext, importer});
		}
	}
	filters.push_front(tr("All importable files (%1)").arg(extensions.join(" ")));

	QString dir = LastIODirectory::get();
	QString selectedFilter;
	QString filename = QFileDialog::getOpenFileName(pre, tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}
	QFileInfo finfo(filename);

	HydraulicDataImporter* importer = nullptr;
	for (int i = 0; i < filters.size(); ++i) {
	 const auto& filter = filters.at(i);
		if (filter == selectedFilter) {
			if (i == 0) {
				QString ext = finfo.suffix();
				auto it = extMap.find(ext);
				if (it == extMap.end()) {
					QMessageBox::warning(pre, tr("Warning"), tr("File type cannot be recognized from the file extension. : %1").arg(finfo.fileName()));
					return;
				}
				importer = it->second;
			} else {
				importer = filterList.at(i - 1);
			}
		}
	}

	// execute import
	int dataCount;
	bool ret = importer->init(filename, selectedFilter, &dataCount, pre);
	if (! ret) {
		QMessageBox::warning(pre, tr("Import failed"), tr("Importing data from %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return;
	}
	if (dataCount == 0){
		QMessageBox::warning(pre, tr("Import failed"), tr("%1 contains no data to import.").arg(QDir::toNativeSeparators(filename)));
		return;
	}
	std::vector<int> failedIds;

	for (int i = 0; i < dataCount; ++i) {
		auto item = new PreProcessorHydraulicDataDataItem(this);
		HydraulicData* data = importer->creator()->create(item);
		item->setHydraulicData(data);
		setUniqueNameAndCaption(data);

		bool ret = importer->importData(data, i, usedCaptions(), pre);
		if (! ret) {
			// failed importing.
			delete item;
			failedIds.push_back(i + 1);
		} else {
			m_childItems.push_back(item);
		}
	}

	LastIODirectory::set(finfo.absolutePath());

	updateItemMap();

	iricMainWindow()->updateCrosssectionWindows();
	if (failedIds.size() > 0) {
		if (dataCount > 1) {
			QStringList idStrs;
			for (auto id : failedIds) {
				idStrs.push_back(QString::number(id));
				QMessageBox::warning(pre, tr("Warning"),tr("Specified file has invalid data, and those were ignored. Ignored data is as follows:\n%1").arg(idStrs.join("\n")));
			}
		}
	} else {
		QMessageBox::information(pre, tr("Information"), tr("Data is successfully imported from %1.").arg(QDir::toNativeSeparators(filename)));
	}
	// import is not undo-able.
	iRICUndoStack::instance().clear();
	setModified();
}

void PreProcessorHydraulicDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clear();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		auto item = new PreProcessorHydraulicDataDataItem(this);
		QDomNode child = children.at(i);
		HydraulicData* data = m_creator->create(item);
		item->setHydraulicData(data);
		item->loadFromProjectMainFile(child);
		m_childItems.push_back(item);
	}
}

void PreProcessorHydraulicDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_creator->typeName().c_str());
	for (auto child : m_childItems) {
		writer.writeStartElement("HydraulicData");
		child->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorHydraulicDataGroupDataItem::clear()
{
	std::vector<GraphicsWindowDataItem*> childrenCopy = m_childItems;
	for (auto child : childrenCopy) {
		delete child;
	}
}

void PreProcessorHydraulicDataGroupDataItem::setUniqueNameAndCaption(HydraulicData* data)
{
	std::set<QString> nameSet;
	std::set<QString> captionSet;

	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorHydraulicDataDataItemInterface*> (child);
		auto data = item->hydraulicData();
		nameSet.insert(data->name());
		captionSet.insert(data->caption());
	}
	unsigned int idx = 1;
	bool ok = false;
	while (! ok) {
		QString n = m_creator->createName(idx);
		QString c = m_creator->createCaption(idx);
		if (nameSet.find(n) == nameSet.end() && captionSet.find(c) == captionSet.end()) {
			data->setName(n);
			data->setCaption(c);
			ok = true;
		}
		++ idx;
	}
}

std::set<QString> PreProcessorHydraulicDataGroupDataItem::usedCaptions() const
{
	std::set<QString> ret;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorHydraulicDataDataItemInterface*> (child);
		auto data = item->hydraulicData();
		ret.insert(data->caption());
	}
	return ret;
}
