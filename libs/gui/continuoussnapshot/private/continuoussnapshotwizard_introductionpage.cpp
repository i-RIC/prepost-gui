#include "continuoussnapshotwizard_introductionpage.h"

#include <QLabel>
#include <QVBoxLayout>

ContinuousSnapshotWizard::IntroductionPage::IntroductionPage(ContinuousSnapshotWizard* wizard) :
	QWizardPage(wizard)
{
	setTitle(tr("Introduction"));
	m_label = new QLabel(ContinuousSnapshotWizard::tr(
			"Use this wizard to specify the requirements to generate continuous snapshot output."));
	m_label->setWordWrap(true);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_label);
	setLayout(layout);
}
