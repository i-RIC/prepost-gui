#ifndef INPUTCONDITIONPAGECONTAINER_H
#define INPUTCONDITIONPAGECONTAINER_H

#include "../../guicore_global.h"

#include <QWidget>
#include <QMap>

class QDomElement;
class SolverDefinitionTranslator;
class InputConditionPage;
class InputConditionWidgetSet;

class GUICOREDLL_EXPORT InputConditionPageContainer : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	InputConditionPageContainer(QWidget* w);
	/// Destructor
	~InputConditionPageContainer();
	void setup(const QDomElement& elem, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);

public slots:
	void pageSelected(const QString& pagename);

private:
	QString m_pageName;
	QMap<QString, InputConditionPage*> m_pages;
	InputConditionPage* m_currentPage;
};


#endif // INPUTCONDITIONPAGECONTAINER_H
