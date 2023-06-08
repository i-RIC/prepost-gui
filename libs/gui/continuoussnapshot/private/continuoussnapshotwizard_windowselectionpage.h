#ifndef CONTINUOUSSNAPSHOTWIZARD_WINDOWSELECTIONPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_WINDOWSELECTIONPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWidget>

namespace Ui {
class ContinuousSnapshotWizard_WindowSelectionPage;
}

class ContinuousSnapshotWizard::WindowSelectionPage : public QWizardPage
{
	Q_OBJECT

public:
	WindowSelectionPage(ContinuousSnapshotWizard* wizard);
	~WindowSelectionPage();

	void setupWindowList();
	void measurePixmapSize(QPoint* p, QSize* s);

	void initializePage() override;
	bool validatePage() override;

private:
	iRICMainWindow* m_mainWindow;
	ContinuousSnapshotWizard* m_wizard;

private:
	Ui::ContinuousSnapshotWizard_WindowSelectionPage *ui;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_WINDOWSELECTIONPAGE_H
