#include "preferencepage.h"

PreferencePage::PreferencePage(QWidget* parent, Qt::WindowFlags f) :
	QWidget {parent, f}
{}

bool PreferencePage::checkSetting()
{
	return true;
}
