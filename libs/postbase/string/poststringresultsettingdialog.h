#ifndef POSTSTRINGRESULTSETTINGDIALOG_H
#define POSTSTRINGRESULTSETTINGDIALOG_H

#include "poststringresultdataitem.h"

#include <QDialog>
#include <QFont>

namespace Ui {
class PostStringResultSettingDialog;
}

class v4PostZoneDataContainer;

class PostStringResultSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostStringResultSettingDialog(QWidget *parent = nullptr);
	~PostStringResultSettingDialog();

	void setZoneDataContainer(v4PostZoneDataContainer* container);

	PostStringResult* stringResult() const;
	void setStringResult(PostStringResult* result);

	PostStringResultDataItem::Setting displaySetting() const;
	void setDisplaySetting(const PostStringResultDataItem::Setting& setting);

private slots:
	void editFont();

public slots:
	void accept() override;

private:
	void updateFontLabel();

	QFont m_font;
	Ui::PostStringResultSettingDialog *ui;
};

#endif // POSTSTRINGRESULTSETTINGDIALOG_H
