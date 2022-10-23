#ifndef VALUEMODIFYCOMMANDT_DETAIL_H
#define VALUEMODIFYCOMMANDT_DETAIL_H

#include "../valuemodifycommandt.h"

template <class V>
ValueModifyCommmand<V>::ValueModifyCommmand(const QString& name, const V& newValue, V* target, QUndoCommand* parent):
	QUndoCommand {name, parent},
	m_newValue {newValue},
	m_oldValue {*target},
	m_target {target},
	m_id {-1},
	m_allowMerge {false}
{}

template <class V>
ValueModifyCommmand<V>::ValueModifyCommmand(int id, bool allowMerge, const V& newValue, V* target) :
	QUndoCommand {},
	m_newValue {newValue},
	m_oldValue {*target},
	m_target {target},
	m_id {id},
	m_allowMerge {allowMerge}
{}

template <class V>
void ValueModifyCommmand<V>::redo()
{
	apply(m_newValue);
}

template <class V>
void ValueModifyCommmand<V>::undo()
{
	apply(m_oldValue);
}

template <class V>
int ValueModifyCommmand<V>::id() const
{
	return m_id;
}

template <class V>
bool ValueModifyCommmand<V>::mergeWith(const QUndoCommand* other)
{
	if (!m_allowMerge) {return false;}

	auto c = dynamic_cast<const ValueModifyCommmand<V>*>(other);
	if (c == nullptr) {return false;}

	if (m_target != c->m_target) {return false;}

	m_newValue = c->m_newValue;
	m_allowMerge = c->m_allowMerge;
	return true;
}

template <class V>
void ValueModifyCommmand<V>::apply(const V& value)
{
	*m_target = value;
}

#endif // VALUEMODIFYCOMMANDT_DETAIL_H
