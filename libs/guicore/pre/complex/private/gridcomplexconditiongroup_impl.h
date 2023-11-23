#ifndef GRIDCOMPLEXCONDITIONGROUP_IMPL_H
#define GRIDCOMPLEXCONDITIONGROUP_IMPL_H

#include "../gridcomplexconditiongroup.h"
#include "../../../project/inputcond/inputconditioncontainerinteger.h"
#include "../../../project/inputcond/inputconditioncontainerstring.h"
#include "../../../project/inputcond/inputconditioncontainerset.h"
#include "../../../project/inputcond/inputconditionwidgetset.h"

#include <vector>

class InputConditionPage;

class GridComplexConditionGroup::Impl
{
public:
	Impl(SolverDefinition* def, const QDomElement& elem);
	~Impl();

	static std::vector<std::string> widgetNames(const QDomElement& elem);
	static void widgetNamesRec(const QDomElement& elem, std::vector<std::string>* names);

	InputConditionPage* m_page;

	InputConditionWidgetSet m_widgetSet;
	InputConditionWidgetSet m_tableWidgetSet;

	std::vector<QWidget*> m_tableLabels;
	std::vector<QWidget*> m_tableWidgets;

	InputConditionContainerString m_caption;
	InputConditionContainerString m_color;
	InputConditionContainerInteger m_isDefault;
	InputConditionContainerSet m_containerSet;

	InputConditionContainerString m_captionBackup;
	InputConditionContainerString m_colorBackup;
	InputConditionContainerInteger m_isDefaultBackup;
	InputConditionContainerSet m_containerSetBackup;
};

#endif // GRIDCOMPLEXCONDITIONGROUP_IMPL_H
