#ifndef INPUTCONDITIONWIDGETFOLDERNAME_H
#define INPUTCONDITIONWIDGETFOLDERNAME_H

#include "../inputconditionwidget.h"

class AsciiOnlyLineEdit;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class InputConditionWidgetFoldername : public InputConditionWidget
{
	Q_OBJECT

public:
	static QString defaultFolder;
	InputConditionWidgetFoldername(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();
	void openDirDialog();

private:
	InputConditionContainerString* m_container;
	AsciiOnlyLineEdit* m_lineEdit;
};

#endif // INPUTCONDITIONWIDGETFOLDERNAME_H
