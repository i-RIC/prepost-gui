#ifndef GEODATARIVERSURVEY_RENAMERIVERPATHPOINTCOMMAND_H
#define GEODATARIVERSURVEY_RENAMERIVERPATHPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::RenameRiverPathPointCommand : public QUndoCommand
{
public:
	RenameRiverPathPointCommand(const QString& newname, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	void setName(const QString& name);

	QString m_newName;
	QString m_oldName;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_RENAMERIVERPATHPOINTCOMMAND_H
