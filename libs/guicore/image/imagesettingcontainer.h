#ifndef IMAGESETTINGCONTAINER_H
#define IMAGESETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class VTKGraphicsView;

class QRect;

class vtkActor2D;

class GUICOREDLL_EXPORT ImageSettingContainer : public CompositeContainer
{
public:
	enum class Position {
		Top,
		Bottom,
		Left,
		Right,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	class Controller;
	class ImageBuilder;

	ImageSettingContainer();
	ImageSettingContainer(const ImageSettingContainer& c);
	~ImageSettingContainer();

	ImageSettingContainer& operator=(const ImageSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	vtkActor2D* actor() const;
	void setActor(vtkActor2D* actor);
	void setImageBuilder(ImageBuilder* builder);

	void apply(VTKGraphicsView* view) const;
	Controller* controller();
	QRect rect(VTKGraphicsView* view) const;

	EnumContainerT<Position> position;
	IntContainer horizontalMargin;
	IntContainer verticalMargin;
	IntContainer width;
	IntContainer height;

private:
	vtkActor2D* m_actor;
	ImageBuilder* m_imageBuilder;
	Controller* m_controller;

	class MoveCommand;
	class UpdateCommand;
	class ResizeCommand;
};

#include "public/imagesettingcontainer_controller.h"

#endif // IMAGESETTINGCONTAINER_H
