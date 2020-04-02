#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditionpagelist.h"

#include <QDomNode>
#include <QTreeWidgetItem>

InputConditionPageList::InputConditionPageList(QWidget* w)
	: QTreeWidget(w)
{
	// setup itself.
	setSelectionMode(SingleSelection);
	setColumnCount(1);
	setHeaderLabel(tr("Groups"));
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelectionChange()));
	// set size limitation
	setMinimumWidth(WIDTH_MIN);
}

void InputConditionPageList::selectFirstItem()
{
	QTreeWidgetItem* first = topLevelItem(0);
	if (first != nullptr) {
		first->setSelected(true);
	}
}

void InputConditionPageList::setup(const QDomElement& elem, const SolverDefinitionTranslator& t)
{
	QDomNode pageNode;
	QTreeWidgetItem* item;

	QDomNodeList children = elem.childNodes();
	for (int i = 0; i < children.length(); ++i) {
		QDomNode c = children.item(i);
		if (c.nodeName() == "Tab" || c.nodeName() == "Page") {
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

void InputConditionPageList::handleSelectionChange()
{
	QList<QTreeWidgetItem*> selectedList = selectedItems();
	QTreeWidgetItem* selected = *(selectedList.begin());
	// get the name of page selected.
	QString pageName = m_nameMap[selected];
	emit selectChanged(pageName);
}
