#ifndef VALUEMODIFYCOMMANDT_H
#define VALUEMODIFYCOMMANDT_H

#include <QUndoCommand>

template <class V>
class ValueModifyCommmand : public QUndoCommand
{
public:
	ValueModifyCommmand(const QString& name, const V& newValue, V* target, QUndoCommand* parent = nullptr);
	ValueModifyCommmand(int id, bool allowMerge, const V& newValue, V* target);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const V& value);

	V m_newValue;
	V m_oldValue;

	V* m_target;

	int m_id;
	bool m_allowMerge;
};

#include "private/valuemodifycommandt_detail.h"

#endif // VALUEMODIFYCOMMANDT_H
