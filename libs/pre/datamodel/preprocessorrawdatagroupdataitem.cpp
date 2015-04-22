#include "preprocessorrawdatagroupdataitem.h"
#include "preprocessorrawdatadataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiondimension.h>
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <guicore/pre/rawdata/rawdataimporter.h>
#include <guicore/pre/rawdatabackground/rawdatabackground.h>
#include <guicore/base/iricmainwindowinterface.h>
#include "../factory/rawdatafactory.h"
#include <guicore/pre/rawdata/rawdata.h>
#include <guicore/pre/rawdata/rawdatamapper.h>
#include <rawdata/pointmap/rawdatapointmap.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include "../preprocessordatamodel.h"
#include "../preobjectbrowserview.h"
#include "../preprocessorgraphicsview.h"
#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
#include "../misc/preprocessorscalarbareditdialog.h"
#include "../misc/preprocessorlegendboxeditdialog.h"
#include "preprocessorrawdatatopdataitem.h"
#include <guicore/scalarstocolors/scalarstocolorseditdialog.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <rawdata/polygon/rawdatapolygonrealcreator.h>
#include <rawdata/polygon/rawdatapolygon.h>
#include <rawdata/polygon/rawdatapolygonshapeexporter.h>
#include <guibase/waitdialog.h>
#include <rawdata/riversurvey/rawdatariversurveycrosssectionwindowprojectdataitem.h>
#include <rawdata/riversurvey/rawdatariversurveycrosssectionwindow.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionselectwidget.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>
#include <guicore/project/projectmainfile.h>

#include <QStandardItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QMenu>
#include <QXmlStreamWriter>
#include <QInputDialog>
#include <QtGlobal>
#include <QTextCodec>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QApplication>

#include <cgnslib.h>

