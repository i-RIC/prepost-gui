#ifndef GRIDINTERNALIMPORTER_H
#define GRIDINTERNALIMPORTER_H

#include "../../guicore_global.h"
#include "gridimporterinterface.h"
#include <QObject>

class Grid;
class ProjectData;

class GUICOREDLL_EXPORT GridInternalImporter : public QObject, public GridImporterInterface
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	GridInternalImporter();
	virtual ~GridInternalImporter(){}

protected:
	ProjectData* getProjectData(Grid* g);
};

#endif // GRIDINTERNALIMPORTER_H
