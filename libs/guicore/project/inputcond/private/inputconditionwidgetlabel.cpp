#include "../../../solverdef/solverdefinitiontranslator.h"
#include "inputconditionwidgetlabel.h"
#include "inputconditiondependencychecksubcaptions.h"

#include <QDomElement>
#include <QHBoxLayout>
#include <QLabel>


InputConditionWidgetLabel::InputConditionWidgetLabel(QDomNode defNode, const SolverDefinitionTranslator& t) :
	InputConditionWidget(defNode),
	m_label {new QLabel(this)}
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_label);
	setLayout(layout);

	QDomElement defElem = defNode.toElement();
	QString caption = defElem.attribute("caption");
	m_caption = t.translate(caption);

	QDomNodeList children = defElem.childNodes();

	for (int i = 0; i < children.size(); ++i) {
		QDomNode subCaption = children.at(i);
		if (subCaption.nodeName() != "SubCaption") {continue;}

		QDomElement subCaptionElem = subCaption.toElement();
		caption = subCaptionElem.attribute("caption");
		QString localCaption = t.translate(caption);
		m_subCaptions.insert({caption, localCaption});
	}

	m_checkSubCaptions = new InputConditionDependencyCheckSubCaptions(this);
}

void InputConditionWidgetLabel::addTooltip(const QString& tooltip)
{
	// do nothing
}

void InputConditionWidgetLabel::setCaption(const QString& caption)
{
	auto it = m_subCaptions.find(caption);
	if (it == m_subCaptions.end()) {return;}

	m_label->setText(it->second);
}

void InputConditionWidgetLabel::restoreCaption()
{
	m_label->setText(m_caption);
}

InputConditionDependencyCheckSubCaptions* InputConditionWidgetLabel::checkSubCaptions() const
{
	return m_checkSubCaptions;
}