PreProcessorRawDataGroupDataItem::PreProcessorRawDataGroupDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent)
	: PreProcessorRawDataGroupDataItemInterface(cond, parent)
{
	m_condition = cond;
	m_subFolder = cond->name();
	m_standardItemCopy = m_standardItem->clone();

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_addSignalMapper = 0;

	m_importAction = new QAction(tr("&Import..."), this);
	m_importAction->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_importAction->setEnabled(importAvailable());

	m_deleteAllAction = new QAction(tr("Delete &All..."), this);
	m_deleteAllAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));

	m_exportAllPolygonsAction = new QAction(tr("Export All Polygons..."), this);
	m_exportAllPolygonsAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));

	m_editColorMapAction = new QAction(tr("&Color Setting..."), this);
	m_editColorMapAction->setIcon(QIcon(":/libs/guibase/images/iconColor.png"));
	m_setupScalarBarAction = new QAction(tr("Set Up Scalarbar..."), this);

	connect(m_importAction, SIGNAL(triggered()), this, SLOT(import()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(m_exportAllPolygonsAction, SIGNAL(triggered()), this, SLOT(exportAllPolygons()));
	connect(this, SIGNAL(selectRawData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(m_editColorMapAction, SIGNAL(triggered()), this, SLOT(editScalarsToColors()));
	connect(m_setupScalarBarAction, SIGNAL(triggered()), dynamic_cast<PreProcessorRawDataTopDataItem*>(this->parent()), SLOT(setupScalarBar()));

	if (m_condition->isOption()){
		m_scalarBarSetting.initForLegendBox();
	}
	// for scalar bar / legend box
	m_title = m_condition->englishCaption();

	// add dimensions container
	m_dimensions = new GridRelatedConditionDimensionsContainer(cond, this);

	// add background data item.
	addBackground();
}

PreProcessorRawDataGroupDataItem::~PreProcessorRawDataGroupDataItem()
{
	delete m_dimensions;
}

void PreProcessorRawDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	RawDataFactory& factory = RawDataFactory::instance();
	// create add menu.
	const QList<RawDataCreator*> creators = factory.compatibleCreators(m_condition);
	m_addMenu = new QMenu(tr("&Add"), menu);

	if (m_addSignalMapper){delete m_addSignalMapper;}
	m_addSignalMapper = new QSignalMapper(this);

	for (auto it = creators.begin(); it != creators.end(); ++it){
		if ((*it)->isCreatable()){
			QString title = (*it)->caption();
			QAction* addAction = m_addMenu->addAction(title.append("..."));
			m_addSignalMapper->setMapping(addAction, *it);
			connect(addAction, SIGNAL(triggered()), m_addSignalMapper, SLOT(map()));
		}
	}
	connect(m_addSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(addRawData(QObject*)));
	menu->addAction(m_importAction);
	if (m_addMenu->actions().count() != 0){
		menu->addMenu(m_addMenu);
	}
	menu->addSeparator();

	m_exportAllPolygonsAction->setEnabled(polygonExists());
	menu->addAction(m_exportAllPolygonsAction);
	menu->addSeparator();
	menu->addAction(m_deleteAllAction);

	menu->addSeparator();
	menu->addAction(m_editColorMapAction);
	menu->addAction(m_setupScalarBarAction);
}

void PreProcessorRawDataGroupDataItem::import()
{
	QStringList filters;
	QList<RawDataImporter*> importers;

	RawDataFactory& factory = RawDataFactory::instance();
	// create add menu.
	const QList<RawDataCreator*> creators = factory.compatibleCreators(m_condition);

	QStringList availableExtensions;
	QMap<QString, RawDataImporter*> extMap;
	for (auto it = creators.begin(); it != creators.end(); ++it){
		const QList<RawDataImporter*>& imps = (*it)->importers();
		for (auto imp_it = imps.begin(); imp_it != imps.end(); ++imp_it){
			QStringList fils = (*imp_it)->fileDialogFilters();
			QStringList exts = (*imp_it)->acceptableExtensions();
			for (auto f_it = fils.begin(); f_it != fils.end(); ++f_it){
				filters.append(*f_it);
				importers.append(*imp_it);
			}
			for (auto f_it = exts.begin(); f_it != exts.end(); ++f_it){
				availableExtensions << QString("*.").append(*f_it);
				extMap.insert(*f_it, *imp_it);
			}
		}
	}
	filters.push_front(QString(tr("All importable files (%1)")).arg(availableExtensions.join(" ")));

	QString dir = LastIODirectory::get();
	QString selectedFilter;
	// Select the file to import.
	QString filename = QFileDialog::getOpenFileName(preProcessorWindow(), tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()){return;}

	RawDataImporter* importer = 0;
	for (int i = 0; i < filters.count(); ++i){
		if (filters[i] == selectedFilter){
			if (i == 0){
				QFileInfo finfo(filename);
				QString extension = finfo.suffix();
				if (extMap.contains(extension)){
					importer = extMap.value(extension);
				} else {
					QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("File type can not be recognized from the file extension. : %1").arg(finfo.fileName()));
					return;
				}
			} else {
				importer = importers[i - 1];
			}
		}
	}
	Q_ASSERT(importer != 0);

	// execute import.
	int dataCount;
	QWidget* w = preProcessorWindow();
	bool ret = importer->importInit(filename, selectedFilter, &dataCount, m_condition, this, w);
	if (! ret){
		QMessageBox::warning(preProcessorWindow(), tr("Import failed"), tr("Importing data from %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return;
	}
	PreProcessorRawdataDataItem* item = 0;

	WaitDialog* wDialog = 0;
	m_cancelImport = false;
	if (dataCount >= 5) {
		wDialog = new WaitDialog(mainWindow());
		wDialog->setRange(0, dataCount - 1);
		wDialog->setProgress(0);
		wDialog->setMessage(tr("Importing data..."));
		wDialog->showProgressBar();
		connect(wDialog, SIGNAL(canceled()), this, SLOT(cancelImport()));
		wDialog->show();
	}
	// All imports succeeded.
	QFileInfo finfo(filename);
	for (int i = 0; i < dataCount; ++i) {
		item = new PreProcessorRawdataDataItem(this);
		// first, create an empty rawdata.
		RawData* rawdata = importer->creator()->create(item, m_condition);
		item->setRawData(rawdata);
		// set name and caption
		importer->creator()->setNameAndDefaultCaption(this->childItems(), rawdata);
		rawdata->setupDataItem();
		if (rawdata->requestCoordinateSystem()){
			if (projectData()->mainfile()->coordinateSystem() == 0){
				QMessageBox::information(preProcessorWindow(), tr("Information"), tr("To import the geographic data, specify coodrinate system first."), QMessageBox::Ok);
				int dialogRet = projectData()->mainfile()->showCoordinateSystemDialog();
				if (dialogRet == QDialog::Rejected){
					delete item;
					item = 0;
					goto ERROR;
				}
			}
		}
		// import data from the specified file
		bool ret = importer->importData(rawdata, i, w);
		if (! ret){
			// failed.
			QMessageBox::warning(preProcessorWindow(), tr("Import failed"), tr("Importing data from %1 failed.").arg(QDir::toNativeSeparators(filename)));
			delete item;
			item = 0;
			goto ERROR;
		} else {
			QVector2D o = offset();
			rawdata->applyOffset(o.x(), o.y());
			// the standarditem is set at the last position, so make it the first.
			QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
			m_standardItem->insertRows(0, takenItems);
			// add the item, in the front.
			m_childItems.push_front(item);
		}
		if (wDialog != 0){
			wDialog->setProgress(i + 1);
		}
		setupConnectionToRawData(rawdata);
	}
	if (wDialog != 0){
		wDialog->hide();
		delete wDialog;
	}
	// All imports succeeded.
	LastIODirectory::set(finfo.absolutePath());

	updateItemMap();
	updateZDepthRange();

	informValueRangeChange();
	informDataChange();

	dataModel()->fit();
	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectRawData(item->standardItem()->index());
	setModified();

	// import is not undo-able.
	iRICUndoStack::instance().clear();
	return;

ERROR:
	if (wDialog != 0){
		wDialog->hide();
		delete wDialog;
	}
	updateItemMap();
	updateZDepthRange();

	informValueRangeChange();
	informDataChange();

	dataModel()->fit();
	dataModel()->graphicsView()->ResetCameraClippingRange();
	setModified();

	// import is not undo-able.
	iRICUndoStack::instance().clear();
	return;
}

void PreProcessorRawDataGroupDataItem::doExport()
{
	QStringList dataNames;
	QList<PreProcessorRawdataDataItem*> datas;

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
		if (item->isExportAvailable()){
			datas.append(item);
			dataNames.append(item->rawData()->caption());
		}
	}
	if (datas.count() == 0){
		// no data to export.
		return;
	} else if (datas.count() == 1){
		// there is only one data to export.
		PreProcessorRawdataDataItem* item = datas.at(0);
		item->exportRawdata();
	} else{
		// there are multiple data to export.
		// ask user to select which data to export.
		bool ok;
		QString selectedName = QInputDialog::getItem(preProcessorWindow(), tr("Select data to export"), tr("Please select which data to export."), dataNames, 0, false, &ok);
		if (! ok){
			// User canceled.
			return;
		}
		int index = dataNames.indexOf(selectedName);
		Q_ASSERT(index >= 0 && index < dataNames.count());
		PreProcessorRawdataDataItem* item = datas.at(index);
		item->exportRawdata();
	}
}

void PreProcessorRawDataGroupDataItem::addRawData(QObject* c)
{
	RawDataCreator* creator = dynamic_cast<RawDataCreator*>(c);
	// create a new Rawdata item.
	PreProcessorRawdataDataItem* item = new PreProcessorRawdataDataItem(this);
	// the standarditem is set at the last position, so make it the first.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	// add the item, in the front.
	m_childItems.push_front(item);
	// create an empty rawdata.
	RawData* rawdata = creator->create(item, m_condition);
	item->setRawData(rawdata);
	setupConnectionToRawData(rawdata);
	// set name and caption
	creator->setNameAndDefaultCaption(this->childItems(), rawdata);
	rawdata->setupDataItem();

	// the background item should be at the last always.
	// update item map.
	updateItemMap();
	// update ZDepthRange
	updateZDepthRange();
	informDataChange();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectRawData(item->standardItem()->index());
	rawdata->showInitialDialog();
	setModified();
}

void PreProcessorRawDataGroupDataItem::addBackground()
{
	// create an instance or RawDataBackground, and add it.
	m_backgroundItem = new PreProcessorRawdataDataItem(this);
	m_childItems.append(m_backgroundItem);
	RawData* rawdata = m_condition->buildBackgroundRawData(m_backgroundItem);
	setupConnectionToRawData(rawdata);
	m_backgroundItem->setRawData(rawdata);
	rawdata->setupDataItem();
}

void PreProcessorRawDataGroupDataItem::moveBackgroundToLast()
{
	// make m_backgroundItem the last item in m_childItems.
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		if (*it == m_backgroundItem){
			m_childItems.erase(it);
			break;
		}
	}
	m_childItems.push_back(m_backgroundItem);

	// QStandardItem order should be reordered manually.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(m_backgroundItem->standardItem()->row());
	// in deed, takenItems containes only one item, and that is background item.
	m_standardItem->appendRows(takenItems);
}

void PreProcessorRawDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	RawDataFactory& factory = RawDataFactory::instance();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count() - 1; ++i){
		PreProcessorRawdataDataItem* item = new PreProcessorRawdataDataItem(this);
		QDomNode child = children.at(i);
		// restore
		RawData* rawdata = factory.restore(child, item, m_condition);
		if (rawdata != 0){
			item->setRawData(rawdata);
			// load data.
			item->loadFromProjectMainFile(child);
			rawdata->setupDataItem();
			item->loadCheckState(child);
			// insert the new item BEFORE the background item.
			int last = m_childItems.count() - 1;
			m_childItems.insert(last, item);
		}else{
			// for some reason, it could not be restored.
			delete item;
		}
	}
	// The last node must be the background item.
	QDomNode child = children.at(children.count() - 1);
	m_backgroundItem->loadFromProjectMainFile(child);

	// the background item should be at the last always.
