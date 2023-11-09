#ifndef GRIDINTERNALEXPORTER_H
#define GRIDINTERNALEXPORTER_H

#include "../../guicore_global.h"
#include "gridexporteri.h"
#include <QObject>

class ProjectData;

class GUICOREDLL_EXPORT GridInternalExporter : public QObject, public GridExporterI
{
	Q_OBJECT
	Q_INTERFACES(GridExporterI)

public:
	GridInternalExporter();
	virtual ~GridInternalExporter() {}
};

#endif // GRIDINTERNALEXPORTER_H
