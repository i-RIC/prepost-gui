#ifndef GRIDINTERNALIMPORTER_H
#define GRIDINTERNALIMPORTER_H

#include "../../guicore_global.h"
#include "gridimporteri.h"
#include <QObject>

class ProjectData;

class GUICOREDLL_EXPORT GridInternalImporter : public QObject, public GridImporterI
{
	Q_OBJECT
	Q_INTERFACES(GridImporterI)

public:
	GridInternalImporter();
	virtual ~GridInternalImporter() {}
};

#endif // GRIDINTERNALIMPORTER_H
