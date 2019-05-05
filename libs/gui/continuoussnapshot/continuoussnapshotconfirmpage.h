#ifndef CONTINUOUSSNAPSHOTCONFIRMPAGE_H
#define CONTINUOUSSNAPSHOTCONFIRMPAGE_H

#include <QWizardPage>

class ContinuousSnapshotWizard;

class QListWidget;

class ContinuousSnapshotConfirmPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotConfirmPage(QWidget* parent = nullptr);
	void initializePage() override;

private:
	ContinuousSnapshotWizard* m_wizard;
	QListWidget* m_fileList;
};

#endif // CONTINUOUSSNAPSHOTCONFIRMPAGE_H
