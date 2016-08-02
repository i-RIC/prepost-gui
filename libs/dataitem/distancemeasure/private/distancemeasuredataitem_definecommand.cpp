#include "distancemeasuredataitem_definecommand.h"

#include <guicore/misc/qundocommandhelper.h>

DistanceMeasureDataItem::DefineCommand::DefineCommand(const QVector2D& v1, const QVector2D& v2, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Define Distance Measure")),
	m_newPoint1 {v1},
	m_newPoint2 {v2},
	m_oldPoint1 {item->m_point1},
	m_oldPoint2 {item->m_point2},
	m_finish {finish}
{}

void DistanceMeasureDataItem::DefineCommand::redo()
{
	m_item->m_point1 = m_newPoint1;
	m_item->m_point2 = m_newPoint2;
	if (m_finish) {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
	} else {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meDefining;
	}
	m_item->m_defined = true;

	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

void DistanceMeasureDataItem::DefineCommand::undo()
{
	m_item->m_point1 = m_oldPoint1;
	m_item->m_point2 = m_oldPoint2;
	m_item->m_mouseEventMode = DistanceMeasureDataItem::meBeforeDefining;
	m_item->m_defined = false;

	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

int DistanceMeasureDataItem::DefineCommand::id() const
{
	return iRIC::generateCommandId("DistanceMeasureDefineCommand");
}

bool DistanceMeasureDataItem::DefineCommand::mergeWith(const QUndoCommand* other)
{
	const DefineCommand* other2 = dynamic_cast<const DefineCommand*>(other);
	if (other2 == nullptr) {return false;}

	m_newPoint1 = other2->m_newPoint1;
	m_newPoint2 = other2->m_newPoint2;
	m_finish = other2->m_finish;
	return true;
}
