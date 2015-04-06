#include "gridexporterfactory.h"
#include <guicore/pre/grid/gridexporterinterface.h>
#include "../gridexporter/cgnsgridexporter.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <QPluginLoader>
#include <QSettings>

GridExporterFactory* GridExporterFactory::m_instance = 0;

void GridExporterFactory::init()
{
	if (m_instance != 0){
		delete m_instance;
	}
	m_instance = new GridExporterFactory();
}

GridExporterFactory::GridExporterFactory()
{
	// load built-in exporters first.
	m_exporterList.append(new CgnsGridExporter());

	// load plug-in exporters.
	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QDir pluginsDir(qApp->applicationDirPath());
	pluginsDir.cdUp();
	pluginsDir.cdUp();
	pluginsDir.cd("gridexporter_plugins");

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
			if (! plugin){
				goto LOADERROR;
			}
			GridExporterInterface* exporter = qobject_cast<GridExporterInterface*> (plugin);
			if (! exporter){
				delete plugin;
				goto LOADERROR;
			}
			if (translator){
				qApp->installTranslator(translator);
			}
			m_exporterList.append(exporter);
			continue;

LOADERROR:
			if (translator != 0){
				delete translator;
			}
		}
	}
}

GridExporterFactory::~GridExporterFactory()
{
	for (int i = 0; i < m_exporterList.size(); ++i){
		GridExporterInterface* iface = m_exporterList.at(i);
		delete iface;
	}
	m_exporterList.clear();
}

const QList<GridExporterInterface*> GridExporterFactory::list(SolverDefinitionGridType::GridType gt) const
{
	QList<GridExporterInterface*> ret;
	for (int i = 0; i < m_exporterList.size(); ++i){
		GridExporterInterface* iface = m_exporterList.at(i);
		if (iface->isGridTypeSupported(gt)){
			ret.append(iface);
		}
	}
	return ret;
}
