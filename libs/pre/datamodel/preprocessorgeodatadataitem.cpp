#include "../preobjectbrowserview.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatadataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "private/preprocessorgeodatadataitem_importsettingdialog.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodataexporter.h>
#include <guicore/pre/geodata/geodataimporter.h>
#include <guicore/pre/geodata/geodataimportersetting.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/geolastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

PreProcessorGeoDataDataItem::PreProcessorGeoDataDataItem(PreProcessorDataItem* parent) :
	PreProcessorGeoDataDataItemI {"", QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_geoData {nullptr},
	m_importAction {new QAction(QIcon(":/libs/guibase/images/iconImport.svg"), PreProcessorGeoDataDataItem::tr("&Import..."), this)},
	m_exportAction {new QAction(QIcon(":/libs/guibase/images/iconExport.svg"), PreProcessorGeoDataDataItem::tr("&Export..."), this)},
	m_showImportSettingAction {new QAction(PreProcessorGeoDataDataItem::tr("Show &import setting..."), this)},
	m_deleteSilently {false}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	connect(m_importAction, &QAction::triggered, this, &PreProcessorGeoDataDataItem::importGeoData);
	connect(m_exportAction, &QAction::triggered, this, &PreProcessorGeoDataDataItem::exportGeoData);
	connect(m_showImportSettingAction, &QAction::triggered, this, &PreProcessorGeoDataDataItem::showImportSetting);
}

PreProcessorGeoDataDataItem::~PreProcessorGeoDataDataItem()
{
	delete m_geoData;
	m_geoData = nullptr;
	PreProcessorGeoDataGroupDataItem* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent());
	if (gItem != nullptr && ! m_deleteSilently) {
		gItem->informDataChange();
		gItem->informValueRangeChange();
	}
}

void PreProcessorGeoDataDataItem::addCustomMenuItems(QMenu* menu)
{
	// Add custom menu first.
	m_geoData->addCustomMenuItems(menu);

	// Add export Action.
	if (m_geoData->dataLoaded()) {
		menu->addAction(m_exportAction);
	} else {
		menu->addAction(m_importAction);
	}

	if (m_geoData->importerSetting() != nullptr) {
		menu->addAction(m_showImportSettingAction);
	}
}

PreProcessorGeoDataGroupDataItemI* PreProcessorGeoDataDataItem::groupDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (parent());
}

GeoData* PreProcessorGeoDataDataItem::geoData() const
{
	return m_geoData;
}

void PreProcessorGeoDataDataItem::setGeoData(GeoData* geodata)
{
	// set geodata
	m_geoData = geodata;
	m_geoData->setParent(this);
	// setup vtk actors.
	m_geoData->setupActors();
	m_geoData->setupMenu();
	connect(m_geoData, &GeoData::valueRangeChanged, this, &PreProcessorGeoDataDataItem::informValueRangeChange);
	connect(m_geoData, &GeoData::dataChanged, this, &PreProcessorGeoDataDataItem::informDataChange);

	m_exportAction->setEnabled(isExportAvailable());
	if (geodata->isReadOnly()) {
		m_standardItem->setIcon(QIcon(":/libs/guibase/images/iconLink.svg"));
	}

	updateZDepthRangeItemCount();
}

void PreProcessorGeoDataDataItem::handleStandardItemChange()
{
	QString newcaption = m_standardItem->data(Qt::EditRole).toString();
	if (newcaption != m_geoData->caption()) {
		if (dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent())->isChildCaptionAvailable(newcaption)) {
			m_geoData->setCaption(newcaption);
			emit captionChanged(newcaption);
		} else {
			QMessageBox::warning(preProcessorWindow(), tr("Failure"), tr("You cannot use this name for this data. A geographic data with the same name already exists."));
			m_standardItem->setData(m_geoData->caption(), Qt::EditRole);
		}
	}
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*> (groupDataItem());
	gItem->gridTypeDataItem()->updateColorBarVisibility(gItem->condition()->name());

	PreProcessorDataItem::handleStandardItemChange();
}

void PreProcessorGeoDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoDataImporterSetting* is = nullptr;
	GeoDataImporter* importer = nullptr;

	auto isNode = iRIC::getChildNode(node, "ImporterSetting");
	if (! isNode.isNull()) {
		auto importerName = iRIC::toStr(isNode.toElement().attribute("name"));
		importer = m_geoData->creator()->importer(importerName);
		is = importer->createSetting();
		is->loadFromProjectMainFile(isNode);
	}

	m_geoData->setImporterSetting(is);

	if (m_geoData->isReadOnly()) {
		m_geoData->loadFromProjectMainFileOnly(node);
		m_standardItem->setIcon(QIcon(":/libs/guibase/images/iconLink.svg"));
	} else {
		m_geoData->loadFromProjectMainFile(node);
		m_geoData->setDataLoaded(true);
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGeoDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", m_geoData->typeName());
	m_geoData->saveToProjectMainFile(writer);

	auto is = m_geoData->importerSetting();
	if (is != nullptr) {
		writer.writeStartElement("ImporterSetting");
		is->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

bool PreProcessorGeoDataDataItem::addToolBarButtons(QToolBar* toolBar)
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent());

	bool added = gItem->addToolBarButtons(toolBar);
	QAction* sep = nullptr;
	if (added) {
		sep = toolBar->addSeparator();
	}
	bool added2 = m_geoData->addToolBarButtons(toolBar);
	if (! added2 && sep) {
		toolBar->removeAction(sep);
	}
	return added || added2;
}

void PreProcessorGeoDataDataItem::importGeoData()
{
	if (m_geoData->dataLoaded()) {return;}

	auto is = m_geoData->importerSetting();
	QFile f(is->fileName());
	if (! f.exists()) {
		QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Import target file \"%1\" does not exists.").arg(QDir::toNativeSeparators(f.fileName())));
		return;
	}

	auto importer = m_geoData->creator()->importer(is->name());
	importer->setSetting(is);

	int dataCount;
	bool ok = importer->importInit(&dataCount, groupDataItem()->condition(), groupDataItem(), preProcessorWindow(), true);
	if (! ok) {
		goto CLEAN;
	}
	ok = importer->importData(m_geoData, 0, preProcessorWindow());
	if (! ok) {
		goto CLEAN;
	}
	m_geoData->setDataLoaded(true);
	auto o = offset();
	m_geoData->applyOffset(o.x(), o.y());

	updateZDepthRange();
	m_geoData->updateActorSetting();
	informValueRangeChange();
	renderGraphicsView();

CLEAN:
	importer->setSetting(nullptr);
}

void PreProcessorGeoDataDataItem::exportGeoData()
{
	QMainWindow* mainW = projectData()->mainWindow();

#if (_MSC_VER == 1800)  // vs2013
	auto& exps = m_geoData->exporters();
#else
	auto exps = m_geoData->exporters();
#endif
	QStringList filters;
	QList<GeoDataExporter*> exporters;
	for (auto exp_it = exps.begin(); exp_it != exps.end(); ++exp_it) {
		GeoDataExporter* exp = *exp_it;
		QStringList fils = exp->fileDialogFilters();
		for (auto s_it = fils.begin(); s_it != fils.end(); ++s_it) {
			filters.append(*s_it);
			exporters.append(exp);
		}
	}
	QString dir = GeoLastIODirectory::get();
	QString selectedFilter;
	// Select the file to export.
	QString filename = QFileDialog::getSaveFileName(mainW, tr("Select File to Export"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}

	GeoDataExporter* exporter = nullptr;
	for (int i = 0; i < filters.count(); ++i) {
		if (filters[i] == selectedFilter) {
			exporter = exporters[i];
		}
	}
	Q_ASSERT(exporter != nullptr);

	// execute export.
	exporter->doExport(m_geoData, filename, selectedFilter, mainW, projectData());
	GeoLastIODirectory::setFromFilename(filename);
}

void PreProcessorGeoDataDataItem::showImportSetting()
{
	auto is = m_geoData->importerSetting();
	if (is == nullptr) {return;}

	ImportSettingDialog dialog(preProcessorWindow());
	dialog.setItems(is->items(m_geoData->creator()));
	dialog.exec();
}

void PreProcessorGeoDataDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	auto parentItem = dynamic_cast<PreProcessorDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}