//	moveBackgroundToLast();
	informValueRangeChange();
}

void PreProcessorRawDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name());
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		writer.writeStartElement("RawData");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

bool PreProcessorRawDataGroupDataItem::isChildCaptionAvailable(const QString& caption)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		if (dynamic_cast<PreProcessorRawdataDataItem*>(*it)->rawData()->caption() == caption){
			return false;
		}
	}
	return true;
}

int PreProcessorRawDataGroupDataItem::mappingCount() const
{
	int dimCount = 1;
	if (dimensions()->containers().count() > 0){
		dimCount = dimensions()->maxIndex() + 1;
	}
	int rawdataCount = m_childItems.count();
	return dimCount * rawdataCount;
}

void PreProcessorRawDataGroupDataItem::executeMapping(Grid* grid, WaitDialog *dialog)
{
	/*
	if (m_childItems.count() == 1){
		// There is only background raw data.
		// No need to execute mapping.
		return;
	}
	*/
	GridRelatedConditionContainer* container = grid->gridRelatedCondition(m_condition->name());
	bool* boolMap;
	int dataCount = container->dataCount();
	boolMap = new bool[dataCount];
	QList<RawDataMapperSetting*> settings;
	if (dimensions()->containers().count() == 0){
		QList <GraphicsWindowDataItem*>::iterator it;
		// initialize

		// reset the boolmap.
		for (int i = 0; i < dataCount; ++i){
			*(boolMap + i) = false;
		}
		for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
			PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
			RawData* rawdata = item->rawData();
			RawDataMapper* mapper = rawdata->mapper();
			mapper->setTarget(grid, container, rawdata);
			settings.append(mapper->initialize(boolMap));
		}
		// do mapping

		// reset the boolmap.
		for (int i = 0; i < dataCount; ++i){
			*(boolMap + i) = false;
		}
		int idx = 0;
		for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
			PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
			RawData* rawdata = item->rawData();
			RawDataMapper* mapper = rawdata->mapper();
			mapper->setTarget(grid, container, rawdata);
			mapper->map(boolMap, settings.at(idx));
			++idx;
			if (dialog != 0){
				dialog->setProgress(dialog->progress() + 1);
				qApp->processEvents();
			}
		}
		// terminate
		idx = 0;
		for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
			PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
			RawData* rawdata = item->rawData();
			RawDataMapper* mapper = rawdata->mapper();
			mapper->terminate(settings.at(idx));
			rawdata->setMapped();
			++idx;
		}
	} else {
		int currentIndex = dimensions()->currentIndex();
		QList <GraphicsWindowDataItem*>::iterator it;
		// initialize

		for (int i = 0; i < dataCount; ++i){
			*(boolMap + i) = false;
		}
		for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
			PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
			RawData* rawdata = item->rawData();
			RawDataMapper* mapper = rawdata->mapper();
			mapper->setTarget(grid, container, rawdata);
			settings.append(mapper->initialize(boolMap));
		}
		// do mapping
		for (int i = 0; i <= dimensions()->maxIndex(); ++i){
			dimensions()->setCurrentIndex(i, true);
			for (int i = 0; i < dataCount; ++i){
				*(boolMap + i) = false;
			}
			int idx = 0;
			for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
				PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
				RawData* rawdata = item->rawData();
				RawDataMapper* mapper = rawdata->mapper();
				mapper->setTarget(grid, container, rawdata);
				mapper->map(boolMap, settings.at(idx));
				++idx;
				if (dialog != 0){
					dialog->setProgress(dialog->progress() + 1);
					qApp->processEvents();
				}
			}
		}
		// terminate
		int idx = 0;
		for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
			PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
			RawData* rawdata = item->rawData();
			RawDataMapper* mapper = rawdata->mapper();
			mapper->terminate(settings.at(idx));
			rawdata->setMapped();
			++idx;
		}
		dimensions()->setCurrentIndex(currentIndex);
	}
	container->setMapped(true);
	delete boolMap;
}

