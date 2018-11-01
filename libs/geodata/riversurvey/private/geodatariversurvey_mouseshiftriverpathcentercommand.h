#ifndef GEODATARIVERSURVEY_MOUSESHIFTRIVERPATHCENTERCOMMAND_H
#define GEODATARIVERSURVEY_MOUSESHIFTRIVERPATHCENTERCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand : public QUndoCommand
{
public:
	MouseShiftRiverPathCenterCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	QList<GeoDataRiverPathPoint*> m_points;
	double m_shiftValue;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_MOUSESHIFTRIVERPATHCENTERCOMMAND_H
