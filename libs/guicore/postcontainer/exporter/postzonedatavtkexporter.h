#ifndef POSTZONEDATAVTKEXPORTER_H
#define POSTZONEDATAVTKEXPORTER_H

#include "postzonedataexporter.h"
#include <QString>

class vtkStructuredGrid;
class vtkUnstructuredGrid;
class vtkDataWriter;

class PostZoneDataVtkExporter : public PostZoneDataExporter
{
public:
	enum class Mode {ASCII, BINARY};
	PostZoneDataVtkExporter(const QString& workDir);
	PostZoneDataVtkExporter(const QString& workDir, Mode mode);
	~PostZoneDataVtkExporter() {}
	QString filename(const QString& prefix, int index) const override;
	bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd) const override;

private:
	void exportStructuredGrid(const char* filename, vtkStructuredGrid* sgrid, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax) const;
	void exportUnstructuredGrid(const char *filename, vtkUnstructuredGrid* ugrid, double time) const;
	void setupWriter(vtkDataWriter* writer, const char *filename, double time) const;

	QString m_workDir;
	Mode m_mode;
};

#endif // POSTZONEDATAVTKEXPORTER_H
