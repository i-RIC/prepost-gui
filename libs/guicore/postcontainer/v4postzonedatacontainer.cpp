#include "../grid/v4grid.h"
#include "../pre/grid/v4inputgrid.h"
#include "../pre/grid/v4inputgridio.h"
#include "v4solutiongrid.h"
#include "v4solutiongridio.h"
#include "v4postcalculatedresult.h"
#include "v4postzonedatacontainer.h"
#include "private/v4postzonedatacontainer_impl.h"

#include <h5cgnsbase.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

v4PostZoneDataContainer::v4PostZoneDataContainer(const std::string& zoneName, SolverDefinitionGridType* gridType, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	impl {new Impl {}}
{
	impl->m_zoneName = zoneName;
	impl->m_gridType = gridType;
}

v4PostZoneDataContainer::~v4PostZoneDataContainer()
{}

const std::string& v4PostZoneDataContainer::zoneName() const
{
	return impl->m_zoneName;
}

QString v4PostZoneDataContainer::caption() const
{
	return impl->m_zoneName.c_str();
}

SolverDefinitionGridType* v4PostZoneDataContainer::gridType() const
{
	return impl->m_gridType;
}

v4InputGrid* v4PostZoneDataContainer::inputGridData() const
{
	return impl->m_inputGridData;
}

v4SolutionGrid* v4PostZoneDataContainer::gridData() const
{
	return impl->m_gridData;
}

v4SolutionGrid* v4PostZoneDataContainer::particleData() const
{
	return impl->m_particleData;
}

v4SolutionGrid* v4PostZoneDataContainer::particleGroup(const std::string& name) const
{
	auto it = impl->m_particleGroupMap.find(name);
	if (it == impl->m_particleGroupMap.end()) {return nullptr;}

	return it->second;
}

const std::map<std::string, v4SolutionGrid*> v4PostZoneDataContainer::particleGroupMap() const
{
	return impl->m_particleGroupMap;
}

v4SolutionGrid* v4PostZoneDataContainer::polyData(const std::string& name) const
{
	auto it = impl->m_polyDataMap.find(name);
	if (it == impl->m_polyDataMap.end()) {return nullptr;}

	return it->second;
}

const std::map<std::string, v4SolutionGrid*> v4PostZoneDataContainer::polyDataMap() const
{
	return impl->m_polyDataMap;
}

int v4PostZoneDataContainer::loadFromCgnsFile(iRICLib::H5CgnsZone* zone, PreProcessorGridTypeDataItemI* gtItem, const QString tmpPath, bool disableCalculatedResult)
{
	impl->clearParticleDataAndPolyData();

	int ier;
	if (impl->m_inputGridData == nullptr && zone->base()->dimension() == 2 && gtItem != nullptr) {
		impl->m_inputGridData = v4InputGridIO::load(*zone, gtItem, tmpPath, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	if (impl->m_gridData == nullptr) {
		impl->m_gridData = v4SolutionGridIO::loadGrid(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
	} else {
		v4SolutionGridIO::loadGrid(impl->m_gridData, zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	double dim = zone->base()->dimension();

	if (dim == 2) {
		impl->m_particleData = v4SolutionGridIO::loadParticles2d(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
		impl->m_particleGroupMap = v4SolutionGridIO::loadParticleGroups2d(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
		impl->m_polyDataMap = v4SolutionGridIO::loadPolyDataGroup2d(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
	} else if (dim == 3) {
		impl->m_particleData = v4SolutionGridIO::loadParticles3d(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
		impl->m_particleGroupMap = v4SolutionGridIO::loadParticleGroups3d(gridType(), zone, offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	if (! disableCalculatedResult) {
		impl->addCalculatedResultArrays();
	}

	return IRIC_NO_ERROR;
}

int v4PostZoneDataContainer::loadIfEmpty(iRICLib::H5CgnsZone* zone, PreProcessorGridTypeDataItemI* gtItem, const QString tmpPath)
{
	if (impl->m_gridData != nullptr) {return IRIC_NO_ERROR;}

	return loadFromCgnsFile(zone, gtItem, tmpPath, false);
}

void v4PostZoneDataContainer::applyOffset(const QPointF& offset)
{
	if (impl->m_inputGridData != nullptr) {impl->m_inputGridData->grid()->applyOffset(offset);}
	if (impl->m_gridData != nullptr) {impl->m_gridData->grid()->applyOffset(offset);}
	if (impl->m_particleData != nullptr) {impl->m_particleData->grid()->applyOffset(offset);}
	for (auto pair : impl->m_particleGroupMap) {
		pair.second->grid()->applyOffset(offset);
	}
	for (auto pair : impl->m_polyDataMap) {
		pair.second->grid()->applyOffset(offset);
	}
}

std::vector<v4PostCalculatedResult*>& v4PostZoneDataContainer::calculatedResults()
{
	return impl->m_calculatedResults;
}

const std::vector<v4PostCalculatedResult*>& v4PostZoneDataContainer::calculatedResults() const
{
	return impl->m_calculatedResults;
}

std::vector<v4PostCalculatedResult*> v4PostZoneDataContainer::detachCalculatedResult()
{
	std::vector<v4PostCalculatedResult*> ret;

	for (auto r : impl->m_calculatedResults) {
		r->setParent(nullptr);
		ret.push_back(r);
	}
	impl->m_calculatedResults.clear();

	return ret;
}

void v4PostZoneDataContainer::attachCalculatedResult(std::vector<v4PostCalculatedResult*> results)
{
	for (auto r : results) {
		r->setParent(this);
		impl->m_calculatedResults.push_back(r);
	}
}

void v4PostZoneDataContainer::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (int i = 0; i < node.childNodes().size(); ++i) {
		auto childNode = node.childNodes().at(i);
		if (childNode.nodeName() == "SimpleOperationResult") {
			auto cr = new v4PostCalculatedResult(this);
			cr->loadFromProjectMainFile(childNode);
			impl->m_calculatedResults.push_back(cr);
		}
	}
}

void v4PostZoneDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", impl->m_zoneName.c_str());

	for (auto result : impl->m_calculatedResults) {
		writer.writeStartElement("SimpleOperationResult");
		result->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
