#include "cgnsgridimporter.h"
#include "projectgridimporter.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/project/projectworkspace.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <sstream>

ProjectGridImporter::ProjectGridImporter() :
	GridInternalImporter {}
{}

ProjectGridImporter::~ProjectGridImporter()
{}

void ProjectGridImporter::setGridDataItem(PreProcessorGridDataItem* gridDataItem)
{
	m_gridDataItem = gridDataItem;
}

QString ProjectGridImporter::caption() const
{
	return tr("Project file");
}

QStringList ProjectGridImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("iRIC Project files (*.ipro)");
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
	QString cgnsFile = tmpProj.masterCgnsFileName();

	CgnsGridImporter* importer = getCgnsImporter();
	importer->setGridDataItem(m_gridDataItem);
	// load grid from CGNS file.
	bool ret = importer->import(grid, cgnsFile, "", parent);
	delete importer;
	return ret;
}
