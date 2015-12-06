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
	m_impl {new Impl(ps, zoneName, type)}
{}

Grid::Grid(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, "", type, parent)
{}

Grid::~Grid()
{
	delete m_impl;
}

SolverDefinitionGridType::GridType Grid::gridType() const
{
	return m_impl->m_gridType;
}

const std::string& Grid::zoneName() const
{
	return m_impl->m_zoneName;
}

void Grid::setZoneName(const std::string& name)
{
	m_impl->m_zoneName = name;
}

void Grid::setParent(QObject* parent)
{
	QObject::setParent(parent);
	for (int i = 0; i < m_impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = m_impl->m_gridAttributes[i];
		c->updateConnections();
	}
}

void Grid::loadFromCgnsFile(const int fn)
{
	int B;
	// goto Base.
	cg_iRIC_GotoBase(fn, &B);

	cgsize_t sizes[9];
	int zoneid = zoneId(m_impl->m_zoneName, fn, B, sizes);
	if (zoneid == 0) {
		// Error. No corresponding zone found.
		return;
	}
	loadFromCgnsFile(fn, B, zoneid);
	cg_iRIC_Set_ZoneId(zoneid);
	m_impl->m_isModified = false;
}

void Grid::saveToCgnsFile(const int fn)
{
	// if not modified, do nothing.
	if (! m_impl->m_isModified) {return;}

	int B;
	// goto Base.
	cg_iRIC_GotoBase(fn, &B);
	saveToCgnsFile(fn, B, const_cast<char*>(m_impl->m_zoneName.c_str()));
	cgsize_t size[9];
	int Z = zoneId(m_impl->m_zoneName, fn, B, size);
	cg_ziter_write(fn, B, Z, "ZoneIterativeData");
	cg_iRIC_Set_ZoneId(Z);
	m_impl->m_isModified = false;
}

vtkPointSet* Grid::vtkGrid() const
{
	return m_impl->m_vtkGrid;
}

vtkAlgorithm* Grid::vtkFilteredShapeAlgorithm() const
{
	return m_impl->m_vtkFilteredShapeAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredPointsAlgorithm() const
{
	return m_impl->m_vtkFilteredPointsAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredCellsAlgorithm() const
{
	return m_impl->m_vtkFilteredCellsAlgorithm;
}

vtkAlgorithm* Grid::vtkFilteredIndexGridAlgorithm() const
{
	return nullptr;
}

QList<GridAttributeContainer*>& Grid::gridAttributes()
{
	return m_impl->m_gridAttributes;
}

GridAttributeContainer* Grid::gridAttribute(const std::string& name) const
{
	return m_impl->m_gridAttributeNameMap.value(name);
}

void Grid::addGridAttribute(GridAttributeContainer* cond)
{
	m_impl->m_gridAttributes.append(cond);
	m_impl->m_gridAttributeNameMap.insert(cond->name(), cond);
}

unsigned int Grid::nodeCount() const
{
	return m_impl->m_vtkGrid->GetNumberOfPoints();
}

bool Grid::isModified() const
{
	return m_impl->m_isModified;
}

void Grid::setModified(bool modified)
{
	ProjectDataItem::setModified(modified);
	m_impl->m_isModified = modified;
	if (modified) {
		m_impl->m_vtkGrid->Modified();
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
	for (int i = 0; i < m_impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = m_impl->m_gridAttributes[i];
		modified = modified || c->isCustomModified();
	}
	return modified;
}

void Grid::setCustomModified(bool modified)
{
	for (int i = 0; i < m_impl->m_gridAttributes.count(); ++i) {
		GridAttributeContainer* c = m_impl->m_gridAttributes[i];
		c->setCustomModified(modified);
	}
}

void Grid::updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	m_impl->m_isMasked = false;

	double xwidth = xmax - xmin;
	double ywidth = ymax - ymin;

	vtkSmartPointer<vtkGeometryFilter> gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
	gfilter->SetExtent(xmin - xwidth * 0.2, xmax + xwidth * 0.2, ymin - ywidth * 0.2, ymax + ywidth * 0.2, -1, 1);
	gfilter->ExtentClippingOn();
	gfilter->SetInputData(m_impl->m_vtkGrid);
	gfilter->Update();
	vtkSmartPointer<vtkPolyData> clippedGrid = gfilter->GetOutput();

	int ccounts = clippedGrid->GetNumberOfCells();
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	if (cullEnable && ccounts <= cullCellLimit) {
		m_impl->m_vtkFilteredShapeAlgorithm = gfilter;
		m_impl->m_vtkFilteredPointsAlgorithm = gfilter;
		m_impl->m_vtkFilteredCellsAlgorithm = gfilter;
		return;
	}

	vtkSmartPointer<vtkMaskPolyData> maskPoly = vtkSmartPointer<vtkMaskPolyData>::New();
	int ratio = static_cast<int>(ccounts / cullCellLimit);
	if (ratio == 1) {ratio = 2;}
	maskPoly->SetOnRatio(ratio);
	maskPoly->SetInputConnection(gfilter->GetOutputPort());

	m_impl->m_vtkFilteredShapeAlgorithm = maskPoly;
	m_impl->m_vtkFilteredPointsAlgorithm = maskPoly;
	m_impl->m_vtkFilteredCellsAlgorithm = maskPoly;

	m_impl->m_isMasked = true;
}

bool Grid::isMasked() const
{
	return m_impl->m_isMasked;
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
	m_impl->m_isMasked = masked;
}

void Grid::setFilteredShapeAlgorithm(vtkAlgorithm* algo)
{
	m_impl->m_vtkFilteredShapeAlgorithm = algo;
}

void Grid::setFilteredPointsAlgorithm(vtkAlgorithm* algo)
{
	m_impl->m_vtkFilteredPointsAlgorithm = algo;
}

void Grid::setFilteredCellsAlgorithm(vtkAlgorithm* algo)
{
	m_impl->m_vtkFilteredCellsAlgorithm = algo;
}

void Grid::doLoadFromProjectMainFile(const QDomNode& node)
{}

void Grid::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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
	for (auto it = m_impl->m_gridAttributes.begin(); it != m_impl->m_gridAttributes.end(); ++it) {
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
	for (auto it = m_impl->m_gridAttributes.begin(); it != m_impl->m_gridAttributes.end(); ++it) {
		bool ret = (*it)->saveToCgnsFile(fn, B, Z);
		allok = allok && ret;
	}
	return allok;
}
