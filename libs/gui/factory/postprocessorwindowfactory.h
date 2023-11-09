#ifndef POSTPROCESSORWINDOWFACTORY_H
#define POSTPROCESSORWINDOWFACTORY_H

#include <guicore/post/postprocessorwindowfactoryi.h>
#include <QObject>
#include <QMap>

class QDomNode;
class QWidget;
class QXmlStreamWriter;
class ProjectDataItem;
class PostProcessorWindowProjectDataItem;

class PostProcessorWindowFactory : public QObject, public PostProcessorWindowFactoryI
{
	Q_OBJECT

public:
	/// Constructor
	PostProcessorWindowFactory(QObject* parent);
	PostProcessorWindowProjectDataItem* factory(const QString& name, ProjectDataItem* parent, QWidget* parentwindow) override;
	PostProcessorWindowProjectDataItem* restore(const QDomNode& node, ProjectDataItem* parent, QWidget* parentwindow, bool ignoreIndex) override;
	void loadWindowCounts(const QDomNode& node);
	void saveWindowCounts(QXmlStreamWriter& writer);
	void resetWindowCounts();

private:
	PostProcessorWindowProjectDataItem* factory(const QString& name, int index, ProjectDataItem* parent, QWidget* parentwindow);
	QMap<QString, int> m_windowCount;
};

#endif // POSTPROCESSORWINDOWFACTORY_H
