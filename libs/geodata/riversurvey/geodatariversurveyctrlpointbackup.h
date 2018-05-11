#ifndef GEODATARIVERSURVEYCTRLPOINTBACKUP_H
#define GEODATARIVERSURVEYCTRLPOINTBACKUP_H

#include "gd_riversurvey_global.h"
#include "geodatariverpathpoint.h"

#include <QList>
#include <QVector>

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCtrlPointBackup
{

public:
	GeoDataRiverSurveyCtrlPointBackup() {}
	virtual ~GeoDataRiverSurveyCtrlPointBackup() {}
	void backup(GeoDataRiverPathPoint* point, GeoDataRiverPathPoint::CtrlZonePosition position);
	void restore();

private:
	struct CtrlPoints {
		GeoDataRiverPathPoint* point;
		GeoDataRiverPathPoint::CtrlZonePosition position;
		QVector<double> ctrlPointVector;
	};
	void myRestore(const CtrlPoints& points);
	/// @brief Backuped control points
	QList<CtrlPoints> m_ctrlPoints;
};

#endif // GEODATARIVERSURVEYCTRLPOINTBACKUP_H
