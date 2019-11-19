#ifndef GEODATAPOLYGONGROUPSHPIMPORTERSETTINGDIALOG_H
#define GEODATAPOLYGONGROUPSHPIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QList>

class QStringList;
class QVariant;

namespace Ui
{
	class GeoDataPolygonGroupShpImporterSettingDialog;
}

class GridAttributeEditWidget;

class QStringList;

class GeoDataPolygonGroupShpImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum NameSetting {nsLoadFromDBF, nsAuto};
	enum ValueSetting {vsLoadFromDBF, vsSpecify};
	explicit GeoDataPolygonGroupShpImporterSettingDialog(const QString& filename, GridAttributeEditWidget* widget, QWidget* parent = 0);
	~GeoDataPolygonGroupShpImporterSettingDialog();
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
	Ui::GeoDataPolygonGroupShpImporterSettingDialog* ui;
};

#endif // GEODATAPOLYGONGROUPSHPIMPORTERSETTINGDIALOG_H
