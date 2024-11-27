#ifndef GEODATAGDALIMPORTERSETTINGDIALOG_H
#define GEODATAGDALIMPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataGdalImporterSettingDialog;
}

class SolverDefinitionGridAttribute;

class QComboBox;

class GeoDataGdalImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	class NcVariable
	{

	public:
		QString name;
		std::vector<QString> dimensions;
	};

	explicit GeoDataGdalImporterSettingDialog(QWidget* parent = 0);
	~GeoDataGdalImporterSettingDialog();

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

	Ui::GeoDataGdalImporterSettingDialog* ui;
};

#endif // GEODATAGDALIMPORTERSETTINGDIALOG_H
