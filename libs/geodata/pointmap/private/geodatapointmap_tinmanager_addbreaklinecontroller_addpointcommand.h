#ifndef GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_ADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_ADDPOINTCOMMAND_H

#include "geodatapointmap_tinmanager_addbreaklinecontroller.h"

#include <QUndoCommand>

class GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand : public QUndoCommand
{
public:
	AddPointCommand(bool click, bool removeLast, vtkIdType newPoint, AddBreakLineController* controller);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_click;
	bool m_removeLast;

	vtkIdType m_newPoint;
	vtkIdType m_oldPoint;
	BreakLine* m_breakLine;
};

#endif // GEODATAPOINTMAP_TINMANAGER_ADDBREAKLINECONTROLLER_ADDPOINTCOMMAND_H