void PreProcessorRawDataGroupDataItem::setDefaultValue(Grid* grid)
{
	GridRelatedConditionContainer* container = grid->gridRelatedCondition(m_condition->name());
	bool* boolMap;
	int dataCount = container->dataCount();
	boolMap = new bool[dataCount];
	// reset the boolmap.
	// do mapping only by the backgroud item.
	RawDataMapper* mapper = m_backgroundItem->rawData()->mapper();
	mapper->setTarget(grid, container, m_backgroundItem->rawData());
	for (int i = 0; i < dataCount; ++i){
		*(boolMap + i) = false;
	}
	RawDataMapperSetting* s = mapper->initialize(boolMap);
	for (int i = 0; i < dataCount; ++i){
		*(boolMap + i) = false;
	}
	mapper->map(boolMap, s);
	mapper->terminate(s);
	delete boolMap;
}

void PreProcessorRawDataGroupDataItem::informValueRangeChange()
{
	PreProcessorRawDataTopDataItem* topItem = dynamic_cast<PreProcessorRawDataTopDataItem*>(parent());
	if (topItem != 0){
		topItem->informValueRangeChange(m_condition->name());
	}
}

void PreProcessorRawDataGroupDataItem::informDataChange()
{
	dynamic_cast<PreProcessorRawDataTopDataItem*>(parent())->informDataChange();
	updateCrossectionWindows();
}

