#ifndef PREFERENCEPAGEGRIDCHECK_H
#define PREFERENCEPAGEGRIDCHECK_H

#include "preferencepage.h"

#include <QSettings>

namespace Ui
{
	class PreferencePageGridCheck;
}

class PreferencePageGridCheck : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageGridCheck(QWidget* parent = nullptr);
	~PreferencePageGridCheck();

	void update() override;

private:
	QSettings m_settings;
	Ui::PreferencePageGridCheck* ui;
};

#endif // PREFERENCEPAGEGRIDCHECK_H
