#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatadataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/complex/gridcomplexconditiondialog.h>
#include <guicore/pre/complex/gridcomplexconditionwidget.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributeeditwidget.h>
#include <guicore/pre/gridcond/stringconverter/gridattributestringconverterenumerate.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodatafactory.h>
#include <guicore/pre/geodatabackground/geodatabackgroundcomplex.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colormapenumeratesettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <geodata/polygon/geodatapolygon.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnsgridcomplexconditiongroup.h>
#include <h5cgnsgridcomplexconditiontop.h>

#include <algorithm>

PreProcessorGeoDataComplexGroupDataItem::PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridComplexAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorGeoDataGroupDataItem {cond, parent}
{
	m_dialog = new GridComplexConditionDialog(cond->solverDefinition(), cond->element(), mainWindow());
	m_dialog->setWindowTitle(PreProcessorGeoDataComplexGroupDataItem::tr("%1 Group Setting").arg(m_condition->caption()));
	m_dialog->createDefaultGroup();

	m_editGroupAction = new QAction(PreProcessorGeoDataComplexGroupDataItem::tr("Edit &Groups..."), this);
	connect(m_editGroupAction, SIGNAL(triggered()), this, SLOT(showEditGroupDialog()));

	m_toolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), preProcessorWindow());

	addBackground();
	updateColorMap();
}

PreProcessorGeoDataComplexGroupDataItem::~PreProcessorGeoDataComplexGroupDataItem()
{
	delete m_dialog;
}

int PreProcessorGeoDataComplexGroupDataItem::loadFromCgnsFile()
{
	auto gccTop = projectData()->mainfile()->cgnsFile()->ccBase()->gccTop();

	if (! gccTop->groupExists(m_condition->name())) {
		goto INITGROUPS;
	}

	auto group = gccTop->group(m_condition->name());
	m_dialog->loadFromCgnsFile(group);

INITGROUPS:
	if (m_dialog->groups().size() == 0) {
		m_dialog->createDefaultGroup();
	}
	updateColorMap();

	return IRIC_NO_ERROR;
}

int PreProcessorGeoDataComplexGroupDataItem::saveComplexGroupsToCgnsFile()
{
	auto gccTop = projectData()->mainfile()->cgnsFile()->ccBase()->gccTop();
	auto group = gccTop->group(m_condition->name());

	return m_dialog->saveToCgnsFile(group);
}

void PreProcessorGeoDataComplexGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	auto& factory = GeoDataFactory::instance();
	// create import menu and add menu.
	m_importMenu = new QMenu(tr("&Import"), menu);
	m_importMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.svg"));
	m_addMenu = new QMenu(tr("&Add"), menu);

	if (m_importSignalMapper) {delete m_importSignalMapper;}
	m_importSignalMapper = new QSignalMapper(this);

	if (m_addSignalMapper) {delete m_addSignalMapper;}
	m_addSignalMapper = new QSignalMapper(this);

	for (GeoDataCreator* creator : factory.compatibleCreators(m_condition)) {
		QString title = creator->caption();
		if (creator->importers().size() > 0) {
			QAction* importAction = m_importMenu->addAction(title.append("..."));
			m_importSignalMapper->setMapping(importAction, creator);
			connect(importAction, SIGNAL(triggered()), m_importSignalMapper, SLOT(map()));
		}
		if (creator->isCreatable()) {
			QAction* addAction = m_addMenu->addAction(title.append("..."));
			m_addSignalMapper->setMapping(addAction, creator);
			connect(addAction, SIGNAL(triggered()), m_addSignalMapper, SLOT(map()));
		}
	}
	connect(m_importSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(importGeoData(QObject*)));
	connect(m_addSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(addGeoData(QObject*)));

	menu->addMenu(m_importMenu);
	menu->addAction(m_webImportAction);
	if (m_addMenu->actions().count() != 0) {
		menu->addMenu(m_addMenu);
	}

	menu->addSeparator();
	menu->addAction(m_editGroupAction);

	menu->addSeparator();

	m_exportAllPolygonsAction->setEnabled(polygonExists());
	menu->addAction(m_exportAllPolygonsAction);

	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
}

ProjectData* PreProcessorGeoDataComplexGroupDataItem::projectData() const
{
	return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());
}

void PreProcessorGeoDataComplexGroupDataItem::updateColorMap()
{
	auto s = gridTypeDataItem()->colorMapSetting(m_condition->name());
	auto es = dynamic_cast<ColorMapEnumerateSettingContainer*>(s);

	std::vector<ColorMapSettingValueColorPairContainer> cols;
	std::map<double, QString> captions;

	auto groups = m_dialog->groups();

	for (int i = 0; i < static_cast<int> (groups.size()); ++i) {
		auto g = groups.at(i);

		double val = i + 1;
		captions.insert({val, g->caption()});

		ColorMapSettingValueColorPairContainer pair;
		pair.value = val;
		pair.color = g->color();
		cols.push_back(pair);
	}
	es->colors = cols;
	es->valueCaptions = captions;

	informValueRangeChange();
}

