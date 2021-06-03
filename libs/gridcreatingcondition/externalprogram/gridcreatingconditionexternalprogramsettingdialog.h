#ifndef GRIDCREATINGCONDITIONEXTERNALPROGRAMSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONEXTERNALPROGRAMSETTINGDIALOG_H

#include <QDialog>
#include <QMap>
class QAbstractButton;
class SolverDefinition;
class QLocale;
class iRICMainWindowInterface;

namespace Ui
{
	class GridCreatingConditionExternalProgramSettingDialog;
}

class InputConditionContainerSet;
class InputConditionWidgetSet;

class GridCreatingConditionExternalProgramSettingDialog : public QDialog
{
	Q_OBJECT

public:
	GridCreatingConditionExternalProgramSettingDialog(SolverDefinition* def, const QLocale& locale, iRICMainWindowInterface* mainW, QWidget* parent);
	~GridCreatingConditionExternalProgramSettingDialog();

	bool load();
	bool save();
	/// Reset to store default values.
	void reset(bool nodialog = false);
	/// Set the file name to use for CGNS.
	void setFilename(const QString& filename);
	QString errorMessage(int errorcode) const;

public slots:
	void accept() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	/// Setup the dialog.
	void setup(const SolverDefinition& def, const QLocale& locale);

	Ui::GridCreatingConditionExternalProgramSettingDialog* ui;
	InputConditionContainerSet* m_containerSet;
	InputConditionWidgetSet* m_widgetSet;
	QMap<int, QString> m_errorMessages;
	QString m_filename;

public:
	friend class InputConditionContainerSet;
};

#endif // GRIDCREATINGCONDITIONEXTERNALPROGRAMSETTINGDIALOG_H
