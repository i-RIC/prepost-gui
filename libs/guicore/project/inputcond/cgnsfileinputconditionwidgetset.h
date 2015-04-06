#ifndef CGNSFILEINPUTCONDITIONWIDGETSET_H
#define CGNSFILEINPUTCONDITIONWIDGETSET_H

#include "../../guicore_global.h"
#include <QMap>
#include <QVector>

class QWidget;
class QDomNode;
class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionWidget;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetSet {
public:
	/// Constructor
	CgnsFileInputConditionWidgetSet(){}
	/// Destructor
	~CgnsFileInputConditionWidgetSet();
	void setup(const QDomNode& condNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC = false);
	CgnsFileInputConditionWidget* widget(const QString& name) const {return m_widgets.value(name);}
	void clear();
	void disableWidgets();
	void enableWidgets();

private:
	void buildWidgets(const QDomNode& condNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC);
	void buildWidgetsSimple(const QDomNode& contNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidgetsCustom(const QDomNode& contNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidgetsCustomRec(const QDomNode& node, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildWidget(QDomNode& itemNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t);
	void buildDeps(const QDomNode&, CgnsFileInputConditionContainerSet& cset, bool forBC);
	void buildDepsSimple(const QDomNode& tabNode, CgnsFileInputConditionContainerSet& cset);
	void buildDepsCustom(const QDomNode& tabNode, CgnsFileInputConditionContainerSet& cset);
	void buildDepsCustomRec(const QDomNode& node, CgnsFileInputConditionContainerSet& cset);
	void buildDepsItem(const QDomNode& itemNode, CgnsFileInputConditionContainerSet& cset);
	void buildDep(const QDomNode&, CgnsFileInputConditionContainerSet& cset, CgnsFileInputConditionWidget *w);
	void addTooltip(QWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t);
	QMap<QString, CgnsFileInputConditionWidget*> m_widgets;
};
#endif // CGNSFILEINPUTCONDITIONWIDGETSET_H
