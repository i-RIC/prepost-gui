#include "geodatapointmap.h"
#include "geodatapointmapriter3dprojectwatcher.h"
#include "geodatapointmapriter3dloader.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/project/projectdata.h>
#include <misc/informationdialog.h>

#include <QDir>
#include <QFileSystemWatcher>

GeoDataPointmapRiter3dProjectWatcher::GeoDataPointmapRiter3dProjectWatcher(const QString& fileName, GeoDataPointmap* pointmap) :
	m_fileName {fileName},
	m_pointmap {pointmap},
	m_watcher {new QFileSystemWatcher(this)}
{
	m_watcher->addPath(fileName);

	connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &GeoDataPointmapRiter3dProjectWatcher::load);
}

void GeoDataPointmapRiter3dProjectWatcher::load()
{
	auto mainWindow = m_pointmap->iricMainWindow();

	InformationDialog::information(mainWindow, tr("Information"), tr("%1 is updated. Starting importing new TIN to %2.").arg(QDir::toNativeSeparators(m_fileName)).arg(m_pointmap->caption()), "geodatapointmapriter3dprojectwatcher_load");

	GeoDataPointmapRiter3dLoader loader(m_fileName);
	bool ok = loader.open(m_pointmap->projectData()->tmpFileName(), mainWindow);
	if (! ok) {return;}

	ok = loader.check(mainWindow);
	if (! ok) {return;}

	ok = loader.load(m_pointmap, mainWindow);
}
