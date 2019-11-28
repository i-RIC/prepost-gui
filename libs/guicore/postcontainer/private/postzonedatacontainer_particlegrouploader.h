#ifndef POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H
#define POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H

#include "../postzonedatacontainer.h"

#include <map>
#include <string>

#include <vtkSmartPointer.h>

class QPointF;
class vtkPolyData;

class PostZoneDataContainer::ParticleGroupLoader
{
public:
	static bool load(int fid, int bid, int zid, int step, std::map<std::string, vtkSmartPointer<vtkPolyData> >* polyDataMap, const QPointF& offset);

private:
	static bool loadScalarData(vtkDataSetAttributes* atts, const QString& groupName);
	static bool loadVectorData(vtkDataSetAttributes* atts, const QString& groupName);
	static int loadParticleGroup(const std::string& name, vtkPolyData* polyData, const QPointF& offset);

	ParticleGroupLoader();
};

#endif // POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H
