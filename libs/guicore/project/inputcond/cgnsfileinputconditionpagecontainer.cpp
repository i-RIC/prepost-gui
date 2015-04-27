#include "cgnsfileinputconditionpagecontainer.h"
#include "cgnsfileinputconditionpage.h"
#include <QDomElement>
#include <QHBoxLayout>

CgnsFileInputConditionPageContainer::CgnsFileInputConditionPageContainer(QWidget* w)
{

	m_currentPage = new CgnsFileInputConditionPage(this);
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(m_currentPage);
	setLayout(layout);
}

CgnsFileInputConditionPageContainer::~CgnsFileInputConditionPageContainer()
{
	for (auto it = m_pages.begin(); it != m_pages.end(); ++it){
		delete *it;
	}
	m_pages.clear();
}

void CgnsFileInputConditionPageContainer::setup(const QDomElement& elem, CgnsFileInputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	QDomNodeList children = elem.childNodes();
	for (int i = 0; i < children.count(); ++i){
		QDomNode child = children.item(i);
		CgnsFileInputConditionPage* p = new CgnsFileInputConditionPage(child, ws, t, this);
		p->hide();
		m_pages.insert(p->name(), p);
	}
}

void CgnsFileInputConditionPageContainer::pageSelected(const QString& pagename)
{
	CgnsFileInputConditionPage* oldpage = m_currentPage;
	m_currentPage = m_pages.value(pagename);
	QLayout* l = layout();
	l->removeWidget(oldpage);
	l->addWidget(m_currentPage);
	oldpage->hide();
	m_currentPage->show();
}
