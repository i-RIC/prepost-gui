#ifndef CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H
#define CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H

#include <QWizardPage>

class iRICMainWindow;
class ContinuousSnapshotWizard;

namespace Ui {
	class ContinuousSnapshotGoogleEarthSettingPage;
}

class ContinuousSnapshotGoogleEarthSettingPage : public QWizardPage {
	Q_OBJECT
public:
	ContinuousSnapshotGoogleEarthSettingPage(QWidget *parent = nullptr);
	~ContinuousSnapshotGoogleEarthSettingPage();

	void initializePage();
	bool validatePage();
	void setupWindowComboBox();
	void setupBackgroundComboBox();
	void calculateKMLInformation();
	QSize targetSnapshotSize();

private:
	void snapshotToWorld(QPointF& p);
	void worldToImage(QPointF& p);
	void imageToLatLong(QPointF& p);
	void snapshotToLatLong(QPointF& p);

	iRICMainWindow* m_mainWindow;
	ContinuousSnapshotWizard* m_wizard;

	int m_imageWidth;
	double m_rate;

	Ui::ContinuousSnapshotGoogleEarthSettingPage *ui;
};

#endif // CONTINUOUSSNAPSHOTGOOGLEEARTHSETTINGPAGE_H
