#include "../base/iricmainwindowinterface.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "postiterationsteps.h"

#include <QMessageBox>
#include <QString>

#include <h5cgnsfile.h>
#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>

#include <vector>

PostIterationSteps::PostIterationSteps(ProjectDataItem* parent) :
	PostAbstractSteps(parent)
{}

void PostIterationSteps::loadFromCgnsFile(iRICLib::H5CgnsFile &file)
{
	auto biterData = file.ccBase()->biterData();

	std::vector<int> buffer;
	biterData->readIteration(&buffer);

	QList<int> tmplist;
	for (auto v : buffer) {
		tmplist.push_back(v);
	}

	bool changed = (tmplist != m_iterationSteps);
	m_iterationSteps = tmplist;
	if (changed) {
		emit stepsUpdated(m_iterationSteps);
		emit stepsUpdated();
	}
}

void PostIterationSteps::loadFromCgnsFile(const int fn)
{
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
