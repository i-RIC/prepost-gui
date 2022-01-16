#include "../project/projectcgnsfile.h"
#include "postcalculatedresult.h"
#include "postcalculatedresultargument.h"
#include "postsolutioninfo.h"
#include "postzonedatacontainer.h"
#include "postzonepointseriesdatacontainer.h"

#include <misc/stringtool.h>

#include <QRegExp>

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>
#include <h5cgnsfilesolutionreader.h>
#include <h5cgnsflowsolution.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <cmath>

PostZonePointSeriesDataContainer::PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const std::string& zoneName, const std::string valueName, int index, iRICLib::H5CgnsZone::SolutionPosition position, PostSolutionInfo* sinfo) :
	PostSeriesDataContainer(dim, sinfo),
	m_zoneName (zoneName),
	m_valueName (valueName),
	m_pointIndex {index},
	m_position {position}
{}

const std::string& PostZonePointSeriesDataContainer::zoneName() const
{
	return m_zoneName;
}

QString PostZonePointSeriesDataContainer::caption() const
{
	return zoneName().c_str();
}

int PostZonePointSeriesDataContainer::pointIndex() const
{
	return m_pointIndex;
}

int PostZonePointSeriesDataContainer::loadData(const std::string &name, iRICLib::H5CgnsZone* zone, double* value)
{
	auto zc = zoneDataContainer();
	std::vector<PostCalculatedResult*> calcResults = zc->calculatedResults();
	for (PostCalculatedResult* result : calcResults) {
		if (m_valueName == result->name()) {
			return loadCalculatedData(result, zone, value);
		}
	}
	return loadResultData(name, zone, value);
}

int PostZonePointSeriesDataContainer::loadCalculatedData(PostCalculatedResult* result, iRICLib::H5CgnsZone* zone, double* value)
{
	std::vector<double> args;
	for (PostCalculatedResultArgument* arg : result->arguments()) {
		double argVal;
		int ier = loadData(arg->name(), zone, &argVal);
		if (! ier) {return ier;}
		args.push_back(argVal);
	}
	*value = result->calculateValue(args);
	return IRIC_NO_ERROR;
}

int PostZonePointSeriesDataContainer::loadResultData(const std::string& name, iRICLib::H5CgnsZone* zone, double* value)
{
	int ier;
	QRegExp rx("^(.+) \\(magnitude\\)$");
	bool magnitude = (rx.indexIn(name.c_str()) != -1);

	auto sol = zone->solution(m_position);
	if (magnitude) {
		auto tmpPhysName = iRIC::toStr(rx.cap(1));
		std::set<std::string> resultNames;
		ier = sol->readValueNames(&resultNames);
		if (ier != 0) {return false;}
		std::vector<std::string> suffixes;
		suffixes.push_back("X");
		suffixes.push_back("Y");
		suffixes.push_back("Z");

		*value = 0;
		for (const auto& suffix : suffixes) {
			auto compName = tmpPhysName + suffix;
			if (resultNames.find(compName) != resultNames.end()) {
				std::vector<double> buffer;
				sol->readValueAsDouble(compName, &buffer);
				double v = buffer[m_pointIndex];
				*value += v * v;
			}
		}
		*value = std::sqrt(*value);
	} else {
		std::vector<double> buffer;
		sol->readValueAsDouble(name, &buffer);
		*value = buffer[m_pointIndex];
	}
	return IRIC_NO_ERROR;
}

PostZoneDataContainer* PostZonePointSeriesDataContainer::zoneDataContainer() const
{
	return solutionInfo()->zoneContainer(m_dimension, m_zoneName);
}

int PostZonePointSeriesDataContainer::loadData()
{
	m_data.clear();
	auto zc = zoneDataContainer();

	std::vector<PostCalculatedResult*> calcResults = zc->calculatedResults();
	for (PostCalculatedResult* r : calcResults) {
		r->updateFunction();
	}

	auto file = solutionInfo()->cgnsFile();

	std::vector<double> timeValues;
	file->ccBase()->biterData()->readTime(&timeValues);

	auto reader = file->solutionReader();
	for (int i = 0; i < static_cast<int> (timeValues.size()); ++i) {
		reader->setSolutionId(i + 1);
		auto zone = reader->targetFile()->base(PostSolutionInfo::toIntDimension(m_dimension))->zone(m_zoneName);

		double val;
		int ret = loadData(m_valueName, zone, &val);
		if (ret != 0) {return false;}
		m_data.push_back(val);
	}
	return IRIC_NO_ERROR;
}

void PostZonePointSeriesDataContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostZonePointSeriesDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
