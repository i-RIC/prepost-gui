#ifndef POSTPROCESSORWINDOWFACTORY_H
#define POSTPROCESSORWINDOWFACTORY_H

#include <guicore/post/postprocessorwindowfactoryinterface.h>
#include <QObject>
#include <QMap>

class QDomNode;
class QWidget;
class QXmlStreamWriter;
class ProjectDataItem;
class PostProcessorWindowProjectDataItem;

class PostProcessorWindowFactory : public QObject, public PostProcessorWindowFactoryInterface
{
	Q_OBJECT

public:
	/// Constructor
	PostProcessorWindowFactory(QObject* parent);
	PostProcessorWindowProjectDataItem* factory(const QString& name, ProjectDataItem* parent, QWidget* parentwindow);
	PostProcessorWindowProjectDataItem* restore(const QDomNode& node, ProjectDataItem* parent, QWidget* parentwindow) const;
	void loadWindowCounts(const QDomNode& node);
	void saveWindowCounts(QXmlStreamWriter& writer);
	void resetWindowCounts();
private:
	QMap<QString, int> m_windowCount;
};

#endif // POSTPROCESSORWINDOWFACTORY_H
