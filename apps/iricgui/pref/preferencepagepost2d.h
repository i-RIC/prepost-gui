#ifndef PREFERENCEPAGEPOST2D_H
#define PREFERENCEPAGEPOST2D_H

#include "preferencepage.h"
#include <QSettings>

namespace Ui
{
	class PreferencePagePost2d;
}

class PreferencePagePost2d : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePagePost2d(QWidget* parent = nullptr);
	~PreferencePagePost2d();
	void update();

private:
	QSettings m_settings;
	Ui::PreferencePagePost2d* ui;
};

#endif // PREFERENCEPAGEPOST2D_H
