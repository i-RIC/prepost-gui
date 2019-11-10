#ifndef POLYLINEEDITCOORDINATESCOMMAND_H
#define POLYLINEEDITCOORDINATESCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

#include <vector>

class PolyLineController;

class GUIBASEDLL_EXPORT PolyLineEditCoordinatesCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolyLineEditCoordinatesCommand(const std::vector<QPointF>& coords, PolyLineController* line);
	PolyLineEditCoordinatesCommand(const QString& title, const std::vector<QPointF>& coords, PolyLineController* line);

	void redo() override;
	void undo() override;

private:
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	PolyLineController* m_line;
};

#endif // POLYLINEEDITCOORDINATESCOMMAND_H
