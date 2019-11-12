#ifndef INPUTCONDITIONDEPENDENCYCHECKSUBCAPTIONS_H
#define INPUTCONDITIONDEPENDENCYCHECKSUBCAPTIONS_H

#include "../inputconditiondependency.h"

class InputConditionWidgetLabel;

class InputConditionDependencyCheckSubCaptions : public InputConditionDependency
{
	Q_OBJECT

public:
	InputConditionDependencyCheckSubCaptions(InputConditionWidgetLabel* label);

public slots:
	void check() override;

private:
	InputConditionWidgetLabel* m_label;
};

#endif // INPUTCONDITIONDEPENDENCYCHECKSUBCAPTIONS_H
