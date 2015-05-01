#include "../base/iricmainwindowinterface.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "postiterationsteps.h"

#include <QMessageBox>
#include <QString>

#include <cgnslib.h>
#include <iriclib.h>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

void PostIterationSteps::loadFromCgnsFile(const int fn)
{
	QList<int> tmplist;
	char buffer[ProjectCgnsFile::BUFFERLEN];
	int nsteps;
	int ier;
	int B;
	bool changed = false;
	// goto Base.
	ier = cg_iRIC_GotoBase(fn, &B);
	if (ier != 0) {goto ERRORMSG;}

	// get the node name of BaseIterativeData_t.
	ier = cg_biter_read(fn, B, buffer, &nsteps);
	if (ier != 0) {
		// there's no BaseIterativeData_t.
		// skip loading and emit signal.
		if (tmplist != m_iterationsteps) {
			m_iterationsteps = tmplist;
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
		// search for "IterationValues" node.
		if (QString(buffer) == "IterationValues") {
			// we've found the node! let's read.
			int* iterationsteps;
			// in case of "IterationValues" node, dataDimension = 1, and dimensionVector[0]
			// containes the number of iterationsteps.
			// anyway, the number of iterationsteps is already read, by cg_biter_read()
			// and stored in nsteps.
			iterationsteps = new int[nsteps];
			ier = cg_array_read(i, iterationsteps);
			for (int j = 0; j < nsteps; ++j) {
				tmplist.push_back(iterationsteps[j]);
			}
			// temporary buffer iterationsteps is not needed anymore.
			delete iterationsteps;

			if (ier != 0) {goto ERRORMSG;}
			// we do not need to do the rest of i loop.
			break;
		}
	}
	changed = (tmplist != m_iterationsteps);
	m_iterationsteps = tmplist;
	if (changed) {
		emit stepsUpdated(m_iterationsteps);
		emit stepsUpdated(fn);
	}
	return;

ERRORMSG:
	QMessageBox::critical(projectData()->mainWindow(), tr("Error"), tr("Error occured while loading calculation result."));
}

void PostIterationSteps::checkStepsUpdate(int fn)
{
	loadFromCgnsFile(fn);
}

void PostIterationSteps::informSteps()
{
	emit stepsUpdated(m_iterationsteps);
}
