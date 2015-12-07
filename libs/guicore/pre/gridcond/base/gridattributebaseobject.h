#ifndef GRIDATTRIBUTEBASEOBJECT_H
#define GRIDATTRIBUTEBASEOBJECT_H

#include "../../../guicore_global.h"

#include <QObject>

class SolverDefinitionGridAttribute;

class GUICOREDLL_EXPORT GridAttributeBaseObject : public QObject
{
	Q_OBJECT

public:
	GridAttributeBaseObject(SolverDefinitionGridAttribute* cond, QObject* parent = nullptr);
	virtual ~GridAttributeBaseObject();

	SolverDefinitionGridAttribute* gridAttribute() const;

private:
	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEBASEOBJECT_H
