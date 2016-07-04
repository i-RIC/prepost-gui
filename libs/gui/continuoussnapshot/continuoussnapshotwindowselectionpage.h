#ifndef CONTINUOUSSNAPSHOTWINDOWSELECTIONPAGE_H
#define CONTINUOUSSNAPSHOTWINDOWSELECTIONPAGE_H

#include <QWizardPage>

class iRICMainWindow;
class ContinuousSnapshotWizard;

namespace Ui
{
	class ContinuousSnapshotWindowSelectionPage;
}

class ContinuousSnapshotWindowSelectionPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotWindowSelectionPage(QWidget* parent = nullptr);
	~ContinuousSnapshotWindowSelectionPage();
	void setupWindowList();
	void initializePage() override;
	bool validatePage() override;
	void measurePixmapSize(QPoint& p, QSize& s);

private:
	iRICMainWindow* m_mainWindow;
	ContinuousSnapshotWizard* m_wizard;

	Ui::ContinuousSnapshotWindowSelectionPage* ui;
};

#endif // CONTINUOUSSNAPSHOTWINDOWSELECTIONPAGE_H
