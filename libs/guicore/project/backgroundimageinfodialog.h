#ifndef BACKGROUNDIMAGEINFODIALOG_H
#define BACKGROUNDIMAGEINFODIALOG_H

#include "backgroundimageinfo.h"
#include "private/backgroundimageinfo_setting.h"

#include <QDialog>

namespace Ui
{
	class BackgroundImageInfoDialog;
}

class QAbstractButton;
class BackgroundImageInfo;

class BackgroundImageInfoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit BackgroundImageInfoDialog(BackgroundImageInfo*);
	~BackgroundImageInfoDialog();

	void setImageWidth(int width);

	BackgroundImageInfo::Setting setting() const;
	void setSetting(const BackgroundImageInfo::Setting& setting);

	void reject() override;
	void reset();
	void apply();

private slots:
	void update();
	void handleButtonBoxClick(QAbstractButton* button);

private:
	void applySettingToWidgets(const BackgroundImageInfo::Setting& setting);
	void updateScaleAndAngle();
	void updateRightBottom();

	BackgroundImageInfo::Setting m_originalSetting;
	int m_imageWidth;

	BackgroundImageInfo* m_info;
	Ui::BackgroundImageInfoDialog* ui;
};

#endif // BACKGROUNDIMAGEINFODIALOG_H
