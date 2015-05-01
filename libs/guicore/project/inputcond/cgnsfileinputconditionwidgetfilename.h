#ifndef CGNSFILEINPUTCONDITIONWIDGETFILENAME_H
#define CGNSFILEINPUTCONDITIONWIDGETFILENAME_H

#include "../../guicore_global.h"

#include "cgnsfileinputconditionwidget.h"
#include <QLineEdit>

class CgnsFileInputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFilenameEdit : public QLineEdit
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetFilenameEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}
	bool checkContent();

protected:
	void focusOutEvent(QFocusEvent* e);
signals:
	void valueChanged(QString txt);
};

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetFilename : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetFilename(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerString* cont);
	void setDisabled(bool disable) {m_edit->setDisabled(disable);}
	static QString defaultFolder;

public slots:
	void setValue(const QString& newvalue) {m_edit->setText(newvalue);}
	void getWidgetValue(const QString& newvalue);

private slots:
	void openFileDialog();

private:
	CgnsFileInputConditionContainerString* m_container;
	CgnsFileInputConditionWidgetFilenameEdit* m_edit;
	bool m_all;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFILENAME_H
