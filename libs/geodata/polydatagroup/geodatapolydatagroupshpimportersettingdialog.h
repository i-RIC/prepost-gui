#ifndef GEODATAPOLYDATAGROUPSHPIMPORTERSETTINGDIALOG_H
#define GEODATAPOLYDATAGROUPSHPIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui
{
	class GeoDataPolyDataGroupShpImporterSettingDialog;
}

class CoordinateSystem;
class CoordinateSystemBuilder;
class GridAttributeEditWidget;

class QStringList;

class GeoDataPolyDataGroupShpImporterSettingDialog: public QDialog
{
	Q_OBJECT

public:
	enum NameSetting {nsLoadFromDBF, nsAuto};
	enum ValueSetting {vsLoadFromDBF, vsSpecify};

	GeoDataPolyDataGroupShpImporterSettingDialog(const QString& filename, GridAttributeEditWidget* widget, QWidget* parent = nullptr);
	~GeoDataPolyDataGroupShpImporterSettingDialog();

	void setCsEnabled(bool enabled);
	void setCsBuilder(CoordinateSystemBuilder* builder);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* cs);

	void hideValueWidgets();

	NameSetting nameSetting() const;
	int nameIndex() const;

	ValueSetting valueSetting() const;
	int valueIndex() const;
	QVariant specifiedValue() const;

	QString codecName() const;

public slots:
	void accept() override;

private slots:
	void updateTable();

private:
	void setLabels(const QStringList& labels);
	void setData(const std::vector<std::vector<QVariant> >& data);

	QString m_filename;
	Ui::GeoDataPolyDataGroupShpImporterSettingDialog* ui;
};

#endif // GEODATAPOLYDATAGROUPSHPIMPORTERSETTINGDIALOG_H
