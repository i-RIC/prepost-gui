#include "gridcreatingcondition.h"
#include "gridcreatingconditionfactoryi.h"
#include "gridcreatingconditionio.h"

#include <misc/filesystemfunction.h>
#include <misc/ziparchive.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QXmlStreamWriter>

GridCreatingCondition* GridCreatingConditionIO::importData(GridCreatingConditionFactoryI* factory, ProjectDataItem* parent, const QString& fileName, const QString& workFolder)
{
	auto fname = iRIC::getTempFileName(workFolder);
	QDir dir(workFolder);
	dir.mkdir(fname);

	iRIC::UnzipArchive(fileName, dir.absoluteFilePath(fname));

	QDir exportDir(dir.absoluteFilePath(fname));
	QString xmlName = exportDir.absoluteFilePath(GridCreatingCondition::XML_FILENAME);
	QFile f(xmlName);

	QDomDocument doc;
	doc.setContent(&f);

	GridCreatingCondition* cond = factory->restore(doc.documentElement(), parent);

	cond->loadFromExportData(exportDir.absolutePath());

	iRIC::rmdirRecursively(exportDir.absolutePath());

	return cond;
}

void GridCreatingConditionIO::exportData(GridCreatingCondition* cond, const QString& fileName, const QString& workFolder)
{
	auto fname = iRIC::getTempFileName(workFolder);
	QDir dir(workFolder);
	dir.mkdir(fname);

	QString exportDir = dir.absoluteFilePath(fname);

	cond->saveToExportData(exportDir);
	iRIC::ZipArchive(fileName, exportDir, GridCreatingCondition::exportDataFileList());

	iRIC::rmdirRecursively(exportDir);
}

GridCreatingConditionIO::GridCreatingConditionIO()
{}
