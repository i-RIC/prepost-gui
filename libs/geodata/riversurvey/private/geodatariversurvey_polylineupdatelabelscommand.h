#ifndef GEODATARIVERSURVEY_POLYLINEUPDATELABELSCOMMAND_H
#define GEODATARIVERSURVEY_POLYLINEUPDATELABELSCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

#include <memory>

class GeoDataRiverSurvey::PolyLineUpdateLabelsCommand : public QUndoCommand
{
public:
	PolyLineUpdateLabelsCommand(QUndoCommand* com, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_POLYLINEUPDATELABELSCOMMAND_H
