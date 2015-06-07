#ifndef CGNSFILEINPUTCONDITIONWIDGETFILENAME_H
#define CGNSFILEINPUTCONDITIONWIDGETFILENAME_H

#include "../../guicore_global.h"

#include "inputconditionwidget.h"
#include <QLineEdit>

class InputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetFilenameEdit : public QLineEdit
{
	Q_OBJECT

public:
	InputConditionWidgetFilenameEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}
	bool checkContent();

protected:
	void focusOutEvent(QFocusEvent* e) override;

signals:
	void valueChanged(QString txt);
};

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFilename : public InputConditionWidget
{
	Q_OBJECT

public:
	CgnsFileInputConditionWidgetFilename(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);
	void setDisabled(bool disable) {m_edit->setDisabled(disable);}
	static QString defaultFolder;

public slots:
	void setValue(const QString& newvalue) {m_edit->setText(newvalue);}
	void getWidgetValue(const QString& newvalue);

private slots:
	void openFileDialog();

private:
	InputConditionContainerString* m_container;
	InputConditionWidgetFilenameEdit* m_edit;
	bool m_all;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFILENAME_H
