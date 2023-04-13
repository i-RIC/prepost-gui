#ifndef COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H
#define COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H

#include "colormapenumeratesettingeditwidget_importdialog.h"

class ColorMapEnumerateSettingEditWidget::ImportDialog::Setting
{
public:
	static void get(ImportDialog* dialog);
	static void set(ImportDialog* dialog);

private:
	bool direction;
	bool titleGroup;
	bool title;
	bool titleFont;
	bool labelDisplayFont;
	bool colorGroup;
	bool colorTitle;
	bool colorLabel;
	bool colorBackground;
	bool sizeAndPosition;
	bool position;
	bool size;
	bool barWidth;
	bool alignment;

	static Setting instance;

	Setting();
};

#endif // COLORMAPENUMERATESETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H
