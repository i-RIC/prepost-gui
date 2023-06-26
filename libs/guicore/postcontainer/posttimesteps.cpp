#include "../base/iricmainwindowinterface.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "postbaseiterativevaluescontainer.h"
#include "postsolutioninfo.h"
#include "posttimesteps.h"
#include "private/postbaseiterativevaluescontainer_basecontainer.h"
#include "private/postbaseiterativevaluescontainer_realvaluecontainer.h"

#include <QMessageBox>
#include <QString>

#include <h5cgnsfile.h>
#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>

#include <vector>

PostTimeSteps::PostTimeSteps(PostSolutionInfo* parent) :
	PostAbstractSteps(parent)
{}

void PostTimeSteps::loadFromCgnsFile(iRICLib::H5CgnsFile &file)
{
	QList<double> tmplist;

	if (postSolutionInfo()->separateResultExists()) {
		auto container = postSolutionInfo()->baseIterativeValuesContainer();
		if (container->baseContainers().size() != 0) {
			auto baseContainer = container->baseContainers().at(0);
			auto timeContainer = baseContainer->realContainer(std::string("TimeValues"));
			if (timeContainer != nullptr) {
				for (double v : timeContainer->values()) {
					tmplist.push_back(v);
				}
			}
		}
	} else {
		auto biterData = file.ccBase()->biterData();
		if (biterData != nullptr) {
			std::vector<double> buffer;
			biterData->readTime(&buffer);

			for (auto v : buffer) {
				tmplist.push_back(v);
			}
		}
	}

	bool changed = (tmplist != m_timesteps);
	m_timesteps = tmplist;
	if (changed) {
		emit stepsUpdated(m_timesteps);
		emit stepsUpdated();
	}
}

const QList<double>& PostTimeSteps::timesteps() const
{
	return m_timesteps;
}

bool PostTimeSteps::dataExists() const
{
	return m_timesteps.size() > 0;
}

void PostTimeSteps::checkStepsUpdate(iRICLib::H5CgnsFile& file)
{
	loadFromCgnsFile(file);
}

void PostTimeSteps::informSteps()
{
	emit stepsUpdated(m_timesteps);
}

void PostTimeSteps::clearArray()
{
	m_timesteps.clear();
}
