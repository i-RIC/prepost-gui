#ifndef CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H
#define CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"
#include <QLineEdit>

class InputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetFoldernameEdit : public QLineEdit
{
	Q_OBJECT

public:
	InputConditionWidgetFoldernameEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}
	bool checkContent();

protected:
	void focusOutEvent(QFocusEvent* e) override;

signals:
	void valueChanged(QString txt);
};

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFoldername : public InputConditionWidget
{
	Q_OBJECT

public:
	CgnsFileInputConditionWidgetFoldername(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);
	void setDisabled(bool disable) {m_edit->setDisabled(disable);}
	static QString defaultFolder;

public slots:
	void setValue(const QString& newvalue) {m_edit->setText(newvalue);}
	void getWidgetValue(const QString& newvalue);

private slots:
	void openDirDialog();

private:
	InputConditionContainerString* m_container;
	InputConditionWidgetFoldernameEdit* m_edit;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H
