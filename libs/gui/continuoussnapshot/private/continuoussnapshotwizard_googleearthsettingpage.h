#ifndef CONTINUOUSSNAPSHOTWIZARD_GOOGLEEARTHSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_GOOGLEEARTHSETTINGPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

namespace Ui {
class ContinuousSnapshotWizard_GoogleEarthSettingPage;
}

class ContinuousSnapshotWizard::GoogleEarthSettingPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit GoogleEarthSettingPage(ContinuousSnapshotWizard *wizard);
	~GoogleEarthSettingPage();

	void calculateKMLInformation();
	QSize targetSnapshotSize();

	void initializePage() override;
	bool validatePage() override;

private:
	QPointF snapshotToWorld(const QPointF& p);
	QPointF worldToLatLong(const QPointF& p);
	QPointF snapshotToLatLong(const QPointF& p);

	std::vector<int> m_post2dWindowIndices;
	int m_imageWidth;

	iRICMainWindow* m_mainWindow;
	ContinuousSnapshotWizard* m_wizard;

	Ui::ContinuousSnapshotWizard_GoogleEarthSettingPage *ui;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_GOOGLEEARTHSETTINGPAGE_H
