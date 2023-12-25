#include "../../../pre/base/preprocessorgriddataitemi.h"
#include "../../../pre/base/preprocessorgridtypedataitemi.h"
#include "../../../pre/base/preprocessorgeodatagroupdataitemi.h"
#include "../../../pre/base/preprocessorgeodatatopdataitemi.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "../../../grid/v4grid.h"
#include "../../../grid/v4structured2dgrid.h"
#include "gridattributecontainer.h"
#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionscontainer.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <misc/filesystemfunction.h>

#include <QDir>

GridAttributeContainer::GridAttributeContainer(v4InputGrid* grid, SolverDefinitionGridAttribute* cond) :
	GridAttributeBaseObject(cond),
	m_grid {grid},
	m_dimensions {nullptr},
	m_temporaryDir {},
	m_mapped {false},
	m_isCustomModified {false}
{}

GridAttributeContainer::~GridAttributeContainer()
{}

const std::string& GridAttributeContainer::name() const
{
	return gridAttribute()->name();
}

v4InputGrid* GridAttributeContainer::grid() const
{
	return m_grid;
}

GridAttributeDimensionsContainer* GridAttributeContainer::dimensions() const
{
	return m_dimensions;
}

void GridAttributeContainer::setDimensions(GridAttributeDimensionsContainer* dims)
{
	m_dimensions = dims;
	connect<void (GridAttributeDimensionsContainer::*)(int, int)>(dims, &GridAttributeDimensionsContainer::currentIndexChanged, this, &GridAttributeContainer::handleDimensionCurrentIndexChange);
	for (auto cont : dims->containers()) {
		connect<void (GridAttributeDimensionContainer::*)(const std::vector<QVariant>&, const std::vector<QVariant>&)>(cont, &GridAttributeDimensionContainer::valuesChanged, this, &GridAttributeContainer::handleDimensionValuesChange);
	}
}

QString GridAttributeContainer::temporaryDir() const
{
	return m_temporaryDir;
}

void GridAttributeContainer::setTemporaryDir(const QString& dir)
{
	m_temporaryDir = dir;
}

void GridAttributeContainer::clearTemporaryData()
{
	int index = 0;
	while (true) {
		auto fname = temporaryExternalFilename(index);
		QFile f(fname);
		if (! f.exists()) {return;}

		f.remove();
		++ index;
	}
}

unsigned int GridAttributeContainer::dataCount() const
{
	if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::Node) {
		return grid()->grid()->nodeCount();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		return grid()->grid()->cellCount();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::IFace) {
		auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		return sgrid->iEdgeCount();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::JFace) {
		auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		return sgrid->jEdgeCount();
	}
	return 0; // this never happens. only for avoiding compiler warning
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

void GridAttributeContainer::updateValueRange()
{
	auto data = grid()->grid()->vtkData();
	auto pos = gridAttribute()->position();
	if (pos == SolverDefinitionGridAttribute::Position::Node) {
		data->updatePointValueRange(name());
	} else if (pos == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->updateCellValueRange(name());
	} else if (pos == SolverDefinitionGridAttribute::Position::IFace) {
		auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		sGrid->vtkIEdgeData()->updateCellValueRange(name());
	} else if (pos == SolverDefinitionGridAttribute::Position::JFace) {
		auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		sGrid->vtkJEdgeData()->updateCellValueRange(name());
	}
}

void GridAttributeContainer::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	if (oldIndex == newIndex) {return;}

	QString fname = temporaryExternalFilename(oldIndex);

	QFileInfo finfo(fname);
	iRIC::mkdirRecursively(finfo.absolutePath());
	saveToExternalFile(fname);

	fname = temporaryExternalFilename(newIndex);
	loadFromExternalFile(fname);
	setModified();

	emit m_grid->grid()->changed();
}

void GridAttributeContainer::handleDimensionValuesChange(const std::vector<QVariant> & /*before*/, const std::vector<QVariant> & /*after*/)
{
	QString fname = temporaryExternalFilename(dimensions()->currentIndex());
	saveToExternalFile(fname);
}

vtkDataSetAttributes* GridAttributeContainer::vtkAttributes() const
{
	if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::Node) {
		return grid()->grid()->vtkData()->data()->GetPointData();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		return grid()->grid()->vtkData()->data()->GetCellData();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::IFace) {
		auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		return sgrid->vtkIEdgeData()->data()->GetCellData();
	} else if (gridAttribute()->position() == SolverDefinitionGridAttribute::Position::JFace) {
		auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid()->grid());
		return sgrid->vtkJEdgeData()->data()->GetCellData();
	}
	return nullptr; // this never happens. only for avoiding compiler warning
}

QString GridAttributeContainer::temporaryExternalFilename(int index) const
{
	QString format("%1_%2.dat");
	QString filename = format.arg(gridAttribute()->name().c_str()).arg(index + 1);

	QDir subDir(m_temporaryDir);
	return subDir.absoluteFilePath(filename);
}
