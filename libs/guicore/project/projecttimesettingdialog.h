#ifndef PROJECTTIMESETTINGDIALOG_H
#define PROJECTTIMESETTINGDIALOG_H

#include "../guicore_global.h"

#include <guibase/timeformat/timeformat.h>

#include <QDialog>
#include <QTimeZone>

namespace Ui {
class ProjectTimeSettingDialog;
}

class QTimeZone;

class GUICOREDLL_EXPORT ProjectTimeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProjectTimeSettingDialog(QWidget *parent = nullptr);
	~ProjectTimeSettingDialog();

	QDateTime zeroDateTime() const;
	void setZeroDateTime(const QDateTime& zeroDateTime);

	QTimeZone timeZone() const;
	void setTimeZone(const QTimeZone& timeZone);

	TimeFormat timeFormat() const;
	void setTimeFormat(TimeFormat timeFormat);

	QString customTimeFormat() const;
	void setCustomTimeFormat(const QString& format);

	bool showTimeZone() const;
	void setShowTimeZone(bool show);

private:
	Ui::ProjectTimeSettingDialog *ui;
};

#endif // PROJECTTIMESETTINGDIALOG_H
