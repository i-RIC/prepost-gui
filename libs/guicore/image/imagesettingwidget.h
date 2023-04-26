#ifndef IMAGESETTINGWIDGET_H
#define IMAGESETTINGWIDGET_H

#include <QWidget>

class ImageSettingContainer;

namespace Ui {
class ImageSettingWidget;
}

class ImageSettingWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ImageSettingWidget(QWidget *parent = nullptr);
	~ImageSettingWidget();

	ImageSettingContainer setting() const;
	void setSetting(const ImageSettingContainer& setting);

	int width() const;
	void setWidth(int width);
	int height() const;
	void setHeight(int height);

private slots:
	void handlePositionChange();

private:
	Ui::ImageSettingWidget *ui;
};

#endif // IMAGESETTINGWIDGET_H
