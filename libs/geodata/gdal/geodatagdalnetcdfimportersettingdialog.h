#ifndef GEODATAGDALNETCDFIMPORTERSETTINGDIALOG_H
#define GEODATAGDALNETCDFIMPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataGdalNetcdfImporterSettingDialog;
}

class SolverDefinitionGridAttribute;

class QComboBox;

class GeoDataGdalNetcdfImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	class NcVariable
	{

	public:
		QString name;
		std::vector<QString> dimensions;
	};

	explicit GeoDataGdalNetcdfImporterSettingDialog(QWidget* parent = nullptr);
	~GeoDataGdalNetcdfImporterSettingDialog();

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

	Ui::GeoDataGdalNetcdfImporterSettingDialog* ui;
};

#endif // GEODATAGDALNETCDFIMPORTERSETTINGDIALOG_H
