#ifndef GEODATANETCDFIMPORTERSETTINGDIALOG_H
#define GEODATANETCDFIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QComboBox>

namespace Ui
{
	class GeoDataNetcdfImporterSettingDialog;
}

class SolverDefinitionGridAttribute;

class GeoDataNetcdfImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	class NcVariable
	{

	public:
		QString name;
		QList<QString> dimensions;
	};

	explicit GeoDataNetcdfImporterSettingDialog(QWidget* parent = 0);
	~GeoDataNetcdfImporterSettingDialog();

	void setCondition(SolverDefinitionGridAttribute*	condition);
	void setVariables(const QList<NcVariable>& vars);
	bool needToShow() const;
	QString variableName() const;
	QList<QString> dimensionMappingSetting() const;

private slots:
	void handleVarChange(int index);

private:
	QList<QComboBox*> m_dimComboBoxes;
	QList<NcVariable> m_variables;

	Ui::GeoDataNetcdfImporterSettingDialog* ui;
};

#endif // GEODATANETCDFIMPORTERSETTINGDIALOG_H
