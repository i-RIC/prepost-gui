#ifndef RAWDATAPOINTMAPDELPTSGREATERTHANDIALOG_H
#define RAWDATAPOINTMAPDELPTSGREATERTHANDIALOG_H

#include <QDialog>
#include <vtkType.h>

class RawDataPointmap;

namespace Ui
{
	class RawDataPointmapDelPtsGreaterThanDialog;
}

class RawDataPointmapDelPtsGreaterThanDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointmapDelPtsGreaterThanDialog(RawDataPointmap* pmap, QWidget* parent = nullptr);
	~RawDataPointmapDelPtsGreaterThanDialog();
	double limitValue();
	const QVector<vtkIdType>& selectedPoints() {return m_selectedPoints;}

public slots:
	void accept() override;

private:
	Ui::RawDataPointmapDelPtsGreaterThanDialog* ui;
	QVector<vtkIdType> m_selectedPoints;
	RawDataPointmap* m_pointMap;
};

#endif // RAWDATAPOINTMAPDELPTSGREATERTHANDIALOG_H
