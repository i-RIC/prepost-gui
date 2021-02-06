#ifndef INPUTCONDITIONWIDGETCGNSFILE_H
#define INPUTCONDITIONWIDGETCGNSFILE_H

#include "../inputconditionwidgetfilename.h"

class InputConditionCgnsFile;

class InputConditionWidgetCgnsFile : public InputConditionWidgetFilename
{
	Q_OBJECT

public:
	InputConditionWidgetCgnsFile(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

public slots:
	void setValue(const QString& newvalue) override;

private slots:
	void handleEditingFinished() override;

private:
	void informChanged();

	InputConditionCgnsFile* m_cgnsFile;
};

#endif // INPUTCONDITIONWIDGETCGNSFILE_H
