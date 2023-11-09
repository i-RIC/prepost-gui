#include "../base/iricmainwindowi.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "postiterationsteps.h"
#include "postsolutioninfo.h"
#include "private/postbaseiterativevaluescontainer_basecontainer.h"
#include "private/postbaseiterativevaluescontainer_integervaluecontainer.h"

#include <QMessageBox>
#include <QString>

#include <h5cgnsfile.h>
#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>

#include <vector>

PostIterationSteps::PostIterationSteps(PostSolutionInfo* parent) :
	PostAbstractSteps(parent)
{}

void PostIterationSteps::loadFromCgnsFile(iRICLib::H5CgnsFile &file)
{
	QList<int> tmplist;

	if (postSolutionInfo()->separateResultExists()) {
		auto container = postSolutionInfo()->baseIterativeValuesContainer();
		if (container->baseContainers().size() != 0) {
			auto baseContainer = container->baseContainers().at(0);
			auto iterationContainer = baseContainer->integerContainer(std::string("IterationValues"));
			if (iterationContainer != nullptr) {
				for (double v : iterationContainer->values()) {
					tmplist.push_back(v);
				}
			}
		}
	} else {
		auto biterData = file.ccBase()->biterData();
		if (biterData != nullptr) {
			std::vector<int> buffer;
			biterData->readIteration(&buffer);

			for (auto v : buffer) {
				tmplist.push_back(v);
			}
		}
	}

	bool changed = (tmplist != m_iterationSteps);
	m_iterationSteps = tmplist;
	if (changed) {
		emit stepsUpdated(m_iterationSteps);
		emit stepsUpdated();
	}
}

const QList<int>& PostIterationSteps::iterationSteps() const
{
	return m_iterationSteps;
}

bool PostIterationSteps::dataExists() const
{
	return m_iterationSteps.size() > 0;
}

void PostIterationSteps::checkStepsUpdate(iRICLib::H5CgnsFile& file)
{
	loadFromCgnsFile(file);
}

void PostIterationSteps::informSteps()
{
	emit stepsUpdated(m_iterationSteps);
}

void PostIterationSteps::clearArray()
{
	m_iterationSteps.clear();
}
