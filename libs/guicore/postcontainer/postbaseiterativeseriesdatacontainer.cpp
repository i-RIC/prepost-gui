#include "../project/projectcgnsfile.h"
#include "postbaseiterativeseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <guicore/postcontainer/postbaseiterativevaluescontainer.h>
#include <guicore/postcontainer/private/postbaseiterativevaluescontainer_basecontainer.h>
#include <guicore/postcontainer/private/postbaseiterativevaluescontainer_valuecontainer.h>
#include <misc/stringtool.h>

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>
#include <iriclib_errorcodes.h>

#include <set>

PostBaseIterativeSeriesDataContainer::PostBaseIterativeSeriesDataContainer(const std::string& name, PostSolutionInfo* parent) :
	PostSeriesDataContainer {PostSolutionInfo::dim2D, parent},
	m_baseIterativeName (name)
{}

int PostBaseIterativeSeriesDataContainer::loadData()
{
	if (solutionInfo()->separateResultExists()) {
		auto cont = solutionInfo()->baseIterativeValuesContainer();
		auto bcontainer = cont->baseContainer(1);
		if (bcontainer == nullptr) {return false;}
		auto vcontainer = bcontainer->container(m_baseIterativeName);
		if (vcontainer == nullptr) {return false;}
		for (auto v : vcontainer->doubleValues()) {
			m_data.push_back(v);
		}
		return IRIC_NO_ERROR;
	}

	auto solInfo = solutionInfo();

	auto biter = solInfo->cgnsFile()->ccBase()->biterData();
	if (biter == nullptr) {return IRIC_DATA_NOT_FOUND;}
	std::unordered_set<std::string> resultNames;
	int ier = biter->getResultNames(&resultNames);
	if (ier != 0) {return ier;}

	if (resultNames.find(m_baseIterativeName) == resultNames.end()) {return IRIC_DATA_NOT_FOUND;}

	iRICLib::H5Util::DataArrayValueType type;
	ier = biter->readValueType(m_baseIterativeName, &type);
	if (ier != 0) {return ier;}

	if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
		std::vector<double> buffer;
		ier = biter->readValues(m_baseIterativeName, &buffer);
		if (ier != 0) {return ier;}
		m_data.clear();
		for (double v : buffer) {
			m_data.push_back(v);
		}
	}	else if (type == iRICLib::H5Util::DataArrayValueType::Int) {
		std::vector<int> buffer;
		ier = biter->readValues(m_baseIterativeName, &buffer);
		if (ier != 0) {return ier;}
		m_data.clear();
		for (int v : buffer) {
			m_data.push_back(v);
		}
	}
	return IRIC_NO_ERROR;
}

void PostBaseIterativeSeriesDataContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostBaseIterativeSeriesDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