void PreProcessorGeoDataDataItem::handleStandardItemDoubleClicked()
{
	m_geoData->handleStandardItemDoubleClicked();
}

void PreProcessorGeoDataDataItem::informSelection(VTKGraphicsView* v)
{
	// delegate to geo data.
	m_geoData->informSelection(dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::informDeselection(VTKGraphicsView* v)
{
	// delegate to geo data.
	m_geoData->informDeselection(dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	m_geoData->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_geoData->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_geoData->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

QStringList PreProcessorGeoDataDataItem::containedFiles() const
{
	return m_geoData->containedFiles();
}

void PreProcessorGeoDataDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_geoData->assignActorZValues(range);
}

void PreProcessorGeoDataDataItem::updateZDepthRangeItemCount()
{
	m_geoData->updateZDepthRangeItemCount(m_zDepthRange);
}

void PreProcessorGeoDataDataItem::informValueRangeChange()
{
	groupDataItem()->informValueRangeChange();
}

void PreProcessorGeoDataDataItem::informDataChange()
{
	groupDataItem()->informDataChange();
}

bool PreProcessorGeoDataDataItem::getValueRange(double* min, double* max)
{
	if (m_geoData == nullptr) {return false;}
	return m_geoData->getValueRange(min, max);
}

void PreProcessorGeoDataDataItem::applyColorMapSetting()
{
	if (m_geoData == nullptr) {return;}
	m_geoData->applyColorMapSetting();
}

void PreProcessorGeoDataDataItem::showPropertyDialog()
{
	m_geoData->showPropertyDialog();
}

QDialog* PreProcessorGeoDataDataItem::propertyDialog(QWidget* parent)
{
	return m_geoData->propertyDialog(parent);
}

void PreProcessorGeoDataDataItem::moveUp()
{
	PreProcessorDataItem::moveUp();
	informDataChange();
}

void PreProcessorGeoDataDataItem::moveDown()
{
	PreProcessorDataItem::moveDown();
	informDataChange();
}

bool PreProcessorGeoDataDataItem::setupExportMenu(QMenu* /*menu*/)
{
	return false;
}

bool PreProcessorGeoDataDataItem::isExportAvailable()
{
	return m_geoData->exporters().size() > 0;
}

void PreProcessorGeoDataDataItem::innerUpdate2Ds()
{
	m_geoData->update2Ds();
}

void PreProcessorGeoDataDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	m_geoData->viewOperationEndedGlobal(dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::doApplyOffset(double x, double y)
{
	m_geoData->applyOffset(x, y);
}

void PreProcessorGeoDataDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_geoData->handleResize(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void PreProcessorGeoDataDataItem::doUpdateActorSetting()
{
	if (m_geoData == nullptr) {return;}

	m_geoData->doUpdateActorSetting();
}

void PreProcessorGeoDataDataItem::setDeleteSilently(bool silent)
{
	m_deleteSilently = silent;
}

ColorMapSettingContainerI* PreProcessorGeoDataDataItem::colorMapSettingContainer() const
{
	auto groupdi = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (parent());
	if (groupdi == nullptr) {return nullptr;}

	auto typedi = dynamic_cast<PreProcessorGridTypeDataItemI*>(groupdi->parent()->parent());
	return typedi->colorMapSetting(groupdi->condition()->name());
}
