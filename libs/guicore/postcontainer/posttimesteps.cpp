#include "../base/iricmainwindowinterface.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "posttimesteps.h"

#include <QMessageBox>
#include <QString>

#include <cgnslib.h>

#include <h5cgnsfile.h>
#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>

#include <vector>

PostTimeSteps::PostTimeSteps(ProjectDataItem* parent) :
	PostAbstractSteps(parent)
{}

void PostTimeSteps::loadFromCgnsFile(iRICLib::H5CgnsFile &file)
{
	auto biterData = file.ccBase()->biterData();

	std::vector<double> buffer;
	biterData->readTime(&buffer);

	QList<double> tmplist;
	for (auto v : buffer) {
		tmplist.push_back(v);
	}

	bool changed = (tmplist != m_timesteps);
	m_timesteps = tmplist;
	if (changed) {
		int fn = 0;
		emit stepsUpdated(m_timesteps);
		emit stepsUpdated(fn);
	}
}

void PostTimeSteps::loadFromCgnsFile(const int fn)
{
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
