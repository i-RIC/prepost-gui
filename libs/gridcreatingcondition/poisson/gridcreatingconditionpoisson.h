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

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // GRIDCREATINGCONDITIONPOISSON_H
