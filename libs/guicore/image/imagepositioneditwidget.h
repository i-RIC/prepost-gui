#ifndef IMAGEPOSITIONEDITWIDGET_H
#define IMAGEPOSITIONEDITWIDGET_H

#include "../guicore_global.h"
#include "imagesettingcontainer.h"

#include <QWidget>

namespace Ui {
class ImagePositionEditWidget;
}

class GUICOREDLL_EXPORT ImagePositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ImagePositionEditWidget(QWidget *parent = nullptr);
	~ImagePositionEditWidget();

	ImageSettingContainer::Position position() const;
	void setPosition(ImageSettingContainer::Position pos);

private:
	Ui::ImagePositionEditWidget *ui;
};

#endif // IMAGEPOSITIONEDITWIDGET_H
