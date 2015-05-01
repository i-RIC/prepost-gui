#ifndef CGNSFILEINPUTCONDITIONWIDGETFUNCTIONAL_H
#define CGNSFILEINPUTCONDITIONWIDGETFUNCTIONAL_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

class CgnsFileInputConditionContainerFunctional;
class SolverDefinitionTranslator;
class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionWidgetFunctionalDialog;
class iRICMainWindow;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFunctional : public CgnsFileInputConditionWidget
{
	Q_OBJECT

public:
	CgnsFileInputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerFunctional* cont);
	~CgnsFileInputConditionWidgetFunctional();
	void setDisabled(bool /*disable*/) {}

private:
	CgnsFileInputConditionContainerFunctional* m_container;
	const SolverDefinitionTranslator* m_translator;
	CgnsFileInputConditionWidgetFunctionalDialog* m_dialog;

private slots:
	void openDialog();
	void dialogAccepted();

public:
	friend class CgnsFileInputConditionWidgetFunctionalDialog;
};

#endif
