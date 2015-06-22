#ifndef GEODATAPOINTMAPINTERPOLATEPOINTS_H
#define GEODATAPOINTMAPINTERPOLATEPOINTS_H

#include <QDialog>

namespace Ui
{
	class GeoDataPointmapInterpolatePoints;
}

class GeoDataPointmap;

class GeoDataPointmapInterpolatePoints : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapInterpolatePoints(GeoDataPointmap* pmap, QWidget* parent = nullptr);
	~GeoDataPointmapInterpolatePoints();
	std::vector<double> xinterp, yinterp, zinterp;

private:
	Ui::GeoDataPointmapInterpolatePoints* ui;
	GeoDataPointmap* m_pmap;
	std::vector<double> xpoint, ypoint, zpoint;
	std::vector<double> si, szi, sout, temp, yp;
	std::vector<int> zindex, izindex;
	double stot;
	int numZPts, numInterpPts;

private slots:
	void accept() override;
	void reject() override;

};

#endif // GEODATAPOINTMAPINTERPOLATEPOINTS_H
