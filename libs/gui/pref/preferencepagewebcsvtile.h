#ifndef PREFERENCEPAGEWEBCSVTILE_H
#define PREFERENCEPAGEWEBCSVTILE_H

#include "preferencepage.h"

#include <geodata/pointmap/geodatapointmapwebimportersetting.h>

#include <QWidget>

#include <vector>

namespace Ui {
class PreferencePageWebCsvTile;
}

class PreferencePageWebCsvTile : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageWebCsvTile(QWidget *parent = nullptr);
	~PreferencePageWebCsvTile();

	void update() override;

private slots:
	void add();
	void deleteSelected();
	void restoreDefault();

	void moveUpSelected();
	void moveDownSelected();

private:
	void updateList();

	std::vector<GeoDataPointmapWebImporterSetting> m_settings;

	Ui::PreferencePageWebCsvTile *ui;
};

#endif // PREFERENCEPAGEWEBCSVTILE_H
