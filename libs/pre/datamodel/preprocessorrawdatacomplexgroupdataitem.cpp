#include "preprocessorrawdatacomplexgroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include <guicore/pre/complex/gridcomplexconditiondialog.h>
#include <guicore/pre/complex/gridcomplexconditionwidget.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/inputcond/cgnsfileinputconditionwidgetfilename.h>
#include <guicore/pre/gridcond/complex/gridrelatedcomplexconditioneditwidget.h>
#include "preprocessorgridtypedataitem.h"
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include "preprocessorrawdatatopdataitem.h"
#include <guicore/pre/rawdatabackground/rawdatabackgroundcomplex.h>
#include "preprocessorrawdatadataitem.h"
#include "../factory/rawdatafactory.h"
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <rawdata/polygon/rawdatapolygon.h>
#include <guicore/pre/gridcond/complex/gridrelatedcomplexconditioncontainer.h>

#include <misc/lastiodirectory.h>

#include <iriclib.h>

#include <QAction>
#include <QMenu>

PreProcessorRawDataComplexGroupDataItem::PreProcessorRawDataComplexGroupDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent)
	: PreProcessorRawDataGroupDataItem(cond, parent)
{
	m_dialog = new GridComplexConditionDialog(this, iricMainWindow(), mainWindow());
	m_dialog->setWindowTitle(tr("%1 Group Setting").arg(m_condition->caption()));

	m_undefinedColor = Qt::gray;

	m_editGroupAction = new QAction(tr("Edit &Groups..."), this);
	connect(m_editGroupAction, SIGNAL(triggered()), this, SLOT(showEditGroupDialog()));
	createDefaultGroup();
	addBackground();
}

PreProcessorRawDataComplexGroupDataItem::~PreProcessorRawDataComplexGroupDataItem()
{
	if (m_dialog){
		delete m_dialog;
	}
}

void PreProcessorRawDataComplexGroupDataItem::loadFromCgnsFile(const int fn)
{
	int count = 0;
	SolverDefinitionGridRelatedComplexCondition* compCond = 0;
	int defId = -1;

	clear();
	int ret = cg_iRIC_Read_Complex_Count(const_cast<char *>(iRIC::toStr(m_condition->name()).c_str()), &count);
	if (ret != 0){
		goto INITGROUPS;
	}
	compCond = dynamic_cast<SolverDefinitionGridRelatedComplexCondition*> (condition());
	for (int i = 0; i < count; ++i){
		GridComplexConditionWidget* w = new GridComplexConditionWidget(iricMainWindow(), mainWindow());
		w->setup(projectData()->solverDefinition(), compCond->element(), iricMainWindow()->locale());
		w->setNameAndNumber(m_condition->name(), i + 1);
		w->load(fn);
		if (w->isDefault()){defId = i;}
		m_widgets.append(w);
	}
	if (defId == -1 && m_widgets.count() > 0){
		// make the first group default.
		m_widgets[0]->setIsDefault(true);
	}

INITGROUPS:

	if (m_widgets.count() == 0){
		createDefaultGroup();
	}
	updateColorMap();
}

void PreProcessorRawDataComplexGroupDataItem::saveComplexGroupsToCgnsFile(const int fn)
{
	for (int i = 0; i < m_widgets.count(); ++i){
		GridComplexConditionWidget* w = m_widgets.at(i);
		w->setNameAndNumber(m_condition->name(), i + 1);
		w->save(fn);
	}
}

void PreProcessorRawDataComplexGroupDataItem::addCustomMenuItems(QMenu* menu)
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
	menu->addAction(m_editGroupAction);

	menu->addSeparator();
	m_exportAllPolygonsAction->setEnabled(polygonExists());
	menu->addAction(m_exportAllPolygonsAction);
	menu->addSeparator();
	menu->addAction(m_deleteAllAction);

	menu->addSeparator();
	menu->addAction(m_editColorMapAction);
	menu->addAction(m_setupScalarBarAction);
}

