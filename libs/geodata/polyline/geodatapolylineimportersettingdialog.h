#ifndef GEODATAPOLYLINEIMPORTERSETTINGDIALOG_H
#define GEODATAPOLYLINEIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QList>

class QStringList;
class QVariant;

namespace Ui
{
	class GeoDataPolyLineImporterSettingDialog;
}

class GridAttributeEditWidget;

class GeoDataPolyLineImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum NameSetting {nsLoadFromDBF, nsAuto};
	enum ValueSetting {vsLoadFromDBF, vsSpecify};
	explicit GeoDataPolyLineImporterSettingDialog(const QString& filename, GridAttributeEditWidget* widget, QWidget* parent = 0);
	~GeoDataPolyLineImporterSettingDialog();
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
	Ui::GeoDataPolyLineImporterSettingDialog* ui;
};

#endif // GEODATAPOLYLINEIMPORTERSETTINGDIALOG_H
