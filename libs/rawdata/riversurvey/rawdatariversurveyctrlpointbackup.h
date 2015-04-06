#ifndef RAWDATARIVERSURVEYCTRLPOINTBACKUP_H
#define RAWDATARIVERSURVEYCTRLPOINTBACKUP_H

#include "rd_riversurvey_global.h"
#include <Qlist>
#include <QVector>
#include "rawdatariverpathpoint.h"

class RD_RIVERSURVEY_EXPORT RawDataRiverSurveyCtrlPointBackup
{
public:
	RawDataRiverSurveyCtrlPointBackup(){}
	virtual ~RawDataRiverSurveyCtrlPointBackup(){}
	void backup(RawDataRiverPathPoint* point, RawDataRiverPathPoint::CtrlZonePosition position);
	void restore();
private:
	struct CtrlPoints {
		RawDataRiverPathPoint* point;
		RawDataRiverPathPoint::CtrlZonePosition position;
		QVector<double> ctrlPointVector;
	};
	void myRestore(CtrlPoints& points);
	/// @brief Backuped control points
	QList<CtrlPoints> m_ctrlPoints;
};

#endif // RAWDATARIVERSURVEYCTRLPOINTBACKUP_H
