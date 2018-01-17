#include "gridattributegeneratorlauncher.h"
#include "../datamodel/preprocessorgriddataitem.h"
#include "../datamodel/preprocessorgridandgridcreatingconditiondataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/filesystemfunction.h>
#include <misc/iricrootpath.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QWidget>

#include <iriclib.h>
#include <cgnslib.h>

bool GridAttributeGeneratorLauncher::launchGenerator(PreProcessorGridDataItem* gItem, const std::string& attName, const QString& workDir, QWidget* parent)
{
	QString fname = iRIC::getTempFileName(workDir);

	cg_set_file_type(CG_FILE_ADF2);
	ProjectCgnsFile::createNewFile(fname, 2, 2);
	cg_set_file_type(CG_FILE_NONE);

	int fn, ier;
	ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_MODIFY, &fn);
	gItem->grid()->setModified(true);
	gItem->saveToCgnsFile(fn);
	cg_close(fn);

	Grid* grid = gItem->grid();
	QString generatorName = grid->gridAttribute(attName)->gridAttribute()->mapping();
	QString generatorArgs = grid->gridAttribute(attName)->gridAttribute()->mappingArgs();
	QDir genDir(iRICRootPath::get());
	genDir.cdUp();
	genDir.cdUp();
	QString path = genDir.absoluteFilePath("gridattrgenerators").append("/").append(generatorName);

	if (! QFile(path).exists()) {
		QMessageBox::critical(parent, tr("Error"), tr("Attribute Generator %1 does not exist.").arg(generatorName));
		return false;
	}

	QProcess process;
	process.setWorkingDirectory(genDir.absoluteFilePath("gridattrgenerators"));
	process.setProcessEnvironment(gItem->iricMainWindow()->processEnvironment());

	QStringList args;
	args << fname << attName.c_str() << generatorArgs;
	process.start(path, args);
	process.waitForFinished();

	ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &fn);
	gItem->loadFromCgnsFile(fn);
	cg_close(fn);
	gItem->grid()->setModified(true);
	QFile(fname).remove();

	return true;
}

GridAttributeGeneratorLauncher::GridAttributeGeneratorLauncher()
{}
