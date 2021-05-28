#ifndef CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H

#include <QWizardPage>

class ContinuousSnapshotWizard;

class QLabel;

namespace Ui
{
	class ContinuousSnapshotTimestepSettingPage;
}

class ContinuousSnapshotTimestepSettingPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotTimestepSettingPage(QWidget* parent = nullptr);
	~ContinuousSnapshotTimestepSettingPage();

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

	Ui::ContinuousSnapshotTimestepSettingPage* ui;
};

#endif // CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H
