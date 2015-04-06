#ifndef POSTALLZONESELECTINGDIALOG_H
#define POSTALLZONESELECTINGDIALOG_H

#include "postbase_global.h"
#include <guicore/postcontainer/postsolutioninfo.h>

#include <QDialog>
#include <QMap>
#include <QList>

class SolverDefinitionGridType;
class PostZoneDataContainer;

namespace Ui {
class PostAllZoneSelectingDialog;
}

class POSTBASEDLL_EXPORT PostAllZoneSelectingDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit PostAllZoneSelectingDialog(QWidget *parent = 0);
	~PostAllZoneSelectingDialog();
	bool setPostSolutionInfo(PostSolutionInfo* info);
	bool needToSelect() const {return m_needToSelect;}
	PostSolutionInfo::Dimension dimension();
	PostZoneDataContainer* container() const;

private slots:
	void setupGridTypeCombobox(int index);
	void setupZoneList(int index);
	
private:
	void setupMaps(const QList<PostZoneDataContainer*>& srcList, QList<SolverDefinitionGridType*>& typeList, QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> >& zoneMap);

	QList<int> m_dimensions;
	QList<SolverDefinitionGridType*> m_gridTypes;
	QList<PostZoneDataContainer*> m_containers;
	QMap<int, QList<SolverDefinitionGridType*> > m_gridTypeMap;
	QMap<int, QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> > > m_zoneMap;

	bool m_needToSelect;
	PostSolutionInfo* m_postSolutionInfo;
	Ui::PostAllZoneSelectingDialog *ui;
};

#endif // POSTALLZONESELECTINGDIALOG_H
