#ifndef INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H
#define INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H

#include "inputconditiondependency.h"

class InputConditionWidgetImage;

class InputConditionDependencyCheckSubimages : public InputConditionDependency
{
	Q_OBJECT

public:
	InputConditionDependencyCheckSubimages(InputConditionWidgetImage* image);

public slots:
	void check() override;

private:
	InputConditionWidgetImage* m_image;
};

#endif // INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H
