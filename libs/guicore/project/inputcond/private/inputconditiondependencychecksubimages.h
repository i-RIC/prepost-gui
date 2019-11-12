#ifndef INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H
#define INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H

#include "../inputconditiondependency.h"

class InputConditionWidgetImage;

class InputConditionDependencyCheckSubImages : public InputConditionDependency
{
	Q_OBJECT

public:
	InputConditionDependencyCheckSubImages(InputConditionWidgetImage* image);

public slots:
	void check() override;

private:
	InputConditionWidgetImage* m_image;
};

#endif // INPUTCONDITIONDEPENDENCYCHECKSUBIMAGES_H
