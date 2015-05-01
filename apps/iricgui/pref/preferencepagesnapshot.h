#ifndef PREFERENCEPAGESNAPSHOT_H
#define PREFERENCEPAGESNAPSHOT_H

#include "preferencepage.h"
#include <QSettings>
#include <QList>

namespace Ui
{
	class PreferencePageSnapshot;
}

class PreferencePageSnapshot : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageSnapshot(QWidget* parent = nullptr);
	~PreferencePageSnapshot();
	void update();
private:
	void setupTypeComboBox();
	QSettings m_settings;
	QList<QString> m_exts;
	Ui::PreferencePageSnapshot* ui;
};

#endif // PREFERENCEPAGESNAPSHOT_H
