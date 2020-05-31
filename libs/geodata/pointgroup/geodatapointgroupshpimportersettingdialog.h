#ifndef GEODATAPOINTGROUPSHPIMPORTERSETTINGDIALOG_H
#define GEODATAPOINTGROUPSHPIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QList>

class QStringList;
class QVariant;

namespace Ui
{
	class GeoDataPointGroupShpImporterSettingDialog;
}

class GridAttributeEditWidget;

class QStringList;

class GeoDataPointGroupShpImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum NameSetting {nsLoadFromDBF, nsAuto};
	enum ValueSetting {vsLoadFromDBF, vsSpecify};
	explicit GeoDataPointGroupShpImporterSettingDialog(const QString& filename, GridAttributeEditWidget* widget, QWidget* parent = nullptr);
	~GeoDataPointGroupShpImporterSettingDialog();
	NameSetting nameSetting() const;
	int nameIndex() const;
	ValueSetting valueSetting() const;
	int valueIndex() const;
	QVariant specifiedValue() const;

public slots:
	void accept() override;

private slots:
	void updateTable();

private:
	void setLabels(const QStringList& labels);
	void setData(const QList<QList<QVariant> >& data);

	QString m_filename;
	Ui::GeoDataPointGroupShpImporterSettingDialog* ui;
};

#endif // GEODATAPOINTGROUPSHPIMPORTERSETTINGDIALOG_H
