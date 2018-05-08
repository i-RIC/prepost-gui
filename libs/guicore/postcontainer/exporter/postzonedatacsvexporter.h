#ifndef POSTZONEDATACSVEXPORTER_H
#define POSTZONEDATACSVEXPORTER_H

#include "postzonedataexporter.h"

class QTextStream;
class vtkStructuredGrid;
class vtkUnstructuredGrid;

class PostZoneDataCsvExporter : public PostZoneDataExporter
{
public:
	PostZoneDataCsvExporter();
	~PostZoneDataCsvExporter();

	QString filename(const QString& prefix, int index) const override;
	bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd, const QPointF& offset) const override;
};

#endif // POSTZONEDATACSVEXPORTER_H
