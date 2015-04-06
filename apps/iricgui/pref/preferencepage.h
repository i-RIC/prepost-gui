#ifndef PREFERENCEPAGE_H
#define PREFERENCEPAGE_H

#include <QWidget>

class PreferencePage : public QWidget
{
public:
	PreferencePage(QWidget* parent = 0, Qt::WindowFlags f = 0)
		: QWidget(parent, f)
	{}
	virtual void update() = 0;
	virtual bool checkSetting(){return true;}
};

#endif // PREFERENCEPAGE_H
