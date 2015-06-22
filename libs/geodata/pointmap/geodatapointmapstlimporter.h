#ifndef GEODATAPOINTMAPSTLIMPORTER_H
#define GEODATAPOINTMAPSTLIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPointmapSTLImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	/// Constructor
	GeoDataPointmapSTLImporter(GeoDataCreator* creator);
	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

protected:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, QWidget* w);

private:
	bool checkHeader(const QString& filename, QWidget* w);
};

#endif // GEODATAPOINTMAPSTLIMPORTER_H
