#include "distancemeasuredataitem_translatecommand.h"
#include "../../misc/qundocommandhelper.h"

DistanceMeasureDataItem::TranslateCommand::TranslateCommand(const QVector2D& p1, const QVector2D& p2, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Translate Distance Measure")),
	m_newPoint1 {p1},
	m_newPoint2 {p2},
	m_oldPoint1 {item->m_point1},
	m_oldPoint2 {item->m_point2},
	m_finish {finish},
	m_item {item}
{}

void DistanceMeasureDataItem::TranslateCommand::redo()
{
	applyPoints(m_newPoint1, m_newPoint2);

	if (m_finish) {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
	} else {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meTranslate;
	}
	m_item->updateMouseCursor();
}

void DistanceMeasureDataItem::TranslateCommand::undo()
{
	applyPoints(m_oldPoint1, m_oldPoint2);

	m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
	m_item->updateMouseCursor();
}

int DistanceMeasureDataItem::TranslateCommand::id() const
{
	return iRIC::generateCommandId("DistanceMeasureTranslateCommand");
}

bool DistanceMeasureDataItem::TranslateCommand::mergeWith(const QUndoCommand *other)
{
	const TranslateCommand* other2 = dynamic_cast<const TranslateCommand*>(other);
	if (other2 == nullptr) {return false;}

	if (m_finish) {return false;}

	m_newPoint1 = other2->m_newPoint1;
	m_newPoint2 = other2->m_newPoint2;
	m_finish = other2->m_finish;
	return true;
}

void DistanceMeasureDataItem::TranslateCommand::applyPoints(const QVector2D& p1, const QVector2D& p2)
{
	m_item->m_point1 = p1;
	m_item->m_point2 = p2;

	m_item->updateActorSettings();
}
