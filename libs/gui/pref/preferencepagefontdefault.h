#ifndef PREFERENCEPAGEFONTDEFAULT_H
#define PREFERENCEPAGEFONTDEFAULT_H

#include "preferencepage.h"

#include <post/post2d/datamodel/post2dwindownodevectorarrowstructuredsetting.h>

#include <QSettings>

namespace Ui
{
	class PreferencePageFontDefault;
}

class PreferencePageFontDefault : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageFontDefault(QWidget* parent = nullptr);
	~PreferencePageFontDefault();

	void update() override;

private:
	QSettings m_settings;
	Ui::PreferencePageFontDefault* ui;
};

#endif // PREFERENCEPAGEFONTDEFAULT_H
