#include "qimagedisplaywidget.h"

#include <QPainter>

QImageDisplayWidget::QImageDisplayWidget(QWidget* parent) :
	QWidget {parent}
{}

void QImageDisplayWidget::setImage(const QImage& image)
{
	m_image = image;

	setMinimumSize(image.size());

	repaint();
}

void QImageDisplayWidget::paintEvent(QPaintEvent*)
{
	drawImage();
}


void QImageDisplayWidget::resizeEvent(QResizeEvent* event)
{
	drawImage();
}

void QImageDisplayWidget::drawImage()
{
	QPainter painter(this);
	auto s = size();
	auto is = m_image.size();
	auto x = (s.width() - is.width()) / 2;
	auto y = (s.height() - is.height()) / 2;
	QRect rect(QPoint(x, y), m_image.size());
	painter.drawImage(rect, m_image);
}
