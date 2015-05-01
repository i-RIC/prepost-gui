#ifndef RAWDATAPOINTMAPEDITPTSDIALOG_H
#define RAWDATAPOINTMAPEDITPTSDIALOG_H

#include <QDialog>
class RawDataPointmap;

namespace Ui
{
	class RawDataPointmapEditPtsDialog;
}

class RawDataPointmapEditPtsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointmapEditPtsDialog(RawDataPointmap* pmap, QWidget* parent = nullptr);
	~RawDataPointmapEditPtsDialog();
	double pointX();
	double pointY();
	double value();

private:
	Ui::RawDataPointmapEditPtsDialog* ui;
	RawDataPointmap* m_pointMap;
};

#endif // RAWDATAPOINTMAPEDITPTSDIALOG_H