void PreProcessorRawDataComplexGroupDataItem::updateColorMap()
{
	PreProcessorGridTypeDataItem* tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* c = tItem->scalarsToColors(m_condition->name());
	ColorTransferFunctionContainer* c2 = dynamic_cast<ColorTransferFunctionContainer*> (c);

	QMap<double, QString> enums;
	QMap<double, QString> englishEnums;
	QMap<double, QColor> colors;

//	colors.insert(0, m_undefinedColor);
//	enums.insert(0, cond->undefinedString());
//	englishEnums.insert(0, cond->undefinedEnglishString());
	for (int i = 0; i < m_widgets.count(); ++i){
		GridComplexConditionWidget* w = m_widgets.at(i);
		double val = i + 1;
		colors.insert(val, w->color());
		enums.insert(val, w->caption());
		englishEnums.insert(val, w->caption());
	}
	c2->setColors(colors);
	c2->setEnumerations(enums);
	c2->setEnglishEnumerations(englishEnums);
	c2->update();

	applySettingsToScalarBar();
}

void PreProcessorRawDataComplexGroupDataItem::showEditGroupDialog()
{
	// set default folder for filename input conditions.
	CgnsFileInputConditionWidgetFilename::defaultFolder = LastIODirectory::get();

	QList<GridComplexConditionWidget::Setting> settings;

	m_dialog->setWidgets(m_widgets);
	for (int i = 0; i < m_widgets.count(); ++i){
		settings.append(m_widgets[i]->setting());
	}

	int ret = m_dialog->exec();
	QList<GridComplexConditionWidget*> widgets = m_dialog->widgets();
	if (ret == QDialog::Rejected){
		for (int i = 0; i < widgets.count(); ++i){
			if (! m_widgets.contains(widgets[i])){
				delete widgets[i];
			}
		}
		for (int i = 0; i < m_widgets.count(); ++i){
			m_widgets[i]->setSetting(settings.at(i));
		}
		return;
	}
	// Apply change values.
	QList<GridComplexConditionWidget*> oldWidgets = m_widgets;
	m_widgets = widgets;

	bool deletedItemExist = false;

	QList<int> valueMap;
	int newindex = 1;
	int newDefault = 0;
	for (int i = 0; i < m_widgets.count(); ++i){
		if (m_widgets[i]->isDefault()){
			newDefault = i + 1;
		}
	}
	// 0 is mapped to new default value
	valueMap.append(newDefault);

	for (int i = 0; i < oldWidgets.count(); ++i){
		if (! m_widgets.contains(oldWidgets[i])){
			deletedItemExist = true;
			delete oldWidgets[i];
			valueMap.append(newDefault);
			-- newindex;
		} else {
			valueMap.append(newindex);
		}
		++ newindex;
	}
	// renumber
	for (int i = 0; i < m_widgets.count(); ++i){
		m_widgets[i]->setNameAndNumber(m_condition->name(), i + 1);
	}

	// modify the rawdata values.
	QList<GraphicsWindowDataItem*>::Iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it)
	{
		PreProcessorRawdataDataItem* di = dynamic_cast<PreProcessorRawdataDataItem*> (*it);
		RawData* rd = di->rawData();
		RawDataPolygon* p = dynamic_cast<RawDataPolygon*>(rd);
		if (p != nullptr){
			int currentVal = p->variantValue().toInt();
			int newVal = valueMap.value(currentVal, newDefault);
			p->setVariantValue(newVal);
		}
	}

	// modify grid related condition values.
	PreProcessorGridTypeDataItem* gtItem =
			dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gtItem->conditions();
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::Iterator ccit;
	for (ccit = conds.begin(); ccit != conds.end(); ++ccit){
		PreProcessorGridAndGridCreatingConditionDataItemInterface* ccItem = *ccit;
		Grid* g = ccItem->gridDataItem()->grid();
		if (g == nullptr){continue;}
		GridRelatedConditionContainer* cont = g->gridRelatedCondition(m_condition->name());
		GridRelatedComplexConditionContainer* cont2 = dynamic_cast<GridRelatedComplexConditionContainer*>(cont);
		for (unsigned int i = 0; i < cont2->dataCount(); ++i){
			int currval = cont2->value(i);
			int newval = valueMap.value(currval, newDefault);
			cont2->setValue(i, newval);
		}
		cont2->dataArray()->Modified();
		g->setModified();
	}

	// set the default folder back.
	LastIODirectory::set(CgnsFileInputConditionWidgetFilename::defaultFolder);

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	for (int i = 0; i < settings.count(); ++i){
		delete settings.at(i).containerSet;
	}

	updateColorMap();
	renderGraphicsView();
}

