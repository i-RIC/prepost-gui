#include "../gridexporter/cgnsgridexporter.h"
#include "gridexporterfactory.h"

#include <guicore/pre/grid/gridexporteri.h>
#include <misc/iricrootpath.h>

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

GridExporterFactory& GridExporterFactory::instance()
{
	return *m_instance;
}

GridExporterFactory::GridExporterFactory()
{
	// load built-in exporters first.
	m_exporterList.push_back(new CgnsGridExporter());

	// load plug-in exporters.
	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QDir pluginsDir(iRICRootPath::get());
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
			GridExporterI* exporter = 0;
			if (! plugin) {
				goto LOADERROR;
			}
			exporter = qobject_cast<GridExporterI*> (plugin);
			if (! exporter) {
				delete plugin;
				goto LOADERROR;
			}
			if (translator) {
				qApp->installTranslator(translator);
			}
			m_exporterList.push_back(exporter);
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

const std::vector<GridExporterI*> GridExporterFactory::listForGridType(SolverDefinitionGridType::GridType gt) const
{
	std::vector<GridExporterI*> ret;
	for (auto exporter: m_exporterList) {
		if (exporter->isGridTypeSupported(gt)) {
			ret.push_back(exporter);
		}
	}

	return ret;
}
