#ifndef COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H
#define COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H

#include "colormapsettingeditwidget_importdialog.h"

class ColorMapSettingEditWidget::ImportDialog::Setting
{
public:
	static void get(ImportDialog* dialog);
	static void set(ImportDialog* dialog);

private:
	bool direction;
	bool titleGroup;
	bool title;
	bool titleFont;
	bool labelDisplayGroup;
	bool labelFormat;
	bool labelNumLabels;
	bool labelFont;
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

#endif // COLORMAPSETTINGEDITWIDGET_IMPORTDIALOG_SETTING_H
