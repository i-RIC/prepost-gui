#include "../factory/geodatafactory.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatadataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/complex/gridcomplexconditiondialog.h>
#include <guicore/pre/complex/gridcomplexconditionwidget.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributeeditwidget.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodatabackground/geodatabackgroundcomplex.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <geodata/polygon/geodatapolygon.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>

#include <QAction>
#include <QLocale>
#include <QMenu>

#include <iriclib.h>

PreProcessorGeoDataComplexGroupDataItem::PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorGeoDataGroupDataItem {cond, parent},
	m_undefinedColor {Qt::gray}
{
	m_dialog = new GridComplexConditionDialog(this, iricMainWindow(), mainWindow());
	m_dialog->setWindowTitle(PreProcessorGeoDataComplexGroupDataItem::tr("%1 Group Setting").arg(m_condition->caption()));

	m_editGroupAction = new QAction(PreProcessorGeoDataComplexGroupDataItem::tr("Edit &Groups..."), this);
	connect(m_editGroupAction, SIGNAL(triggered()), this, SLOT(showEditGroupDialog()));
	createDefaultGroup();
	addBackground();
}

PreProcessorGeoDataComplexGroupDataItem::~PreProcessorGeoDataComplexGroupDataItem()
{
	delete m_dialog;
}

void PreProcessorGeoDataComplexGroupDataItem::loadFromCgnsFile(const int fn)
{
	int count = 0;
	SolverDefinitionGridComplexAttribute* compCond = 0;
	int defId = -1;

	clear();
	int ret = cg_iRIC_Read_Complex_Count(const_cast<char*>(m_condition->name().c_str()), &count);
	if (ret != 0) {
		goto INITGROUPS;
	}
	compCond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(condition());
	for (int i = 0; i < count; ++i) {
		GridComplexConditionWidget* w = new GridComplexConditionWidget(iricMainWindow(), mainWindow());
		w->setup(projectData()->solverDefinition(), compCond->element(), iricMainWindow()->locale());
		w->setNameAndNumber(m_condition->name(), i + 1);
		w->load(fn);
		if (w->isDefault()) {defId = i;}
		m_widgets.append(w);
	}
	if (defId == -1 && m_widgets.count() > 0) {
		// make the first group default.
		m_widgets[0]->setIsDefault(true);
	}

INITGROUPS:

	if (m_widgets.count() == 0) {
		createDefaultGroup();
	}
	updateColorMap();
}

void PreProcessorGeoDataComplexGroupDataItem::saveComplexGroupsToCgnsFile(const int fn)
{
	for (int i = 0; i < m_widgets.count(); ++i) {
		GridComplexConditionWidget* w = m_widgets.at(i);
		w->setNameAndNumber(m_condition->name(), i + 1);
		w->save(fn);
	}
}

void PreProcessorGeoDataComplexGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	m_addMenu = new QMenu(tr("&Add"), menu);

	if (m_addSignalMapper) {delete m_addSignalMapper;}
	m_addSignalMapper = new QSignalMapper(this);

	GeoDataFactory& factory = GeoDataFactory::instance();
	for (auto creator : factory.compatibleCreators(m_condition)) {
		if (creator->isCreatable()) {
			QString title = creator->caption();
			QAction* addAction = m_addMenu->addAction(title.append("..."));
			m_addSignalMapper->setMapping(addAction, creator);
			connect(addAction, SIGNAL(triggered()), m_addSignalMapper, SLOT(map()));
		}
	}
	connect(m_addSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(addGeoData(QObject*)));
	menu->addAction(m_importAction);
	if (m_addMenu->actions().count() != 0) {
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

void PreProcessorGeoDataComplexGroupDataItem::updateColorMap()
{
	PreProcessorGridTypeDataItem* tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* c = tItem->scalarsToColors(m_condition->name());
	ColorTransferFunctionContainer* c2 = dynamic_cast<ColorTransferFunctionContainer*>(c);

	QMap<double, QString> enums;
	QMap<double, QString> englishEnums;
	QMap<double, QColor> colors;

	for (int i = 0; i < m_widgets.count(); ++i) {
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

void PreProcessorGeoDataComplexGroupDataItem::showEditGroupDialog()
{
	// set default folder for filename input conditions.
	InputConditionWidgetFilename::defaultFolder = LastIODirectory::get();

	QList<GridComplexConditionWidget::Setting> settings;

	m_dialog->setWidgets(m_widgets);
	for (int i = 0; i < m_widgets.count(); ++i) {
		settings.append(m_widgets[i]->setting());
	}

	int ret = m_dialog->exec();
	QList<GridComplexConditionWidget*> widgets = m_dialog->widgets();
	if (ret == QDialog::Rejected) {
		for (int i = 0; i < widgets.count(); ++i) {
			if (! m_widgets.contains(widgets[i])) {
				delete widgets[i];
			}
		}
		for (int i = 0; i < m_widgets.count(); ++i) {
			m_widgets[i]->setSetting(settings.at(i));
		}
		return;
	}
	// Apply change values.
	QList<GridComplexConditionWidget*> oldWidgets = m_widgets;
	m_widgets = widgets;

	QList<int> valueMap;
	int newindex = 1;
	int newDefault = 0;
	for (int i = 0; i < m_widgets.count(); ++i) {
		if (m_widgets[i]->isDefault()) {
			newDefault = i + 1;
		}
	}
	// 0 is mapped to new default value
	valueMap.append(newDefault);

	for (int i = 0; i < oldWidgets.count(); ++i) {
		if (! m_widgets.contains(oldWidgets[i])) {
			delete oldWidgets[i];
			valueMap.append(newDefault);
			-- newindex;
		} else {
			valueMap.append(newindex);
		}
		++ newindex;
	}
	// renumber
	for (int i = 0; i < m_widgets.count(); ++i) {
		m_widgets[i]->setNameAndNumber(m_condition->name(), i + 1);
	}

	// modify the geodata values.
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* di = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		GeoData* rd = di->geoData();
		GeoDataPolygon* p = dynamic_cast<GeoDataPolygon*>(rd);
		if (p != nullptr) {
			int currentVal = p->variantValue().toInt();
			int newVal = valueMap.value(currentVal, newDefault);
			p->setVariantValue(newVal);
		}
	}

	// modify grid related condition values.
	PreProcessorGridTypeDataItem* gtItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	for (auto ccItem : gtItem->conditions()) {
		Grid* g = ccItem->gridDataItem()->grid();
		if (g == nullptr) {continue;}
		GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
		auto cont2 = dynamic_cast<GridComplexAttributeContainer*>(cont);
		for (unsigned int i = 0; i < cont2->dataCount(); ++i) {
			int currval = cont2->value(i);
			int newval = valueMap.value(currval, newDefault);
			cont2->setValue(i, newval);
		}
		cont2->dataArray()->Modified();
		g->setModified();
	}

	// set the default folder back.
	LastIODirectory::set(InputConditionWidgetFilename::defaultFolder);

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	for (int i = 0; i < settings.count(); ++i) {
		delete settings.at(i).containerSet;
	}

	updateColorMap();
	renderGraphicsView();
}

void PreProcessorGeoDataComplexGroupDataItem::clear()
{
	for (int i = 0; i < m_widgets.size(); ++i) {
		delete m_widgets.at(i);
	}
	m_widgets.clear();
}

void PreProcessorGeoDataComplexGroupDataItem::setupEditWidget(GridAttributeEditWidget* widget)
{
	GridComplexAttributeEditWidget* w = dynamic_cast<GridComplexAttributeEditWidget*>(widget);
	QMap<int, QString> enums;
	int defIndex = 0;
	for (int i = 0; i < m_widgets.count(); ++i) {
		enums.insert(i + 1, m_widgets[i]->caption());
		if (m_widgets[i]->isDefault()) {
			defIndex = i + 1;
		}
	}
	w->setEnumerations(enums);
	w->setDefaultIndex(defIndex);
}

void PreProcessorGeoDataComplexGroupDataItem::editScalarsToColors()
{
	PreProcessorGeoDataGroupDataItem::editScalarsToColors();
	applyScalarsToColorsSetting();
	applySettingsToScalarBar();
}

void PreProcessorGeoDataComplexGroupDataItem::applyScalarsToColorsSetting()
{
	PreProcessorGridTypeDataItem* tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* c = tItem->scalarsToColors(m_condition->name());
	ColorTransferFunctionContainer* c2 = dynamic_cast<ColorTransferFunctionContainer*>(c);

	QMap<double, QColor> colors = c2->colors();
	for (int i = 0; i < m_widgets.count(); ++i) {
		GridComplexConditionWidget* w = m_widgets[i];
		w->setColor(colors.value(i + 1));
	}
}

void PreProcessorGeoDataComplexGroupDataItem::applySettingsToScalarBar()
{
	PreProcessorGeoDataTopDataItem* tItem = dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent());
	tItem->updateLegendBoxItems();
}

void PreProcessorGeoDataComplexGroupDataItem::createDefaultGroup()
{
	SolverDefinitionGridComplexAttribute* compCond =
		dynamic_cast<SolverDefinitionGridComplexAttribute*>(condition());
	GridComplexConditionWidget* w = new GridComplexConditionWidget(iricMainWindow(), mainWindow());
	w->setup(projectData()->solverDefinition(), compCond->element(), iricMainWindow()->locale());
	w->setCaption("Default");
	w->setColor(m_undefinedColor);
	w->setIsDefault(true);
	w->setNameAndNumber(m_condition->name(), 1);
	m_widgets.append(w);
}

void PreProcessorGeoDataComplexGroupDataItem::addBackground()
{
	delete m_backgroundItem;
	m_childItems.clear();

	// create an instance or GeoDataBackground, and add it.
	m_backgroundItem = new PreProcessorGeoDataDataItem(this);
	m_childItems.push_back(m_backgroundItem);
	GeoData* geodata = m_condition->buildBackgroundGeoData(m_backgroundItem);
	GeoDataBackgroundComplex* r = dynamic_cast<GeoDataBackgroundComplex*>(geodata);
	r->setItem(this);
	m_backgroundItem->setGeoData(geodata);
	geodata->setupDataItem();
}

SolverDefinitionGridAttribute* PreProcessorGeoDataComplexGroupDataItem::condition()
{
	return PreProcessorGeoDataGroupDataItem::condition();
}
