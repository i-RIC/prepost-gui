#include "../gridimporter/structured15dgridwithcrosssectioncgnsimporter.h"
#include "../gridimporter/structured15dgridwithcrosssectionprojectimporter.h"
#include "../gridimporter/structured2dgridcgnsimporter.h"
#include "../gridimporter/structured2dgridprojectimporter.h"
#include "../gridimporter/unstructured2dgridcgnsimporter.h"
#include "../gridimporter/unstructured2dgridprojectimporter.h"
#include "gridimporterfactory.h"

#include <guicore/pre/grid/gridimporteri.h>
#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QPluginLoader>
#include <QSettings>
#include <QTranslator>

GridImporterFactory* GridImporterFactory::m_instance = nullptr;

void GridImporterFactory::init()
{
	delete m_instance;
	m_instance = new GridImporterFactory();
}

GridImporterFactory& GridImporterFactory::instance()
{
	return *m_instance;
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
	QDir pluginsDir(iRICRootPath::get());
	pluginsDir.cdUp();
	pluginsDir.cdUp();
	pluginsDir.cd("gridimporter_plugins");

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
			GridImporterI* importer = nullptr;
			if (! plugin) {
				goto LOADERROR;
			}
			importer = qobject_cast<GridImporterI*> (plugin);
			if (! importer) {
				delete plugin;
				goto LOADERROR;
			}
			if (translator) {
				qApp->installTranslator(translator);
			}
			m_importerList.append(importer);
			continue;

LOADERROR:
			delete translator;
		}
	}
}

GridImporterFactory::~GridImporterFactory()
{
	for (auto i : m_importerList) {
		delete i;
	}
	m_importerList.clear();
}

const QList<GridImporterI*> GridImporterFactory::list(const SolverDefinitionGridType& gt) const
{
	QList<GridImporterI*> ret;

	auto types = gt.availableGridTypes();
	for (auto importer : m_importerList) {
		for (auto gt : types) {
			if (importer->supportedGridType() == gt) {
				ret.append(importer);
				break;
			}
		}
	}
	return ret;
}
