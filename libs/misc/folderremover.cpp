#include "filesystemfunction.h"
#include "folderremover.h"

FolderRemover::FolderRemover(const QString& folderName) :
	m_folderName {folderName}
{}

FolderRemover::~FolderRemover()
{
	iRIC::rmdirRecursively(m_folderName);
}