void PreProcessorRawDataComplexGroupDataItem::clear()
{
	for (int i = 0; i < m_widgets.size(); ++i){
		delete m_widgets.at(i);
	}
	m_widgets.clear();
}

void PreProcessorRawDataComplexGroupDataItem::setupEditWidget(GridRelatedConditionEditWidget* widget)
{
	GridRelatedComplexConditionEditWidget* w = dynamic_cast<GridRelatedComplexConditionEditWidget*>(widget);
	QMap<int, QString> enums;
	int defIndex = 0;
	for (int i = 0; i < m_widgets.count(); ++i){
		enums.insert(i + 1, m_widgets[i]->caption());
		if (m_widgets[i]->isDefault()){
			defIndex = i + 1;
		}
	}
	w->setEnumerations(enums);
	w->setDefaultIndex(defIndex);
}

void PreProcessorRawDataComplexGroupDataItem::editScalarsToColors()
{
	PreProcessorRawDataGroupDataItem::editScalarsToColors();
	applyScalarsToColorsSetting();
	applySettingsToScalarBar();
}

void PreProcessorRawDataComplexGroupDataItem::applyScalarsToColorsSetting()
{
	PreProcessorGridTypeDataItem* tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* c = tItem->scalarsToColors(m_condition->name());
	ColorTransferFunctionContainer* c2 = dynamic_cast<ColorTransferFunctionContainer*> (c);

	QMap<double, QColor> colors = c2->colors();
	for (int i = 0; i < m_widgets.count(); ++i){
		GridComplexConditionWidget* w = m_widgets[i];
		w->setColor(colors.value(i + 1));
	}
}

void PreProcessorRawDataComplexGroupDataItem::applySettingsToScalarBar()
{
	PreProcessorRawDataTopDataItem* tItem = dynamic_cast<PreProcessorRawDataTopDataItem*>(parent());
	tItem->updateLegendBoxItems();
}

void PreProcessorRawDataComplexGroupDataItem::createDefaultGroup()
{
	SolverDefinitionGridRelatedComplexCondition* compCond =
			dynamic_cast<SolverDefinitionGridRelatedComplexCondition*> (condition());
	GridComplexConditionWidget* w = new GridComplexConditionWidget(iricMainWindow(), mainWindow());
	w->setup(projectData()->solverDefinition(), compCond->element(), iricMainWindow()->locale());
	w->setCaption("Default");
	w->setColor(m_undefinedColor);
	w->setIsDefault(true);
	w->setNameAndNumber(m_condition->name(), 1);
	m_widgets.append(w);
}

void PreProcessorRawDataComplexGroupDataItem::addBackground()
{
	delete m_backgroundItem;
	m_childItems.clear();

	// create an instance or RawDataBackground, and add it.
	m_backgroundItem = new PreProcessorRawdataDataItem(this);
	m_childItems.append(m_backgroundItem);
	RawData* rawdata = m_condition->buildBackgroundRawData(m_backgroundItem);
	RawDataBackgroundComplex* r = dynamic_cast<RawDataBackgroundComplex*>(rawdata);
	r->setItem(this);
	m_backgroundItem->setRawData(rawdata);
	rawdata->setupDataItem();
}

SolverDefinitionGridRelatedCondition* PreProcessorRawDataComplexGroupDataItem::condition()
{
	return PreProcessorRawDataGroupDataItem::condition();
}
