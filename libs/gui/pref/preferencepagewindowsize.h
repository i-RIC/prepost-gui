#ifndef PREFERENCEPAGEWINDOWSIZE_H
#define PREFERENCEPAGEWINDOWSIZE_H

#include "preferencepage.h"

#include <misc/windowsizestandardsetting.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>

#include <vector>

namespace Ui {
class PreferencePageWindowSize;
}

class PreferencePageWindowSize : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageWindowSize(QWidget *parent = nullptr);
	~PreferencePageWindowSize();

	void update() override;

private slots:
	void add();
	void editSelected();
	void deleteSelected();
	void reset();

private:
	void updateList();

	std::vector<WindowSizeStandardSetting> m_standardSettings;
	QStandardItemModel m_model;
	QItemSelectionModel m_selectionModel;

	Ui::PreferencePageWindowSize *ui;
};

#endif // PREFERENCEPAGEWINDOWSIZE_H
