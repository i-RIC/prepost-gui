#ifndef PREPROCESSORGRIDSHAPEDATAITEM_UPDATEPOINTSCOMMAND_H
#define PREPROCESSORGRIDSHAPEDATAITEM_UPDATEPOINTSCOMMAND_H

#include "../preprocessorgridshapedataitem.h"

#include <QUndoCommand>

#include <vtkSmartPointer.h>

class PreProcessorGridShapeDataItem::UpdatePointsCommand : public QUndoCommand
{
public:
	UpdatePointsCommand(bool finish, vtkPoints* newPoints, PreProcessorGridDataItem* item);
	~UpdatePointsCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void setPoints(vtkPoints* points, bool buildLocator);

	bool m_finish;

	vtkPoints* m_newPoints;
	vtkPoints* m_oldPoints;

	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDSHAPEDATAITEM_UPDATEPOINTSCOMMAND_H
