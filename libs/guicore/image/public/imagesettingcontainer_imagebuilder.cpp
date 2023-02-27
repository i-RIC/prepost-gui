#include "imagesettingcontainer_imagebuilder.h"


ImageSettingContainer::ImageBuilder::ImageBuilder()
{}

ImageSettingContainer::ImageBuilder::~ImageBuilder()
{}

bool ImageSettingContainer::ImageBuilder::build(QImage* image)
{
	QPainter painter;
	painter.begin(image);

	QPen pen(Qt::gray);
	QBrush brush(QColor(120, 120, 120, 150));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0, 0, image->width(), image->height());
	painter.end();

	return true;
}
