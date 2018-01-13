#include "../preobjectbrowserview.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatadataitem.h"
#include "preprocessorgeodatagroupdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodataexporter.h>
#include <guicore/pre/geodata/geodataimporter.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStandardItem>
#include <QStringList>
#include <QToolBar>
#include <QXmlStreamWriter>

#include <cgnslib.h>

PreProcessorGeoDataDataItem::PreProcessorGeoDataDataItem(PreProcessorDataItem* parent) :
	PreProcessorGeoDataDataItemInterface {"", QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_geoData {nullptr},
	m_deleteSilently {false}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	m_exportAction = new QAction(PreProcessorGeoDataDataItem::tr("&Export..."), this);
	m_exportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportGeoData()));
}

PreProcessorGeoDataDataItem::~PreProcessorGeoDataDataItem()
{
	delete m_geoData;
	m_geoData = nullptr;
	PreProcessorGeoDataGroupDataItem* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent());
	if (gItem != nullptr && ! m_deleteSilently) {
		gItem->informValueRangeChange();
	}
}

void PreProcessorGeoDataDataItem::addCustomMenuItems(QMenu* menu)
{
	// Add custom menu first.
	m_geoData->addCustomMenuItems(menu);
	// Add export Action.
	menu->addAction(m_exportAction);
}

void PreProcessorGeoDataDataItem::setGeoData(GeoData* geodata)
{
	// set geodata
	m_geoData = geodata;
	m_geoData->setParent(this);
	// setup vtk actors.
	m_geoData->setupActors();
	m_geoData->setupMenu();
	m_exportAction->setEnabled(isExportAvailable());

	updateZDepthRangeItemCount();
}

void PreProcessorGeoDataDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
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
}

void PreProcessorGeoDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_geoData->loadFromProjectMainFile(node);
}

void PreProcessorGeoDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", m_geoData->typeName());
	m_geoData->saveToProjectMainFile(writer);
}

bool PreProcessorGeoDataDataItem::addToolBarButtons(QToolBar* tb)
{
	PreProcessorGeoDataGroupDataItem* gItem =
		dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent());

	bool added = gItem->addToolBarButtons(tb);
	QAction* sep = nullptr;
	if (added) {
		sep = tb->addSeparator();
	}
	bool added2 = m_geoData->addToolBarButtons(tb);
	if (! added2 && sep) {
		tb->removeAction(sep);
	}
	return added || added2;
}

void PreProcessorGeoDataDataItem::exportGeoData()
{
	QMainWindow* mainW = projectData()->mainWindow();

	auto& exps = m_geoData->exporters();
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
	QString dir = LastIODirectory::get();
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
	QFileInfo finfo(filename);
	LastIODirectory::set(finfo.absolutePath());
}

void PreProcessorGeoDataDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	QStandardItem* parentItem = dynamic_cast<PreProcessorDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}

void PreProcessorGeoDataDataItem::handleStandardItemClicked()
{
	m_geoData->handleStandardItemClicked();
}

void PreProcessorGeoDataDataItem::handleStandardItemDoubleClicked()
{
	m_geoData->handleStandardItemDoubleClicked();
}

void PreProcessorGeoDataDataItem::informSelection(VTKGraphicsView* v)
{
	// delegate to raw data.
	m_geoData->informSelection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::informDeselection(VTKGraphicsView* v)
{
	// delegate to raw data.
	m_geoData->informDeselection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	m_geoData->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_geoData->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_geoData->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_geoData->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

QStringList PreProcessorGeoDataDataItem::containedFiles()
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
	dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent())->informValueRangeChange();
}

void PreProcessorGeoDataDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGeoDataGroupDataItem*>(parent())->informDataChange();
}

bool PreProcessorGeoDataDataItem::getValueRange(double* min, double* max)
{
	if (m_geoData == nullptr) {return false;}
	return m_geoData->getValueRange(min, max);
}

QDialog* PreProcessorGeoDataDataItem::propertyDialog(QWidget* parent)
{
	return m_geoData->propertyDialog(parent);
}

void PreProcessorGeoDataDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_geoData->handlePropertyDialogAccepted(propDialog);
	renderGraphicsView();
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
	bool ok = false;
	/*
			if (m_exportSignalMapper != nullptr){delete m_exportSignalMapper;}
			m_exportSignalMapper = new QSignalMapper(this);
			const QList<GeoDataExporter*>& exporters = m_geoData->exporters();
			for (auto exp_it = exporters.begin(); exp_it != exporters.end(); ++exp_it){
					QString title = (*exp_it)->caption();
					QAction* exportAction = menu->addAction(title.append("..."));
					m_exportSignalMapper->setMapping(exportAction, *exp_it);
					connect(exportAction, SIGNAL(triggered()), m_exportSignalMapper, SLOT(map()));
					ok = true;
			}
			connect(m_exportSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(exportGeoData(QObject*)));
	 */
	return ok;
}

bool PreProcessorGeoDataDataItem::isExportAvailable()
{
	return m_geoData->exporters().size() > 0;
}

void PreProcessorGeoDataDataItem::innerUpdate2Ds()
{
	m_geoData->update2Ds();
}

void PreProcessorGeoDataDataItem::saveToCgnsFile(const int fn)
{
	std::string idxstr =  iRIC::toStr(QString::number(m_index));
	cg_user_data_write(idxstr.c_str());
	cg_gorel(fn, idxstr.c_str(), 0, NULL);
	m_geoData->saveToCgnsFile(fn);
	cg_gorel(fn, "..", 0, NULL);
}

void PreProcessorGeoDataDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	m_geoData->viewOperationEndedGlobal(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorGeoDataDataItem::doApplyOffset(double x, double y)
{
	m_geoData->applyOffset(x, y);
}

void PreProcessorGeoDataDataItem::setDeleteSilently(bool silent)
{
	m_deleteSilently = silent;
}

void PreProcessorGeoDataDataItem::removeFile()
{
	if (m_geoData->filename().isEmpty()) {return;}
	QFile::remove(m_geoData->filename());
}
