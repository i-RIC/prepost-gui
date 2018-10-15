#ifndef INPUTCONDITIONWIDGETFILENAME_H
#define INPUTCONDITIONWIDGETFILENAME_H

#include "../../guicore_global.h"

#include "inputconditionwidget.h"

class AsciiOnlyLineEdit;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetFilename : public InputConditionWidget
{
	Q_OBJECT

public:
	static QString defaultFolder;
	InputConditionWidgetFilename(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);

	void addTooltip(const QString& tooltip) override;

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();
	void openFileDialog();

private:
	InputConditionContainerString* m_container;
	AsciiOnlyLineEdit* m_lineEdit;
	bool m_all;
};

#endif // INPUTCONDITIONWIDGETFILENAME_H
