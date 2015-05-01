#ifndef CGNSFILEINPUTCONDITIONPAGE_H
#define CGNSFILEINPUTCONDITIONPAGE_H

#include "../../guicore_global.h"

#include <QString>
#include <QWidget>
class QDomNode;
class CgnsFileInputConditionWidgetSet;
class SolverDefinitionTranslator;
class QLayout;
class QBoxLayout;
class QGridLayout;
class QDomNodeList;
class QLabel;

class GUICOREDLL_EXPORT CgnsFileInputConditionPage : public QWidget
{
public:
	/// Constructor
	CgnsFileInputConditionPage(QWidget* parent) : QWidget(parent) {}
	/// Constructor
	CgnsFileInputConditionPage(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t, QWidget* parent);
	const QString& name() {return m_name;}

private:
	void load(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadAuto(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadSimple(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLayout* loadCustom(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QObject* loadRec(const QDomNode& node, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	void loadBL(QBoxLayout* l, const QDomNodeList& list, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	void loadGL(QGridLayout* l, const QDomNodeList& list, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);
	QLabel* getCaption(const QString& caption);
	QString m_name;
};

#endif // CGNSFILEINPUTCONDITIONPAGE_H
