#ifndef CONTINUOUSSNAPSHOTWIZARD_INTRODUCTIONPAGE_H
#define CONTINUOUSSNAPSHOTWIZARD_INTRODUCTIONPAGE_H

#include "../continuoussnapshotwizard.h"

#include <QWizardPage>

class QLabel;

class ContinuousSnapshotWizard::IntroductionPage : public QWizardPage
{
public:
	IntroductionPage(ContinuousSnapshotWizard* wizard);

private:
	QLabel* m_label;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_INTRODUCTIONPAGE_H
