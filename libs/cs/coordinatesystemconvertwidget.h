#ifndef COORDINATESYSTEMCONVERTWIDGET_H
#define COORDINATESYSTEMCONVERTWIDGET_H

#include "cs_api.h"

#include <QWidget>

class CoordinateSystem;
class CoordinateSystemBuilder;

namespace Ui {
class CoordinateSystemConvertWidget;
}

class CS_API CoordinateSystemConvertWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CoordinateSystemConvertWidget(QWidget *parent = nullptr);
	~CoordinateSystemConvertWidget();

	void setBuilder(CoordinateSystemBuilder* builder);

	void setEnabled(bool enabled);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* cs);

private slots:
	void edit();

private:
	CoordinateSystem* m_coordinateSystem;
	CoordinateSystemBuilder* m_builder;

	Ui::CoordinateSystemConvertWidget *ui;
};

#endif // COORDINATESYSTEMCONVERTWIDGET_H
