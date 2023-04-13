#ifndef QIMAGEDISPLAYWIDGET_H
#define QIMAGEDISPLAYWIDGET_H

#include "misc_global.h"

#include <QImage>
#include <QWidget>

class MISCDLL_EXPORT QImageDisplayWidget : public QWidget
{
public:
	QImageDisplayWidget(QWidget* parent);

	void setImage(const QImage& image);

private:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	void drawImage();

	QImage m_image;
};

#endif // QIMAGEDISPLAYWIDGET_H
