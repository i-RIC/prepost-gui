#include "preprocessorbcdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcgroupsettingdialog.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "private/preprocessorbcgroupdataitem_setsettingcommand.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <QFileDialog>
#include <QRegExp>

#include <h5cgnsbc.h>
#include <h5cgnszone.h>
#include <h5cgnszonebc.h>
#include <iriclib_errorcodes.h>

#include <yaml-cpp/yaml.h>

PreProcessorBCGroupDataItem::PreProcessorBCGroupDataItem(PreProcessorDataItem* parent) :
	PreProcessorDataItem {tr("Boundary Condition"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorBCGroupDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorBCGroupDataItem::tr("Delete &All..."), this)},
	m_importAction {new QAction(QIcon(":/libs/guibase/images/iconImport.svg"), PreProcessorBCGroupDataItem::tr("&Import..."), this)},
	m_exportAction {new QAction(QIcon(":/libs/guibase/images/iconExport.svg"), PreProcessorBCGroupDataItem::tr("&Export..."), this)},
	m_dummyEditAction {new QAction(PreProcessorBCGroupDataItem::tr("&Edit Condition..."), this)},
	m_dummyDeleteAction {new QAction(PreProcessorBCGroupDataItem::tr("&Delete..."), this)},
	m_dummyAssignAction {new QAction(PreProcessorBCGroupDataItem::tr("&Assign Condition"), this)},
	m_dummyReleaseAction {new QAction(PreProcessorBCGroupDataItem::tr("&Release Condition"), this)},
	m_bcMenu {new QMenu(PreProcessorBCGroupDataItem::tr("&Boundary Condition"), mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath("bc");

	// setup default setting
	m_nameSetting.addPrefix("nameSetting");
	m_nameSetting.fontFamily = vtkTextPropertySettingContainer::ffArial;
	m_nameSetting.fontSize = 14;

	QSettings s;
	m_nameSetting.loadSetting(s, "fontsetting/bc");

	m_colorSource = new ColorSource(this);

	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent->parent());
	auto gridType = gtItem->gridType();
	for (int i = 0; i < gridType->boundaryConditions().size(); ++i) {
		auto bc = gridType->boundaryConditions().at(i);
		QString str(PreProcessorBCGroupDataItem::tr("Add %1"));
		QAction* addAction = new QAction(str.arg(bc->caption()), this);
		connect(addAction, SIGNAL(triggered()), this, SLOT(addCondition()));
		m_addActions.append(addAction);
	}
	connect(m_deleteSelectedAction, &QAction::triggered, this, &PreProcessorBCGroupDataItem::deleteSelected);
	connect(m_deleteAllAction, &QAction::triggered, this, &PreProcessorBCGroupDataItem::deleteAll);
	connect(m_importAction, &QAction::triggered, this, &PreProcessorBCGroupDataItem::importBc);
	connect(m_exportAction, &QAction::triggered, this, &PreProcessorBCGroupDataItem::exportBc);

	m_dummyEditAction->setDisabled(true);
	m_dummyDeleteAction->setDisabled(true);
	m_dummyAssignAction->setDisabled(true);
	m_dummyReleaseAction->setDisabled(true);
}

PreProcessorBCGroupDataItem::~PreProcessorBCGroupDataItem()
{
	delete m_colorSource;
}

const vtkTextPropertySettingContainer& PreProcessorBCGroupDataItem::nameSetting() const
{
	return m_nameSetting;
}

int PreProcessorBCGroupDataItem::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	for (auto child : m_childItems) {
		auto bcItem = dynamic_cast<PreProcessorBCDataItem*>(child);
		bcItem->loadFromCgnsFile(zone);
	}

	return IRIC_NO_ERROR;
}

