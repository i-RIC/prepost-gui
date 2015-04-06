#ifndef CGNSFILEINPUTCONDITIONPAGECONTAINER_H
#define CGNSFILEINPUTCONDITIONPAGECONTAINER_H

#include "../../guicore_global.h"

#include <QWidget>
#include <QMap>

class QDomElement;
class SolverDefinitionTranslator;
class CgnsFileInputConditionPage;
class CgnsFileInputConditionWidgetSet;

class GUICOREDLL_EXPORT CgnsFileInputConditionPageContainer : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	CgnsFileInputConditionPageContainer(QWidget* w);
	/// Destructor
	~CgnsFileInputConditionPageContainer();
	void setup(const QDomElement& elem, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t);

public slots:
	void pageSelected(const QString& pagename);

private:
	QString m_pageName;
	QMap<QString, CgnsFileInputConditionPage*> m_pages;
	CgnsFileInputConditionPage* m_currentPage;
};


#endif // CGNSFILEINPUTCONDITIONPAGECONTAINER_H
