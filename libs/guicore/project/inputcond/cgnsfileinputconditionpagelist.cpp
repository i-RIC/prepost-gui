#include "cgnsfileinputconditionpagelist.h"
#include "../../solverdef/solverdefinitiontranslator.h"

#include <QDomNode>
#include <QTreeWidgetItem>

CgnsFileInputConditionPageList::CgnsFileInputConditionPageList(QWidget* w)
	: QTreeWidget(w)
{
	// setup itself.
	setSelectionMode(SingleSelection);
	setColumnCount(1);
	setHeaderLabel(tr("Groups"));
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelectionChange()));
	// set size limitation
	setMinimumWidth(WIDTH_MIN);
	setMaximumWidth(WIDTH_MAX);
}

void CgnsFileInputConditionPageList::selectFirstItem()
{
	QTreeWidgetItem* first = topLevelItem(0);
	if (first != 0){
		first->setSelected(true);
	}
}

void CgnsFileInputConditionPageList::setup(const QDomElement& elem, const SolverDefinitionTranslator& t)
{
	QDomNode pageNode;
	QTreeWidgetItem* item;

	QDomNodeList children = elem.childNodes();
	for (unsigned int i = 0; i < children.length(); ++i){
		QDomNode c = children.item(i);
		if (c.nodeName() == "Tab" || c.nodeName() == "Page"){
			// add item
			QDomElement e = c.toElement();
			item = new QTreeWidgetItem(this);
			QString caption = t.translate(e.attribute("caption"));
			item->setText(0, caption);
			addTopLevelItem(item);
			m_nameMap[item] = e.attribute("name");
		}
	}
}

void CgnsFileInputConditionPageList::handleSelectionChange()
{
	QList<QTreeWidgetItem*> selectedList = selectedItems();
	QTreeWidgetItem* selected = *(selectedList.begin());
	// get the name of page selected.
	QString pageName = m_nameMap[selected];
	emit selectChanged(pageName);
}
