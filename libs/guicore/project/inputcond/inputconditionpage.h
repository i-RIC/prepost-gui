#ifndef INPUTCONDITIONPAGE_H
#define INPUTCONDITIONPAGE_H

#include "../../guicore_global.h"

#include <QString>
#include <QWidget>
class QDomNode;
class InputConditionWidgetSet;
class SolverDefinitionTranslator;
class QLayout;
class QBoxLayout;
class QGridLayout;
class QDomNodeList;
class QLabel;

class GUICOREDLL_EXPORT InputConditionPage : public QWidget
{

public:
	InputConditionPage(QWidget* parent);
	InputConditionPage(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t, QWidget* parent);

	const QString& name() const;

private:
	void load(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadAuto(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadSimple(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadCustom(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QObject* loadRec(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	void loadBL(QBoxLayout* l, const QDomNodeList& list, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	void loadGL(QGridLayout* l, const QDomNodeList& list, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);

	QString m_name;
};

#endif // INPUTCONDITIONPAGE_H
