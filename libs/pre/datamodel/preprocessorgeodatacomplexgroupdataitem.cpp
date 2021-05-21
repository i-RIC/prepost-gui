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
#include <guicore/pre/gridcond/stringconverter/gridattributestringconverterenumerate.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodatabackground/geodatabackgroundcomplex.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <geodata/polygon/geodatapolygon.h>

#include <QAction>
#include <QLocale>
#include <QMenu>

#include <iriclib.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnsgridcomplexconditiongroup.h>
#include <h5cgnsgridcomplexconditiontop.h>

#include <algorithm>

namespace {

void deleteWidgets(std::vector<GridComplexConditionWidget*>* widgets, std::vector<GridComplexConditionWidget*>* widgets2){
	for (auto w : *widgets2) {
		auto it = std::find(widgets->begin(), widgets->end(), w);
		if (it == widgets->end()) {
			delete w;
		}
	}
	widgets2->clear();
	for (auto w : *widgets) {
		delete w;
	}
	widgets->clear();
}

} // namespace

PreProcessorGeoDataComplexGroupDataItem::PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorGeoDataGroupDataItem {cond, parent},
	m_undefinedColor {Qt::gray}
{
	m_dialog = new GridComplexConditionDialog(this, mainWindow());
	m_dialog->setWindowTitle(PreProcessorGeoDataComplexGroupDataItem::tr("%1 Group Setting").arg(m_condition->caption()));

	m_editGroupAction = new QAction(PreProcessorGeoDataComplexGroupDataItem::tr("Edit &Groups..."), this);
	connect(m_editGroupAction, SIGNAL(triggered()), this, SLOT(showEditGroupDialog()));
	createDefaultGroup();
	addBackground();
}

PreProcessorGeoDataComplexGroupDataItem::~PreProcessorGeoDataComplexGroupDataItem()
{
	clearGroups();
	delete m_dialog;
}

int PreProcessorGeoDataComplexGroupDataItem::loadFromCgnsFile()
{
	int defId = -1;

	auto solverDef = projectData()->solverDefinition();
	auto compCond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(condition());
	auto elem = compCond->element();

	clearGroups();

	auto gccTop = projectData()->mainfile()->cgnsFile()->ccBase()->gccTop();

	if (! gccTop->groupExists(m_condition->name())) {
		goto INITGROUPS;
	}

	auto group = gccTop->group(m_condition->name());

	for (int i = 0; i < group->itemCount(); ++i) {
		auto g = new GridComplexConditionGroup(solverDef, elem);
		auto item = group->item(i + 1);

		int ier = g->load(*item);
		if (ier != IRIC_NO_ERROR) {return ier;}

		if (g->isDefault()) {defId = i;}
		m_groups.push_back(g);
	}
	if (defId == -1 && m_groups.size() > 0) {
		// make the first group default.
		m_groups[0]->setIsDefault(true);
	}

INITGROUPS:
	if (m_groups.size() == 0) {
		createDefaultGroup();
	}
	updateColorMap();

	return IRIC_NO_ERROR;
}

