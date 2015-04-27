#ifndef RAWDATARIVERPATHPOINTMOVEDIALOG_H
#define RAWDATARIVERPATHPOINTMOVEDIALOG_H

#include <QDialog>
#include <QVector2D>

class RawDataRiverSurvey;
class QAbstractButton;

namespace Ui {
	class RawDataRiverPathPointMoveDialog;
}

class RawDataRiverPathPointMoveDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RawDataRiverPathPointMoveDialog(RawDataRiverSurvey* rs, QWidget *parent = nullptr);
	~RawDataRiverPathPointMoveDialog();
	void setSingleSelection(bool single);
	void setCurrentCenter(const QVector2D& current);
public slots:
	void accept();
	void reject();
private slots:
	void offsetChange();
	void centerChange();
	void handleButtonClick(QAbstractButton* button);
private:
	void doReset();
	void apply();

	Ui::RawDataRiverPathPointMoveDialog *ui;
	QVector2D m_currentCenter;
	RawDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // RAWDATARIVERPATHPOINTMOVEDIALOG_H
