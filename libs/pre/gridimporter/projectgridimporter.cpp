#include "projectgridimporter.h"
#include <guicore/project/projectdata.h>
#include <misc/stringtool.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include "cgnsgridimporter.h"
#include <guicore/base/iricmainwindowinterface.h>

#include <misc/errormessage.h>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVector2D>
#include <QString>
#include <QMessageBox>
#include <sstream>

#include <cgnslib.h>

ProjectGridImporter::ProjectGridImporter()
	: GridInternalImporter()
{

}

QString ProjectGridImporter::caption() const
{
	return tr("Project file");
}

const QStringList ProjectGridImporter::fileDialogFilters()
{
	QStringList ret;
	ret << QObject::tr("iRIC Project files (*.ipro)");
	return ret;
}

bool ProjectGridImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	ProjectWorkspace* w = getProjectData(grid)->mainWindow()->workspace();
	QString tmpworkfolder = ProjectData::newWorkfolderName(w->workspace());
	ProjectData tmpProj(tmpworkfolder, 0);
	tmpProj.unzipFrom(filename);
	try {
		tmpProj.loadSolverInformation();
	} catch (ErrorMessage& /*m*/) {
		return false;
	}
	tmpProj.loadCgnsList();
	QString cgnsFile = tmpProj.currentCgnsFileName();

	CgnsGridImporter* importer = getCgnsImporter();
	// load grid from CGNS file.
	bool ret = importer->import(grid, cgnsFile, "", parent);
	delete importer;
	return ret;
}
