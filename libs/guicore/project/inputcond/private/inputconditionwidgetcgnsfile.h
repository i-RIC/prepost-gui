#ifndef INPUTCONDITIONWIDGETCGNSFILE_H
#define INPUTCONDITIONWIDGETCGNSFILE_H

#include <QWidget>

#include "../inputconditionwidget.h"

namespace Ui {
class InputConditionWidgetCgnsFile;
}

class InputConditionCgnsFile;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class InputConditionWidgetCgnsFile : public InputConditionWidget
{
	Q_OBJECT

public:
	explicit InputConditionWidgetCgnsFile(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);
	~InputConditionWidgetCgnsFile();

	void addTooltip(const QString& tooltip) override;

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();
	void handleFileChange();
	void openFileDialog();

private:
	void informChanged();

	InputConditionContainerString* m_container;
	InputConditionCgnsFile* m_cgnsFile;

	Ui::InputConditionWidgetCgnsFile *ui;
};

#endif // INPUTCONDITIONWIDGETCGNSFILE_H
