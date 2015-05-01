#ifndef PREFERENCEPAGEGENERAL_H
#define PREFERENCEPAGEGENERAL_H

#include <QWidget>
#include <QList>
#include <QLocale>
#include "preferencepage.h"

namespace Ui
{
	class PreferencePageGeneral;
}

class PreferencePageGeneral : public PreferencePage
{
	Q_OBJECT
public:
	explicit PreferencePageGeneral(QWidget* parent = nullptr);
	~PreferencePageGeneral();
	bool checkSetting();
	void update();

private:
	QString defaultWorkspace() const;
	void setupLanguageComboBox();
	QList<QLocale> m_locales;
	Ui::PreferencePageGeneral* ui;
};

#endif // PREFERENCEPAGEGENERAL_H
