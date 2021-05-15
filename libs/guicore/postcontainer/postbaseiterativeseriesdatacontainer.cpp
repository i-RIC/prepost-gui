#include "../project/projectcgnsfile.h"
#include "postbaseiterativeseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <misc/stringtool.h>

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>

#include <set>

PostBaseIterativeSeriesDataContainer::PostBaseIterativeSeriesDataContainer(const std::string& name, PostSolutionInfo* parent) :
	PostSeriesDataContainer {PostSolutionInfo::dim2D, parent},
	m_baseIterativeName (name)
{}

bool PostBaseIterativeSeriesDataContainer::loadData(const int fn)
{
	auto solInfo = solutionInfo();

	auto biter = solInfo->cgnsFile()->ccBase()->biterData();
	std::set<std::string> resultNames;
	int ier = biter->getResultNames(&resultNames);
	if (ier != 0) {return false;}

	if (resultNames.find(m_baseIterativeName) == resultNames.end()) {return false;}

	iRICLib::H5Util::DataArrayValueType type;
	ier = biter->readValueType(m_baseIterativeName, &type);
	if (ier != 0) {return false;}

	if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
		std::vector<double> buffer;
		ier = biter->readValues(m_baseIterativeName, &buffer);
		if (ier != 0) {return false;}
		m_data.clear();
		for (double v : buffer) {
			m_data.push_back(v);
		}
	}	else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
		std::vector<int> buffer;
		ier = biter->readValues(m_baseIterativeName, &buffer);
		if (ier != 0) {return false;}
		m_data.clear();
		for (int v : buffer) {
			m_data.push_back(v);
		}
	}
	return true;
}

void PostBaseIterativeSeriesDataContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostBaseIterativeSeriesDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
