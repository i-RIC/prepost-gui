#ifndef GEODATARIVERPATHPOINTINSERTDIALOG_H
#define GEODATARIVERPATHPOINTINSERTDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverPathPointInsertDialog;
}

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
class QAbstractButton;
class QVector2D;
class Interpolator2D1;

class GeoDataRiverPathPointInsertDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointInsertDialog(GeoDataRiverPathPoint* target, bool insert, GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointInsertDialog();

public slots:
	void accept() override;
	void reject() override;
	void setPoint(const QPointF& position);
	void setDefaultName();

private slots:
	void handleButtonClick(QAbstractButton* button);
	void handleRatioToggle(bool toggled);

private:
	void initializeItems();
	void setDefaultPosition();
	void setupComboBox();
	void updatePoint();
	void apply();
	Ui::GeoDataRiverPathPointInsertDialog* ui;
	GeoDataRiverPathPoint* m_insertTarget;

	GeoDataRiverPathPoint* m_newPoint;
	GeoDataRiverSurvey* m_rs;
	Interpolator2D1* m_lineCenter;
	bool m_insert;
	bool m_applyed;
};

#endif // GEODATARIVERPATHPOINTINSERTDIALOG_H
