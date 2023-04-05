#ifndef GEODATAPROXY_UPDATEACTORSETTINGCOMMAND_H
#define GEODATAPROXY_UPDATEACTORSETTINGCOMMAND_H

#include "../../../guicore_global.h"
#include "../geodataproxy.h"

#include <QUndoCommand>

#include <memory>

class GUICOREDLL_EXPORT GeoDataProxy::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(QUndoCommand* command, GeoDataProxy* proxy);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GeoDataProxy* m_proxy;
};

#endif // GEODATAPROXY_UPDATEACTORSETTINGCOMMAND_H
