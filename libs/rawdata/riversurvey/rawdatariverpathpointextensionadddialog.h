#ifndef RAWDATARIVERPATHPOINTEXTENSIONADDDIALOG_H
#define RAWDATARIVERPATHPOINTEXTENSIONADDDIALOG_H

#include <QDialog>
#include <QVector2D>
class QAbstractButton;
class RawDataRiverPathPoint;
class RawDataRiverSurvey;
class QVector2D;
namespace Ui {
	class RawDataRiverPathPointExtensionAddDialog;
}

class RawDataRiverPathPointExtensionAddDialog : public QDialog
{
	Q_OBJECT
public:
	enum LineMode {Left, Right};
	explicit RawDataRiverPathPointExtensionAddDialog(RawDataRiverPathPoint* p, RawDataRiverSurvey* rs, QWidget *parent = nullptr);
	~RawDataRiverPathPointExtensionAddDialog();
	void setLineMode(LineMode lm){
		m_lineMode = lm;
	}
public slots:
	void accept();
	void reject();
	void setPoint(const QVector2D& position);
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	void apply();
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
	bool m_applyed;
	QVector2D m_position;
private:
	Ui::RawDataRiverPathPointExtensionAddDialog *ui;
	LineMode m_lineMode;
};

#endif // RAWDATARIVERPATHPOINTEXTENSIONADDDIALOG_H
