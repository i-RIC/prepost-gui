#include "startpagehelptabwidget.h"
#include "startpagelinks.h"

#include <QVBoxLayout>

StartPageHelpTabWidget::StartPageHelpTabWidget(QWidget* parent) :
	QWidget(parent)
{
	m_links = new StartPageLinks(parent);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_links);
	setLayout(layout);
}

void StartPageHelpTabWidget::setLocale(const QString& locale)
{
	m_links->setupLinks(locale);
}
