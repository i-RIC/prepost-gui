#ifndef POSTABSTRACTSTEPS_H
#define POSTABSTRACTSTEPS_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

/// This class is an abstract class that contains how many steps of
/// results are included in the current CGNS file.
class GUICOREDLL_EXPORT PostAbstractSteps : public ProjectDataItem
{

public:
	/// Constructor
	PostAbstractSteps(ProjectDataItem* parent);
	/// Load data from CGNS file.
	virtual void loadFromCgnsFile(const int fn) = 0;
	/// Returns true if the current CGNS file containes more than one result step.
	virtual bool dataExists() const = 0;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	/// Clear the iteration array data copied to this instance.
	virtual void clearArray() = 0;
};

#endif // POSTABSTRACTSTEPS_H
