#ifndef COORDINATESYSTEMCONVERTDIALOG_H
#define COORDINATESYSTEMCONVERTDIALOG_H

#include "cs_api.h"

#include <QDialog>

class CoordinateSystem;
class CoordinateSystemBuilder;

namespace Ui {
class CoordinateSystemConvertDialog;
}

class CS_API CoordinateSystemConvertDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CoordinateSystemConvertDialog(QWidget *parent = nullptr);
	~CoordinateSystemConvertDialog();

	void setBuilder(CoordinateSystemBuilder* builder);

	void setEnabled(bool enabled);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* cs);

private:
	Ui::CoordinateSystemConvertDialog *ui;
};

#endif // COORDINATESYSTEMCONVERTDIALOG_H
