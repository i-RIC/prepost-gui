#include "continuoussnapshotintroductionpage.h"

#include <QLabel>
#include <QVBoxLayout>

ContinuousSnapshotIntroductionPage::ContinuousSnapshotIntroductionPage(QWidget* parent) :
	QWizardPage(parent)
{
	setTitle(tr("Introduction"));
	m_label = new QLabel(tr(
			"This wizard will generate continuous snapshots."
			"You need to specify some informations for that."
				));
	m_label->setWordWrap(true);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_label);
	setLayout(layout);
}
