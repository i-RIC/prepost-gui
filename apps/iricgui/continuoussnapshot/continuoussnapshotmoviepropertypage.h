#ifndef CONTINUOUSSNAPSHOTMOVIEPROPERTYPAGE_H
#define CONTINUOUSSNAPSHOTMOVIEPROPERTYPAGE_H

#include <QWizardPage>

class ContinuousSnapshotWizard;

namespace Ui
{
	class ContinuousSnapshotMoviePropertyPage;
}

class ContinuousSnapshotMoviePropertyPage : public QWizardPage
{
	Q_OBJECT

public:
	ContinuousSnapshotMoviePropertyPage(QWidget* parent = nullptr);
	~ContinuousSnapshotMoviePropertyPage();
	void initializePage() override;
	bool validatePage() override;
	static QStringList getProfile(int profileid);

private:
	ContinuousSnapshotWizard* m_wizard;

	Ui::ContinuousSnapshotMoviePropertyPage* ui;
};

#endif // CONTINUOUSSNAPSHOTMOVIEPROPERTYPAGE_H
