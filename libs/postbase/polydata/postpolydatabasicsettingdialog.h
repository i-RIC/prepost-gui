#ifndef POSTPOLYDATABASICSETTINGDIALOG_H
#define POSTPOLYDATABASICSETTINGDIALOG_H

#include "../postbase_global.h"

class PostPolyDataBasicSetting;

#include <QDialog>

class QColor;

namespace Ui
{
	class PostPolyDataBasicSettingDialog;
}

class POSTBASEDLL_EXPORT PostPolyDataBasicSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostPolyDataBasicSettingDialog(QWidget* parent = nullptr);
	~PostPolyDataBasicSettingDialog();

	PostPolyDataBasicSetting setting() const;
	void setSetting(const PostPolyDataBasicSetting& s);

private:
	Ui::PostPolyDataBasicSettingDialog* ui;
};

#endif // POSTPOLYDATABASICSETTINGDIALOG_H
