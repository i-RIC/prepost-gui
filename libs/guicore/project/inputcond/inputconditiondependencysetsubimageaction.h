#ifndef INPUTCONDITIONDEPENDENCYSETSUBIMAGEACTION_H
#define INPUTCONDITIONDEPENDENCYSETSUBIMAGEACTION_H

#include "inputconditiondependency.h"
#include "inputconditionwidgetimage.h"

class InputConditionDependencySetSubimageAction : public InputConditionDependency::Action
{
public:
	InputConditionDependencySetSubimageAction(InputConditionWidgetImage* w, const QString& name);
	~InputConditionDependencySetSubimageAction();

	void positiveAction();
	void negativeAction();

private:
	QString m_imageName;
};

#endif // INPUTCONDITIONDEPENDENCYSETSUBIMAGEACTION_H
