#ifndef POSTTITLEEDITDIALOG_H
#define POSTTITLEEDITDIALOG_H

#include "../postbase_global.h"
#include "posttitlesetting.h"
#include <QDialog>

namespace Ui {
		class PostTitleEditDialog;
}

class POSTBASEDLL_EXPORT PostTitleEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostTitleEditDialog(QWidget *parent = 0);
	~PostTitleEditDialog();
	void setEnabled(bool enabled);
	bool isEnabled() const;

	void setSetting(const PostTitleSetting& setting);
	const PostTitleSetting setting();

private:
		Ui::PostTitleEditDialog *ui;
};

#endif // POSTTITLEEDITDIALOG_H
