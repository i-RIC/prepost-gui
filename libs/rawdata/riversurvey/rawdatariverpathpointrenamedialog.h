#ifndef RAWDATARIVERPATHPOINTRENAMEDIALOG_H
#define RAWDATARIVERPATHPOINTRENAMEDIALOG_H

#include <QDialog>

namespace Ui {
	class RawDataRiverPathPointRenameDialog;
}

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class RawDataRiverPathPointRenameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverPathPointRenameDialog(RawDataRiverPathPoint* p, RawDataRiverSurvey* rs, QWidget *parent = nullptr);
	~RawDataRiverPathPointRenameDialog();
public slots:
	void accept();
private:
	Ui::RawDataRiverPathPointRenameDialog *ui;
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
};

#endif // RAWDATARIVERPATHPOINTRENAMEDIALOG_H
