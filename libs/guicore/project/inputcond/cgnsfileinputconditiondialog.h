#ifndef CGNSFILEINPUTCONDITIONDIALOG_H
#define CGNSFILEINPUTCONDITIONDIALOG_H

#include "../../guicore_global.h"

#include <QDialog>
class QAbstractButton;
class SolverDefinition;
class QLocale;

namespace Ui {
	class CgnsFileInputConditionDialog;
}

class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionWidgetSet;

class GUICOREDLL_EXPORT CgnsFileInputConditionDialog : public QDialog {
	Q_OBJECT
public:
	/// Constructor
	CgnsFileInputConditionDialog(SolverDefinition* solverDef, const QLocale& locale, QWidget *parent = 0);
	/// Destructor
	~CgnsFileInputConditionDialog();
	void setFileName(const QString& fileName){m_fileName = fileName;}
	void setWorkFolder(const QString& workFolder){m_workFolder = workFolder;}
	/// Load data fron CGNS file.
	void load(const int fn);
	/// Save data into CGNS file.
	void save(const int fn);
	/// Import input condition from extra file.
	bool import(const QString& filename);
	/// Export input condition into extra file.
	bool doExport(const QString& filename);
	/// Set Readonly mode
	void setReadOnly(bool readonly);

public slots:
	void accept();
	void reject();

protected:
	void changeEvent(QEvent *e);

private slots:
	void handleButtonClick(QAbstractButton* button);
	void setModified();

private:
	/// Setup the dialog.
	void setup(const SolverDefinition& def, const QLocale& locale);
	void reset();

	CgnsFileInputConditionContainerSet* m_containerSet;
	CgnsFileInputConditionWidgetSet* m_widgetSet;

	SolverDefinition* m_solverDefinition;
	/// CGNS Filename to load and save input condition.
	QString m_fileName;
	/// Work directory to create temporary files when importing and exporting.
	QString m_workFolder;
	bool m_modified;
	bool m_readonly;

	Ui::CgnsFileInputConditionDialog *ui;
public:
	friend class CgnsFileInputConditionContainerSet;
};

#endif // CGNSFILEINPUTCONDITIONDIALOG_H
