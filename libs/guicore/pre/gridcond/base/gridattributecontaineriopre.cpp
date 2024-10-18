#include "gridattributecontainer.h"
#include "gridattributecontaineriopre.h"
#include "gridattributedimensionscontainer.h"
#include "../../../grid/v4grid.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"

#include <misc/filesystemfunction.h>

#include <iriclib_errorcodes.h>

GridAttributeContainerIoPre::GridAttributeContainerIoPre(const QString& tempDir) :
	m_tempDir {tempDir}
{}

int GridAttributeContainerIoPre::loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts, GridAttributeContainer* cont)
{
	auto dims = cont->dimensions();
	int ier = 0;
	if (dims != nullptr && dims->containers().size() > 0) {
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			ier = cont->loadFromCgnsFileForIndex(atts, index);
			if (ier == IRIC_NO_ERROR) {
				auto fileName = tempExternalFilename(cont, index);
				QFileInfo finfo(fileName);
				iRIC::mkdirRecursively(finfo.absolutePath());
				bool ok = cont->saveToExternalFile(fileName);
				if (! ok) {return IRIC_FILE_COPY_FAIL;}
			}
		}
	}

	ier = cont->loadFromCgnsFileForIndex(atts, 0);
	if (ier == IRIC_NO_ERROR) {
		cont->setMapped(true);
	}
	return ier;
}

int GridAttributeContainerIoPre::saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts, GridAttributeContainer* cont)
{
	auto dims = cont->dimensions();

	// output dimension values
	for (auto dim : dims->containers()) {
		int ier = dim->saveToCgnsFile(atts);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	if (dims->containers().size() == 0) {
		return cont->saveToCgnsFileForIndex(atts, 0);
	} else {
		for (int index = 0; index <= dims->maxIndex(); ++index) {
			bool ok = cont->loadFromExternalFile(tempExternalFilename(cont, index));
			if (ok) {
				int ier = cont->saveToCgnsFileForIndex(atts, index);
				if (ier != IRIC_NO_ERROR) {return ier;}
			}
		}
		return IRIC_NO_ERROR;
	}
}

void GridAttributeContainerIoPre::clearTemporaryData(GridAttributeContainer* cont)
{
	int index = 0;
	while (true) {
		auto fname = tempExternalFilename(cont, index);
		QFile f(fname);
		if (! f.exists()) {return;}

		f.remove();
		++ index;
	}
}

void GridAttributeContainerIoPre::handleDimensionCurrentIndexChange(int oldIndex, int newIndex, GridAttributeContainer* cont)
{
	if (oldIndex == newIndex) {return;}

	QString fname = tempExternalFilename(cont, oldIndex);

	QFileInfo finfo(fname);
	iRIC::mkdirRecursively(finfo.absolutePath());
	cont->saveToExternalFile(fname);

	fname = tempExternalFilename(cont, newIndex);
	cont->loadFromExternalFile(fname);

	emit cont->grid()->grid()->changed();
}

QString GridAttributeContainerIoPre::tempExternalFilename(GridAttributeContainer* cont, int index) const
{
	QString format("%1_%2.dat");
	QString filename = format.arg(cont->gridAttribute()->name().c_str()).arg(index + 1);

	QDir subDir(m_tempDir);
	return subDir.absoluteFilePath(filename);
}
