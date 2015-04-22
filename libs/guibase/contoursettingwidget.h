#ifndef CONTOURSETTINGWIDGET_H
#define CONTOURSETTINGWIDGET_H

#include "guibase_global.h"
#include <QWidget>

namespace Ui {
	class ContourSettingWidget;
}

class GUIBASEDLL_EXPORT ContourSettingWidget : public QWidget {
	Q_OBJECT
public:
	enum Contour {Points, Isolines, ContourFigure, ColorFringe};
	ContourSettingWidget(QWidget *parent = nullptr);
	~ContourSettingWidget();
	void setContour(Contour contour);
	Contour contour();
	void hidePoints();
	void setNoPolyData(bool nopolydata);
signals:
	void contourChanged();
	void contourChanged(ContourSettingWidget::Contour c);

private slots:
	void handleContourChange();

private:
	Ui::ContourSettingWidget *ui;
};

#endif // CONTOURSETTINGWIDGET_H
