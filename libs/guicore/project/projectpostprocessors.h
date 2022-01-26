#ifndef PROJECTPOSTPROCESSORS_H
#define PROJECTPOSTPROCESSORS_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include <QList>

class QDir;
class QMdiSubWindow;
class PostProcessorWindowProjectDataItem;
class PostProcessorWindowFactoryInterface;

class GUICOREDLL_EXPORT ProjectPostProcessors : public ProjectDataItem
{
public:
	static const QString subwindowsFolder();

	ProjectPostProcessors(ProjectDataItem* parent);
	~ProjectPostProcessors();
	QMdiSubWindow* add(PostProcessorWindowProjectDataItem* newitem);
	void requestDelete(PostProcessorWindowProjectDataItem* item);
	int windowCount() const;
	void setFactory(PostProcessorWindowFactoryInterface* factory);
	void applyOffset(double x_diff, double y_diff);
	QStringList containedFiles() const override;

	void loadFromXmlFile(const QDomNode& node, const QDir workDir);
	void saveToXmlFile(QXmlStreamWriter& writer, const QDir workDir);

private:
	// Never used.
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QList<PostProcessorWindowProjectDataItem*> m_postProcessorWindows;
	PostProcessorWindowFactoryInterface* m_factory;
};

#endif // PROJECTPOSTPROCESSORS_H
