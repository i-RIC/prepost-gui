#ifndef INPUTCONDITIONDIALOG_H
#define INPUTCONDITIONDIALOG_H

#include "../../guicore_global.h"

#include <QDialog>
class QAbstractButton;
class SolverDefinition;
class QLocale;

namespace Ui
{
	class InputConditionDialog;
}

class InputConditionContainerSet;
class InputConditionWidgetSet;

class GUICOREDLL_EXPORT InputConditionDialog : public QDialog
{
	Q_OBJECT

public:
	InputConditionDialog(SolverDefinition* solverDef, const QLocale& locale, QWidget* parent = nullptr);
	~InputConditionDialog();

	void setFileName(const QString& fileName) {m_fileName = fileName;}
	void setWorkFolder(const QString& workFolder) {m_workFolder = workFolder;}
	/// Load data fron CGNS file.
	void load(const int fn);
	/// Save data into CGNS file.
	void save(const int fn);
	/// Import input condition from CGNS file.
	bool importFromCgns(const QString& filename);
	/// Import input condition from yaml file.
	bool importFromYaml(const QString& filename);
	/// Export input condition into CGNS file.
	bool exportToCgns(const QString& filename);
	/// Export input condition to yaml file.
	bool exportToYaml(const QString& filename);
	/// Set Readonly mode
	void setReadOnly(bool readonly);

	void checkImportSourceUpdate();

public slots:
	void accept() override;
	void reject() override;

protected:
	void changeEvent(QEvent* e) override;

private slots:
	void handleButtonClick(QAbstractButton* button);
	void setModified();

private:
	/// Setup the dialog.
	void setup(const SolverDefinition& def, const QLocale& locale);
	void reset();

	InputConditionContainerSet* m_containerSet;
	InputConditionContainerSet* m_containerSetBackup;
	InputConditionWidgetSet* m_widgetSet;

	SolverDefinition* m_solverDefinition;
	/// CGNS Filename to load and save input condition.
	QString m_fileName;
	/// Work directory to create temporary files when importing and exporting.
	QString m_workFolder;
	bool m_modified;
	bool m_readonly;

	Ui::InputConditionDialog* ui;

public:
	friend class InputConditionContainerSet;
};

#endif // INPUTCONDITIONDIALOG_H
