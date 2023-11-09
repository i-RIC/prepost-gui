#ifndef GEODATAEXPORTER_H
#define GEODATAEXPORTER_H

#include "../../guicore_global.h"
#include <QObject>

class ProjectData;
class GeoData;
class GeoDataCreator;

class QString;

class GUICOREDLL_EXPORT GeoDataExporter : public QObject
{
public:
	GeoDataExporter(const QString& caption, GeoDataCreator* creator);
	virtual ~GeoDataExporter();

	const QString& caption() const;

	virtual bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) = 0;
	virtual const QStringList fileDialogFilters() = 0;

	GeoDataCreator* creator() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/geodataexporter_impl.h"
#endif // _DEBUG

#endif // GEODATAEXPORTER_H
