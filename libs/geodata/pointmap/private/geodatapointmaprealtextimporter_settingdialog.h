#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_SETTINGDIALOG_H
#define GEODATAPOINTMAPREALTEXTIMPORTER_SETTINGDIALOG_H

#include "../geodatapointmaprealtextimporter.h"

#include <QByteArray>
#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPointmapRealTextImporter_SettingDialog;
}

class GeoDataPointmapRealTextImporter::SettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingDialog(QWidget *parent = nullptr);
	~SettingDialog();

	void setFileName(const QString& fileName);
	void setIsCsv(bool isCsv);
	void setPreviewData(const std::vector<QByteArray>& data);

	void updateComboBoxes();
	LineParser buildParser(bool* ok, QString* error) const;

private slots:
	void csvToggled(bool toggled);
	void headerLinesChange(int lines);

	void updatePreview();

private:
	void autoDetectHeaderLines();
	void autoDetectDelimiters();
	void autoDetectColumns();
	QStringList getFields(LineParser* parser) const;
	std::vector<QByteArray> getDataLinesForPreview() const;

	std::vector<QByteArray> m_previewData;

	Ui::GeoDataPointmapRealTextImporter_SettingDialog *ui;
};

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_SETTINGDIALOG_H
