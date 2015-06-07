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
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void closeCgnsFile() override;
	QStringList containedFiles() override;
	bool gridEdited();
	void toggleGridEditFlag();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void createConnections();
	PreProcessorWindow* m_preProcessorWindow;
};

#endif // PREPROCESSORWINDOWPROJECTDATAITEM_H