bool PreProcessorRawDataGroupDataItem::getValueRange(double* min, double* max)
{
	*min = 0;
	*max = 0;
	bool first = true;
	bool result = false;
	QList <GraphicsWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
		// background item is not used for this.
		if (m_backgroundItem == item){continue;}
		double tmpmin;
		double tmpmax;
		if (item->getValueRange(&tmpmin, &tmpmax)){
			if (first || tmpmin < *min){*min = tmpmin;}
			if (first || tmpmax > *max){*max = tmpmax;}
			result = true;
			first = false;
		}
	}
	return result;
}

/*
	 bool PreProcessorRawDataGroupDataItem::setupImportMenu(QMenu* menu)
	 {
		bool ok = false;
		RawDataFactory& factory = RawDataFactory::instance();
		const QList<RawDataCreator*> creators = factory.compatibleCreators(m_condition);

		if (m_importSignalMapper){delete m_importSignalMapper;}
		m_importSignalMapper = new QSignalMapper(this);
		QList<RawDataCreator*>::const_iterator it;
		for (it = creators.begin(); it != creators.end(); ++it){
				const QList<RawDataImporter*>& importers = (*it)->importers();
				QList<RawDataImporter*>::const_iterator imp_it;
				for (imp_it = importers.begin(); imp_it != importers.end(); ++imp_it){
						QString title = (*imp_it)->caption();
						QAction* importAction = menu->addAction(title.append("..."));
						m_importSignalMapper->setMapping(importAction, *imp_it);
						connect(importAction, SIGNAL(triggered()), m_importSignalMapper, SLOT(map()));
						ok = true;
				}
		}
		connect(m_importSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(importRawdata(QObject*)));
		return ok;
	 }
 */

const QList<PreProcessorRawdataDataItemInterface*> PreProcessorRawDataGroupDataItem::rawDatas() const
{
	QList<PreProcessorRawdataDataItemInterface*> ret;
	QList <GraphicsWindowDataItem*>::const_iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(*it);
		ret.append(item);
	}
	return ret;
}

void PreProcessorRawDataGroupDataItem::editScalarsToColors()
{
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(condition()->name());
	ScalarsToColorsEditDialog* dialog = condition()->createScalarsToColorsEditDialog(preProcessorWindow());
	dialog->setWindowTitle(tr("%1 Color Setting").arg(m_condition->caption()));
	dialog->setContainer(stc);

	// @todo this operation is not made undo-able yet.
	int ret = dialog->exec();
	if (ret != QDialog::Accepted){return;}
	renderGraphicsView();
}

