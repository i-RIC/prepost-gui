#ifndef PREFERENCEPAGEBACKGROUNDGRID_H
#define PREFERENCEPAGEBACKGROUNDGRID_H

#include "preferencepage.h"

#include <QSettings>

namespace Ui {
class PreferencePageBackgroundGrid;
}

class PreferencePageBackgroundGrid : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageBackgroundGrid(QWidget *parent = nullptr);
	~PreferencePageBackgroundGrid();

	void update() override;

private:
	QSettings m_settings;
	Ui::PreferencePageBackgroundGrid *ui;
};

#endif // PREFERENCEPAGEBACKGROUNDGRID_H
