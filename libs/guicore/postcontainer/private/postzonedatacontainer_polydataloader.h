#ifndef POSTZONEDATACONTAINER_POLYDATALOADER_H
#define POSTZONEDATACONTAINER_POLYDATALOADER_H

#include "../postzonedatacontainer.h"

#include <map>
#include <string>

#include <vtkSmartPointer.h>

class QPointF;
class vtkPolyData;

class PostZoneDataContainer::PolyDataLoader
{
public:
	static bool load(int fid, int bid, int zid, int step, std::map<std::string, vtkSmartPointer<vtkPolyData> >* polyDataMap, const QPointF& offset);

private:
	PolyDataLoader();
};

#endif // POSTZONEDATACONTAINER_POLYDATALOADER_H