void PreProcessorRawDataGroupDataItem::editScalarBarLegendBox(PreProcessorScalarBarLegendBoxSettingDialog* dialog)
{
	if (m_condition->isOption()){
		// discrete
		PreProcessorLegendBoxEditDialog* legendboxDialog = new PreProcessorLegendBoxEditDialog(dialog);
		legendboxDialog->setWidth(m_scalarBarSetting.width);
		legendboxDialog->setHeight(m_scalarBarSetting.height);
		legendboxDialog->setPositionX(m_scalarBarSetting.positionX);
		legendboxDialog->setPositionY(m_scalarBarSetting.positionY);
		legendboxDialog->setEntryTextSetting(dialog->labelTextSetting());
		if (legendboxDialog->exec() == QDialog::Accepted){
			m_scalarBarSetting.width = legendboxDialog->width();
			m_scalarBarSetting.height = legendboxDialog->height();
			m_scalarBarSetting.positionX = legendboxDialog->positionX();
			m_scalarBarSetting.positionY = legendboxDialog->positionY();
			dialog->setLabelTextSetting(legendboxDialog->entryTextSetting());
		}
		delete legendboxDialog;
	} else {
		// continuous
		PreProcessorScalarBarEditDialog* scalarbarDialog = new PreProcessorScalarBarEditDialog(dialog);
		scalarbarDialog->hideDisplayCheckBox();
		scalarbarDialog->setOrientation(m_scalarBarSetting.orientation);
		scalarbarDialog->setScalarBarTitle(m_title);
		scalarbarDialog->setNumberOfLabels(m_scalarBarSetting.numberOfLabels);
		scalarbarDialog->setWidth(m_scalarBarSetting.width);
		scalarbarDialog->setHeight(m_scalarBarSetting.height);
		scalarbarDialog->setPositionX(m_scalarBarSetting.positionX);
		scalarbarDialog->setPositionY(m_scalarBarSetting.positionY);
		scalarbarDialog->setTitleTextSetting(dialog->titleTextSetting());
		scalarbarDialog->setLabelTextSetting(dialog->labelTextSetting());
		scalarbarDialog->setLabelFormat(m_scalarBarSetting.labelFormat);

		if (scalarbarDialog->exec() == QDialog::Accepted){
			m_scalarBarSetting.orientation = scalarbarDialog->orientation();
			m_title = scalarbarDialog->scalarBarTitle();
			m_scalarBarSetting.numberOfLabels = scalarbarDialog->numberOfLabels();
			m_scalarBarSetting.width = scalarbarDialog->width();
			m_scalarBarSetting.height = scalarbarDialog->height();
			m_scalarBarSetting.positionX = scalarbarDialog->positionX();
			m_scalarBarSetting.positionY = scalarbarDialog->positionY();
			dialog->setTitleTextSetting(scalarbarDialog->titleTextSetting());
			dialog->setLabelTextSetting(scalarbarDialog->labelTextSetting());
			m_scalarBarSetting.labelFormat = scalarbarDialog->labelFormat();
		}
		delete scalarbarDialog;
	}
	renderGraphicsView();
}

void PreProcessorRawDataGroupDataItem::addImportAction(QMenu* menu)
{
	QString cap = m_condition->caption();
	cap.append("...");
	QAction* a = new QAction(cap, menu);
	a->setEnabled(importAvailable());
	menu->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(import()));
}

bool PreProcessorRawDataGroupDataItem::importAvailable()
{
	RawDataFactory& factory = RawDataFactory::instance();
	// create add menu.
	const QList<RawDataCreator*> creators = factory.compatibleCreators(m_condition);
	QList<RawDataCreator*>::const_iterator it;
	for (it = creators.begin(); it != creators.end(); ++it){
		const QList<RawDataImporter*>& imps = (*it)->importers();
		if (imps.count() > 0){return true;}
	}
	return false;
}

QStringList PreProcessorRawDataGroupDataItem::getRawDatasNotMapped()
{
	QStringList ret;
	QList <GraphicsWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
		RawData* rawData = item->rawData();
		if (! rawData->isMapped()){
			ret.append(rawData->caption());
		}
	}
	return ret;
}

