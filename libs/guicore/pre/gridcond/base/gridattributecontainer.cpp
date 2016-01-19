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

GridAttributeContainer::GridAttributeContainer(Grid* grid, SolverDefinitionGridAttribute* cond) :
	GridAttributeBaseObject(cond, grid),
	m_grid {grid},
	m_mapped {false},
	m_isCustomModified {false}
{}

GridAttributeContainer::~GridAttributeContainer()
{}

const std::string& GridAttributeContainer::name() const
{
	return gridAttribute()->name();
}

Grid* GridAttributeContainer::grid() const
{
	return m_grid;
}

GridAttributeDimensionsContainer* GridAttributeContainer::dimensions() const
{
	ProjectDataItem* item = m_grid->parent();
	if (item == 0) {return nullptr;}
	item = item->parent();
	if (item == 0) {return nullptr;}
	item = item->parent();
	if (item == 0) {return nullptr;}
	PreProcessorGridTypeDataItemInterface* gtItem =
		dynamic_cast<PreProcessorGridTypeDataItemInterface*>(item);
	return gtItem->geoDataTop()->groupDataItem(name())->dimensions();
}

bool GridAttributeContainer::mapped() const
{
	return m_mapped;
}

void GridAttributeContainer::setMapped(bool mapped)
{
	m_mapped = mapped;
	setModified();
}

bool GridAttributeContainer::isCustomModified() const
{
	return m_isCustomModified;
}

void GridAttributeContainer::setCustomModified(bool c)
{
	m_isCustomModified = c;
}

void GridAttributeContainer::updateConnections()
{
	GridAttributeDimensionsContainer* dims = dimensions();
	connect(dims, SIGNAL(currentIndexChanged(int,int)), this, SLOT(handleDimensionCurrentIndexChange(int,int)));
	const QList<GridAttributeDimensionContainer*>& conts = dims->containers();
	for (int i = 0; i < conts.size(); ++i) {
		GridAttributeDimensionContainer* cont = conts.at(i);
		connect(cont, SIGNAL(valuesChanged(std::vector<QVariant>,std::vector<QVariant>)), this, SLOT(handleDimensionValuesChange(std::vector<QVariant>,std::vector<QVariant>)));
	}
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

void GridAttributeContainer::handleDimensionValuesChange(const std::vector<QVariant> & /*before*/, const std::vector<QVariant> & /*after*/)
{
	QString fname = temporaryExternalFilename(dimensions()->currentIndex());
	saveToExternalFile(fname);

	// @todo not implemented yet!
}

QString GridAttributeContainer::temporaryExternalFilename(int index) const
{
	ProjectDataItem* item = m_grid->parent();
	PreProcessorGridDataItemInterface* gItem = dynamic_cast<PreProcessorGridDataItemInterface*>(item);
	QString format("%1_%2.dat");
	QString filename = format.arg(gridAttribute()->name().c_str()).arg(index + 1);
	QDir subDir(gItem->subPath());
	return subDir.absoluteFilePath(filename);
}
