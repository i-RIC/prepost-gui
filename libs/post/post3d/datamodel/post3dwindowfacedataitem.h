#ifndef POST3DWINDOWFACEDATAITEM_H
#define POST3DWINDOWFACEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

class vtkActor;
class vtkAlgorithmOutput;
class vtkPolyData;
class Post3dWindowFaceDataSetSetting;

class Post3dWindowFaceDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	enum Direction {dirI, dirJ, dirK};
	class Setting
	{

	public:
		bool enabled;
		Post3dWindowFaceDataItem::Direction direction;
		unsigned int iMin;
		unsigned int iMax;
		unsigned int jMin;
		unsigned int jMax;
		unsigned int kMin;
		unsigned int kMax;
	};
	Post3dWindowFaceDataItem(const QString& label, GraphicsWindowDataItem* parent);
	~Post3dWindowFaceDataItem();

	Setting setting();
	void setSetting(Setting setting, bool draw);

	bool enabled() const;
	void setEnabled(bool e);

	Direction direction() const;
	void setDirection(Direction dir);

	unsigned int iMin() const;
	unsigned int iMax() const;
	unsigned int jMin() const;
	unsigned int jMax() const;
	unsigned int kMin() const;
	unsigned int kMax() const;
	void setIMin(unsigned int val);
	void setIMax(unsigned int val);
	void setJMin(unsigned int val);
	void setJMax(unsigned int val);
	void setKMin(unsigned int val);
	void setKMax(unsigned int val);

	void setActor(vtkActor* actor);
	vtkPolyData* getOutputPolyData();
	vtkAlgorithmOutput* getOutputPort();
	void handleStandardItemChange() override;
	void innerUpdateZScale(double scale) override;

	vtkExtractGrid* filter() const;
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateFilter();
	Post3dWindowZoneDataItem* getZoneDataItem();

	bool m_enabled;
	bool m_dataOK;

	unsigned int m_iMin;
	unsigned int m_iMax;
	unsigned int m_jMin;
	unsigned int m_jMax;
	unsigned int m_kMin;
	unsigned int m_kMax;
	Direction m_direction;
	vtkActor* m_actor;
	vtkSmartPointer<vtkExtractGrid> m_filter;
	friend class Post3dWindowFaceDataSetSetting;
};

#endif // POST3DWINDOWFACEDATAITEM_H
