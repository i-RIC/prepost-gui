#include "gridimporterfactory.h"
#include <guicore/pre/grid/gridimporterinterface.h>
#include "../gridimporter/structured2dgridcgnsimporter.h"
#include "../gridimporter/structured2dgridprojectimporter.h"
#include "../gridimporter/structured15dgridwithcrosssectioncgnsimporter.h"
#include "../gridimporter/structured15dgridwithcrosssectionprojectimporter.h"
#include "../gridimporter/unstructured2dgridcgnsimporter.h"
#include "../gridimporter/unstructured2dgridprojectimporter.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <QPluginLoader>
#include <QSettings>

GridImporterFactory* GridImporterFactory::m_instance = 0;

void GridImporterFactory::init()
{
	if (m_instance != 0){
		delete m_instance;
	}
	m_instance = new GridImporterFactory();
}

GridImporterFactory::GridImporterFactory()
{
	// load built-in importers first.
	m_importerList.append(new Structured2DGridCgnsImporter());
	m_importerList.append(new Structured2DGridProjectImporter());
	m_importerList.append(new Structured15DGridWithCrossSectionCgnsImporter());
	m_importerList.append(new Structured15DGridWithCrossSectionProjectImporter());
	m_importerList.append(new Unstructured2DGridCgnsImporter());
	m_importerList.append(new Unstructured2DGridProjectImporter());

	// load plug-in importers.

	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QDir pluginsDir(qApp->applicationDirPath());
	pluginsDir.cdUp();
	pluginsDir.cdUp();
	pluginsDir.cd("gridimporter_plugins");

	QStringList nameFilters;
	nameFilters.append("*.dll");
	foreach (QString folderName, pluginsDir.entryList(QDir::Dirs)){
		QDir pluginDir(pluginsDir.filePath(folderName));
		foreach (QString fileName, pluginDir.entryList(nameFilters, QDir::Files)){
			QFileInfo finfo(fileName);
			QString dictName = QString("%1_%2.qm").arg(finfo.baseName()).arg(locale);
			QFile dictFile(pluginDir.filePath(dictName));
			QTranslator* translator = 0;
			if (dictFile.exists()){
				translator = new QTranslator(this);
				translator->load(QString("%1_%2").arg(finfo.baseName()).arg(locale), pluginDir.absolutePath());
			}
			QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
			QObject* plugin = loader.instance();
			GridImporterInterface* importer = 0;
			if (! plugin){
				goto LOADERROR;
			}
			importer = qobject_cast<GridImporterInterface*> (plugin);
			if (! importer){
				delete plugin;
				goto LOADERROR;
			}
			if (translator){
				qApp->installTranslator(translator);
			}
			m_importerList.append(importer);
			continue;

LOADERROR:
			if (translator != 0){
				delete translator;
			}
		}
	}
}

GridImporterFactory::~GridImporterFactory()
{
	for (int i = 0; i < m_importerList.size(); ++i){
		GridImporterInterface* iface = m_importerList.at(i);
		delete iface;
	}
	m_importerList.clear();
}

const QList<GridImporterInterface*> GridImporterFactory::list(SolverDefinitionGridType::GridType gt) const
{
	QList<GridImporterInterface*> ret;
	for (int i = 0; i < m_importerList.size(); ++i){
		GridImporterInterface* iface = m_importerList.at(i);
		if (iface->isGridTypeSupported(gt)){
			ret.append(iface);
		}
	}
	return ret;
}
