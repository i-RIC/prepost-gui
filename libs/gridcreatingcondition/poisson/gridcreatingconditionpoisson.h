#ifndef GRIDCREATINGCONDITIONPOISSON_H
#define GRIDCREATINGCONDITIONPOISSON_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

class GridCreatingConditionPoisson : public GridCreatingCondition
{
	Q_OBJECT

public:
	GridCreatingConditionPoisson(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionPoisson();

	bool create(QWidget* parent) override;
	bool ready() const override;
	void clear() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/gridcreatingconditionpoisson_impl.h"
#endif // _DEBUG

#endif // GRIDCREATINGCONDITIONPOISSON_H
