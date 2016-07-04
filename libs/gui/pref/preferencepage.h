#ifndef PREFERENCEPAGE_H
#define PREFERENCEPAGE_H

#include <QWidget>

class PreferencePage : public QWidget
{

public:
	PreferencePage(QWidget* parent = nullptr, Qt::WindowFlags f = 0);

	virtual void update() = 0;
	virtual bool checkSetting();
};

#endif // PREFERENCEPAGE_H
