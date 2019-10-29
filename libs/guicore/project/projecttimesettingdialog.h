#ifndef PROJECTTIMESETTINGDIALOG_H
#define PROJECTTIMESETTINGDIALOG_H

#include "../guicore_global.h"

#include <guibase/timeformat/timeformat.h>

#include <QDialog>

namespace Ui {
class ProjectTimeSettingDialog;
}

class GUICOREDLL_EXPORT ProjectTimeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProjectTimeSettingDialog(QWidget *parent = nullptr);
	~ProjectTimeSettingDialog();

	QDateTime zeroDateTime() const;
	void setZeroDateTime(const QDateTime& zeroDateTime);

	TimeFormat timeFormat() const;
	void setTimeFormat(TimeFormat timeFormat);

	QString customTimeFormat() const;
	void setCustomTimeFormat(const QString& format);

private:
	Ui::ProjectTimeSettingDialog *ui;
};

#endif // PROJECTTIMESETTINGDIALOG_H
