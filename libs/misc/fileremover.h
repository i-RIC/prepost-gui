#ifndef FILEREMOVER_H
#define FILEREMOVER_H

#include "misc_global.h"

#include <QString>

class MISCDLL_EXPORT FileRemover
{
public:
	FileRemover(const QString& fileName);
	~FileRemover();

private:
	QString m_fileName;
};

#endif // FILEREMOVER_H
