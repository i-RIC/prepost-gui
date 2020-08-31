#ifndef GEODATAPOINTMAPINTERPOLATEPOINTS_H
#define GEODATAPOINTMAPINTERPOLATEPOINTS_H

#include <QDialog>

namespace Ui
{
	class GeoDataPointmapInterpolatePoints;
}

class GeoDataPointmap;

class GeoDataPointmapInterpolatePointsDialog : public QDialog
{
	Q_OBJECT

public:
	GeoDataPointmapInterpolatePointsDialog(QWidget* parent = nullptr);
	~GeoDataPointmapInterpolatePointsDialog();

	void setLineLength(double length);
	double increment() const;

public slots:
	void accept() override;

private:
	Ui::GeoDataPointmapInterpolatePoints* ui;
	double m_lineLength;
};

#endif // GEODATAPOINTMAPINTERPOLATEPOINTS_H
