#ifndef CGNSFILEENTRY_H
#define CGNSFILEENTRY_H

#include "../guicore_global.h"
#include "projectdataitem.h"

class QString;

/// This class handles the information about a CGNS file in the project.
class GUICOREDLL_EXPORT CgnsFileEntry : public ProjectDataItem
{
public:
	CgnsFileEntry(ProjectDataItem* parent);
	CgnsFileEntry(const QString& filename, ProjectDataItem* parent);
	~CgnsFileEntry();

	/// file name (".cgn" is not included. For example "calculation1".
	QString filename() const;
	/// Comment for this calculation
	QString comment() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/cgnsfileentry_impl.h"
#endif // _DEBUG

#endif // CGNSFILEENTRY_H
