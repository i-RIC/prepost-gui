#ifndef PREFERENCEPAGEPRE_H
#define PREFERENCEPAGEPRE_H

#include "preferencepage.h"
#include <QSettings>

namespace Ui
{
	class PreferencePagePre;
}

class PreferencePagePre : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePagePre(QWidget* parent = nullptr);
	~PreferencePagePre();
	void update() override;

private:
	QSettings m_settings;
	Ui::PreferencePagePre* ui;
};

#endif // PREFERENCEPAGEPRE_H
