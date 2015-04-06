#ifndef RAWDATARIVERPATHPOINTINSERTDIALOG_H
#define RAWDATARIVERPATHPOINTINSERTDIALOG_H

#include <QDialog>

namespace Ui {
	class RawDataRiverPathPointInsertDialog;
}

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class QAbstractButton;
class QVector2D;
class Interpolator2D1;

class RawDataRiverPathPointInsertDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverPathPointInsertDialog(RawDataRiverPathPoint* target, bool insert, RawDataRiverSurvey* rs, QWidget *parent = 0);
	~RawDataRiverPathPointInsertDialog();
public slots:
	void accept();
	void reject();
	void setPoint(const QVector2D& position);
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
	Ui::RawDataRiverPathPointInsertDialog *ui;
	RawDataRiverPathPoint* m_insertTarget;

	RawDataRiverPathPoint* m_newPoint;
	RawDataRiverSurvey* m_rs;
	Interpolator2D1* m_lineCenter;
	bool m_insert;
	bool m_applyed;
};

#endif // RAWDATARIVERPATHPOINTINSERTDIALOG_H