void PreProcessorRawDataGroupDataItem::addCopyPolygon(RawDataPolygon* polygon)
{
	RawDataFactory& factory = RawDataFactory::instance();
	// create add menu.
	const QList<RawDataCreator*> creators = factory.compatibleCreators(m_condition);
	RawDataPolygonCreator* c = 0;
	for (int i = 0; i < creators.count(); ++i){
		c = dynamic_cast<RawDataPolygonCreator*>(creators[i]);
		if (c != 0){break;}
	}
	if (c == 0){return;}

	// create a new Rawdata item.
	PreProcessorRawdataDataItem* item = new PreProcessorRawdataDataItem(this);
	// the standarditem is set at the last position, so make it the first.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	// add the item, in the front.
	m_childItems.push_front(item);
	// create an empty rawdata.
	RawData* rawdata = c->create(item, m_condition);
	item->setRawData(rawdata);
	// set name and caption
	c->setNameAndDefaultCaption(this->childItems(), rawdata);
	rawdata->setupDataItem();

	// now copy the grid shape.
	RawDataPolygon* newpol = dynamic_cast<RawDataPolygon*>(rawdata);
	newpol->copyShape(polygon);
	setupConnectionToRawData(newpol);

	// the background item should be at the last always.
	// update item map.
	updateItemMap();
	// update ZDepthRange
	updateZDepthRange();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectRawData(item->standardItem()->index());
	setModified();

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

void PreProcessorRawDataGroupDataItem::informSelection(VTKGraphicsView * v)
{
	PreProcessorRawDataTopDataItem* tItem = dynamic_cast<PreProcessorRawDataTopDataItem*>(parent());
	tItem->informSelection(v);
}

void PreProcessorRawDataGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	PreProcessorRawDataTopDataItem* tItem = dynamic_cast<PreProcessorRawDataTopDataItem*>(parent());
	tItem->informDeselection(v);
}

void PreProcessorRawDataGroupDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyPressEvent(event);
}

void PreProcessorRawDataGroupDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyReleaseEvent(event);
}

void PreProcessorRawDataGroupDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseDoubleClickEvent(event);
}

void PreProcessorRawDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void PreProcessorRawDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void PreProcessorRawDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void PreProcessorRawDataGroupDataItem::exportAllPolygons()
{
	QString filename = QFileDialog::getSaveFileName(mainWindow(), tr("Export Polygons"), LastIODirectory::get(), tr("ESRI Shape file (*.shp)"));
	if (filename.isEmpty()){return;}
	QString dbfFilename;
	RawDataPolygonRealCreator* c = new RawDataPolygonRealCreator();
	RawDataPolygonShapeExporter* exporter = 0;
	for (int i = 0; i < c->exporters().count(); ++i){
		exporter = dynamic_cast<RawDataPolygonShapeExporter*>(c->exporters().at(i));
		if (exporter != 0){break;}
	}
	if (exporter == 0){goto ERROR;}

	bool isDouble;
	dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	SHPHandle shpHandle = exporter->getSHPHandle(filename);
	DBFHandle dbfHandle = exporter->getDBFHandle(dbfFilename, condition(), &isDouble);

	QTextCodec* codec = QTextCodec::codecForLocale();
	bool codecOK = true;
	for (int i = 0; i < m_childItems.count(); ++i){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*> (m_childItems.at(i));
		RawData* rd = item->rawData();
		if (dynamic_cast<RawDataPolygon*>(rd) == 0){continue;}
		codecOK = codecOK && codec->canEncode(rd->caption());
	}
	if (! codecOK){
		codec = QTextCodec::codecForName("UTF-8");
	}

	int index = 0;
	for (int i = m_childItems.count() - 1; i >= 0; --i){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*> (m_childItems.at(i));
		RawData* rd = item->rawData();
		RawDataPolygon* rdp = dynamic_cast<RawDataPolygon*>(rd);
		if ((rdp) == 0){continue;}
		SHPObject* obj = exporter->getSHPObject(rdp, shpHandle, index);
		SHPWriteObject(shpHandle, -1, obj);
		SHPDestroyObject(obj);

		exporter->outputAttributes(rdp, dbfHandle, index, isDouble, codec);
		++ index;
	}
	SHPClose(shpHandle);
	DBFClose(dbfHandle);
	return;

ERROR:
	delete c;
}

void PreProcessorRawDataGroupDataItem::deleteAll()
{
	if (QMessageBox::No == QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all items in %1 group?").arg(standardItem()->text()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
	{
		return;
	}
	for (int i = m_childItems.count() - 1; i >= 0; --i){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*> (m_childItems.at(i));
		RawData* rd = item->rawData();
		if (dynamic_cast<RawDataBackground*>(rd) != 0){continue;}

		QModelIndex index = item->standardItem()->index();
		dataModel()->deleteItem(index);
	}
}

bool PreProcessorRawDataGroupDataItem::polygonExists() const
{
	bool ret = false;
	for (int i = 0; i < m_childItems.count(); ++i){
		PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*> (m_childItems.at(i));
		RawData* rd = item->rawData();
		ret = ret || (dynamic_cast<RawDataPolygon*>(rd) != 0);
	}
	return ret;
}

void PreProcessorRawDataGroupDataItem::saveToCgnsFile(const int fn)
{
	int index = 1;
	cg_user_data_write(iRIC::toStr(m_condition->name()).c_str());
	cg_gorel(fn, iRIC::toStr(m_condition->name()).c_str(), 0, NULL);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawdataDataItem* ritem = dynamic_cast<PreProcessorRawdataDataItem*>(*it);
		ritem->setIndex(index);
		ritem->saveToCgnsFile(fn);
		++ index;
	}
	cg_gorel(fn, "..", 0, NULL);
}

