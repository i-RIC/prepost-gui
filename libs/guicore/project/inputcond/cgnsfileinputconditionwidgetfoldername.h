#ifndef CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H
#define CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"
#include <QLineEdit>

class CgnsFileInputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFoldernameEdit : public QLineEdit {
	Q_OBJECT

public:
	CgnsFileInputConditionWidgetFoldernameEdit(QWidget* parent = 0) : QLineEdit(parent){}
	bool checkContent();

protected:
	void focusOutEvent(QFocusEvent* e);
signals:
	void valueChanged(QString txt);
};

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFoldername : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetFoldername(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerString* cont);
	void setDisabled(bool disable){m_edit->setDisabled(disable);}
	static QString defaultFolder;

public slots:
	void setValue(const QString& newvalue){m_edit->setText(newvalue);}
	void getWidgetValue(const QString& newvalue);

private slots:
	void openDirDialog();

private:
	CgnsFileInputConditionContainerString* m_container;
	CgnsFileInputConditionWidgetFoldernameEdit* m_edit;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFOLDERNAME_H
