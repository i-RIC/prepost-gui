#ifndef GEODATANETCDFIMPORTERSETTINGDIALOG_H
#define GEODATANETCDFIMPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataNetcdfImporterSettingDialog;
}

class SolverDefinitionGridAttribute;

class QComboBox;

class GeoDataNetcdfImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	class NcVariable
	{

	public:
		QString name;
		std::vector<QString> dimensions;
	};

	explicit GeoDataNetcdfImporterSettingDialog(QWidget* parent = 0);
	~GeoDataNetcdfImporterSettingDialog();

	void setCondition(SolverDefinitionGridAttribute*	condition);
	void setVariables(const std::vector<NcVariable>& vars);
	bool needToShow() const;
	QString variableName() const;
	std::vector<QString> dimensionMappingSetting() const;

private slots:
	void handleVarChange(int index);

private:
	std::vector<QComboBox*> m_dimComboBoxes;
	std::vector<NcVariable> m_variables;

	Ui::GeoDataNetcdfImporterSettingDialog* ui;
};

#endif // GEODATANETCDFIMPORTERSETTINGDIALOG_H
