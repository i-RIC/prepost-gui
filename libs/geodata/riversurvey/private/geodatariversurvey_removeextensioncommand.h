#ifndef GEODATARIVERSURVEY_REMOVEEXTENSIONCOMMAND_H
#define GEODATARIVERSURVEY_REMOVEEXTENSIONCOMMAND_H

#include "geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::RemoveExtensionCommand : public QUndoCommand
{
public:
	RemoveExtensionCommand(bool left, const QPointF& pos, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	void updateData();

	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	bool m_left;
	QPointF m_position;
};

#endif // GEODATARIVERSURVEY_REMOVEEXTENSIONCOMMAND_H
