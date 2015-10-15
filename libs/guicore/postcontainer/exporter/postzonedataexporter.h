#ifndef POSTZONEDATAEXPORTER_H
#define POSTZONEDATAEXPORTER_H

class QString;
class ProjectData;
class PostZoneDataContainer;

class PostZoneDataExporter
{
public:
	virtual ~PostZoneDataExporter() {}
	virtual QString filename(const QString& prefix, int index) const = 0;
	virtual bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd) const = 0;
};

#endif // POSTZONEDATAEXPORTER_H
