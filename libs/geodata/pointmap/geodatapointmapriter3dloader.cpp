#include "geodatapointmap.h"
#include "geodatapointmapriter3dloader.h"
#include "private/geodatapointmapriter3dloader_tarheaderblock.h"

#include <guibase/executer/executerwatcher.h>
#include <guibase/executer/unziparchiveprocessexecuter.h>
#include <guicore/base/iricmainwindowi.h>
#include <misc/folderremover.h>
#include <misc/stringtool.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <Poco/BinaryReader.h>

#include <QDir>
#include <QMessageBox>

#include <fstream>
#include <iomanip>

namespace {
const QString FILENAME = "tin.tin";
}

GeoDataPointmapRiter3dLoader::GeoDataPointmapRiter3dLoader(const QString& fileName) :
	m_fileName {fileName},
	m_tmpFileName {},
	m_folderRemover {nullptr}
{}

GeoDataPointmapRiter3dLoader::~GeoDataPointmapRiter3dLoader()
{
	close();
}

bool GeoDataPointmapRiter3dLoader::open(const QString& tmpFileName, iRICMainWindowI* mainWindow)
{
	m_tmpFileName = tmpFileName;

	UnzipArchiveProcessExecuter executer(m_fileName, tmpFileName);
	auto watcher = mainWindow->buildExecuteWatcher(&executer);
	watcher->setMessage(tr("Loading RiTER3D project file..."));
	watcher->execute();

	if (executer.isCanceled()) {return false;}

	m_folderRemover = new FolderRemover(m_tmpFileName);
	return true;
}

void GeoDataPointmapRiter3dLoader::close()
{
	if (m_folderRemover == nullptr) {return;}

	delete m_folderRemover;
	m_folderRemover = nullptr;
}

bool GeoDataPointmapRiter3dLoader::check(QWidget* w)
{
	QDir dir(m_tmpFileName);
	auto fname = dir.absoluteFilePath(FILENAME);
	if (! QFile::exists(fname)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not exist in the project file").arg(FILENAME));
		return false;
	}

	return true;
}

bool GeoDataPointmapRiter3dLoader::load(GeoDataPointmap* pointmap, QWidget* w)
{
	// use TinTinImporter implementation to load
	QDir dir(m_tmpFileName);
	auto fname = dir.absoluteFilePath(FILENAME);

	std::ifstream s(iRIC::toStr(fname).c_str(), std::ios_base::in | std::ios_base::binary);
	if (! s) {return false;}

	Poco::BinaryReader reader(s);
	TarHeaderBlock header;

	// metadata
	std::string content, offset;

	header.read(&reader);
	auto size = header.size();
	reader.readRaw(size, content);
	reader.readRaw(header.offset(), offset);

	// points
	header.read(&reader);
	auto fileSize = header.size();

	// skip header
	std::string str;
	reader.readRaw(30 + 10 + 40, str);

	int dataSize;
	reader >> dataSize;
	double x, y, e;
	auto points = vtkSmartPointer<vtkPoints>::New();
	auto values = vtkSmartPointer<vtkDoubleArray>::New();
	points->Allocate(dataSize);
	values->Allocate(dataSize);

	for (int i = 0; i < dataSize; ++i) {
		reader >> x >> y >> e;
		points->InsertNextPoint(x, y, 0);
		values->InsertNextValue(e);
	}
	reader.readRaw(header.offset(), offset);

	// skip header
	header.read(&reader);

	size = header.size() / (sizeof(int) * 3);
	auto tris = vtkSmartPointer<vtkCellArray>::New();

	int index1, index2, index3;
	vtkIdType indices[3];
	for (unsigned int i = 0; i < size; ++i) {
		reader >> index1 >> index2 >> index3;
		indices[0] = index1;
		indices[1] = index2;
		indices[2] = index3;
		tris->InsertNextCell(3, indices);
	}

	reader.readRaw(header.offset(), offset);

	auto tin = vtkSmartPointer<vtkPolyData>::New();
	tin->SetPoints(points);
	tin->SetPolys(tris);

	pointmap->setTin(tin, values);

	return true;
}
