#include "../../../solverdef/solverdefinitiongridrelatedcondition.h"
#include "gridrelatedconditioncontainer.h"
#include "../../../pre/base/preprocessorrawdatagroupdataiteminterface.h"
#include "../../../pre/base/preprocessorgriddataiteminterface.h"
#include "../../../pre/base/preprocessorgridtypedataiteminterface.h"
#include "../../../pre/base/preprocessorrawdatatopdataiteminterface.h"
#include "gridrelatedconditiondimensionscontainer.h"
#include "gridrelatedconditiondimensioncontainer.h"
#include <misc/filesystemfunction.h>

#include <QDir>

GridRelatedConditionContainer::GridRelatedConditionContainer(Grid* grid, SolverDefinitionGridRelatedCondition* cond)
	: GridRelatedConditionBaseObject(cond, grid)
{
	m_grid = grid;
	m_mapped = false;
	m_isCustomModified = false;
}

const QString& GridRelatedConditionContainer::name() const
{
	return condition()->name();
}

GridRelatedConditionDimensionsContainer* GridRelatedConditionContainer::dimensions() const
{
	ProjectDataItem *item = m_grid->parent();
	item = item->parent();
	item = item->parent();
	PreProcessorGridTypeDataItemInterface* gtItem =
			dynamic_cast<PreProcessorGridTypeDataItemInterface*>(item);
	return gtItem->rawdataTop()->groupDataItem(name())->dimensions();
}

const QString GridRelatedConditionContainer::temporaryExternalFilename(int index) const
{
	ProjectDataItem* item = m_grid->parent();
	PreProcessorGridDataItemInterface* gItem = dynamic_cast<PreProcessorGridDataItemInterface*> (item);
	QString format("%1_%2.dat");
	QString filename = format.arg(m_condition->name()).arg(index + 1);
	QDir subDir(gItem->subPath());
	return subDir.absoluteFilePath(filename);
}

void GridRelatedConditionContainer::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	QString fname = temporaryExternalFilename(oldIndex);
	QFileInfo finfo(fname);
	iRIC::mkdirRecursively(finfo.absolutePath());
	saveToExternalFile(fname);
	fname = temporaryExternalFilename(newIndex);
	loadFromExternalFile(fname);
	setModified();
	PreProcessorGridDataItemInterface* gItem =
			dynamic_cast<PreProcessorGridDataItemInterface*> (m_grid->parent());
	gItem->updateSimplefiedGrid();
}

void GridRelatedConditionContainer::handleDimensionValuesChange(const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/)
{
	QString fname = temporaryExternalFilename(dimensions()->currentIndex());
	saveToExternalFile(fname);

	// @todo not implemented yet!
}

void GridRelatedConditionContainer::updateConnections()
{
	GridRelatedConditionDimensionsContainer* dims = dimensions();
	connect(dims, SIGNAL(currentIndexChanged(int,int)), this, SLOT(handleDimensionCurrentIndexChange(int,int)));
	const QList<GridRelatedConditionDimensionContainer*>& conts = dims->containers();
	for (int i = 0; i < conts.size(); ++i){
	GridRelatedConditionDimensionContainer * cont = conts.at(i);
		connect(cont, SIGNAL(valuesChanged(QList<QVariant>,QList<QVariant>)), this, SLOT(handleDimensionValuesChange(QList<QVariant>,QList<QVariant>)));
	}
}
