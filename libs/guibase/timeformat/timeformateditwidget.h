#ifndef TIMEFORMATEDITWIDGET_H
#define TIMEFORMATEDITWIDGET_H

#include "../guibase_global.h"
#include "timeformat.h"

#include <QWidget>

namespace Ui {
class TimeFormatEditWidget;
}

class GUIBASEDLL_EXPORT TimeFormatEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TimeFormatEditWidget(QWidget *parent = nullptr);
	~TimeFormatEditWidget();

public slots:
	void setActualTimeAvailable(bool available);
	void setActualTimeNotAvailable(bool notAvailable);

public:
	TimeFormat timeFormat() const;
	void setTimeFormat(TimeFormat timeFormat);
	QString customTimeFormat() const;
	void setCustomTimeFormat(const QString& customTimeFormat);

private slots:
	void actual_yyyy_mm_dd_HH_MM_SS_toggled(bool toggled);
	void actual_yyyy_mm_dd_HH_MM_toggled(bool toggled);
	void actual_mm_dd_HH_MM_SS_toggled(bool toggled);
	void actual_mm_dd_HH_MM_toggled(bool toggled);
	void actual_HH_MM_SS_toggled(bool toggled);
	void actual_HH_MM_toggled(bool toggled);

private:
	Ui::TimeFormatEditWidget *ui;
};

#endif // TIMEFORMATEDITWIDGET_H
