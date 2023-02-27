#include "imagesettingcontainer.h"
#include "public/imagesettingcontainer_controller.h"
#include "public/imagesettingcontainer_imagebuilder.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/datamodel/vtkgraphicsview.h>

#include <QBrush>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QRect>

#include <vtkImageMapper.h>
#include <vtkSmartPointer.h>
#include <vtkQImageToImageSource.h>

ImageSettingContainer::ImageSettingContainer() :
	CompositeContainer({&position, &horizontalMargin, &verticalMargin, &width, &height}),
	position {"position", Position::TopLeft},
	horizontalMargin {"horizontalMargin", 0},
	verticalMargin {"verticalMargin", 0},
	width {"width", 100},
	height {"height", 100},
	m_actor {nullptr},
	m_imageBuilder {nullptr},
	m_controller {new Controller {this}}
{}

ImageSettingContainer::ImageSettingContainer(const ImageSettingContainer& c) :
	ImageSettingContainer {}
{
	copyValue(c);
}

ImageSettingContainer::~ImageSettingContainer()
{
	delete m_controller;
}

ImageSettingContainer& ImageSettingContainer::operator=(const ImageSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ImageSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ImageSettingContainer&> (c));
}

vtkActor2D* ImageSettingContainer::actor() const
{
	return m_actor;
}

void ImageSettingContainer::setActor(vtkActor2D* actor)
{
	m_actor = actor;
	m_actor->VisibilityOff();
}

void ImageSettingContainer::setImageBuilder(ImageBuilder* builder)
{
	m_imageBuilder = builder;
}

void ImageSettingContainer::apply(VTKGraphicsView* v) const
{
	if (m_actor == nullptr) {return;}
	auto item = m_controller->item();
	if (item == nullptr) {return;}

	item->actor2DCollection()->RemoveItem(m_actor);

	QImage canvasImage(int(width * v->devicePixelRatioF()), int(height * v->devicePixelRatioF()), QImage::Format::Format_ARGB32);
	canvasImage.setDevicePixelRatio(v->devicePixelRatioF());
	canvasImage.fill(Qt::transparent);

	bool ok = m_imageBuilder->build(&canvasImage);
	if (! ok) {
		m_actor->VisibilityOff();
		return;
	}
	auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
	imgToImg->SetQImage(&canvasImage);
	imgToImg->Update();

	auto mapper = vtkSmartPointer<vtkImageMapper>::New();
	mapper->SetColorWindow(255);
	mapper->SetColorLevel(127.5);
	mapper->SetInputData(imgToImg->GetOutput());
	m_actor->SetMapper(mapper);

	auto r = rect(v);

	m_actor->SetPosition(r.left(), v->height() * v->devicePixelRatioF() - r.bottom());

	item->actor2DCollection()->AddItem(m_actor);
	item->updateVisibilityWithoutRendering();
}

ImageSettingContainer::Controller* ImageSettingContainer::controller()
{
	return m_controller;
}

QRect ImageSettingContainer::rect(VTKGraphicsView* view) const
{
	QRect ret;

	Position pos = position.value();

	if (pos == Position::Top || pos == Position::TopLeft || pos == Position::TopRight) {
		ret.setTop(verticalMargin * view->devicePixelRatioF());
	} else if (pos == Position::Bottom || pos == Position::BottomLeft || pos == Position::BottomRight) {
		ret.setTop((view->height() - verticalMargin - height) * view->devicePixelRatioF());
	} else {
		ret.setTop((view->height() / 2 - height / 2) * view->devicePixelRatioF());
	}
	if (pos == Position::Left || pos == Position::TopLeft || pos == Position::BottomLeft) {
		ret.setLeft(horizontalMargin * view->devicePixelRatioF());
	} else if (pos == Position::Right || pos == Position::TopRight || pos == Position::BottomRight) {
		ret.setLeft((view->width() - horizontalMargin - width) * view->devicePixelRatioF());
	} else {
		ret.setLeft((view->width() / 2 - width / 2) * view->devicePixelRatioF());
	}

	ret.setWidth(width * view->devicePixelRatioF());
	ret.setHeight(height * view->devicePixelRatioF());

	return ret;
}