int PreProcessorGeoDataComplexGroupDataItem::saveComplexGroupsToCgnsFile()
{
	auto gccTop = projectData()->mainfile()->cgnsFile()->ccBase()->gccTop();
	auto group = gccTop->group(m_condition->name());

	for (int i = 0; i < static_cast<int> (m_groups.size()); ++i) {
		auto g = m_groups.at(i);
		auto item = group->item(i + 1);
		int ier = g->save(item);

		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	return IRIC_NO_ERROR;
}

void PreProcessorGeoDataComplexGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	GeoDataFactory& factory = GeoDataFactory::instance();
	// create import menu and add menu.
	m_importMenu = new QMenu(tr("&Import"), menu);
	m_importMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
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

	if (! m_condition->isReferenceInformation()) {
		menu->addSeparator();
		menu->addAction(m_editColorMapAction);
		menu->addAction(m_setupScalarBarAction);
	}
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
	PreProcessorGridTypeDataItem* tItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* c = tItem->scalarsToColors(m_condition->name());
	ColorTransferFunctionContainer* c2 = dynamic_cast<ColorTransferFunctionContainer*>(c);

	QMap<double, QString> enums;
	QMap<double, QString> englishEnums;
	QMap<double, QColor> colors;

	for (int i = 0; i < m_groups.size(); ++i) {
		auto g = m_groups.at(i);
		double val = i + 1;
		colors.insert(val, g->color());
		enums.insert(val, g->caption());
		englishEnums.insert(val, g->caption());
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

	std::vector<GridComplexConditionGroup::Setting> settings;
	std::vector<GridComplexConditionWidget*> widgets;

	for (int i = 0; i < m_groups.size(); ++i) {
		auto g = m_groups.at(i);

		settings.push_back(g->setting());

		auto w = new GridComplexConditionWidget(m_dialog);
		w->setGroup(g);
		widgets.push_back(w);
	}

	m_dialog->setWidgets(widgets);

	int ret = m_dialog->exec();

	std::vector<GridComplexConditionWidget*> newWidgets = m_dialog->widgets();
	if (ret == QDialog::Rejected) {
		deleteWidgets(&widgets, &newWidgets);

		for (int i = 0; i < m_groups.size(); ++i) {
			m_groups[i]->setSetting(settings.at(i));
		}
		return;
	}

	// Apply change values.
	std::vector<GridComplexConditionGroup*> oldGroups = m_groups;
	m_groups.clear();
	for (int i = 0; i < newWidgets.size(); ++i) {
		auto w = newWidgets.at(i);
		m_groups.push_back(w->group());
	}

	deleteWidgets(&widgets, &newWidgets);

	std::vector<int> valueMap;
	int newindex = 1;
	int newDefault = 0;
	for (int i = 0; i < m_groups.size(); ++i) {
		if (m_groups[i]->isDefault()) {
			newDefault = i + 1;
		}
	}
	// 0 is mapped to new default value
	valueMap.push_back(newDefault);

	for (int i = 0; i < oldGroups.size(); ++i) {
		auto g = oldGroups[i];
		auto it = std::find(m_groups.begin(), m_groups.end(), g);
		if (it == m_groups.end()) {
			valueMap.push_back(newDefault);
			-- newindex;
		} else {
			valueMap.push_back(newindex);
		}
		++ newindex;
	}
	for (int i = 0; i < m_groups.size(); ++i) {
		m_groups[i]->setNameAndNumber(m_condition->name(), i + 1);
	}

	// modify the geodata values.
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* di = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		GeoData* rd = di->geoData();
		GeoDataPolygon* p = dynamic_cast<GeoDataPolygon*>(rd);
		if (p != nullptr) {
			int currval = p->variantValue().toInt();
			int newVal = 0;
			if (currval >= valueMap.size()) {
				newVal = newDefault;
			} else {
				newVal = valueMap.at(currval);
			}
			p->setVariantValue(newVal);
		}
	}

	// modify grid attribute values.
	PreProcessorGridTypeDataItem* gtItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	for (auto ccItem : gtItem->conditions()) {
		Grid* g = ccItem->gridDataItem()->grid();
		if (g == nullptr) {continue;}
		GridAttributeContainer* cont = g->gridAttribute(m_condition->name());
		auto cont2 = dynamic_cast<GridComplexAttributeContainer*>(cont);
		for (unsigned int i = 0; i < cont2->dataCount(); ++i) {
			int currval = cont2->value(i);
			int newVal = 0;
			if (currval >= valueMap.size()) {
				newVal = newDefault;
			} else {
				newVal = valueMap.at(currval);
			}
			cont2->setValue(i, newVal);
		}
		cont2->dataArray()->Modified();
		g->setModified();
	}

	// set the default folder back.
	LastIODirectory::set(InputConditionWidgetFilename::defaultFolder);

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	for (auto g : oldGroups) {
		auto it = std::find(m_groups.begin(), m_groups.end(), g);
		if (it == m_groups.end()) {
			delete g;
		}
	}
	updateColorMap();
	renderGraphicsView();
}

void PreProcessorGeoDataComplexGroupDataItem::setupStringConverter(GridAttributeStringConverter* converter)
{
	auto c = dynamic_cast<GridAttributeStringConverterEnumerate*> (converter);
	QMap<QVariant, QString> enums;

	for (int i = 0; i < m_groups.size(); ++i) {
		enums.insert(QVariant(i + 1), m_groups[i]->caption());
	}

	c->setEnumerations(enums);
}

void PreProcessorGeoDataComplexGroupDataItem::setupEditWidget(GridAttributeEditWidget* widget)
{
	auto w = dynamic_cast<GridComplexAttributeEditWidget*>(widget);
	QMap<int, QString> enums;
	int defIndex = 0;
	for (int i = 0; i < m_groups.size(); ++i) {
		enums.insert(i + 1, m_groups[i]->caption());
		if (m_groups[i]->isDefault()) {
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
	for (int i = 0; i < m_groups.size(); ++i) {
		auto g = m_groups[i];
		g->setColor(colors.value(i + 1));
	}
}

void PreProcessorGeoDataComplexGroupDataItem::applySettingsToScalarBar()
{
	PreProcessorGeoDataTopDataItem* tItem = dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent());
	tItem->updateLegendBoxItems();
}

void PreProcessorGeoDataComplexGroupDataItem::createDefaultGroup()
{
	auto compCond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(condition());
	auto g = new GridComplexConditionGroup(projectData()->solverDefinition(), compCond->element());
	g->setCaption("Default");
	g->setColor(m_undefinedColor);
	g->setIsDefault(true);
	g->setNameAndNumber(m_condition->name(), 1);
	m_groups.push_back(g);
}

void PreProcessorGeoDataComplexGroupDataItem::clearGroups()
{
	for (auto g : m_groups) {
		delete g;
	}
	m_groups.clear();
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
	clearGroups();

	auto compCond = dynamic_cast<SolverDefinitionGridComplexAttribute*>(condition());

	for (int i = 0; i < count; ++i) {
		auto g = new GridComplexConditionGroup(projectData()->solverDefinition(), compCond->element());
		if (i == 0) {g->setIsDefault(true);}
		m_groups.push_back(g);
	}
}

std::vector<GridComplexConditionGroup*> PreProcessorGeoDataComplexGroupDataItem::groups() const
{
	return m_groups;
}
