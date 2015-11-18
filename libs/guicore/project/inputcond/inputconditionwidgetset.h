#ifndef INPUTCONDITIONWIDGETSET_H
#define INPUTCONDITIONWIDGETSET_H

#include "../../guicore_global.h"
#include <QMap>
#include <QVector>

class QWidget;
class QDomNode;
class InputConditionContainerSet;
class InputConditionWidget;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetSet
{

public:
	/// Constructor
	InputConditionWidgetSet() {}
	/// Destructor
	~InputConditionWidgetSet();
	void setup(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC = false);
	InputConditionWidget* widget(const QString& name) const {return m_widgets.value(name);}
	void clear();
	void disableWidgets();
	void enableWidgets();

private:
	void buildWidgets(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC);
	void buildWidgetsSimple(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidgetsCustom(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidgetsCustomRec(const QDomNode& node, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidget(QDomNode& itemNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildDeps(const QDomNode&, InputConditionContainerSet& cset, bool forBC);
	void buildDepsSimple(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustom(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustomRec(const QDomNode& node, InputConditionContainerSet& cset);
	void buildDepsItem(const QDomNode& itemNode, InputConditionContainerSet& cset);
	void buildDep(const QDomNode&, InputConditionContainerSet& cset, InputConditionWidget* w);
	void addTooltip(QWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t);
	QMap<QString, InputConditionWidget*> m_widgets;
};
#endif // INPUTCONDITIONWIDGETSET_H
