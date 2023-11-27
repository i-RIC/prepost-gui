#include "inputconditionpage.h"
#include "inputconditionpagecontainer.h"
#include "inputconditionwidgetset.h"

#include <QDomElement>
#include <QHBoxLayout>

InputConditionPageContainer::InputConditionPageContainer(QWidget* /*w*/)
{
	m_currentPage = new InputConditionPage(this);
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(m_currentPage);
	setLayout(layout);
}

InputConditionPageContainer::~InputConditionPageContainer()
{
	for (auto p : m_pages) {
		delete p;
	}
}

void InputConditionPageContainer::setup(const QDomElement& elem, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	ws->clearLabelAndImageCount();

	QDomNodeList children = elem.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomNode child = children.item(i);
		InputConditionPage* p = new InputConditionPage(child, ws, t, this);
		p->hide();
		m_pages.insert(p->name(), p);
	}
}

void InputConditionPageContainer::pageSelected(const QString& pagename)
{
	InputConditionPage* oldpage = m_currentPage;
	m_currentPage = m_pages.value(pagename);
	QLayout* l = layout();
	l->removeWidget(oldpage);
	l->addWidget(m_currentPage);
	oldpage->hide();
	m_currentPage->show();
}
