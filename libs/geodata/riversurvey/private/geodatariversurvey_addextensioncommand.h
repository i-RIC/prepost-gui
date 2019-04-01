#ifndef GEODATARIVERSURVEY_ADDEXTENSIONCOMMAND_H
#define GEODATARIVERSURVEY_ADDEXTENSIONCOMMAND_H

#include "../geodatariversurvey.h"
#include "../geodatariverpathpointextensionadddialog.h"

#include <QPointF>
#include <QUndoCommand>

class GeoDataRiverPathPoint;

class GeoDataRiverSurvey::AddExtensionCommand : public QUndoCommand
{
public:
	AddExtensionCommand(bool apply, GeoDataRiverPathPointExtensionAddDialog::LineMode lm, const QPointF& pos, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	QPointF m_position;
	GeoDataRiverPathPointExtensionAddDialog::LineMode m_lineMode;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
};


#endif // GEODATARIVERSURVEY_ADDEXTENSIONCOMMAND_H
