#ifndef COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_H
#define COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_H

#include "../colormapsettingcontainer.h"
#include "../colormapsettingeditwidget.h"

#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDialog>

namespace Ui {
class ColorMapSettingEditWidget_ImportDialog;
}

class ColorMapSettingEditWidget::ImportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ImportDialog(QWidget *parent = nullptr);
	~ImportDialog();

	const ColorMapSettingContainer& setting() const;
	void setOriginalSetting(const ColorMapSettingContainer& setting);

	void accept() override;

private slots:
	void handleModeChange();
	void handleFileChange(const QString& fileName);

private:
	void updatePreview();
	void clearPreview();

	bool importData();
	void loadStandards();

	QStandardItemModel m_standardsModel;
	QItemSelectionModel m_selectionModel;

	QString m_selectedFileName;
	ColorMapSettingContainer m_setting;
	ColorMapSettingContainer m_originalSetting;

	Ui::ColorMapSettingEditWidget_ImportDialog *ui;

	class Setting;
};

#endif // COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_H
