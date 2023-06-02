#include "../../project/projectcgnsfile.h"
#include "../../solverdef/solverdefinitiongridattribute.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "grid.h"
#include "private/grid_impl.h"

#include <guibase/vtktool/vtkpointsetregionandcellsizefilter.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <misc/rectregion.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>

#include <vtkAbstractPointLocator.h>
#include <vtkGeometryFilter.h>
#include <vtkMaskPoints.h>
#include <vtkMaskPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <iriclib_errorcodes.h>

const int Grid::MAX_DRAWCELLCOUNT = 40000;
const int Grid::MAX_DRAWINDEXCOUNT = 300;
const char Grid::LABEL_NAME[] = "_LABEL";

Grid::Impl::Impl(vtkPointSet* ps, const std::string& zoneName, SolverDefinitionGridType::GridType gt) :
	m_gridType {gt},
	m_zoneName (zoneName),
	m_vtkGrid {ps},
	m_vtkFilteredGrid {nullptr},
	m_vtkFilteredIndexGrid {nullptr},
	m_pointLocator {nullptr},
	m_dataItem {nullptr},
	m_isModified {false},
	m_isMasked {false}
{}

Grid::Impl::~Impl()
{
	if (m_vtkGrid != nullptr) {
		m_vtkGrid->Delete();
	}
	if (m_vtkFilteredGrid != nullptr) {
		m_vtkFilteredGrid->Delete();
	}
	if (m_vtkFilteredIndexGrid != nullptr) {
		m_vtkFilteredIndexGrid->Delete();
	}
	if (m_pointLocator != nullptr) {
		m_pointLocator->Delete();
	}
}

// public interfaces

Grid::Grid(vtkPointSet* ps, const std::string& zoneName, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	ProjectDataItem(parent),
	impl {new Impl(ps, zoneName, type)}
{}

Grid::Grid(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, "", type, parent)
{}

Grid::~Grid()
{
	delete impl;
}

SolverDefinitionGridType::GridType Grid::gridType() const
{
	return impl->m_gridType;
}

const std::string& Grid::zoneName() const
{
	return impl->m_zoneName;
}

void Grid::setZoneName(const std::string& name)
{
	impl->m_zoneName = name;
}

PreProcessorGridDataItemInterface* Grid::dataItem() const
{
	return impl->m_dataItem;
}

void Grid::setDataItem(PreProcessorGridDataItemInterface* item)
{
	impl->m_dataItem = item;
}

void Grid::setParent(QObject* parent)
{
	QObject::setParent(parent);
	for (int i = 0; i < impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = impl->m_gridAttributes[i];
		c->updateConnections();
	}
}

vtkPointSet* Grid::vtkGrid() const
{
	return impl->m_vtkGrid;
}

vtkAbstractPointLocator* Grid::pointLocator() const
{
	return impl->m_pointLocator;
}

vtkPointSet* Grid::vtkFilteredGrid() const
{
	return impl->m_vtkFilteredGrid;
}

vtkPointSet* Grid::vtkFilteredIndexGrid() const
{
	return impl->m_vtkFilteredIndexGrid;
}

void Grid::setPoints(vtkPoints* points)
{
	impl->m_vtkGrid->SetPoints(points);
	setModified();

	auto v = impl->m_dataItem->dataModel()->graphicsView();

	double xmin, xmax, ymin, ymax;
	v->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	updateSimplifiedGrid(xmin, xmax, ymin, ymax);
}

QList<GridAttributeContainer*>& Grid::gridAttributes()
{
	return impl->m_gridAttributes;
}

GridAttributeContainer* Grid::gridAttribute(const std::string& name) const
{
	return impl->m_gridAttributeNameMap.value(name);
}

void Grid::addGridAttribute(GridAttributeContainer* cond)
{
	impl->m_gridAttributes.append(cond);
	impl->m_gridAttributeNameMap.insert(cond->name(), cond);
}

bool Grid::hasGeneratingAttributes() const
{
	bool has = false;
	for (GridAttributeContainer* att : impl->m_gridAttributes) {
		SolverDefinitionGridAttribute* def = att->gridAttribute();
		has = has || (! def->mapping().isEmpty());
	}
	return has;
}

unsigned int Grid::nodeCount() const
{
	return impl->m_vtkGrid->GetNumberOfPoints();
}

bool Grid::isModified() const
{
	return impl->m_isModified;
}

void Grid::setModified(bool modified)
{
	ProjectDataItem::setModified(modified);
	impl->m_isModified = modified;
	if (modified) {
		impl->m_vtkGrid->Modified();
	}
}

const QStringList Grid::checkShape(QTextStream&)
{
	QStringList ret;
	return ret;
}

bool Grid::isValid(QTextStream&) const
{
	return true;
}

bool Grid::isCustomModified()
{
	bool modified = false;
	for (int i = 0; i < impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = impl->m_gridAttributes[i];
		modified = modified || c->isCustomModified();
	}
	return modified;
}

void Grid::setCustomModified(bool modified)
{
	for (int i = 0; i < impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = impl->m_gridAttributes[i];
		c->setCustomModified(modified);
	}
}

void Grid::updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	int maxcells = -1;
	if (cullEnable) {
		maxcells = cullCellLimit;
	}
	RectRegion region(xmin, xmax, ymin, ymax);

	auto filtered = vtkPointSetRegionAndCellSizeFilter::filterGeneral(impl->m_vtkGrid, region, maxcells, &impl->m_isMasked);

	setFilteredGrid(filtered);
	filtered->Delete();
}

