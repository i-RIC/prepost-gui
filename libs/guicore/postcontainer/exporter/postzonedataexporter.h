#ifndef POSTZONEDATAEXPORTER_H
#define POSTZONEDATAEXPORTER_H

#include <vtkSmartPointer.h>

class QString;
class QVector2D;
class ProjectData;
class PostZoneDataContainer;

class PostZoneDataExporter
{
public:
	virtual ~PostZoneDataExporter() {}
	virtual QString filename(const QString& prefix, int index) const = 0;
	virtual bool exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd, const QVector2D& offset) const = 0;

	template<typename T>
	static T* applyOffset(T* src, vtkSmartPointer<T>& dest, const QVector2D& offset)
	{
		if (offset.isNull()) {
			return src;
		}
		else {
			dest = vtkSmartPointer<T>::New();
			dest->DeepCopy(src);
			double x[3];
			vtkPoints *points = src->GetPoints();
			vtkPoints *opoints = dest->GetPoints();
			for (vtkIdType id = 0; id < points->GetNumberOfPoints(); ++id) {
				points->GetPoint(id, x);
				x[0] += offset.x(); x[1] += offset.y();
				opoints->SetPoint(id, x);
			}
		}
		return dest;
	}
};

#endif // POSTZONEDATAEXPORTER_H
