#ifndef PREFERENCEPAGETMS_H
#define PREFERENCEPAGETMS_H

#include "preferencepage.h"

#include <guicore/tmsimage/tmsimagesetting.h>

#include <QWidget>

#include <string>

namespace Ui {
class PreferencePageTms;
}

class QListWidgetItem;

class PreferencePageTms : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageTms(QWidget *parent = nullptr);
	~PreferencePageTms();

	void update() override;

private slots:
	void add();
	void edit();
	void deleteSelected();
	void restoreDefault();

	void moveUpSelected();
	void moveDownSelected();

	void handleListWidgetSelectChange(int current);
	void handleListWidgetItemChange(QListWidgetItem *item);

	void editGoogleMapsKey();

private:
	void updateList();

	std::vector<TmsImageSetting> m_settings;
	std::string m_googleMapsApiKey;

	Ui::PreferencePageTms *ui;
};

#endif // PREFERENCEPAGETMS_H
