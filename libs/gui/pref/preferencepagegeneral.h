#ifndef PREFERENCEPAGEGENERAL_H
#define PREFERENCEPAGEGENERAL_H

#include "preferencepage.h"

#include <QList>
#include <QLocale>
#include <QWidget>

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

	void update() override;
	bool checkSetting() override;

private:
	QString defaultWorkspace() const;
	void setupLanguageComboBox();
	QList<QLocale> m_locales;
	Ui::PreferencePageGeneral* ui;
};

#endif // PREFERENCEPAGEGENERAL_H
