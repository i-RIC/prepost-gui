#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_JMKDATAEDITDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_JMKDATAEDITDIALOG_H

#include "../geodatariversurveycrosssectionwindow.h"
#include "../geodatariverpathpointjmkdata.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyCrosssectionWindow_JmkDataEditDialog;
}

class GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit JmkDataEditDialog(QWidget *parent = nullptr);
	~JmkDataEditDialog();

	GeoDataRiverPathPointJmkData::Item item() const;
	void setItem(const GeoDataRiverPathPointJmkData::Item& item);

private:
	Ui::GeoDataRiverSurveyCrosssectionWindow_JmkDataEditDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_JMKDATAEDITDIALOG_H
