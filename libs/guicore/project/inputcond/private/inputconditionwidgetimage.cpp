#include "../../../solverdef/solverdefinitiontranslator.h"
#include "inputconditiondependencychecksubimages.h"
#include "inputconditionwidgetimage.h"

#include <QDir>
#include <QDomElement>
#include <QHBoxLayout>
#include <QLabel>

InputConditionWidgetImage::InputConditionWidgetImage(QDomNode defNode, const SolverDefinitionTranslator& t, const QDir& dir) :
	InputConditionWidget(defNode),
	m_label {new QLabel(this)}
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_label);
	setLayout(layout);

	QDomElement defElem = defNode.toElement();
	QString src = defElem.attribute("src");
	QString localeSrc = t.imageFilename(src);

	if (QFile(localeSrc).exists()) {
		m_image = QPixmap(dir.absoluteFilePath(localeSrc));
	} else {
		m_image = QPixmap(dir.absoluteFilePath(src));
	}

	QDomNodeList children = defElem.childNodes();

	for (int i = 0; i < children.size(); ++i) {
		QDomNode subImage = children.at(i);
		if (subImage.nodeName() != "SubImage") {continue;}

		QDomElement subImageElem = subImage.toElement();
		src = subImageElem.attribute("src");
		localeSrc = t.imageFilename(src);

		QPixmap subPixmap;
		if (QFile(localeSrc).exists()) {
			subPixmap = QPixmap(dir.absoluteFilePath(localeSrc));
		} else {
			subPixmap = QPixmap(dir.absoluteFilePath(src));
		}
		m_subImages.insert(std::pair<QString, QPixmap>(src, subPixmap));
	}

	m_label->setPixmap(m_image);
	m_label->setMaximumSize(m_image.size());

	m_checkSubImages = new InputConditionDependencyCheckSubImages(this);
}

void InputConditionWidgetImage::addTooltip(const QString &)
{
	// do nothing
}

void InputConditionWidgetImage::activateSubImage(const QString& name)
{
	std::map<QString, QPixmap>::iterator it = m_subImages.find(name);
	if (it == m_subImages.end()) {return;}

	m_label->setPixmap(it->second);
}

void InputConditionWidgetImage::inactivateSubImages()
{
	m_label->setPixmap(m_image);
}

InputConditionDependencyCheckSubImages* InputConditionWidgetImage::checkSubImages() const
{
	return m_checkSubImages;
}
