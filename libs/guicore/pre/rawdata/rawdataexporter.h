#ifndef RAWDATAEXPORTER_H
#define RAWDATAEXPORTER_H

#include "../../guicore_global.h"
#include <QObject>
#include <QString>

class ProjectData;
class RawData;
class RawDataCreator;

class GUICOREDLL_EXPORT RawDataExporter : public QObject
{

public:
	/// Constructor
	RawDataExporter(RawDataCreator* creator);
	/// Export the rawdata into the specified file.
	virtual bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	RawDataCreator* creator();
	const QString& caption() const {return m_caption;}

protected:
	QString m_caption;
};

#endif // RAWDATAEXPORTER_H
