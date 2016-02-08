#ifndef PREFERENCEPAGETMS_H
#define PREFERENCEPAGETMS_H

#include "preferencepage.h"

#include <guicore/tmsimage/tmsimagesetting.h>

#include <QWidget>

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
	void deleteSelected();
	void restoreDefault();

	void moveUpSelected();
	void moveDownSelected();

	void handleListWidgetItemChange(QListWidgetItem *item);

private:
	void updateList();

	std::vector<TmsImageSetting> m_settings;

	Ui::PreferencePageTms *ui;
};

#endif // PREFERENCEPAGETMS_H
