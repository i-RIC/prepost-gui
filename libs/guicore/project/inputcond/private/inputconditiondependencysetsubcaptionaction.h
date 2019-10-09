#ifndef INPUTCONDITIONDEPENDENCYSETSUBCAPTIONACTION_H
#define INPUTCONDITIONDEPENDENCYSETSUBCAPTIONACTION_H

#include "../inputconditiondependency.h"

class InputConditionWidgetLabel;

class InputConditionDependencySetSubCaptionAction : public InputConditionDependency::Action
{
public:
	InputConditionDependencySetSubCaptionAction(InputConditionWidgetLabel* w, const QString& caption);

	void positiveAction() override;
	void negativeAction() override;

private:
	QString m_caption;
};

#endif // INPUTCONDITIONDEPENDENCYSETSUBCAPTIONACTION_H
