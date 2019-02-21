#ifndef PREFERENCEPAGERIVERSURVEY_H
#define PREFERENCEPAGERIVERSURVEY_H

#include "preferencepage.h"

#include <QSettings>

namespace Ui {
	class PreferencePageRiverSurvey;
}

class PreferencePageRiverSurvey : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageRiverSurvey(QWidget *parent = nullptr);
	~PreferencePageRiverSurvey();

	void update() override;

private slots:
	void add();
	void deleteSelected();
	void restoreDefault();

private:
	void updateList();

	QSettings m_settings;
	std::vector<double> m_fractions;
	Ui::PreferencePageRiverSurvey *ui;
};

#endif // PREFERENCEPAGERIVERSURVEY_H
