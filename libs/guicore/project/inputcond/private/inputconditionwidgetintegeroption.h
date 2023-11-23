#ifndef INPUTCONDITIONWIDGETINTEGEROPTION_H
#define INPUTCONDITIONWIDGETINTEGEROPTION_H

#include "../inputconditionwidget.h"

class InputConditionContainerInteger;
class InputConditionDependencyCheckSubEnumerations;
class SolverDefinitionTranslator;

class QComboBox;

class InputConditionWidgetIntegerOption : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetIntegerOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont, bool noStretch);

	void addTooltip(const QString& tooltip) override;

	void setDisabled(bool disable);

	static QString subEnumerationsName(const QDomNode& subEnumerationsNode, size_t index);

	void activateSubEnumerations(const QString& name);
	void inactivateSubEnumerations();

	InputConditionDependencyCheckSubEnumerations* checkSubEnumerations() const;

public slots:
	void setValue(int);

private slots:
	void informChange(int);

private:
	InputConditionContainerInteger* m_container;
	QComboBox* m_comboBox;
	std::list< std::pair<QString, QVariant> > m_enumerations;
	std::map< QString, std::list< std::pair<QString, QVariant> > > m_subEnumerations;
	InputConditionDependencyCheckSubEnumerations* m_checkSubEnumerations;
};

#endif // INPUTCONDITIONWIDGETINTEGEROPTION_H
