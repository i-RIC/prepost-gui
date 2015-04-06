#ifndef RAWDATAPOINTMAPSTLIMPORTER_H
#define RAWDATAPOINTMAPSTLIMPORTER_H

#include <guicore/pre/rawdata/rawdataimporter.h>

class RawDataPointmapSTLImporter : public RawDataImporter
{
	Q_OBJECT
public:
	/// Constructor
	RawDataPointmapSTLImporter(RawDataCreator* creator);
	bool importData(RawData* data, int index, QWidget* w);
	const QStringList fileDialogFilters();
	const QStringList acceptableExtensions();
protected:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, QWidget* w);
private:
	bool checkHeader(const QString& filename, QWidget* w);
};

#endif // RAWDATAPOINTMAPSTLIMPORTER_H
