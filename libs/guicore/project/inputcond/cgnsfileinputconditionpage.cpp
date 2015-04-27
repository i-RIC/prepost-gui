#include "cgnsfileinputconditionpage.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "cgnsfileinputconditionwidget.h"
#include "cgnsfileinputconditionwidgetset.h"

#include <misc/xmlsupport.h>

#include <QLabel>
#include <QDomNodeList>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

CgnsFileInputConditionPage::CgnsFileInputConditionPage(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t, QWidget* parent)
	: QWidget(parent)
{
	// load and construct itself.
	load(node, ws, t);
}

void CgnsFileInputConditionPage::load(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	QDomElement elem = node.toElement();
	m_name = elem.attribute("name");
	QVBoxLayout* masterLayout = new QVBoxLayout(this);
	QLayout* layout = nullptr;
	// obtain "Content" node.
	QDomNode contNode = iRIC::getChildNode(node, "Content");
	if (! contNode.isNull()){
		// legacy mode.
		if (contNode.toElement().attribute("layout", "") == "custom"){
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

QLayout* CgnsFileInputConditionPage::loadAuto(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	bool simple = true;
	QDomNodeList items = node.childNodes();
	for (unsigned int i = 0; i < items.length(); ++i){
		QDomNode child = items.item(i);
		simple = simple && (child.nodeName() == "Item" || child.nodeName() == "GroupBox");
	}
	if (simple){
		return loadSimple(node, ws, t);
	} else {
		return loadCustom(node, ws, t);
	}
}

QLayout* CgnsFileInputConditionPage::loadSimple(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	QDomNodeList items = node.childNodes();
	QFormLayout* layout = new QFormLayout();
	QDomElement elem;
	for (unsigned int i = 0; i < items.length(); ++i){
		elem = items.item(i).toElement();
		if (elem.nodeName() == "Item"){
			QString itemname = elem.attribute("name");
			layout->addRow(t.translate(elem.attribute("caption")), ws->widget(itemname));
		} else if (elem.nodeName() == "GroupBox"){
			QString caption = t.translate(elem.attribute("caption"));
			QGroupBox* g = new QGroupBox(caption, this);
			QLayout* layout2 = loadAuto(elem, ws, t);
			g->setLayout(layout2);
			layout->addRow(g);
		}
	}
	return layout;
}

QLayout* CgnsFileInputConditionPage::loadCustom(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	QDomElement elem = node.toElement();
	QVBoxLayout *layout = new QVBoxLayout();
	QDomNodeList children = node.childNodes();
	loadBL(layout, children, ws, t);
	if (elem.attribute("withSpacer") == "true"){
		layout->addStretch(1);
	}
	return layout;
}

QObject* CgnsFileInputConditionPage::loadRec(const QDomNode& node, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	QString nodeName = node.nodeName();
	QDomElement elem = node.toElement();
	if (nodeName == "Item"){
		QString itemName = elem.attribute("name");
		return ws->widget(itemName);
	} else if (nodeName == "Label"){
		QString caption = t.translate(elem.attribute("caption"));
		return getCaption(caption);
	} else if (nodeName == "GroupBox"){
		QString caption = t.translate(elem.attribute("caption"));
		QGroupBox* g = new QGroupBox(caption, this);
		QLayout* layout = loadAuto(node, ws, t);
		g->setLayout(layout);
		return g;
	} else if (nodeName == "GridLayout"){
		QGridLayout* l = new QGridLayout();
		loadGL(l, node.childNodes(), ws, t);
		return l;
	} else if (nodeName == QString("HBoxLayout")){
		QHBoxLayout* l = new QHBoxLayout();
		loadBL(l, node.childNodes(), ws, t);
		if (elem.attribute("withSpacer") == "true"){
			l->addStretch(1);
		}
		return l;
	} else if (nodeName == QString("VBoxLayout")){
		QVBoxLayout* l = new QVBoxLayout();
		loadBL(l, node.childNodes(), ws, t);
		if (elem.attribute("withSpacer") == "true"){
			l->addStretch(1);
		}
		return l;
	}
	return nullptr;
}

void CgnsFileInputConditionPage::loadBL(QBoxLayout* layout, const QDomNodeList& list, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	for (unsigned int i = 0; i < list.length(); ++i){
		QDomNode c = list.item(i);
		QObject* obj = loadRec(c, ws, t);
		if (obj == nullptr){continue;}
		if (QWidget * w = qobject_cast<QWidget *>(obj)){
			layout->addWidget(w);
		} else if (QLayout * l = qobject_cast<QLayout*>(obj)){
			layout->addLayout(l);
		}
	}
}

void CgnsFileInputConditionPage::loadGL(QGridLayout* layout, const QDomNodeList& list, CgnsFileInputConditionWidgetSet *ws, const SolverDefinitionTranslator& t)
{
	for (unsigned int i = 0; i < list.length(); ++i){
		QDomNode c = list.item(i);
		QDomElement e = c.toElement();
		int row = e.attribute("row").toInt();
		int col = e.attribute("col").toInt();
		QObject* obj = loadRec(c, ws, t);
		if (obj == nullptr){continue;}
		if (QWidget * w = qobject_cast<QWidget*>(obj)){
			layout->addWidget(w, row, col);
		}else if (QLayout * l = qobject_cast<QLayout*>(obj)){
			layout->addLayout(l, row, col);
		}
	}

}

QLabel* CgnsFileInputConditionPage::getCaption(const QString& caption)
{
	QLabel* l = new QLabel(this);
	l->setText(caption);
	return l;
}
