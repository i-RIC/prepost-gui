#include "../base/iricmainwindowinterface.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "posttimesteps.h"

#include <QMessageBox>
#include <QString>

#include <cgnslib.h>
#include <iriclib.h>
#include <vector>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

PostTimeSteps::PostTimeSteps(ProjectDataItem* parent) :
	PostAbstractSteps(parent)
{}

void PostTimeSteps::loadFromCgnsFile(const int fn)
{
	QList<double> tmplist;
	char buffer[ProjectCgnsFile::BUFFERLEN];
	int nsteps;
	int ier;
	int B;
	bool changed = false;
	// goto base.
	ier = cg_iRIC_GotoBase(fn, &B);
	if (ier != 0) {goto ERRORMSG;}

	// get the node name of BaseIterativeData_t.
	ier = cg_biter_read(fn, B, buffer, &nsteps);
	if (ier != 0) {
		// there's no BaseIterativeData_t.
		// skip loading and emit signal.
		if (tmplist != m_timesteps) {
			m_timesteps = tmplist;
			emit stepsUpdated(tmplist);
		}
		return;
	}
	// goto baseiterativedata node.
	ier = cg_goto(fn, B, buffer, 0, "end");
	if (ier != 0) {goto ERRORMSG;}
	int narrays;
	ier = cg_narrays(&narrays);
	if (ier != 0) {goto ERRORMSG;}
	for (int i = 1; i <= narrays; ++i) {
		DataType_t dataType;
		int dataDimension;
		cgsize_t dimensionVector[3];
		// get cg_array_info
		ier = cg_array_info(i, buffer, &dataType, &dataDimension, dimensionVector);
		if (ier != 0) {goto ERRORMSG;}
		// search for "TimeValues" node.
		if (QString(buffer) == "TimeValues") {
			// we've found the node! let's read.
			// in case of "TimeValues" node, dataDimension = 1, and dimensionVector[0]
			// containes the number of timesteps.
			// anyway, the number of timesteps is already read, by cg_biter_read()
			// and stored in nsteps.
			std::vector<double> timesteps(nsteps);
			ier = cg_array_read(i, timesteps.data());
			for (int j = 0; j < nsteps; ++j) {
				tmplist.push_back(timesteps[j]);
			}

			if (ier != 0) {goto ERRORMSG;}
			// we do not need to do the rest of i loop.
			break;
		}
	}
	changed = (tmplist != m_timesteps);
	m_timesteps = tmplist;
	if (changed) {
		emit stepsUpdated(m_timesteps);
		emit stepsUpdated(fn);
	}
	return;
ERRORMSG:
	QMessageBox::critical(projectData()->mainWindow(), tr("Error"), tr("Error occured while loading calculation result."));
}

const QList<double>& PostTimeSteps::timesteps() const
{
	return m_timesteps;
}

bool PostTimeSteps::dataExists() const
{
	return m_timesteps.size() > 0;
}

void PostTimeSteps::checkStepsUpdate(int fn)
{
	loadFromCgnsFile(fn);
}


void PostTimeSteps::informSteps()
{
	emit stepsUpdated(m_timesteps);
}

void PostTimeSteps::clearArray()
{
	m_timesteps.clear();
}
