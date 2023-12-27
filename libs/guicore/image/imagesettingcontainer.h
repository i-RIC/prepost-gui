#ifndef IMAGESETTINGCONTAINER_H
#define IMAGESETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

#include <QObject>

class VTKGraphicsView;

class QRect;

class vtkActor2D;

class GUICOREDLL_EXPORT ImageSettingContainer : public CompositeContainer
{
	Q_OBJECT

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

	void setSetting(ImageSettingContainer* setting);

	bool negativePositionForbidden() const;
	void setNegativePositionForbidden(bool forbidden);
	bool autoSizeForced() const;
	void setAutoSizeForced(bool forced);

	ImageSettingContainer& operator=(const ImageSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	vtkActor2D* actor() const;
	void setActor(vtkActor2D* actor);
	ImageBuilder* imageBuilder() const;
	void setImageBuilder(ImageBuilder* builder);

	void optimizePosition(VTKGraphicsView* view);
	void apply(VTKGraphicsView* view) const;
	void apply(const QSize& size, VTKGraphicsView* view) const;
	QImage buildImage(QWidget* w) const;
	Controller* controller();
	QRect rect(const QSize& size, VTKGraphicsView* view) const;

	EnumContainerT<Position> position;
	DoubleContainer horizontalMargin;
	DoubleContainer verticalMargin;
	BoolContainer autoSize;
	IntContainer width;
	IntContainer height;

signals:
	void updated();

private:
	void copyValue(const XmlAttributeContainer& c) override;

	vtkActor2D* m_actor;
	ImageBuilder* m_imageBuilder;
	Controller* m_controller;
	bool m_negativePositionForbidden;
	bool m_autoSizeForced;

	ImageSettingContainer* m_setting;

	class MoveCommand;
	class UpdateCommand;
	class ResizeCommand;
};

#include "public/imagesettingcontainer_controller.h"

#endif // IMAGESETTINGCONTAINER_H
