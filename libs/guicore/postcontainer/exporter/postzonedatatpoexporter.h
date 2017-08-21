#ifndef POSTZONEDATATPOEXPORTER_H
#define POSTZONEDATATPOEXPORTER_H

#include "postzonedataexporter.h"

class PostZoneDataTpoExporter : public PostZoneDataExporter
{
public:
	PostZoneDataTpoExporter();
	~PostZoneDataTpoExporter();

	QString filename(const QString& prefix, int index) const override;
	bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd, const QVector2D& offset) const override;
};

#endif // POSTZONEDATATPOEXPORTER_H
