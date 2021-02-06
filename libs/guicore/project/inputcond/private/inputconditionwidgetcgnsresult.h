#ifndef INPUTCONDITIONWIDGETCGNSRESULT_H
#define INPUTCONDITIONWIDGETCGNSRESULT_H

#include "../inputconditionwidget.h"

#include <QStringList>

class InputConditionCgnsFile;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class QComboBox;

class InputConditionWidgetCgnsResult : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetCgnsResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

	void addTooltip(const QString& tooltip) override;

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleFileChange();
	void informChange(int);

protected:
	InputConditionCgnsFile* cgnsFile() const;

private:
	void updateEnums();
	virtual QStringList getEnums() const = 0;

	InputConditionContainerString* m_container;
	QComboBox* m_comboBox;

	InputConditionCgnsFile* m_cgnsFile;

	const SolverDefinitionTranslator* m_translator;
};

#endif // INPUTCONDITIONWIDGETCGNSRESULT_H
