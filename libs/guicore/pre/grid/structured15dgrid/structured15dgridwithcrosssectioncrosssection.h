#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H

#include "../grid.h"

#include <QObject>
#include <QVector>

class Structured15DGridWithCrossSectionCrossSection : public QObject
{

public:
	struct Altitude {

	public:
		double m_position;
		double m_height;
	};
	struct CtrlPointsAddMethod {

	public:
		enum {
			am_Uniform,          ///< Uniform
			am_EqRatio_Ratio     ///< Equal ratio
		} method;
		unsigned int number; ///< Number of points to add
		double param; ///< parameter
	};
	Structured15DGridWithCrossSectionCrossSection(QString name, Grid* grid)
		: QObject(grid) {
		m_grid = grid;
		m_name = name;
	}
	Grid* grid() const {return m_grid;}
	void loadFromCgnsFile(int fn, int B, int Z, int index);
	void saveToCgnsFile(int fn, int B, int Z, int index);
	const QString& name() const {return m_name;}
	const QVector<Altitude>& altitudeInfo() const {return m_altitudeInfo;}
	QVector<Altitude>& altitudeInfo() {return m_altitudeInfo;}
	void setAltitudeInfo(const QVector<Altitude>& alt) {m_altitudeInfo = alt;}

protected:
	Grid* m_grid;
	QString m_name;
	QVector<Altitude> m_altitudeInfo;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H
