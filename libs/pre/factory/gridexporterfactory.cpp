#include "../gridexporter/cgnsgridexporter.h"
#include "gridexporterfactory.h"

#include <guicore/pre/grid/gridexporterinterface.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QSettings>
#include <QTranslator>

GridExporterFactory* GridExporterFactory::m_instance = nullptr;

void GridExporterFactory::init()
{
	delete m_instance;
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
	for (const QString& folderName : pluginsDir.entryList(QDir::Dirs)) {
		QDir pluginDir(pluginsDir.filePath(folderName));
		for (const QString& fileName : pluginDir.entryList(nameFilters, QDir::Files)) {
			QFileInfo finfo(fileName);
			QString dictName = QString("%1_%2.qm").arg(finfo.baseName()).arg(locale);
			QFile dictFile(pluginDir.filePath(dictName));
			QTranslator* translator = nullptr;
			if (dictFile.exists()) {
				translator = new QTranslator(this);
				translator->load(QString("%1_%2").arg(finfo.baseName()).arg(locale), pluginDir.absolutePath());
			}
			QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
			QObject* plugin = loader.instance();
			GridExporterInterface* exporter = 0;
			if (! plugin) {
				goto LOADERROR;
			}
			exporter = qobject_cast<GridExporterInterface*> (plugin);
			if (! exporter) {
				delete plugin;
				goto LOADERROR;
			}
			if (translator) {
				qApp->installTranslator(translator);
			}
			m_exporterList.append(exporter);
			continue;

LOADERROR:
			delete translator;
		}
	}
}

GridExporterFactory::~GridExporterFactory()
{
	for (auto e : m_exporterList) {
		delete e;
	}
	m_exporterList.clear();
}

const QList<GridExporterInterface*> GridExporterFactory::list(SolverDefinitionGridType::GridType gt) const
{
	QList<GridExporterInterface*> ret;
	for (GridExporterInterface* iface : m_exporterList) {
		if (iface->isGridTypeSupported(gt)) {
			ret.append(iface);
		}
	}
	return ret;
}
