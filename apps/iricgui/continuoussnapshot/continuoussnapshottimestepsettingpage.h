#ifndef CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H

#include <QWizardPage>

class ContinuousSnapshotWizard;

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
	void modifyStart(int time);
	void modifyStop(int time);

private:
	ContinuousSnapshotWizard* m_wizard;
	int m_startTimestep;
	int m_stopTimestep;
	int m_samplingRate;

	Ui::ContinuousSnapshotTimestepSettingPage* ui;
};

#endif // CONTINUOUSSNAPSHOTTIMESTEPSETTINGPAGE_H
