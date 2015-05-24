#include "../misc/preprocessorgridattributemappingmode.h"
#include "../preprocessorwindow.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributemappingsettingtopdataitem.h"
#include "preprocessorgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridrelatedconditioncellgroupdataitem.h"
#include "preprocessorgridrelatedconditionnodegroupdataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorrootdataitem.h"
#include "preprocessorstructured2dgriddataitem.h"
#include "preprocessorunstructured2dgriddataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDir>
#include <QDomElement>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

PreProcessorGridAndGridCreatingConditionDataItem::PreProcessorGridAndGridCreatingConditionDataItem(const QString& zonename, const QString& caption, PreProcessorDataItem* p)
	: PreProcessorGridAndGridCreatingConditionDataItemInterface(caption, p)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItem->setEditable(false);

	m_standardItemCopy = m_standardItem->clone();

	m_zoneName = zonename;
	m_caption = caption;
	QDir workdir(projectData()->workDirectory());

	m_subFolder = "gridandgridcreatingconditiondataitem";
	QDir subdir1(workdir.absoluteFilePath(relativeSubPath()));

	m_subFolder = m_zoneName;
	QDir subdir2(workdir.absoluteFilePath(relativeSubPath()));

	if (subdir1.exists()) {
		if (subdir2.exists()) {
			// new folder and old folder both exists. remove the old folder.
			iRIC::rmdirRecursively(subdir1.absolutePath());
		} else {
			// old folder only exists. rename the old folder.
			subdir1.cdUp();
			subdir1.rename("gridandgridcreatingconditiondataitem", m_zoneName);
		}
	}

	m_creatingConditionDataItem = new PreProcessorGridCreatingConditionDataItem(this);
	m_childItems.append(m_creatingConditionDataItem);

	m_bcSettingGroupDataItem = new PreProcessorBCSettingGroupDataItem(this);
	m_childItems.append(m_bcSettingGroupDataItem);

	m_mappingSettingDataItem = new PreProcessorGridAttributeMappingSettingTopDataItem(this);
	m_childItems.append(m_mappingSettingDataItem);

	SolverDefinitionGridType* gType = dynamic_cast<PreProcessorGridTypeDataItem*>(parent())->gridType();
	switch (gType->defaultGridType()) {
	case SolverDefinitionGridType::gtNormal1DGrid:
		// @todo not implemented yet.
		break;
	case SolverDefinitionGridType::gtNormal1_5DGrid:
		// @todo not implemented yet.
		break;
	case SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection:
		m_gridDataItem = new PreProcessorNormal15DGridWithCrossSectionDataItem(this);
		break;
	case SolverDefinitionGridType::gtStructured2DGrid:
		m_gridDataItem = new PreProcessorStructured2dGridDataItem(this);
		break;
	case SolverDefinitionGridType::gtUnstructured2DGrid:
		m_gridDataItem = new PreProcessorUnstructured2dGridDataItem(this);
		break;
	case SolverDefinitionGridType::gtUnknownGrid:
	default:
		break;
	}
	m_childItems.append(m_gridDataItem);
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	if (gItem->bcGroupDataItem() == nullptr) {
		m_standardItem->takeChild(m_bcSettingGroupDataItem->standardItem()->row());
	}

	m_deleteAction = new QAction(tr("&Delete Grid Creating Condition and Grid Shape..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteGridAndCondition()));

	// create connections.
	connect(m_creatingConditionDataItem, SIGNAL(gridCreated()), this, SLOT(informGridCreation()));
	connect(dynamic_cast<PreProcessorGridTypeDataItem*>(p)->rawdataTop(), SIGNAL(dataChanged()), m_mappingSettingDataItem, SLOT(informRawDataChange()));
	if (gItem->bcGroupDataItem() != nullptr) {
		connect(gItem->bcGroupDataItem(), SIGNAL(itemsUpdated()), m_bcSettingGroupDataItem, SLOT(updateItems()));
		connect(gItem->bcGroupDataItem(), SIGNAL(itemsLoaded()), m_bcSettingGroupDataItem, SLOT(loadItems()));
	}
}

void PreProcessorGridAndGridCreatingConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_zoneName = elem.attribute("zoneName");
	// load grid creating condition information.
	QDomNode condNode = iRIC::getChildNode(node, "GridCreatingCondition");
	if (! condNode.isNull()) {m_creatingConditionDataItem->loadFromProjectMainFile(condNode);}
	// load boundary condition setting information.
	QDomNode bcNode = iRIC::getChildNode(node, "BoundaryConditionSetting");
	if (! bcNode.isNull()) {m_bcSettingGroupDataItem->loadFromProjectMainFile(bcNode);}
	// load grid information.
	QDomNode gridNode = iRIC::getChildNode(node, "Grid");
	if (! gridNode.isNull()) {m_gridDataItem->loadFromProjectMainFile(gridNode);}
}

void PreProcessorGridAndGridCreatingConditionDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	if (gItem->bcGroupDataItem()) {
		gItem->bcGroupDataItem()->renumberItemsForProject();
	}

	writer.writeAttribute("zoneName", m_zoneName);
	writer.writeAttribute("caption", m_caption);

	// save grid creating condition information
	writer.writeStartElement("GridCreatingCondition");
	m_creatingConditionDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// save boundary condition setting information
	writer.writeStartElement("BoundaryConditionSettings");
	m_bcSettingGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// save grid information
	writer.writeStartElement("Grid");
	m_gridDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorGridAndGridCreatingConditionDataItem::saveExternalData(const QString&)
{

}

bool PreProcessorGridAndGridCreatingConditionDataItem::isDeletable() const
{
	// Ask parent whether I am deletable.
	return dynamic_cast<PreProcessorGridTypeDataItem*>(parent())->isChildDeletable(this);
}

void PreProcessorGridAndGridCreatingConditionDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
}


void PreProcessorGridAndGridCreatingConditionDataItem::addCustomMenuItems(QMenu* menu)
{
	// add "Add Grid" menu.
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	gtItem->addCustomMenuItems(menu);
}

bool PreProcessorGridAndGridCreatingConditionDataItem::gridEdited() const
{
	Grid* g = m_gridDataItem->grid();
	if (g == nullptr) {return false;}
	return g->isModified();
}

void PreProcessorGridAndGridCreatingConditionDataItem::toggleGridEditFlag()
{
	Grid* g = m_gridDataItem->grid();
	if (g == nullptr) {return;}
	g->setModified();
}

void PreProcessorGridAndGridCreatingConditionDataItem::deleteGridAndCondition()
{
	if (QMessageBox::No == QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to discard grid creating condition and grid?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}
	m_gridDataItem->silentDeleteGrid();
	dynamic_cast<PreProcessorGridCreatingConditionDataItem*>(m_creatingConditionDataItem)->silentDeleteCondition();
	iRICUndoStack::instance().clear();
}

void PreProcessorGridAndGridCreatingConditionDataItem::informGridCreation()
{
	// set default values first. this is done, always.
	m_mappingSettingDataItem->setDefaultValues();


	if (PreProcessorGridAttributeMappingMode::mode == PreProcessorGridAttributeMappingMode::mAuto) {
		int ret = QMessageBox::information(mainWindow(), tr("Confirmation"), tr("Do you want to map geographic data to grid attributes now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (ret == QMessageBox::Yes) {
			m_mappingSettingDataItem->executeMapping();
			m_bcSettingGroupDataItem->executeMapping(true);
		}
	}
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	gItem->informgridRelatedConditionChangeAll();
}
