#include "../preobjectbrowserview.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorrawdatadataitem.h"
#include "preprocessorrawdatagroupdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/rawdata/rawdata.h>
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <guicore/pre/rawdata/rawdataexporter.h>
#include <guicore/pre/rawdata/rawdataimporter.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
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

PreProcessorRawdataDataItem::PreProcessorRawdataDataItem(PreProcessorDataItem* parent)
	: PreProcessorRawdataDataItemInterface("", QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_rawData = nullptr;
	m_isReorderable = true;
	m_exportAction = new QAction(tr("&Export..."), this);
	m_exportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportRawdata()));
}

PreProcessorRawdataDataItem::~PreProcessorRawdataDataItem()
{
	if (m_rawData) {delete m_rawData;}
	m_rawData = nullptr;
	PreProcessorRawDataGroupDataItem* gItem = dynamic_cast<PreProcessorRawDataGroupDataItem*>(parent());
	if (gItem != nullptr) {
		gItem->informValueRangeChange();
	}
}

void PreProcessorRawdataDataItem::addCustomMenuItems(QMenu* menu)
{
	// Add custom menu first.
	m_rawData->addCustomMenuItems(menu);
	// Add export Action.
	menu->addAction(m_exportAction);
}

void PreProcessorRawdataDataItem::setRawData(RawData* rawdata)
{
	// set rawdata
	m_rawData = rawdata;
	m_rawData->setParent(this);
	// setup vtk actors.
	m_rawData->setupActors();
	m_rawData->setupMenu();
	m_exportAction->setEnabled(isExportAvailable());

	updateZDepthRangeItemCount();
}

void PreProcessorRawdataDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
	QString newcaption = m_standardItem->data(Qt::EditRole).toString();
	if (newcaption != m_rawData->caption()) {
		if (dynamic_cast<PreProcessorRawDataGroupDataItem*>(parent())->isChildCaptionAvailable(newcaption)) {
			m_rawData->setCaption(newcaption);
			emit captionChanged(newcaption);
		} else {
			QMessageBox::warning(preProcessorWindow(), tr("Failure"), tr("You cannot use this name for this data. A geographic data with the same name already exists."));
			m_standardItem->setData(m_rawData->caption(), Qt::EditRole);
		}
	}
}

void PreProcessorRawdataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_rawData->loadFromProjectMainFile(node);
}

void PreProcessorRawdataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", m_rawData->typeName());
	m_rawData->saveToProjectMainFile(writer);
}

bool PreProcessorRawdataDataItem::addToolBarButtons(QToolBar* tb)
{
	PreProcessorRawDataGroupDataItem* gItem =
		dynamic_cast<PreProcessorRawDataGroupDataItem*>(parent());

	bool added = gItem->addToolBarButtons(tb);
	QAction* sep = nullptr;
	if (added) {
		sep = tb->addSeparator();
	}
	bool added2 = m_rawData->addToolBarButtons(tb);
	if (! added2 && sep) {
		tb->removeAction(sep);
	}
	return added || added2;
}

void PreProcessorRawdataDataItem::exportRawdata()
{
	QMainWindow* mainW = projectData()->mainWindow();

	const QList<RawDataExporter*>& exps = m_rawData->exporters();
	QStringList filters;
	QList<RawDataExporter*> exporters;
	for (auto exp_it = exps.begin(); exp_it != exps.end(); ++exp_it) {
		RawDataExporter* exp = *exp_it;
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

	RawDataExporter* exporter = nullptr;
	for (int i = 0; i < filters.count(); ++i) {
		if (filters[i] == selectedFilter) {
			exporter = exporters[i];
		}
	}
	Q_ASSERT(exporter != nullptr);

	// execute export.
	exporter->doExport(m_rawData, filename, selectedFilter, mainW, projectData());
	QFileInfo finfo(filename);
	LastIODirectory::set(finfo.absolutePath());
}

void PreProcessorRawdataDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	QStandardItem* parentItem = dynamic_cast<PreProcessorDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}

void PreProcessorRawdataDataItem::handleStandardItemClicked()
{
	m_rawData->handleStandardItemClicked();
}

void PreProcessorRawdataDataItem::handleStandardItemDoubleClicked()
{
	m_rawData->handleStandardItemDoubleClicked();
}

void PreProcessorRawdataDataItem::informSelection(VTKGraphicsView* v)
{
	// delegate to raw data.
	m_rawData->informSelection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::informDeselection(VTKGraphicsView* v)
{
	// delegate to raw data.
	m_rawData->informDeselection(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::viewOperationEnded(VTKGraphicsView* v)
{
	m_rawData->viewOperationEnded(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_rawData->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	m_rawData->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_rawData->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_rawData->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_rawData->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_rawData->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

QStringList PreProcessorRawdataDataItem::containedFiles()
{
	return m_rawData->containedFiles();
}

void PreProcessorRawdataDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_rawData->assignActorZValues(range);
}

void PreProcessorRawdataDataItem::updateZDepthRangeItemCount()
{
	m_rawData->updateZDepthRangeItemCount(m_zDepthRange);
}

void PreProcessorRawdataDataItem::informValueRangeChange()
{
	dynamic_cast<PreProcessorRawDataGroupDataItem*>(parent())->informValueRangeChange();
}

void PreProcessorRawdataDataItem::informDataChange()
{
	dynamic_cast<PreProcessorRawDataGroupDataItem*>(parent())->informDataChange();
}

bool PreProcessorRawdataDataItem::getValueRange(double* min, double* max)
{
	if (m_rawData == nullptr) {return false;}
	return m_rawData->getValueRange(min, max);
}

QDialog* PreProcessorRawdataDataItem::propertyDialog(QWidget* parent)
{
	return m_rawData->propertyDialog(parent);
}

void PreProcessorRawdataDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_rawData->handlePropertyDialogAccepted(propDialog);
	renderGraphicsView();
}

bool PreProcessorRawdataDataItem::setupExportMenu(QMenu* /*menu*/)
{
	bool ok = false;
	/*
			if (m_exportSignalMapper != nullptr){delete m_exportSignalMapper;}
			m_exportSignalMapper = new QSignalMapper(this);
			const QList<RawDataExporter*>& exporters = m_rawData->exporters();
			for (auto exp_it = exporters.begin(); exp_it != exporters.end(); ++exp_it){
					QString title = (*exp_it)->caption();
					QAction* exportAction = menu->addAction(title.append("..."));
					m_exportSignalMapper->setMapping(exportAction, *exp_it);
					connect(exportAction, SIGNAL(triggered()), m_exportSignalMapper, SLOT(map()));
					ok = true;
			}
			connect(m_exportSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(exportRawdata(QObject*)));
	 */
	return ok;
}

bool PreProcessorRawdataDataItem::isExportAvailable()
{
	const QList<RawDataExporter*>& exporters = m_rawData->exporters();
	return exporters.count() > 0;
}

void PreProcessorRawdataDataItem::innerUpdate2Ds()
{
	m_rawData->update2Ds();
}

void PreProcessorRawdataDataItem::saveToCgnsFile(const int fn)
{
	std::string idxstr =  iRIC::toStr(QString::number(m_index));
	cg_user_data_write(idxstr.c_str());
	cg_gorel(fn, idxstr.c_str(), 0, NULL);
	m_rawData->saveToCgnsFile(fn);
	cg_gorel(fn, "..", 0, NULL);
}

void PreProcessorRawdataDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	m_rawData->viewOperationEndedGlobal(dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void PreProcessorRawdataDataItem::doApplyOffset(double x, double y)
{
	m_rawData->applyOffset(x, y);
}