int PreProcessorBCGroupDataItem::saveToCgnsFile(iRICLib::H5CgnsZone* zone)
{
	renumberItemsForCgns();

	for (auto child : m_childItems) {
		auto bcItem = dynamic_cast<PreProcessorBCDataItem*> (child);
		int ier = bcItem->saveToCgnsFile(zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	return IRIC_NO_ERROR;
}

int PreProcessorBCGroupDataItem::importFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	auto zoneBc = zone.zoneBc();
	if (zoneBc == nullptr) {return IRIC_NO_ERROR;}

	auto gtItem = gridTypeDataItem();
	const auto& conditions = gtItem->gridType()->boundaryConditions();
	for (const auto& c : conditions) {
		auto count = zoneBc->bcCount(c->name());
		for (int i = 1; i <= count; ++i) {
			auto item = new PreProcessorBCDataItem(projectData()->solverDefinition(), c, this, false);
			item->setCgnsNumber(i);
			int ret = item->importFromCgnsFile(zone);
			if (ret != IRIC_NO_ERROR) {return ret;}

			m_childItems.push_back(item);
		}
	}
	return IRIC_NO_ERROR;
}

void PreProcessorBCGroupDataItem::renumberItemsForProject()
{
	// set numbers.
	auto gtItem = gridTypeDataItem();
	const auto& conditions = gtItem->gridType()->boundaryConditions();

	int number = 1;
	int condIndex = -1;
	auto it = m_childItems.begin();
	while (it != m_childItems.end()) {
		auto tmpItem = dynamic_cast<PreProcessorBCDataItem*>(*it);
		auto tmpbc = tmpItem->condition();
		auto it2 = std::find(conditions.begin(), conditions.end(), tmpbc);
		int tmpindex = it2 - conditions.begin();
		if (tmpindex != condIndex) {
			number = 1;
			condIndex = tmpindex;
		} else {
			++ number;
		}
		tmpItem->setProjectNumber(number);
		++ it;
	}
}

const QList<QAction*> PreProcessorBCGroupDataItem::addActions() const
{
	return m_addActions;
}

PreProcessorGridTypeDataItem* PreProcessorBCGroupDataItem::gridTypeDataItem() const
{
	return gridDataItem()->gridTypeDataItem();
}

PreProcessorGridDataItem* PreProcessorBCGroupDataItem::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent());
}

QDialog* PreProcessorBCGroupDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new PreProcessorBcGroupSettingDialog(parent);
	dialog->setNameSetting(m_nameSetting);

	return dialog;
}

void PreProcessorBCGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<PreProcessorBcGroupSettingDialog*> (propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->nameSetting(), this), this);
}

void PreProcessorBCGroupDataItem::renumberItemsForCgns()
{
	// set numbers again.
	auto gtItem = gridDataItem()->gridTypeDataItem();
	const auto& conditions = gtItem->gridType()->boundaryConditions();

	int number = 1;
	int condIndex = -1;
	auto it = m_childItems.begin();
	while (it != m_childItems.end()) {
		auto tmpItem = dynamic_cast<PreProcessorBCDataItem*>(*it);
		if (tmpItem->isValid()) {
			auto tmpbc = tmpItem->condition();
			auto it2 = std::find(conditions.begin(), conditions.end(), tmpbc);
			int tmpindex = it2 - conditions.begin();
			if (tmpindex != condIndex) {
				number = 1;
				condIndex = tmpindex;
			} else {
				++ number;
			}
			tmpItem->setCgnsNumber(number);
		} else {
			tmpItem->setCgnsNumber(0);
		}
		++ it;
	}
}

void PreProcessorBCGroupDataItem::updateNameActorSettingsOfChildren()
{
	for (auto c : m_childItems) {
		auto c2 = dynamic_cast<PreProcessorBCDataItem*> (c);
		c2->updateNameActorSettings();
	}
}

void PreProcessorBCGroupDataItem::informGridUpdate()
{}

void PreProcessorBCGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_nameSetting.load(node);

	m_projectBuildNumber = projectData()->version().build();
	auto gtItem = gridTypeDataItem();
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.count(); ++i) {
		QDomElement childElem = childNodes.at(i).toElement();
		std::string condType = iRIC::toStr(childElem.attribute("type"));
		auto bc = gtItem->gridType()->boundaryCondition(condType);
		if (bc == nullptr) {continue;}

		bool settingHidden = (childElem.attribute("settingHidden") == "true");
		auto bcItem = new PreProcessorBCDataItem(projectData()->solverDefinition(), bc, this, settingHidden);
		bcItem->loadFromProjectMainFile(childElem);
		m_childItems.push_back(bcItem);
	}
	assignActorZValues(m_zDepthRange);
	updateItemMap();

	emit itemsUpdated();
	emit itemsLoaded();
}

void PreProcessorBCGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_nameSetting.save(writer);

	renumberItemsForProject();
	for (auto item : m_childItems) {
		writer.writeStartElement("BoundaryCondition");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorBCGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	for (int i = 0; i < m_addActions.count(); ++i) {
		menu->addAction(m_addActions[i]);
	}
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
	menu->addSeparator();
	menu->addAction(m_importAction);
	menu->addAction(m_exportAction);
}

void PreProcessorBCGroupDataItem::addCondition()
{
	QAction* a = dynamic_cast<QAction*>(sender());
	int index = 0;
	for (index = 0; index < m_addActions.count(); ++index) {
		if (m_addActions[index] == a) {break;}
	}

	auto item = addCondition(index, true);
	if (item == nullptr) {return;}

	dataModel()->objectBrowserView()->expand(m_standardItem->index());
	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->handleObjectBrowserSelection(item->standardItem()->index());
	bool ok = item->showDialog();
	if (! ok) {
		// Cancel button is clicked.
		delete item;
		dataModel()->handleObjectBrowserSelection(m_standardItem->index());
		updateItemMap();
	}
}

void PreProcessorBCGroupDataItem::deleteSelected()
{
	if (m_childItems.size() == 0) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("There is no boundary condition."), QMessageBox::Ok);
		return;
	}

	auto items = m_childItems;
	std::vector<QString> names;
	for (auto item : items) {
		names.push_back(item->standardItem()->text());
	}

	ItemMultiSelectingDialog dialog(mainWindow());
	dialog.setWindowTitle(tr("Delete selected boundary conditions"));
	dialog.setItems(names);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto settings = dialog.selectSettings();
	for (int i = 0; i < settings.size(); ++i) {
		if (settings.at(i)) {
			// delete the item
			delete items.at(i);
		}
	}

	renderGraphicsView();

	// this operation is not undoable.
	iRICUndoStack::instance().clear();
}

void PreProcessorBCGroupDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all boundary conditions?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	auto items = m_childItems;
	for (auto item : items) {
		delete item;
	}

	renderGraphicsView();

	// this operation is not undoable.
	iRICUndoStack::instance().clear();
}

void PreProcessorBCGroupDataItem::importBc()
{
	auto fname = QFileDialog::getOpenFileName(preProcessorWindow(), tr("Select file to import"), LastIODirectory::get(), tr("YAML file (*.yaml)"));
	if (fname.isNull()) {return;}

	clearChildItems();

	QFileInfo finfo(fname);
	YAML::Node data = YAML::LoadFile(iRIC::toStr(fname));

	auto gType = gridTypeDataItem()->gridType();

	QRegExp exp("(.+)(\\d+)");
	for (auto it = data.begin(); it != data.end(); ++it) {
		auto name = it->first.Scalar();
		int pos = exp.indexIn(name.c_str());
		if (pos == -1) {continue;}

		auto bcName = iRIC::toStr(exp.cap(1));
		auto bc = gType->boundaryCondition(bcName);
		if (bc == nullptr) {continue;}

		auto item = new PreProcessorBCDataItem(projectData()->solverDefinition(), bc, this, false);
		item->importFromYaml(it->second, finfo.absoluteDir());
		m_childItems.push_back(item);
	}

	auto gItem = gridDataItem();
	if (gItem->grid() != nullptr) {
		gItem->grid()->setIsModified(true);
	}

	updateItemMap();
	assignActorZValues(m_zDepthRange);
	emit itemsUpdated();

	auto bcsItem = gridDataItem()->gridAndGridCreatingConditionDataItem()->bcSettingGroupDataItem();
	auto shpFileName = fname.replace(".yaml", ".shp");
	bcsItem->importPolygons(shpFileName);

	LastIODirectory::setFromFilename(fname);

	projectData()->mainfile()->setModified();
}

