#ifndef GEODATA_UPDATEACTORSETTINGCOMMAND_H
#define GEODATA_UPDATEACTORSETTINGCOMMAND_H

#include "../../../guicore_global.h"
#include "../geodata.h"

#include <QUndoCommand>

#include <memory>

class GUICOREDLL_EXPORT GeoData::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(QUndoCommand* command, GeoData* geoData);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GeoData* m_geoData;
};

#endif // GEODATA_UPDATEACTORSETTINGCOMMAND_H
