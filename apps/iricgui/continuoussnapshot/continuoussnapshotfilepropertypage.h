#ifndef CONTINUOUSSNAPSHOTFILEPROPERTYPAGE_H
#define CONTINUOUSSNAPSHOTFILEPROPERTYPAGE_H

#include <QWizardPage>

class ContinuousSnapshotWizard;

namespace Ui
{
	class ContinuousSnapshotFilePropertyPage;
}

class ContinuousSnapshotFilePropertyPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotFilePropertyPage(QWidget* parent = nullptr);
	~ContinuousSnapshotFilePropertyPage();

	void initializePage() override;
	bool validatePage() override;

private:
	ContinuousSnapshotWizard* m_wizard;
	QStringList m_extensionList;

	Ui::ContinuousSnapshotFilePropertyPage* ui;
};

#endif // CONTINUOUSSNAPSHOTFILEPROPERTYPAGE_H
