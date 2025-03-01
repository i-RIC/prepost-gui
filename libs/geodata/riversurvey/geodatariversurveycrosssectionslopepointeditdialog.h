#ifndef GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H

#include <QDialog>

class GeoDataRiverSurveyCrosssectionWindow;

namespace Ui {
class GeoDataRiverSurveyCrosssectionSlopePointEditDialog;
}

class GeoDataRiverSurveyCrosssectionSlopePointEditDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Mode {
		LeftAdd,
		LeftSub,
		RightAdd,
		RightSub,
	};

	explicit GeoDataRiverSurveyCrosssectionSlopePointEditDialog(GeoDataRiverSurveyCrosssectionWindow *parent);
	~GeoDataRiverSurveyCrosssectionSlopePointEditDialog();

	void setMode(Mode mode);
	int slope() const;
	void setSlope(int slope);
	void apply();

public slots:
	void accept() override;
	void reject() override;
	void setPoint(const QPointF& point);

private slots:
	void handleSlopeEdit(int slope);
	void handleButtonClick(QAbstractButton* button);

private:
	GeoDataRiverSurveyCrosssectionWindow* crosssectionWindow() const;

	Mode m_mode;
	bool m_applied;

	Ui::GeoDataRiverSurveyCrosssectionSlopePointEditDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H
