#ifndef GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H

#include "geodatarivercrosssection.h"

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

	static void calculateLeftAndRightPoints(const GeoDataRiverCrosssection::AltitudeList& alist, Mode mode, const QPointF& point, int slope, QPointF* left, QPointF* right);

public slots:
	void accept() override;
	void reject() override;
	void setPoint(const QPointF& point);

private slots:
	void handleSlopeEdit(int slope);
	void handleButtonClick(QAbstractButton* button);

private:
	GeoDataRiverSurveyCrosssectionWindow* crosssectionWindow() const;
	QUndoCommand* createCommand(bool apply) const;

	Mode m_mode;
	bool m_applied;

	Ui::GeoDataRiverSurveyCrosssectionSlopePointEditDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONSLOPEPOINTEDITDIALOG_H