void PreProcessorGeoDataComplexGroupDataItem::showEditGroupDialog()
{
	// set default folder for filename input conditions.
	InputConditionWidgetFilename::defaultFolder = LastIODirectory::get();

	std::vector<std::weak_ptr<GridComplexConditionGroup> > oldGroups;
	for (const auto& group : m_dialog->groups()) {
		std::weak_ptr<GridComplexConditionGroup> g(group);
		oldGroups.push_back(g);
	}

	int ret = m_dialog->exec();
	if (ret == QDialog::Rejected) {
		return;
	}

	// Apply change values.
	std::vector<GridComplexConditionGroup*> newGroups;
	for (const auto& g : m_dialog->groups()) {
		newGroups.push_back(g.get());
	}

	std::vector<int> valueMap;
	int newDefault = 0;

	for (int i = 0; i < static_cast<int> (newGroups.size()); ++i) {
		if (newGroups[i]->isDefault()) {
			newDefault = i + 1;
		}
	}
	// 0 is mapped to new default value
	valueMap.push_back(newDefault);

	for (int i = 0; i < static_cast<int> (oldGroups.size()); ++i) {
		auto g = oldGroups[i];
		if (g.expired()) {
			valueMap.push_back(newDefault);
		} else {
			auto it = std::find(newGroups.begin(), newGroups.end(), g.lock().get());
			valueMap.push_back(it - newGroups.begin() + 1);
		}
	}

	// modify the geodata values.
	// TODO fix this to apply to all types og geodata.
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorGeoDataDataItem*>(child);
		GeoData* rd = item->geoData();
		auto polygon = dynamic_cast<GeoDataPolygon*>(rd);
		if (polygon != nullptr) {
			int currval = polygon->variantValue().toInt();
			int newVal = 0;
			if (currval >= static_cast<int> (valueMap.size())) {
				newVal = newDefault;
			} else {
				newVal = valueMap.at(currval);
			}
			polygon->setVariantValue(newVal);
		}
	}

	// modify grid attribute values.

	for (auto ccItem : gridTypeDataItem()->conditions()) {
		v4InputGrid* g = ccItem->gridDataItem()->grid();
		if (g == nullptr) {continue;}
		auto cont = g->attribute(m_condition->name());
		auto cont2 = dynamic_cast<GridComplexAttributeContainer*>(cont);
		for (unsigned int i = 0; i < cont2->dataCount(); ++i) {
			int currval = cont2->value(i);
			int newVal = 0;
			if (currval >= static_cast<int> (valueMap.size())) {
				newVal = newDefault;
			} else {
				newVal = valueMap.at(currval);
			}
			cont2->setValue(i, newVal);
		}
		cont2->dataArray()->Modified();
		g->setIsModified(true);
	}

	// set the default folder back.
	LastIODirectory::set(InputConditionWidgetFilename::defaultFolder);

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	updateColorMap();
	renderGraphicsView();
}

void PreProcessorGeoDataComplexGroupDataItem::setupStringConverter(GridAttributeStringConverter* converter)
{
	auto c = dynamic_cast<GridAttributeStringConverterEnumerate*> (converter);
	QMap<QVariant, QString> enums;

	const auto& groups = m_dialog->groups();

	for (int i = 0; i < static_cast<int> (groups.size()); ++i) {
		enums.insert(QVariant(i + 1), groups[i]->caption());
	}

	c->setEnumerations(enums);
}

void PreProcessorGeoDataComplexGroupDataItem::setupEditWidget(GridAttributeEditWidget* widget)
{
	auto w = dynamic_cast<GridComplexAttributeEditWidget*>(widget);
	QMap<int, QString> enums;
	int defIndex = 0;

	const auto& groups = m_dialog->groups();
	for (int i = 0; i < static_cast<int> (groups.size()); ++i) {
		enums.insert(i + 1, groups[i]->caption());
		if (groups[i]->isDefault()) {
			defIndex = i + 1;
		}
	}
	w->setEnumerations(enums);
	w->setDefaultIndex(defIndex);
}

void PreProcessorGeoDataComplexGroupDataItem::showPropertyDialog()
{
	PreProcessorGeoDataGroupDataItem::showPropertyDialog();
	applyScalarsToColorsSetting();
}

void PreProcessorGeoDataComplexGroupDataItem::applyScalarsToColorsSetting()
{
	auto s = gridTypeDataItem()->colorMapSetting(m_condition->name());
	auto es = dynamic_cast<ColorMapEnumerateSettingContainer*>(s);

	const auto& groups = m_dialog->groups();
	for (int i = 0; i < static_cast<int> (es->colors.size()); ++i) {
		const auto& c = es->colors.at(i);
		auto& g = groups[i];
		g->setColor(c.color.value());
	}
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

void PreProcessorGeoDataComplexGroupDataItem::setupGroups(int count)
{
	m_dialog->setupGroups(count);
}

std::vector<GridComplexConditionGroup*> PreProcessorGeoDataComplexGroupDataItem::groups() const
{
	std::vector<GridComplexConditionGroup*> ret;
	for (const auto& g : m_dialog->groups()) {
		ret.push_back(g.get());
	}

	return ret;
}

bool PreProcessorGeoDataComplexGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	bool added = false;

	if (m_toolBarWidgetController != nullptr) {
		auto widget = m_toolBarWidgetController->widget();
		widget->setParent(toolBar);
		widget->show();
		toolBar->addWidget(widget);
		added = true;
	}

	return added;
}
