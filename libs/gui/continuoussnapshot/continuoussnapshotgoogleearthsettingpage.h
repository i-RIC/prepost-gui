#ifndef CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H

#include <QWizardPage>

class iRICMainWindow;
class ContinuousSnapshotWizard;

namespace Ui
{
	class ContinuousSnapshotGoogleEarthSettingPage;
}

class ContinuousSnapshotGoogleEarthSettingPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotGoogleEarthSettingPage(QWidget* parent = nullptr);
	~ContinuousSnapshotGoogleEarthSettingPage();

	void initializePage() override;
	bool validatePage() override;

	void calculateKMLInformation();
	QSize targetSnapshotSize();

private:
	QPointF snapshotToWorld(const QPointF& p);
	QPointF worldToLatLong(const QPointF& p);

	QPointF snapshotToLatLong(const QPointF& p);

	iRICMainWindow* m_mainWindow;
	ContinuousSnapshotWizard* m_wizard;

	int m_imageWidth;

	Ui::ContinuousSnapshotGoogleEarthSettingPage* ui;
};

#endif // CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H