void PreProcessorRawDataGroupDataItem::openCrossSectionWindow(RawDataRiverSurvey* rs, double crosssection)
{
	RawDataRiverSurveyCrosssectionWindowProjectDataItem* i = new RawDataRiverSurveyCrosssectionWindowProjectDataItem(this, preProcessorWindow());
	i->window()->setRiverSurvey(rs);
	i->window()->setCrosssection(crosssection);
	m_crosssectionWindows.append(i);
	QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QMdiSubWindow* container = cent->addSubWindow(i->window());
	container->setWindowIcon(i->window()->icon());
	container->show();
	container->setFocus();
	i->window()->cameraFit();
}

void PreProcessorRawDataGroupDataItem::updateCrossectionWindows()
{
	QList<RawDataRiverSurveyCrosssectionWindowProjectDataItem*>::iterator w_it;
	for (w_it = m_crosssectionWindows.begin(); w_it != m_crosssectionWindows.end(); ++w_it){
		RawDataRiverSurveyCrosssectionWindow* w = (*w_it)->window();
		w->updateRiverSurveys();
	}
}

void PreProcessorRawDataGroupDataItem::toggleCrosssectionWindowsGridCreatingMode(bool gridMode, RawDataRiverSurvey* rs)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it){
		RawDataRiverSurveyCrosssectionWindow* w = (*it)->window();
		w->toggleGridCreatingMode(gridMode, rs);
	}
}

void PreProcessorRawDataGroupDataItem::informCtrlPointUpdateToCrosssectionWindows()
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it){
		RawDataRiverSurveyCrosssectionWindow* w = (*it)->window();
		w->update();
	}
}

void PreProcessorRawDataGroupDataItem::requestCrosssectionWindowDelete(RawDataRiverSurveyCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it){
		if (*it == item){
			m_crosssectionWindows.erase(it);
			delete item;
			return;
		}
	}
}

bool PreProcessorRawDataGroupDataItem::addToolBarButtons(QToolBar* parent)
{
	if (m_dimensions->selectWidgets().size() == 0){return false;}
	for (int i = 0; i < m_dimensions->selectWidgets().size(); ++i){
		GridRelatedConditionDimensionSelectWidget* w = m_dimensions->selectWidgets().at(i);
		QAction* action = parent->addWidget(w);
		action->setVisible(true);
	}
	return true;
}

void PreProcessorRawDataGroupDataItem::loadExternalData(const QString& /*filename*/)
{
	const QList<GridRelatedConditionDimensionContainer*>& conts = m_dimensions->containers();
	QDir subDir(subPath());
	for (int i = 0; i < conts.size(); ++i){
		GridRelatedConditionDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		cont->loadFromExternalFile(subDir.absoluteFilePath(fileName));
	}
}

void PreProcessorRawDataGroupDataItem::saveExternalData(const QString& /*filename*/)
{
	const QList<GridRelatedConditionDimensionContainer*>& conts = m_dimensions->containers();
	QDir subDir(subPath());
	for (int i = 0; i < conts.size(); ++i){
		GridRelatedConditionDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		cont->saveToExternalFile(subDir.absoluteFilePath(fileName));
	}
}

QStringList PreProcessorRawDataGroupDataItem::containedFiles()
{
	QStringList ret = PreProcessorDataItem::containedFiles();
	const QList<GridRelatedConditionDimensionContainer*>& conts = m_dimensions->containers();
	for (int i = 0; i < conts.size(); ++i){
		GridRelatedConditionDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		ret.append(fileName);
	}
	return ret;
}

void PreProcessorRawDataGroupDataItem::setupConnectionToRawData(RawData* rawdata)
{
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	connect(dims, SIGNAL(currentIndexChanged(int,int)), rawdata, SLOT(handleDimensionCurrentIndexChange(int,int)));
	for (int i = 0; i < dims->containers().size(); ++i){
		GridRelatedConditionDimensionContainer* cont = dims->containers().at(i);
		connect(cont, SIGNAL(valuesChanged(QList<QVariant>,QList<QVariant>)), rawdata, SLOT(handleDimensionValuesChange(QList<QVariant>,QList<QVariant>)));
	}
}
