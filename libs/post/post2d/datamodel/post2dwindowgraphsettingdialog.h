#ifndef POST2DWINDOWGRAPHSETTINGDIALOG_H
#define POST2DWINDOWGRAPHSETTINGDIALOG_H

#include <QDialog>

#include <string>
#include <vector>

class Post2dWindowGraphSetting;

namespace Ui {
class Post2dWindowGraphSettingDialog;
}

class Post2dWindowGraphSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowGraphSettingDialog(QWidget *parent = 0);
	~Post2dWindowGraphSettingDialog();

	void setDimensions(int dimI, int dimJ);
	void setTargets(const std::vector<std::string>& targets);

	Post2dWindowGraphSetting setting() const;
	void setSetting(const Post2dWindowGraphSetting& setting);

private slots:
	void handleDirChange(bool checked);
	void handleStartChange(int val);
	void handleEndChange(int val);

	void editRegionCustomSetting();

private:
	void updateStartEndValueRange();
	void resetRegionCustomSetting();

	Ui::Post2dWindowGraphSettingDialog *ui;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/post2dwindowgraphsettingdialog_impl.h"
#endif // _DEBUG

#endif // POST2DWINDOWGRAPHSETTINGDIALOG_H
