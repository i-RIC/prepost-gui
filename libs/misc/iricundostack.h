#ifndef IRICUNDOSTACK_H
#define IRICUNDOSTACK_H

#include "misc_global.h"

#include <QUndoStack>

/// QUndoStack instance to be used in iRIC GUI. It implements singleton design pattern.
class MISCDLL_EXPORT iRICUndoStack : public QUndoStack
{

public:
	static const int DEFAULT_UNDOLIMIT;
	static void initialize(QObject* w);
	static iRICUndoStack& instance();

private:
	iRICUndoStack(QObject* parent = nullptr);

	static iRICUndoStack* m_instance;
};

#endif // IRICUNDOSTACK_H
