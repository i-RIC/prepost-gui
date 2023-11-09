#include "../../gridimporter/projectgridimporter.h"
#include "preprocessorgriddataitem_cgnsimporter.h"
#include "preprocessorgriddataitem_projectimporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/base/iricmainwindowi.h>
#include <misc/errormessage.h>

PreProcessorGridDataItem::ProjectImporter::ProjectImporter(PreProcessorGridDataItem* item) :
	m_item {item}
{}

bool PreProcessorGridDataItem::ProjectImporter::importGrid(const QString& fileName, ProjectGridImporter* importer)
{
	auto w = m_item->projectData()->mainWindow()->workspace();
	QString tmpworkfolder = ProjectData::newWorkfolderName(w->workspace());

	ProjectData tmpProj(tmpworkfolder, 0);
	tmpProj.unzipFrom(fileName);
	try {
		tmpProj.loadSolverInformation();
	} catch (ErrorMessage& /*m*/) {
		return false;
	}
	QString cgnsFile = tmpProj.masterCgnsFileName();

	CgnsGridImporter* cgnsImporter = importer->getCgnsImporter();
	CgnsImporter cgnsImporter2(m_item);
	auto ret = cgnsImporter2.importGrid(tmpProj.masterCgnsFileName(), cgnsImporter);

	return ret;
}
