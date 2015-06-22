#ifndef GEODATAPOINTMAPDELPTSGREATERTHANDIALOG_H
#define GEODATAPOINTMAPDELPTSGREATERTHANDIALOG_H

#include <QDialog>
#include <vtkType.h>

class GeoDataPointmap;

namespace Ui
{
	class GeoDataPointmapDelPtsGreaterThanDialog;
}

class GeoDataPointmapDelPtsGreaterThanDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapDelPtsGreaterThanDialog(GeoDataPointmap* pmap, QWidget* parent = nullptr);
	~GeoDataPointmapDelPtsGreaterThanDialog();
	double limitValue();
	const QVector<vtkIdType>& selectedPoints() {return m_selectedPoints;}

public slots:
	void accept() override;

private:
	Ui::GeoDataPointmapDelPtsGreaterThanDialog* ui;
	QVector<vtkIdType> m_selectedPoints;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAPDELPTSGREATERTHANDIALOG_H
