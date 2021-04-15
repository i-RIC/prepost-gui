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
	virtual void setValue(const QString& newvalue);

protected slots:
	virtual void handleEditingFinished();
	void openFileDialog();

protected:
	InputConditionContainerString* m_container;

private:
	AsciiOnlyLineEdit* m_lineEdit;
	bool m_all;
};

#endif // INPUTCONDITIONWIDGETFILENAME_H
