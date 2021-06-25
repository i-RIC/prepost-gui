#ifndef POSTZONEDATACONTAINER_POLYDATALOADER_H
#define POSTZONEDATACONTAINER_POLYDATALOADER_H

#include "../postzonedatacontainer.h"

#include <map>
#include <string>

#include <vtkSmartPointer.h>

namespace iRICLib {
	class H5CgnsPolyDataSolution;
} // namespace iRICLib

class QPointF;
class vtkPolyData;

class PostZoneDataContainer::PolyDataLoader
{
public:
	static bool load(std::map<std::string, vtkSmartPointer<vtkPolyData> >* polyDataMap, std::map<std::string, std::vector<int> >* polyDataCellIdsMap, iRICLib::H5CgnsPolyDataSolution* sol, const QPointF& offset);

private:
	PolyDataLoader();
};

#endif // POSTZONEDATACONTAINER_POLYDATALOADER_H
