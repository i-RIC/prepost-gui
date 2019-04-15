#ifndef CONTINUOUSSNAPSHOTINTRODUCTIONPAGE_H
#define CONTINUOUSSNAPSHOTINTRODUCTIONPAGE_H

#include <QWizardPage>

class QLabel;

class ContinuousSnapshotIntroductionPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotIntroductionPage(QWidget* parent = nullptr);

private:
	QLabel* m_label;
};

#endif // CONTINUOUSSNAPSHOTINTRODUCTIONPAGE_H
