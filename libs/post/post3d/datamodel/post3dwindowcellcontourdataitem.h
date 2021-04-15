#ifndef POST3DWINDOWCELLCONTOURDATAITEM_H
#define POST3DWINDOWCELLCONTOURDATAITEM_H

#include "../post3dcellrangesettingcontainer.h"
#include "../post3dwindowdataitem.h"

class vtkActor;
class vtkPolyDataMapper;
class vtkPolyData;

class Post3dWindowCellContourDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowCellContourDataItem(const QString& label, Post3dWindowDataItem* p);
	~Post3dWindowCellContourDataItem();

	const Post3dCellRangeSettingContainer& setting() const;
	void setSetting(const Post3dCellRangeSettingContainer& setting);

	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void innerUpdateZScale(double scale) override;

	void updateActorSettings();
	void updatePolyData();
	void updateColorSetting();

	vtkActor* m_actor;
	vtkPolyDataMapper* m_mapper;
	vtkPolyData* m_polyData;

	Post3dCellRangeSettingContainer m_setting;
};

#endif // POST3DWINDOWCELLCONTOURDATAITEM_H
