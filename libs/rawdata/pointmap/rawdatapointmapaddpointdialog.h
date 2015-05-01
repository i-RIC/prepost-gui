#ifndef RAWDATAPOINTMAPADDPOINTDIALOG_H
#define RAWDATAPOINTMAPADDPOINTDIALOG_H

#include <QDialog>

class RawDataPointmap;

namespace Ui
{
	class RawDataPointmapAddPointDialog;
}

class RawDataPointmapAddPointDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointmapAddPointDialog(RawDataPointmap* pmap, QWidget* parent = nullptr);
	~RawDataPointmapAddPointDialog();
	std::vector<double> xpoint, ypoint, zpoint;

private:
	Ui::RawDataPointmapAddPointDialog* ui;
	RawDataPointmap* m_pmap;

private slots:
	void accept();

};

#endif // RAWDATAPOINTMAPADDPOINTDIALOG_H
