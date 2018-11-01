#ifndef INPUTCONDITIONWIDGETSET_H
#define INPUTCONDITIONWIDGETSET_H

#include "../../guicore_global.h"
#include <QMap>
#include <QVector>

#include <string>

class QWidget;
class QDomNode;
class InputConditionContainerSet;
class InputConditionWidget;
class SolverDefinition;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetSet
{

public:
	InputConditionWidgetSet();
	~InputConditionWidgetSet();

	void setup(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC = false);
	InputConditionWidget* widget(const std::string& name) const;

	bool checkImportSourceUpdate();
	void clear();
	void toggleReadOnly(bool readonly);

private:
	void buildWidgets(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC);
	void buildWidgetsSimple(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidgetsCustom(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void buildWidgetsCustomRec(const QDomNode& node, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void buildWidget(QDomNode& itemNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildImage(QDomNode& itemNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void buildDeps(const QDomNode&, InputConditionContainerSet& cset, bool forBC);
	void buildDepsSimple(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustom(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustomRec(const QDomNode& node, InputConditionContainerSet& cset);
	void buildDepsItem(const QDomNode& itemNode, InputConditionContainerSet& cset);
	void buildDepsImage(const QDomNode& imageNode, InputConditionContainerSet& cset);
	void buildDep(const QDomNode&, InputConditionContainerSet& cset, InputConditionWidget* w);
	void addTooltip(InputConditionWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t);

	QMap<std::string, InputConditionWidget*> m_widgets;
};
#endif // INPUTCONDITIONWIDGETSET_H
