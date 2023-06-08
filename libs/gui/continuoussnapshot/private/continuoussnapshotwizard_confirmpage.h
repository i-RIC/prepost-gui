#ifndef CONTINUOUSSNAPSHOTWIZARD_CONFIRMPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_CONFIRMPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

namespace Ui {
class ContinuousSnapshotWizard_ConfirmPage;
}

class ContinuousSnapshotWizard::ConfirmPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit ConfirmPage(ContinuousSnapshotWizard *wizard);

	void initializePage() override;

private:
	ContinuousSnapshotWizard* m_wizard;
	QListWidget* m_fileList;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_CONFIRMPAGE_H
