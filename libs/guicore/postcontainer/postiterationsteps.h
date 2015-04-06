#ifndef POSTITERATIONSTEPS_H
#define POSTITERATIONSTEPS_H

#include "../guicore_global.h"
#include "postabstractsteps.h"
#include <QList>
/// This class stores the iterationsteps included in cgns file.
class GUICOREDLL_EXPORT PostIterationSteps : public PostAbstractSteps
{
	Q_OBJECT
public:
	/// Constructor
	PostIterationSteps(ProjectDataItem* parent) : PostAbstractSteps(parent){}
	void loadFromCgnsFile(const int fn);
	const QList<int>& iterationsteps() const{return m_iterationsteps;}
	bool dataExists(){return m_iterationsteps.count() > 0;}
	void checkStepsUpdate(int fn);
	void informSteps();
protected:
	void clearArray(){m_iterationsteps.clear();}
signals:
	void stepsUpdated(QList<int> steps);
	void stepsUpdated(int fn);
private:
	QList<int> m_iterationsteps;
};

#endif // POSTITERATIONSTEPS_H