void PreProcessorBCGroupDataItem::exportBc()
{
	renumberItemsForProject();

	auto fname = QFileDialog::getSaveFileName(preProcessorWindow(), tr("Select file to export"), LastIODirectory::get(), tr("YAML file (*.yaml)"));
	if (fname.isNull()) {return;}

	QFileInfo finfo(fname);

	QFile file(fname);
	bool ok = file.open(QFile::WriteOnly);
	if (! ok) {
		QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Cannot open file for writing"));
		return;
	}

	QTextStream stream(&file);
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	stream.setCodec(codec);

	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorBCDataItem*> (child);
		auto name = QString("%1%2").arg(item->condition()->name().c_str()).arg(item->projectNumber());
		stream << name << ":" << "\r\n";
		item->setFileNamePrefix(name);
		item->exportToYaml(&stream, finfo.absoluteDir(), "  ");
	}

	file.close();

	auto bcsItem = gridDataItem()->gridAndGridCreatingConditionDataItem()->bcSettingGroupDataItem();
	auto shpFileName = fname.replace(".yaml", ".shp");
	bcsItem->exportPolygons(shpFileName);

	LastIODirectory::setFromFilename(fname);
}

PreProcessorBCDataItem* PreProcessorBCGroupDataItem::addCondition(int index, bool hideSetting)
{
	if (index > m_addActions.count()) {return 0;}

	auto gtItem = gridDataItem()->gridTypeDataItem();
	auto gtype = gtItem->gridType();
	auto conditions = gtype->boundaryConditions();
	auto bc = conditions.at(index);
	auto item = new PreProcessorBCDataItem(projectData()->solverDefinition(), bc, this, hideSetting);
	QString tmpName(QString("New %1").arg(bc->englishCaption().c_str()));
	item->setName(tmpName);
	int number = 1;
	auto it = m_childItems.begin();
	while (it != m_childItems.end()) {
		auto tmpItem = dynamic_cast<PreProcessorBCDataItem*>(*it);
		auto tmpbc = tmpItem->condition();
		auto it2 = std::find(conditions.begin(), conditions.end(), tmpbc);
		int tmpindex = it2 - conditions.begin();
		if (tmpindex == index) {++ number;}
		if (tmpindex > index) {break;}
		++ it;
	}
	item->setProjectNumber(number);
	item->setCgnsNumber(number);
	item->setColor(m_colorSource->getColor(static_cast<int> (m_childItems.size())));
	// add it simply.
	m_childItems.insert(it, item);
	auto gItem = gridDataItem();
	if (gItem->grid() != nullptr) {
		gItem->grid()->setIsModified(true);
	}

	// setup object browser tree again.
	int rows = m_standardItem->rowCount();
	for (int row = rows - 1; row >= 0; --row) {
		m_standardItem->takeRow(row);
	}
	for (auto child : m_childItems) {
		m_standardItem->appendRow(child->standardItem());
	}
	updateItemMap();
	assignActorZValues(m_zDepthRange);
	emit itemsUpdated();

	return item;
}

void PreProcessorBCGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(5);
}

QMenu* PreProcessorBCGroupDataItem::bcMenu() const
{
	return m_bcMenu;
}

void PreProcessorBCGroupDataItem::updateBCMenu(PreProcessorBCDataItem* item)
{
	m_bcMenu->clear();
	for (auto action : m_addActions) {
		m_bcMenu->addAction(action);
	}
	m_bcMenu->addSeparator();
	if (item == 0) {
		// add dummy menu items.
		m_bcMenu->addAction(m_dummyEditAction);
		m_bcMenu->addAction(m_dummyDeleteAction);
		m_bcMenu->addSeparator();
		m_bcMenu->addAction(m_dummyAssignAction);
		m_bcMenu->addAction(m_dummyReleaseAction);
	} else {
		m_bcMenu->addAction(item->editAction());
		m_bcMenu->addAction(item->deleteAction());
		m_bcMenu->addSeparator();
		m_bcMenu->addAction(item->assignAction());
		m_bcMenu->addAction(item->releaseAction());
	}
}

void PreProcessorBCGroupDataItem::clear()
{
	auto itemCopys = m_childItems;
	for (auto item : itemCopys) {
		delete item;
	}
	m_childItems.clear();
	updateItemMap();
}

void PreProcessorBCGroupDataItem::clearPoints()
{
	for (auto item : m_childItems) {
		auto bcItem = dynamic_cast<PreProcessorBCDataItem*>(item);
		bcItem->clearPoints();
	}
}
