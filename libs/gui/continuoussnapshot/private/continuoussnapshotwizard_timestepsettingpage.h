#ifndef CONTINUOUSSNAPSHOTWIZARD_TIMESTEPSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_TIMESTEPSETTINGPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

namespace Ui {
class ContinuousSnapshotWizard_TimestepSettingPage;
}

class ContinuousSnapshotWizard::TimestepSettingPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit TimestepSettingPage(ContinuousSnapshotWizard *wizard);
	~TimestepSettingPage();

	void initializePage() override;
	bool validatePage() override;

private slots:
	void handleStartChange(int time);
	void handleStopChange(int time);

private:
	void updateStartLabel();
	void updateStopLabel();

	void updateTimeLabel(double timeValue, QLabel* label);

	ContinuousSnapshotWizard* m_wizard;
	int m_startTimestep;
	int m_stopTimestep;
	int m_samplingRate;

	Ui::ContinuousSnapshotWizard_TimestepSettingPage *ui;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_TIMESTEPSETTINGPAGE_H
