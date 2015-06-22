#ifndef GEODATAPOINTMAPDELPTSLESSTHANDIALOG_H
#define GEODATAPOINTMAPDELPTSLESSTHANDIALOG_H

#include <QDialog>
#include <vtkType.h>

class GeoDataPointmap;

namespace Ui
{
	class GeoDataPointMapDelPtsLessThanDialog;
}

class GeoDataPointMapDelPtsLessThanDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointMapDelPtsLessThanDialog(GeoDataPointmap* pmap, QWidget* parent);
	~GeoDataPointMapDelPtsLessThanDialog();
	double limitValue();
	const QVector<vtkIdType>& selectedPoints() {return m_selectedPoints;}

public slots:
	void accept() override;

private:
	Ui::GeoDataPointMapDelPtsLessThanDialog* ui;
	QVector<vtkIdType> m_selectedPoints;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAPDELPTSLESSTHANDIALOG_H
