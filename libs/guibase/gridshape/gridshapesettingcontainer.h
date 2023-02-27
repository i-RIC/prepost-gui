#ifndef GRIDSHAPESETTINGCONTAINER_H
#define GRIDSHAPESETTINGCONTAINER_H

#include "../guibase_global.h"
#include "../vtktextpropertysettingcontainer.h"

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>

#include <string>

class VTKGraphicsView;

class vtkActor;
class vtkActorCollection;
class vtkActor2D;
class vtkActor2DCollection;
class vtkTransformFilter;

class GUIBASEDLL_EXPORT GridShapeSettingContainer : public CompositeContainer
{
public:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;

	enum class Shape {
		Outline,    ///< Outline only
		Wireframe   ///< Show all grid cell lines
	};

	GridShapeSettingContainer();
	GridShapeSettingContainer(const GridShapeSettingContainer& setting);
	~GridShapeSettingContainer();

	GridShapeSettingContainer& operator=(const GridShapeSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	ColorContainer color;
	EnumContainerT<Shape> shape;
	BoolContainer indexVisible;
	vtkTextPropertySettingContainer indexTextSetting;

	vtkActor* outlineActor() const;
	vtkActor* wireframeActor() const;
	vtkActor2D* indexActor() const;

	void update(vtkActorCollection* col, vtkActor2DCollection* col2d,
							vtkPointSet* outlineGrid, vtkPointSet* wireframeGrid,
							vtkPointSet* labelGrid, const std::string& labelName);
	void setScale(double xScale, double yScale, double zScale);

private:
	void updateOutlineActor(vtkStructuredGrid* grid);
	void updateWireframeActor(vtkPointSet* grid);
	void updateIndexActor(vtkPointSet* grid, const std::string& labelName);

	vtkActor* m_outlineActor;
	vtkActor* m_wireframeActor;
	vtkActor2D* m_indexActor;
	vtkTransformFilter* m_transform;
};

#endif // GRIDSHAPESETTINGCONTAINER_H
