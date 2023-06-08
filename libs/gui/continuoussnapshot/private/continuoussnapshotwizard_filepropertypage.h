#ifndef CONTINUOUSSNAPSHOTWIZARD_FILEPROPERTYPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_FILEPROPERTYPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

namespace Ui {
class ContinuousSnapshotWizard_FilePropertyPage;
}

class ContinuousSnapshotWizard::FilePropertyPage : public QWizardPage
{
	Q_OBJECT

public:
	FilePropertyPage(ContinuousSnapshotWizard *wizard);
	~FilePropertyPage();

	void initializePage() override;
	bool validatePage() override;

private:
	ContinuousSnapshotWizard* m_wizard;
	QStringList m_extensionList;

	Ui::ContinuousSnapshotWizard_FilePropertyPage *ui;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_FILEPROPERTYPAGE_H
