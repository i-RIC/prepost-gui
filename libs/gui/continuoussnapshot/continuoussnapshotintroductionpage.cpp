#include "continuoussnapshotintroductionpage.h"

#include <QLabel>
#include <QVBoxLayout>

ContinuousSnapshotIntroductionPage::ContinuousSnapshotIntroductionPage(QWidget* parent) :
	QWizardPage(parent)
{
	setTitle(tr("Introduction"));
	m_label = new QLabel(tr(
			"Use this wizard to specify the requirements to generate continuous snapshot output."
				));
	m_label->setWordWrap(true);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_label);
	setLayout(layout);
}
