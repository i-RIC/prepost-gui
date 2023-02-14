#ifndef INPUTCONDITIONWIDGETSTRING_H
#define INPUTCONDITIONWIDGETSTRING_H

#include "../inputconditionwidget.h"

class TextEditI;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class QComboBox;
class QLayout;

#include "../inputconditiondependency.h"  // can't forward declare nested classes (InputConditionDependency::ActionSetValue)

class InputConditionWidgetString : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, const QString& folderName);

	void addTooltip(const QString& tooltip) override;

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();
	void applyTemplate(int index);

private:
	void setupComboBoxIfNeeded(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, const QString& folderName, QLayout* l);

	InputConditionContainerString* m_container;
	TextEditI* m_textEdit;
	QComboBox* m_sourceComboBox;
	std::vector<QString> m_sourceFileNames;

public:
	friend class InputConditionDependency::ActionSetValue;
};
#endif // INPUTCONDITIONWIDGETSTRING_H
