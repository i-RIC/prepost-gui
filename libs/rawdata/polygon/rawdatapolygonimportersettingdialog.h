#ifndef RAWDATAPOLYGONIMPORTERSETTINGDIALOG_H
#define RAWDATAPOLYGONIMPORTERSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QStringList>

namespace Ui
{
	class RawDataPolygonImporterSettingDialog;
}

class GridRelatedConditionEditWidget;

class RawDataPolygonImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum NameSetting {nsLoadFromDBF, nsAuto};
	enum ValueSetting {vsLoadFromDBF, vsSpecify};
	explicit RawDataPolygonImporterSettingDialog(const QString& filename, GridRelatedConditionEditWidget* widget, QWidget* parent = 0);
	~RawDataPolygonImporterSettingDialog();
	NameSetting nameSetting() const;
	int nameIndex() const;
	ValueSetting valueSetting() const;
	int valueIndex() const;
	QVariant specifiedValue() const;

public slots:
	void accept();
private slots:
	void updateTable();
private:
	void setLabels(const QStringList& labels);
	void setData(const QList<QList<QVariant> >& data);

	QString m_filename;
	Ui::RawDataPolygonImporterSettingDialog* ui;
};

#endif // RAWDATAPOLYGONIMPORTERSETTINGDIALOG_H
