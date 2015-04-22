#include "preprocessorgridtypedataitem.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorrawdatagroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include "preprocessorrootdataitem.h"
#include "preprocessorbackgroundimagesdataitem.h"
#include <guicore/pre/grid/grid.h>
#include <misc/xmlsupport.h>
#include <misc/iricundostack.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>

#include <QMenu>
#include <QAction>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridTypeDataItem::PreProcessorGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent)
	: PreProcessorGridTypeDataItemInterface(type->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItemCopy = m_standardItem->clone();

	m_isDeletable = false;

	m_gridType = type;
	m_subFolder = type->name();

	// setup ScalarsToColors instances
	setupScalarsToColors(type);

	// add raw data node and grid data node.
	m_rawdataTop = new PreProcessorRawDataTopDataItem(this);
	connect(m_rawdataTop, SIGNAL(valueRangeChanged(QString)), this, SLOT(changeValueRange(QString)));
	m_childItems.append(m_rawdataTop);

	// add default grid, if this gridtype is not optional
	if (! type->isOptional()){
		QString zonename;
		if (type->isPrimary()){
			// use the special name
			zonename = "iRICZone";
		}else{
			zonename = nextChildZonename();
		}
		PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(zonename, nextChildCaption(), this);
		connect(cond->gridDataItem(), SIGNAL(gridRelatedConditionChanged(QString)), this, SLOT(changeValueRange(QString)));
		m_conditions.append(cond);
		m_childItems.append(cond);
	}

	// setup action items.
	m_addNewGridAction = new QAction(tr("Add New Grid"), this);
	connect(m_addNewGridAction, SIGNAL(triggered()), this, SLOT(addNewCondition()));
	updateNewGridActionStatus();
}

PreProcessorGridTypeDataItem::~PreProcessorGridTypeDataItem()
{
	QMap<QString, ScalarsToColorsContainer*>::iterator it;
	for (it = m_scalarsToColors.begin(); it != m_scalarsToColors.end(); ++it){
		delete it.value();
	}
}

const QString& PreProcessorGridTypeDataItem::name() const
{
	return m_gridType->name();
}

PreProcessorGridAndGridCreatingConditionDataItemInterface* PreProcessorGridTypeDataItem::condition(const QString& zonename) const
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::const_iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
		if ((*it)->zoneName() == zonename){return *it;}
	}
	return 0;
}

bool PreProcessorGridTypeDataItem::isChildDeletable(PreProcessorGridAndGridCreatingConditionDataItemInterface* /*child*/) const
{
	// if this gridtype is not optional and there is only one
	// condition, this item is not deletable.
	if (! m_gridType->isOptional() && m_conditions.count() == 1){
		return false;
	}
	return true;
}

void PreProcessorGridTypeDataItem::addCustomMenuItems(QMenu* menu)
{
	if (m_gridType->multiple()){
		menu->addAction(m_addNewGridAction);
	}else{
		if (m_conditions.count() == 0){
			menu->addAction(m_addNewGridAction);
		}
	}
}

void PreProcessorGridTypeDataItem::addNewCondition()
{
	PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
	// In case there is only one grid type, add the grid creating condition item as root item.
	int row = standardItem()->row();
	if (row == - 1){
		standardItem()->takeRow(cond->standardItem()->row());
		// find the background image row.
		PreProcessorRootDataItem* rootItem = dynamic_cast<PreProcessorRootDataItem*>(parent());
		int imgRow = rootItem->backgroundImagesDataItem()->standardItem()->row();
		dataModel()->itemModel()->insertRow(imgRow, cond->standardItem());
	}
	connect(cond->gridDataItem(), SIGNAL(gridRelatedConditionChanged(QString)), this, SLOT(changeValueRange(QString)));
	m_conditions.append(cond);
	m_childItems.append(cond);
	updateNewGridActionStatus();
	updateItemMap();
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

const QString PreProcessorGridTypeDataItem::nextChildCaption()
{
	bool ok = true;
	// first, try "Region".
	QString nomination(tr("Region"));
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
		ok = ok && ((*it)->caption() != nomination);
	}
	if (ok){return nomination;}
	nomination = tr("Region%1");
	QString nom;
	ok = false;
	int i = 2;
	while (! ok){
		nom = nomination.arg(i);
		ok = true;
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
		for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
			ok = ok && ((*it)->caption() != nom);
		}
		++i;
	}
	return nom;
}

const QString PreProcessorGridTypeDataItem::nextChildZonename()
{
	QString nametemplate(m_gridType->name());
	nametemplate.append("%1");
	QString nom;
	bool ok = false;
	int i = 1;
	while (! ok){
		nom = nametemplate.arg(i);
		ok = true;
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
		for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
			ok = ok && ((*it)->zoneName() != nom);
		}
		++i;
	}
	return nom;

}

void PreProcessorGridTypeDataItem::unregisterChild(GraphicsWindowDataItem* child)
{
	PreProcessorDataItem::unregisterChild(child);
	QList <PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
		if (*it == child){
			m_conditions.erase(it);
			return;
		}
	}
	updateNewGridActionStatus();
}

bool PreProcessorGridTypeDataItem::isChildCaptionAvailable(const QString& caption)
{
	QList <PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
		if ((*it)->caption() == caption){
			return false;
		}
	}
	return true;
}

void PreProcessorGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	// load raw data.
	QDomNode rdNode = iRIC::getChildNode(node, "RawData");
	if (! rdNode.isNull()){m_rawdataTop->loadFromProjectMainFile(rdNode);}
	// load region datas.
	QDomNode c = node.firstChild();
	while (! c.isNull()){
		if (c.nodeName() == "Region"){
			// find whether a corresponding condition already exists.
			QList <PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
			bool found = false;
			for (it = m_conditions.begin(); ! found && it != m_conditions.end(); ++it){
				if ((*it)->zoneName() == c.toElement().attribute("zoneName")){
					// found!
					found = true;
					(*it)->loadFromProjectMainFile(c);
				}
			}
			if (! found){
				PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
				cond->loadFromProjectMainFile(c);
				int row = standardItem()->row();
				if (row == - 1){
					standardItem()->takeRow(cond->standardItem()->row());
					// find the background image row.
					PreProcessorRootDataItem* rootItem = dynamic_cast<PreProcessorRootDataItem*>(parent());
					int imgRow = rootItem->backgroundImagesDataItem()->standardItem()->row();
					dataModel()->itemModel()->insertRow(imgRow, cond->standardItem());
				}
				connect(cond->gridDataItem(), SIGNAL(gridRelatedConditionChanged(QString)), this, SLOT(changeValueRange(QString)));
				m_conditions.append(cond);
				m_childItems.append(cond);
			}
		}
		c = c.nextSibling();
	}
}

void PreProcessorGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// write name.
	writer.writeAttribute("name", name());
	// write raw data.
	writer.writeStartElement("RawData");
	m_rawdataTop->saveToProjectMainFile(writer);
	writer.writeEndElement();
	// write region datas.
	QList <PreProcessorGridAndGridCreatingConditionDataItemInterface*>::iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it){
		writer.writeStartElement("Region");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridTypeDataItem::setupScalarsToColors(SolverDefinitionGridType* type)
{
	QList<SolverDefinitionGridRelatedCondition*>::iterator it;
	QList<SolverDefinitionGridRelatedCondition*> conditions = type->gridRelatedConditions();
	QList<SolverDefinitionGridRelatedComplexCondition*>::iterator it2;
	QList<SolverDefinitionGridRelatedComplexCondition*> conditions2 = type->gridRelatedComplexConditions();
	for (it = conditions.begin(); it != conditions.end(); ++it){
		ScalarsToColorsContainer* c = (*it)->createScalarsToColorsContainer(0);
		m_scalarsToColors.insert((*it)->name(), c);
	}
	for (it2 = conditions2.begin(); it2 != conditions2.end(); ++it2){
		ScalarsToColorsContainer* c = (*it2)->createScalarsToColorsContainer(0);
		m_scalarsToColors.insert((*it2)->name(), c);
	}
}

void PreProcessorGridTypeDataItem::changeValueRange(const QString& name)
{
	// The value range of the specified grid related condition is changed.
	// Check the new min and max values.
	bool valueExist = false;
	double min = 0;
	double max = 0;
	// check raw data
	PreProcessorRawDataGroupDataItemInterface* i = m_rawdataTop->groupDataItem(name);
	if (i == 0){return;}
	valueExist = i->getValueRange(&min, &max);

	// check grid values.
	for (auto it = m_conditions.begin(); it != m_conditions.end(); ++it){
		Grid* g = (*it)->gridDataItem()->grid();
		if (g != 0){
			GridRelatedConditionContainer* c = g->gridRelatedCondition(name);
			double tmpmin, tmpmax;
			if (c->getValueRange(&tmpmin, &tmpmax)){
				if (tmpmin < min || (! valueExist)){min = tmpmin;}
				if (tmpmax > max || (! valueExist)){max = tmpmax;}
				valueExist = true;
			}
		}
	}
	if (max - min < 1E-4){
		// the width is too small.
		double mid = (min + max) * 0.5;
		double width = mid * 0.01;
		if (width < 1E-4){
			width = 1E-4;
		}
		min = mid - width;
		max = mid + width;
	}

	// Now, new min, max are stored.
	m_scalarsToColors.value(name)->setValueRange(min, max);
	renderGraphicsView();
}

void PreProcessorGridTypeDataItem::assignActionZValues(const ZDepthRange& range)
{
	if (m_childItems.count() == 0){return;}

	/// assign ZDepthRanges to child items.
	double max = range.max();
	double rangeWidth = range.width();
	double divNum = 0;
	divNum += m_childItems.count() - 1;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		int itemCount = ((*it)->zDepthRange().itemCount() - 1);
		if (itemCount >= 0){
			divNum += itemCount;
		}
	}
	double divWidth = rangeWidth / divNum;
	// assign regions to GridAndGridCreatingConditionDataItem instances.
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit){
		int itemCount = ((*cit)->zDepthRange().itemCount() - 1);
		int itemCount2 = 0;
		if (itemCount >= 0){
			itemCount2 = itemCount;
		}
		double min = max - itemCount2 * divWidth;
		if (min < range.min()){min = range.min();}
		ZDepthRange r = (*cit)->zDepthRange();
		r.setMin(min);
		r.setMax(max);
		(*cit)->setZDepthRange(r);
		max = min - divWidth;
	}
	// rawdata gets the lowest region.
	ZDepthRange r = m_rawdataTop->zDepthRange();
	r.setMin(range.min());
	r.setMax(max);
	m_rawdataTop->setZDepthRange(r);
}

void PreProcessorGridTypeDataItem::updateNewGridActionStatus()
{
	bool enable = (m_gridType->multiple() || (m_conditions.count() == 0));
	m_addNewGridAction->setEnabled(enable);
}

bool PreProcessorGridTypeDataItem::gridEdited() const
{
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit){
		bool edited = (*cit)->gridEdited();
		if (edited){return true;}
	}
	return false;
}

void PreProcessorGridTypeDataItem::toggleGridEditFlag()
{
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit){
		(*cit)->toggleGridEditFlag();
	}
}
