#include "model_projectloader.h"

#include <QDir>

Model::ProjectLoader::ProjectLoader(Model* model) :
	m_model {model}
{}

QDomNode Model::ProjectLoader::projectPreProcessorNode() const
{
	return m_projectPreProcessorNode;
}

bool Model::ProjectLoader::load()
{
	QDir modelPath(m_model->absolutePath());
	m_projectFile.setFileName(modelPath.absoluteFilePath("project.xml"));
	bool ok = m_projectFile.open(QFile::ReadOnly);
	if (! ok) {return false;}

	QString errorStr;
	int errorLine, errorColumn;
	m_projectDocument.setContent(&m_projectFile, &errorStr, &errorLine, &errorColumn);

	return loadRecursive(m_projectDocument.documentElement());
}

bool Model::ProjectLoader::loadRecursive(const QDomNode& node)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);

		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "PreProcessorWindow") {
			auto elem = c.toElement();
			auto ref = elem.attribute("ref");
			if (ref == "") {
				m_mode = Mode::SingleFile;
				m_projectPreProcessorNode = c;
			} else {
				QDir modelPath(m_model->absolutePath());
				m_preProcessorFile.setFileName(modelPath.absoluteFilePath(ref));
				bool ok = m_preProcessorFile.open(QFile::ReadOnly);
				if (! ok) {return false;}

				QString errorStr;
				int errorLine, errorColumn;
				m_preProcessorDocument.setContent(&m_preProcessorFile, &errorStr, &errorLine, &errorColumn);

				m_mode = Mode::SeparateFile;
				m_projectPreProcessorNode = m_preProcessorDocument.documentElement();
				return true;
			}
		} else {
			bool ok = loadRecursive(c);
			if (! ok) {return false;}
		}
	}

	return true;
}
