#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditionpage.h"
#include "inputconditionwidget.h"
#include "inputconditionwidgetset.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNodeList>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

InputConditionPage::InputConditionPage(QWidget* parent) :
	QWidget {parent}
{}

InputConditionPage::InputConditionPage(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t, QWidget* parent) :
	QWidget {parent}
{
	// load and construct itself.
	load(node, ws, t);
}

const QString& InputConditionPage::name() const
{
	return m_name;
}

void InputConditionPage::load(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	QDomElement elem = node.toElement();
	m_name = elem.attribute("name");
	QVBoxLayout* masterLayout = new QVBoxLayout(this);
	QLayout* layout = nullptr;
	// obtain "Content" node.
	QDomNode contNode = iRIC::getChildNode(node, "Content");
	if (! contNode.isNull()) {
		// legacy mode.
		if (contNode.toElement().attribute("layout", "") == "custom") {
			layout = loadCustom(contNode, ws, t);
		} else {
			QDomNode itemsNode = iRIC::getChildNode(contNode, "Items");
			layout = loadSimple(itemsNode, ws, t);
		}
	} else {
		// new mode.
		layout = loadAuto(node, ws, t);
	}
	masterLayout->addLayout(layout);
	masterLayout->addStretch(1);
	setLayout(masterLayout);
}

QLayout* InputConditionPage::loadAuto(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	bool simple = true;
	QDomNodeList items = node.childNodes();
	for (int i = 0; i < items.length(); ++i) {
		QDomNode child = items.item(i);
		simple = simple && (child.nodeName() == "Item" || child.nodeName() == "GroupBox");
	}
	if (simple) {
		return loadSimple(node, ws, t);
	} else {
		return loadCustom(node, ws, t);
	}
}

QLayout* InputConditionPage::loadSimple(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	QDomNodeList items = node.childNodes();
	QFormLayout* layout = new QFormLayout();
	QDomElement elem;
	for (int i = 0; i < items.length(); ++i) {
		elem = items.item(i).toElement();
		if (elem.nodeName() == "Item") {
			std::string lname = iRIC::toStr(InputConditionWidgetSet::labelName(elem));
			std::string itemname = iRIC::toStr(elem.attribute("name"));
			layout->addRow(ws->widget(lname), ws->widget(itemname));
		} else if (elem.nodeName() == "GroupBox") {
			QString caption = t.translate(elem.attribute("caption"));
			QGroupBox* g = new QGroupBox(caption, this);
			QLayout* layout2 = loadAuto(elem, ws, t);
			g->setLayout(layout2);
			layout->addRow(g);
		}
	}
	return layout;
}

QLayout* InputConditionPage::loadCustom(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	QDomElement elem = node.toElement();
	QVBoxLayout* layout = new QVBoxLayout();
	QDomNodeList children = node.childNodes();
	loadBL(layout, children, ws, t);
	if (elem.attribute("withSpacer") == "true") {
		layout->addStretch(1);
	}
	return layout;
}

QObject* InputConditionPage::loadRec(const QDomNode& node, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	QString nodeName = node.nodeName();
	QDomElement elem = node.toElement();
	if (nodeName == "Item") {
		std::string itemName = iRIC::toStr(elem.attribute("name"));
		return ws->widget(itemName);
	} else if (nodeName == "Label") {
		std::string lName = iRIC::toStr(InputConditionWidgetSet::labelName(node));
		return ws->widget(lName);
	} else if (nodeName == "Image"){
		std::string itemName = iRIC::toStr(elem.attribute("src"));
		return ws->widget(itemName);
	} else if (nodeName == "GroupBox") {
		QString caption = t.translate(elem.attribute("caption"));
		QGroupBox* g = new QGroupBox(caption, this);
		QLayout* layout = loadAuto(node, ws, t);
		g->setLayout(layout);
		return g;
	} else if (nodeName == "GridLayout") {
		QGridLayout* l = new QGridLayout();
		loadGL(l, node.childNodes(), ws, t);
		return l;
	} else if (nodeName == "HBoxLayout") {
		QHBoxLayout* l = new QHBoxLayout();
		loadBL(l, node.childNodes(), ws, t);
		if (elem.attribute("withSpacer") == "true") {
			l->addStretch(1);
		}
		return l;
	} else if (nodeName == "VBoxLayout") {
		QVBoxLayout* l = new QVBoxLayout();
		loadBL(l, node.childNodes(), ws, t);
		if (elem.attribute("withSpacer") == "true") {
			l->addStretch(1);
		}
		return l;
	}
	return nullptr;
}

void InputConditionPage::loadBL(QBoxLayout* layout, const QDomNodeList& list, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	for (int i = 0; i < list.length(); ++i) {
		QDomNode c = list.item(i);
		QObject* obj = loadRec(c, ws, t);
		if (obj == nullptr) {continue;}
		int stretch = c.toElement().attribute("stretch", "0").toInt();
		if (QWidget* w = qobject_cast<QWidget*>(obj)) {
			layout->addWidget(w, stretch);
		} else if (QLayout* l = qobject_cast<QLayout*>(obj)) {
			layout->addLayout(l, stretch);
		}
	}
}

void InputConditionPage::loadGL(QGridLayout* layout, const QDomNodeList& list, InputConditionWidgetSet* ws, const SolverDefinitionTranslator& t)
{
	for (int i = 0; i < list.length(); ++i) {
		QDomNode c = list.item(i);
		QDomElement e = c.toElement();
		int row = e.attribute("row").toInt();
		int col = e.attribute("col").toInt();
		QObject* obj = loadRec(c, ws, t);
		if (obj == nullptr) {continue;}
		if (QWidget* w = qobject_cast<QWidget*>(obj)) {
			layout->addWidget(w, row, col);
		} else if (QLayout* l = qobject_cast<QLayout*>(obj)) {
			layout->addLayout(l, row, col);
		}
	}

}
