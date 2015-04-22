#ifndef RAWDATAPOINTMAPINTERPOLATEPOINTS_H
#define RAWDATAPOINTMAPINTERPOLATEPOINTS_H

#include <QDialog>

namespace Ui {
    class RawDataPointmapInterpolatePoints;
}

class RawDataPointmap;

class RawDataPointmapInterpolatePoints : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointmapInterpolatePoints(RawDataPointmap *pmap, QWidget *parent = nullptr);
	~RawDataPointmapInterpolatePoints();
	std::vector<double> xinterp, yinterp, zinterp;

private:
    Ui::RawDataPointmapInterpolatePoints *ui;
	RawDataPointmap* m_pmap;
	std::vector<double> xpoint, ypoint, zpoint;
	std::vector<double> si, szi, sout, temp, yp;
	std::vector<int> zindex, izindex;
	double stot;
	int numZPts, numInterpPts;

private slots:
	void accept();
	void reject();

};

#endif // RAWDATAPOINTMAPINTERPOLATEPOINTS_H
