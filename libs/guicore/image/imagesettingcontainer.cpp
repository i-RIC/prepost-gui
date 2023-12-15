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
	CompositeContainer({&position, &horizontalMargin, &verticalMargin, &autoSize, &width, &height}),
	position {"position", Position::TopLeft},
	horizontalMargin {"horizontalMarginRatio", 0},
	verticalMargin {"verticalMarginRatio", 0},
	autoSize {"autoSize", false},
	width {"width", 100},
	height {"height", 100},
	m_actor {nullptr},
	m_imageBuilder {nullptr},
	m_controller {new Controller {this}},
	m_setting {this}
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

void ImageSettingContainer::setSetting(ImageSettingContainer* setting)
{
	m_setting = setting;
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

ImageSettingContainer::ImageBuilder* ImageSettingContainer::imageBuilder() const
{
	return m_imageBuilder;
}

void ImageSettingContainer::setImageBuilder(ImageBuilder* builder)
{
	m_imageBuilder = builder;
}

void ImageSettingContainer::optimizePosition(VTKGraphicsView* view)
{
	double topMargin = 0, bottomMargin = 0, leftMargin = 0, rightMargin = 0;

	if (position == Position::TopLeft || position == Position::Left || position == Position::BottomLeft) {
		leftMargin = horizontalMargin;
		rightMargin = 1 - leftMargin - width / static_cast<double> (view->width());
	} else if (position == Position::TopRight || position == Position::Right || position == Position::BottomRight) {
		rightMargin = horizontalMargin;
		leftMargin = 1 - horizontalMargin - width / static_cast<double> (view->width());
	}

	if (position == Position::TopLeft || position == Position::Top || position == Position::TopRight) {
		topMargin = verticalMargin;
		bottomMargin = 1 - verticalMargin - height / static_cast<double> (view->height());
	} else if (position == Position::BottomLeft || position == Position::Bottom || position == Position::BottomRight) {
		bottomMargin = verticalMargin;
		topMargin = 1 - verticalMargin - height / static_cast<double> (view->height());
	}

	if (position == Position::TopLeft || position == Position::TopRight || position == Position::BottomLeft || position == Position::BottomRight) {
		if (leftMargin < rightMargin) {
			horizontalMargin = leftMargin;
			if (topMargin < bottomMargin) {
				verticalMargin = topMargin;
				position = Position::TopLeft;
			} else {
				verticalMargin = bottomMargin;
				position = Position::BottomLeft;
			}
		} else {
			horizontalMargin = rightMargin;
			if (topMargin < bottomMargin) {
				verticalMargin = topMargin;
				position = Position::TopRight;
			} else {
				verticalMargin = bottomMargin;
				position = Position::BottomRight;
			}
		}
	} else if (position == Position::Left || position == Position::Right) {
		if (leftMargin < rightMargin) {
			position = Position::Left;
			horizontalMargin = leftMargin;
		} else {
			position = Position::Right;
			horizontalMargin = rightMargin;
		}
	} else if (position == Position::Top || position == Position::Bottom) {
		if (topMargin < bottomMargin) {
			position = Position::Top;
			verticalMargin = topMargin;
		} else {
			position = Position::Bottom;
			verticalMargin = bottomMargin;
		}
	}
}

void ImageSettingContainer::apply(VTKGraphicsView* view) const
{
	apply(view->size(), view);
}

void ImageSettingContainer::apply(const QSize& size, VTKGraphicsView* v) const
{
	if (m_actor == nullptr) {return;}

	bool visible = false;
	bool itemTested = false;

	auto item = m_controller->item();
	if (item != nullptr) {
		if (item->isAncientChecked() && item->isChecked()) {
			visible = true;
		}
	}
	for (auto item : m_controller->items()) {
		itemTested = true;
		if (! item->isAncientChecked()) {continue;}
		if (! item->isChecked()) {continue;}
		visible = true;
	}
	if (itemTested && ! visible) {
		m_actor->VisibilityOff();
	}

	if (item != nullptr) {
		item->actor2DCollection()->RemoveItem(m_actor);
	}

	int w, h;
	if (m_setting->autoSize) {
		QSize size = m_imageBuilder->autoSize();
		w = size.width();
		h = size.height();
	} else {
		w = m_setting->width;
		h = m_setting->height;
	}

	QImage canvasImage(int(w * v->devicePixelRatioF()), int(h * v->devicePixelRatioF()), QImage::Format::Format_ARGB32);
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

	auto r = m_setting->rect(size, v);

	m_actor->SetPosition(r.left(), size.height() * v->devicePixelRatioF() - r.bottom());
	
	if (item != nullptr) {
		item->actor2DCollection()->AddItem(m_actor);
		item->updateVisibilityWithoutRendering();
	} else {
		if (itemTested && visible) {
			m_actor->VisibilityOn();
		}
	}
}

QImage ImageSettingContainer::buildImage(QWidget* w) const
{
	QImage canvasImage(int(width) * w->devicePixelRatioF(), int(height * w->devicePixelRatioF()), QImage::Format::Format_ARGB32);
	canvasImage.setDevicePixelRatio(w->devicePixelRatioF());
	canvasImage.fill(Qt::transparent);

	m_imageBuilder->build(&canvasImage);

	return canvasImage;
}

ImageSettingContainer::Controller* ImageSettingContainer::controller()
{
	return m_controller;
}

QRect ImageSettingContainer::rect(const QSize& size, VTKGraphicsView* view) const
{
	QRect ret;
	int w, h;

	if (m_setting->autoSize) {
		QSize size = m_imageBuilder->autoSize();
		w = size.width();
		h = size.height();
	} else {
		w = m_setting->width;
		h = m_setting->height;
	}

	Position pos = position.value();

	if (pos == Position::Top || pos == Position::TopLeft || pos == Position::TopRight) {
		ret.setTop(verticalMargin * size.height() * view->devicePixelRatioF());
	} else if (pos == Position::Bottom || pos == Position::BottomLeft || pos == Position::BottomRight) {
		ret.setTop((size.height() - verticalMargin * size.height() - h) * view->devicePixelRatioF());
	} else {
		ret.setTop((size.height() / 2 - h / 2) * view->devicePixelRatioF());
	}
	if (pos == Position::Left || pos == Position::TopLeft || pos == Position::BottomLeft) {
		ret.setLeft(horizontalMargin * size.width() * view->devicePixelRatioF());
	} else if (pos == Position::Right || pos == Position::TopRight || pos == Position::BottomRight) {
		ret.setLeft((size.width() - horizontalMargin * size.width() - w) * view->devicePixelRatioF());
	} else {
		ret.setLeft((size.width() / 2 - w / 2) * view->devicePixelRatioF());
	}

	ret.setWidth(w * view->devicePixelRatioF());
	ret.setHeight(h * view->devicePixelRatioF());

	return ret;
}
