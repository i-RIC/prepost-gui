#ifndef MODEL_PROJECTLOADER_H
#define MODEL_PROJECTLOADER_H

#include "../model.h"

#include <QFile>
#include <QDomDocument>

class QDomNode;

class Model::ProjectLoader
{
public:
	ProjectLoader(Model* model);

	QDomNode projectPreProcessorNode() const;

	bool load();

private:
	enum class Mode {
		SingleFile,
		SeparateFile
	};

	bool loadRecursive(const QDomNode& node);

	Mode m_mode;

	QFile m_projectFile;
	QDomDocument m_projectDocument;

	QFile m_preProcessorFile;
	QDomDocument m_preProcessorDocument;

	QDomNode m_projectPreProcessorNode;

	Model* m_model;
};

#endif // MODEL_PROJECTLOADER_H
