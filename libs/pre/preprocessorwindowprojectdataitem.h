#ifndef PREPROCESSORWINDOWPROJECTDATAITEM_H
#define PREPROCESSORWINDOWPROJECTDATAITEM_H

#include "pre_global.h"
#include <guicore/project/projectdataitem.h>
class PreProcessorWindow;
class ProjectMainFile;

/// This class load or save the status of PreProcessorWindow into ProjectMainFile .
class PREDLL_EXPORT PreProcessorWindowProjectDataItem : public ProjectDataItem
{
	Q_OBJECT
public:
	/// constructor
	explicit PreProcessorWindowProjectDataItem(PreProcessorWindow* w, ProjectMainFile* parent);
	~PreProcessorWindowProjectDataItem();
	/// Initializes pre-processor window for the specified solver definition.
	void initForSolverDefinition();
	void loadFromCgnsFile(const int fn);
	void saveToCgnsFile(const int fn);
	void closeCgnsFile();
	QStringList containedFiles() override;
	bool gridEdited();
	void toggleGridEditFlag();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	void createConnections();
	PreProcessorWindow* m_preProcessorWindow;
};

#endif // PREPROCESSORWINDOWPROJECTDATAITEM_H
