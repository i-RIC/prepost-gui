#ifndef COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_H
#define COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_H

#include "../colormapenumeratesettingcontainer.h"
#include "../colormapenumeratesettingeditwidget.h"

#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDialog>

namespace Ui {
class ColorMapEnumerateSettingEditWidget_ImportDialog;
}

class ColorMapEnumerateSettingEditWidget::ImportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ImportDialog(QWidget *parent = nullptr);
	~ImportDialog();

	const ColorMapEnumerateSettingContainer& setting() const;
	void setOriginalSetting(const ColorMapEnumerateSettingContainer& setting);

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
	ColorMapEnumerateSettingContainer m_setting;
	ColorMapEnumerateSettingContainer m_originalSetting;

	Ui::ColorMapEnumerateSettingEditWidget_ImportDialog *ui;

	class Setting;
};

#endif // COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_H
