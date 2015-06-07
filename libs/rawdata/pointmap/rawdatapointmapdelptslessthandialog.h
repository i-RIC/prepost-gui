#ifndef RAWDATAPOINTMAPDELPTSLESSTHANDIALOG_H
#define RAWDATAPOINTMAPDELPTSLESSTHANDIALOG_H

#include <QDialog>
#include <vtkType.h>

class RawDataPointmap;

namespace Ui
{
	class RawDataPointMapDelPtsLessThanDialog;
}

class RawDataPointMapDelPtsLessThanDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointMapDelPtsLessThanDialog(RawDataPointmap* pmap, QWidget* parent);
	~RawDataPointMapDelPtsLessThanDialog();
	double limitValue();
	const QVector<vtkIdType>& selectedPoints() {return m_selectedPoints;}

public slots:
	void accept() override;

private:
	Ui::RawDataPointMapDelPtsLessThanDialog* ui;
	QVector<vtkIdType> m_selectedPoints;
	RawDataPointmap* m_pointMap;
};

#endif // RAWDATAPOINTMAPDELPTSLESSTHANDIALOG_H
