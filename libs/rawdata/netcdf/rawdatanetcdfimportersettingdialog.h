#ifndef RAWDATANETCDFIMPORTERSETTINGDIALOG_H
#define RAWDATANETCDFIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QComboBox>

namespace Ui
{
	class RawDataNetcdfImporterSettingDialog;
}

class SolverDefinitionGridRelatedCondition;

class RawDataNetcdfImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	class NcVariable
	{
	public:
		QString name;
		QList<QString> dimensions;
	};

	explicit RawDataNetcdfImporterSettingDialog(QWidget* parent = 0);
	~RawDataNetcdfImporterSettingDialog();

	void setCondition(SolverDefinitionGridRelatedCondition*	condition);
	void setVariables(const QList<NcVariable>& vars);
	bool needToShow() const;
	QString variableName() const;
	QList<QString> dimensionMappingSetting() const;

private slots:
	void handleVarChange(int index);

private:
	QList<QComboBox*> m_dimComboBoxes;
	QList<NcVariable> m_variables;

	Ui::RawDataNetcdfImporterSettingDialog* ui;
};

#endif // RAWDATANETCDFIMPORTERSETTINGDIALOG_H
