#ifndef DIRECTIONSETTINGCONTAINER_H
#define DIRECTIONSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class SolverDefinitionGridAttribute;

class vtkConeSource;
class vtkPointSet;
class vtkPolyData;
class VTKGraphicsView;

class GUICOREDLL_EXPORT DirectionSettingContainer : public CompositeContainer
{
public:
	enum class Mode {
		Mode1to8,
		Mode1to128
	};

	static const int DEFAULT_LINEWIDTH;
	static const int DEFAULT_ARROWSIZE;

	DirectionSettingContainer();
	DirectionSettingContainer(const DirectionSettingContainer& c);

	DirectionSettingContainer& operator=(const DirectionSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	std::vector<vtkIdType> findWrongPoints(vtkPointSet* data);
	std::vector<vtkIdType> findWrongPoints(vtkPointSet* data, vtkIdType downstreamPoint);
	vtkIdType findDownstreamPoint(vtkPointSet* data);

	void buildDirectionPolygonData(vtkPointSet* data, SolverDefinitionGridAttribute* att, VTKGraphicsView* view, const std::vector<vtkIdType>& wrongCells, vtkActor* actor, vtkActor* wrongActor) const;
	void getDiff(int value, int* iDiff, int* jDiff) const;

	EnumContainerT<Mode> mode;
	ColorContainer color;
	ColorContainer wrongColor;
	OpacityContainer opacity;
	IntContainer lineWidth;
	IntContainer arrowSize;

private:
	vtkConeSource* buildConeSource(VTKGraphicsView* view) const;
	void findWrongPointsRecursively(vtkIntArray* data, vtkIdType cellICount, vtkIdType cellJCount, vtkIdType current, std::unordered_set<vtkIdType> visited, std::vector<bool>* wrong);
};

#endif // DIRECTIONSETTINGCONTAINER_H
