#include "../../../pre/base/preprocessorgriddataiteminterface.h"
#include "../../../pre/base/preprocessorgridtypedataiteminterface.h"
#include "../../../pre/base/preprocessorgeodatagroupdataiteminterface.h"
#include "../../../pre/base/preprocessorgeodatatopdataiteminterface.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "gridattributecontainer.h"
#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionscontainer.h"

#include <misc/filesystemfunction.h>

#include <QDir>

GridAttributeContainer::GridAttributeContainer(Grid* grid, SolverDefinitionGridAttribute* cond)
	: GridAttributeBaseObject(cond, grid)
{
	m_grid = grid;
	m_mapped = false;
	m_isCustomModified = false;
}

const QString& GridAttributeContainer::name() const
{
	return condition()->name();
}

GridAttributeDimensionsContainer* GridAttributeContainer::dimensions() const
{
	ProjectDataItem* item = m_grid->parent();
	if (item == 0) {return 0;}
	item = item->parent();
	if (item == 0) {return 0;}
	item = item->parent();
	if (item == 0) {return 0;}
	PreProcessorGridTypeDataItemInterface* gtItem =
		dynamic_cast<PreProcessorGridTypeDataItemInterface*>(item);
	return gtItem->geoDataTop()->groupDataItem(name())->dimensions();
}

const QString GridAttributeContainer::temporaryExternalFilename(int index) const
{
	ProjectDataItem* item = m_grid->parent();
	PreProcessorGridDataItemInterface* gItem = dynamic_cast<PreProcessorGridDataItemInterface*>(item);
	QString format("%1_%2.dat");
	QString filename = format.arg(m_condition->name()).arg(index + 1);
	QDir subDir(gItem->subPath());
	return subDir.absoluteFilePath(filename);
}

void GridAttributeContainer::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	QString fname = temporaryExternalFilename(oldIndex);
	QFileInfo finfo(fname);
	iRIC::mkdirRecursively(finfo.absolutePath());
	saveToExternalFile(fname);
	fname = temporaryExternalFilename(newIndex);
	loadFromExternalFile(fname);
	setModified();
	PreProcessorGridDataItemInterface* gItem =
		dynamic_cast<PreProcessorGridDataItemInterface*>(m_grid->parent());
	gItem->updateSimplifiedGrid();
}

void GridAttributeContainer::handleDimensionValuesChange(const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/)
{
	QString fname = temporaryExternalFilename(dimensions()->currentIndex());
	saveToExternalFile(fname);

	// @todo not implemented yet!
}

void GridAttributeContainer::updateConnections()
{
	GridAttributeDimensionsContainer* dims = dimensions();
	connect(dims, SIGNAL(currentIndexChanged(int,int)), this, SLOT(handleDimensionCurrentIndexChange(int,int)));
	const QList<GridAttributeDimensionContainer*>& conts = dims->containers();
	for (int i = 0; i < conts.size(); ++i) {
		GridAttributeDimensionContainer* cont = conts.at(i);
		connect(cont, SIGNAL(valuesChanged(QList<QVariant>,QList<QVariant>)), this, SLOT(handleDimensionValuesChange(QList<QVariant>,QList<QVariant>)));
	}
}