bool Grid::isMasked() const
{
	return impl->m_isMasked;
}

void Grid::setMasked(bool masked)
{
	impl->m_isMasked = masked;
}

void Grid::setFilteredGrid(vtkPointSet* data)
{
	if (impl->m_vtkFilteredGrid != nullptr) {
		impl->m_vtkFilteredGrid->Delete();
	}

	impl->m_vtkFilteredGrid = data;
	impl->m_vtkFilteredGrid->Register(nullptr);
}

void Grid::setFilteredIndexGrid(vtkPointSet* data)
{
	if (impl->m_vtkFilteredIndexGrid != nullptr) {
		impl->m_vtkFilteredIndexGrid->Delete();
	}

	impl->m_vtkFilteredIndexGrid = data;
	impl->m_vtkFilteredIndexGrid->Register(nullptr);
}

void Grid::setPointLocator(vtkAbstractPointLocator* locator)
{
	impl->m_pointLocator = locator;
	locator->SetDataSet(impl->m_vtkGrid);
	if (impl->m_vtkGrid->GetNumberOfPoints() > 0) {
		locator->BuildLocator();
	}
}

void Grid::doLoadFromProjectMainFile(const QDomNode&)
{}

void Grid::doSaveToProjectMainFile(QXmlStreamWriter&)
{}

void Grid::getCullSetting(bool* enable, int* cellLimit, int* indexLimit)
{
	QSettings settings;
	*enable = settings.value("general/cullcellenable", true).toBool();
	*cellLimit = settings.value("general/cullcelllimit", Grid::MAX_DRAWCELLCOUNT).toInt();
	*indexLimit = settings.value("general/cullindexlimit", Grid::MAX_DRAWINDEXCOUNT).toInt();
}

int Grid::loadGridAttributes(const iRICLib::H5CgnsGridAttributes& atts)
{
	for (auto att : impl->m_gridAttributes) {
		att->allocate();
		int ier = att->loadFromCgnsFile(atts);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	return IRIC_NO_ERROR;
}

int Grid::saveGridAttributes(iRICLib::H5CgnsGridAttributes* atts)
{
	for (auto att : impl->m_gridAttributes) {
		int ier = att->saveToCgnsFile(atts);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	return IRIC_NO_ERROR;
}
