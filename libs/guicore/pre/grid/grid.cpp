#include "../../project/projectcgnsfile.h"
#include "../../solverdef/solverdefinitiongridattribute.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "grid.h"
#include "private/grid_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>

#include <vtkGeometryFilter.h>
#include <vtkMaskPoints.h>
#include <vtkMaskPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <cgnslib.h>
#include <iriclib.h>

const int Grid::MAX_DRAWCELLCOUNT = 40000;
const int Grid::MAX_DRAWINDEXCOUNT = 300;
const char Grid::LABEL_NAME[] = "_LABEL";

Grid::Impl::Impl(vtkPointSet* ps, const std::string& zoneName, SolverDefinitionGridType::GridType gt) :
	m_gridType {gt},
	m_zoneName (zoneName),
	m_vtkGrid {ps},
	m_isModified {false},
	m_isMasked {false}
{}

Grid::Impl::~Impl()
{
	if (m_vtkGrid != nullptr) {
		m_vtkGrid->Delete();
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

void Grid::setParent(QObject* parent)
{
	QObject::setParent(parent);
	for (int i = 0; i < impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = impl->m_gridAttributes[i];
		c->updateConnections();
	}
}

void Grid::loadFromCgnsFile(const int fn)
{
	int B;
	// goto Base.
	cg_iRIC_GotoBase(fn, &B);

	cgsize_t sizes[9];
	int zoneid = zoneId(impl->m_zoneName, fn, B, sizes);
	if (zoneid == 0) {
		// Error. No corresponding zone found.
		return;
	}
	loadFromCgnsFile(fn, B, zoneid);
	cg_iRIC_Set_ZoneId(zoneid);
	impl->m_isModified = false;
}

void Grid::saveToCgnsFile(const int fn)
{
	// if not modified, do nothing.
	if (! impl->m_isModified) {return;}

	int B;
	// goto Base.
	cg_iRIC_GotoBase(fn, &B);
	saveToCgnsFile(fn, B, const_cast<char*>(impl->m_zoneName.c_str()));
	cgsize_t size[9];
	int Z = zoneId(impl->m_zoneName, fn, B, size);
	cg_ziter_write(fn, B, Z, "ZoneIterativeData");
	cg_iRIC_Set_ZoneId(Z);
	impl->m_isModified = false;
}

vtkPointSet* Grid::vtkGrid() const
{
	return impl->m_vtkGrid;
}

vtkAlgorithm* Grid::vtkFilteredShapeAlgorithm() const
{
	return impl->m_vtkFilteredShapeAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredPointsAlgorithm() const
{
	return impl->m_vtkFilteredPointsAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredCellsAlgorithm() const
{
	return impl->m_vtkFilteredCellsAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredIndexGridAlgorithm() const
{
	return nullptr;
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
	impl->m_isMasked = false;

	double xwidth = xmax - xmin;
	double ywidth = ymax - ymin;

	vtkSmartPointer<vtkGeometryFilter> gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
	gfilter->SetExtent(xmin - xwidth * 0.2, xmax + xwidth * 0.2, ymin - ywidth * 0.2, ymax + ywidth * 0.2, -1, 1);
	gfilter->ExtentClippingOn();
	gfilter->SetInputData(impl->m_vtkGrid);
	gfilter->Update();
	vtkSmartPointer<vtkPolyData> clippedGrid = gfilter->GetOutput();

	int ccounts = clippedGrid->GetNumberOfCells();
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	if (cullEnable && ccounts <= cullCellLimit) {
		impl->m_vtkFilteredShapeAlgorithm = gfilter;
		impl->m_vtkFilteredPointsAlgorithm = gfilter;
		impl->m_vtkFilteredCellsAlgorithm = gfilter;
		return;
	}

	vtkSmartPointer<vtkMaskPolyData> maskPoly = vtkSmartPointer<vtkMaskPolyData>::New();
	int ratio = static_cast<int>(ccounts / cullCellLimit);
	if (ratio == 1) {ratio = 2;}
	maskPoly->SetOnRatio(ratio);
	maskPoly->SetInputConnection(gfilter->GetOutputPort());

	impl->m_vtkFilteredShapeAlgorithm = maskPoly;
	impl->m_vtkFilteredPointsAlgorithm = maskPoly;
	impl->m_vtkFilteredCellsAlgorithm = maskPoly;

	impl->m_isMasked = true;
}

bool Grid::isMasked() const
{
	return impl->m_isMasked;
}

int Grid::zoneId(const std::string& zonename, int fn, int B, cgsize_t sizes[9])
{
	char zn[ProjectCgnsFile::BUFFERLEN];
	// get the number of zones;
	int nzones;
	cg_nzones(fn, B, &nzones);
	for (int zoneid = 1; zoneid <= nzones; ++zoneid) {
		cg_zone_read(fn, B, zoneid, zn, sizes);
		if (zonename == zn) {
			return zoneid;
		}
	}
	return 0;
}

void Grid::setMasked(bool masked)
{
	impl->m_isMasked = masked;
}

void Grid::setFilteredShapeAlgorithm(vtkAlgorithm* algo)
{
	impl->m_vtkFilteredShapeAlgorithm = algo;
}

void Grid::setFilteredPointsAlgorithm(vtkAlgorithm* algo)
{
	impl->m_vtkFilteredPointsAlgorithm = algo;
}

void Grid::setFilteredCellsAlgorithm(vtkAlgorithm* algo)
{
	impl->m_vtkFilteredCellsAlgorithm = algo;
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

bool Grid::loadGridAttributes(int fn, int B, int Z)
{
	// Grid coordinates are loaded.
	// Next, grid related condition data is loaded.
	bool allok = true;
	for (auto it = impl->m_gridAttributes.begin(); it != impl->m_gridAttributes.end(); ++it) {
		(*it)->allocate();
		bool ret = (*it)->loadFromCgnsFile(fn, B, Z);
		allok = allok && ret;
	}
	return allok;
}

bool Grid::saveGridAttributes(int fn, int B, int Z)
{
	// Grid coordinates are saved.
	// Next grid related condition data is saved.
	// Create "GridConditions" node under the zone node.
	cg_goto(fn, B, "Zone_t", Z, "end");
	cg_user_data_write("GridConditions");

	bool allok = true;
	for (auto it = impl->m_gridAttributes.begin(); it != impl->m_gridAttributes.end(); ++it) {
		bool ret = (*it)->saveToCgnsFile(fn, B, Z);
		allok = allok && ret;
	}
	return allok;
}
