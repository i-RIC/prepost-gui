#ifndef GRIDINTERNALEXPORTER_H
#define GRIDINTERNALEXPORTER_H

#include "../../guicore_global.h"
#include "gridexporterinterface.h"
#include <QObject>

class Grid;
class ProjectData;

class GUICOREDLL_EXPORT GridInternalExporter : public QObject, public GridExporterInterface
{
	Q_OBJECT
	Q_INTERFACES(GridExporterInterface)

public:
	GridInternalExporter();
	virtual ~GridInternalExporter(){}

protected:
	ProjectData* getProjectData(Grid* g);
};

#endif // GRIDINTERNALEXPORTER_H
