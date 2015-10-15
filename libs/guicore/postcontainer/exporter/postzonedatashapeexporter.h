#ifndef POSTZONEDATASHAPEEXPORTER_H
#define POSTZONEDATASHAPEEXPORTER_H

#include "postzonedataexporter.h"
#include <QString>

class PostZoneDataShapeExporter : public PostZoneDataExporter
{
public:
	PostZoneDataShapeExporter(const QString& workDir);
	~PostZoneDataShapeExporter() {}
	QString filename(const QString& prefix, int index) const;
	bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd) const override;

private:
	QString m_workDir;
};

#endif // POSTZONEDATASHAPEEXPORTER_H
