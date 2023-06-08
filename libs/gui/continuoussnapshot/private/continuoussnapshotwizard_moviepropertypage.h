#ifndef CONTINUOUSSNAPSHOTWIZARD_MOVIEPROPERTYPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_MOVIEPROPERTYPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

namespace Ui {
class ContinuousSnapshotWizard_MoviePropertyPage;
}

class ContinuousSnapshotWizard::MoviePropertyPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit MoviePropertyPage(ContinuousSnapshotWizard *wizard);
	~MoviePropertyPage();

	void initializePage() override;
	bool validatePage() override;

	static QStringList getProfile(ContinuousSnapshotSetting::MovieProfile profile);

private:
	ContinuousSnapshotWizard* m_wizard;

	Ui::ContinuousSnapshotWizard_MoviePropertyPage *ui;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_MOVIEPROPERTYPAGE_H
