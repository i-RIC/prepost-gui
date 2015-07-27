#ifndef POSTZONEDATACSVEXPORTER_H
#define POSTZONEDATACSVEXPORTER_H

#include "postzonedataexporter.h"

class QTextStream;
class vtkStructuredGrid;
class vtkUnstructuredGrid;

class PostZoneDataCsvExporter : public PostZoneDataExporter
{
public:
	PostZoneDataCsvExporter() {}
	~PostZoneDataCsvExporter() {}
	QString filename(const QString& prefix, int index) const override;
	bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax) const override;

private:
	static void exportStructuredGrid(PostZoneDataContainer* data, QTextStream& stream, vtkStructuredGrid* sgrid, int imin, int imax, int jmin, int jmax, int kmin, int kmax);
	static void exportUnstructuredGrid(QTextStream& stream, vtkUnstructuredGrid* ugrid);
};

#endif // POSTZONEDATACSVEXPORTER_H
