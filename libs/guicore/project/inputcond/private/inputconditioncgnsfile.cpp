#include "inputconditioncgnsfile.h"

#include <guicore/project/projectcgnsfile.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>
#include <h5cgnsflowsolution.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <string>

namespace {

int loadSolutionData(iRICLib::H5CgnsFlowSolution* solution, QStringList* realResults, QStringList* integerResults)
{
	if (solution == nullptr) {return IRIC_NO_ERROR;}

	std::vector<std::string> names;
	int ier = solution->readValueNames(&names);
	if (ier != IRIC_NO_ERROR) {return ier;}

	for (const auto& name : names) {
		iRICLib::H5Util::DataArrayValueType type;
		ier = solution->readValueType(name, &type);
		if (ier != IRIC_NO_ERROR) {return ier;}

		if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			realResults->push_back(name.c_str());
		} else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			integerResults->push_back(name.c_str());
		}
	}
	return IRIC_NO_ERROR;
}

} // namespace

InputConditionCgnsFile::InputConditionCgnsFile() :
	m_fileName {},
	m_isEffective {false},
	m_baseIterativeIntegerResults {},
	m_baseIterativeRealResults {},
	m_gridNodeIntegerResults {},
	m_gridNodeRealResults {},
	m_gridCellIntegerResults {},
	m_gridCellRealResults {},
	m_gridEdgeIIntegerResults{},
	m_gridEdgeIRealResults{},
	m_gridEdgeJIntegerResults{},
	m_gridEdgeJRealResults{}
{}

QString InputConditionCgnsFile::fileName() const
{
	return m_fileName;
}

void InputConditionCgnsFile::setFileName(const QString& filename)
{
	bool c = (filename != m_fileName);

	m_fileName = filename;
	if (c) {
		loadData();
		emit changed();
	}
}

bool InputConditionCgnsFile::isEffective() const
{
	return m_isEffective;
}

const InputConditionCgnsFile::SolverInformation& InputConditionCgnsFile::solverInformation() const
{
	return m_solverInformation;
}

QStringList InputConditionCgnsFile::baseIterativeIntegerResults() const
{
	return m_baseIterativeIntegerResults;
}

QStringList InputConditionCgnsFile::baseIterativeRealResults() const
{
	return m_baseIterativeRealResults;
}

QStringList InputConditionCgnsFile::gridNodeIntegerResults() const
{
	return m_gridNodeIntegerResults;
}

QStringList InputConditionCgnsFile::gridNodeRealResults() const
{
	return m_gridNodeRealResults;
}

QStringList InputConditionCgnsFile::gridCellIntegerResults() const
{
	return m_gridCellIntegerResults;
}

QStringList InputConditionCgnsFile::gridCellRealResults() const
{
	return m_gridCellRealResults;
}

QStringList InputConditionCgnsFile::gridEdgeIIntegerResults() const
{
	return m_gridEdgeIIntegerResults;
}

QStringList InputConditionCgnsFile::gridEdgeIRealResults() const
{
	return m_gridEdgeIRealResults;
}

QStringList InputConditionCgnsFile::gridEdgeJIntegerResults() const
{
	return m_gridEdgeJIntegerResults;
}

QStringList InputConditionCgnsFile::gridEdgeJRealResults() const
{
	return m_gridEdgeJRealResults;
}

bool InputConditionCgnsFile::loadData()
{
	m_isEffective = false;
	m_solverInformation.timeSteps = 0;

	m_baseIterativeIntegerResults.clear();
	m_baseIterativeRealResults.clear();
	m_gridNodeIntegerResults.clear();
	m_gridNodeRealResults.clear();
	m_gridCellIntegerResults.clear();
	m_gridCellRealResults.clear();
	m_gridEdgeIIntegerResults.clear();
	m_gridEdgeIRealResults.clear();
	m_gridEdgeJIntegerResults.clear();
	m_gridEdgeJRealResults.clear();

	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(m_fileName), iRICLib::H5CgnsFile::Mode::OpenReadOnly);

		std::string solverName;
		VersionNumber solverVersion;

		int ier = ProjectCgnsFile::readSolverInfo(file, &solverName, &solverVersion);
		if (ier != IRIC_NO_ERROR) {return false;}

		m_solverInformation.solverName = solverName.c_str();
		m_solverInformation.solverVersion = solverVersion.toAboutString();

		auto ccBase = file.ccBase();
		auto biterData = ccBase->biterData();
		if (biterData == nullptr) {
			// no result
			return true;
		}

		std::vector<std::string> resultNames;
		ier = biterData->getResultNames(&resultNames);
		if (ier != IRIC_NO_ERROR) {return false;}

		std::vector<double> timeValues;
		ier = biterData->readTime(&timeValues);
		if (ier != IRIC_NO_ERROR) {return false;}

		m_solverInformation.timeSteps = static_cast<int> (timeValues.size());

		for (const auto& name : resultNames) {
			iRICLib::H5Util::DataArrayValueType vType;
			ier = biterData->readValueType(name, &vType);
			if (ier != IRIC_NO_ERROR) {return false;}
			if (vType == iRICLib::H5Util::DataArrayValueType::Int) {
				m_baseIterativeIntegerResults.push_back(name.c_str());
			} else if (vType == iRICLib::H5Util::DataArrayValueType::RealDouble) {
				m_baseIterativeRealResults.push_back(name.c_str());
			}
		}

		int zones = ccBase->zoneNum();
		if (zones > 0) {
			auto zone = ccBase->zoneById(1);
			auto size = zone->size();
			if (zone->type() == iRICLib::H5CgnsZone::Type::Structured) {
				m_solverInformation.gridType = SolverInformation::GridType::Structured;
				m_solverInformation.iSize = size[0];
				m_solverInformation.jSize = size[1];
			} else {
				m_solverInformation.gridType = SolverInformation::GridType::Unstructured;
				m_solverInformation.size = size[0];
			}
			if (timeValues.size() > 0) {
				file.setSolutionId(1);

				ier = loadSolutionData(zone->nodeSolution(), &m_gridNodeRealResults, &m_gridNodeIntegerResults);
				if (ier != IRIC_NO_ERROR) {return false;}
				ier = loadSolutionData(zone->cellSolution(), &m_gridCellRealResults, &m_gridCellIntegerResults);
				if (ier != IRIC_NO_ERROR) {return false;}
				ier = loadSolutionData(zone->iFaceSolution(), &m_gridEdgeIRealResults, &m_gridEdgeIIntegerResults);
				if (ier != IRIC_NO_ERROR) {return false;}
				ier = loadSolutionData(zone->jFaceSolution(), &m_gridEdgeJRealResults, &m_gridEdgeJIntegerResults);
				if (ier != IRIC_NO_ERROR) {return false;}
			}
		}
		m_isEffective = true;
		return true;
	}  catch (...) {
		return false;
	}
}
