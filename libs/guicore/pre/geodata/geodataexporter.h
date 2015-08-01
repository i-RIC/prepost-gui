#ifndef GEODATAEXPORTER_H
#define GEODATAEXPORTER_H

#include "../../guicore_global.h"
#include <QObject>
#include <QString>

class ProjectData;
class GeoData;
class GeoDataCreator;

class GUICOREDLL_EXPORT GeoDataExporter : public QObject
{

public:
	/// Constructor
	GeoDataExporter(GeoDataCreator* creator);
	/// Export the geodata into the specified file.
	virtual bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	GeoDataCreator* creator() const;
	const QString& caption() const {return m_caption;}

protected:
	QString m_caption;
};

#endif // GEODATAEXPORTER_H
