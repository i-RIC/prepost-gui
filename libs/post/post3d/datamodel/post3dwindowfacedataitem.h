#ifndef POST3DWINDOWFACEDATAITEM_H
#define POST3DWINDOWFACEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

class vtkActor;
class vtkAlgorithmOutput;
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
	/// Constructor
	Post3dWindowFaceDataItem(const QString& label, GraphicsWindowDataItem* parent);
	~Post3dWindowFaceDataItem();
	Setting setting();
	void setSetting(Setting setting, bool draw);
	bool enabled() const {return m_enabled;}
	void setEnabled(bool e) {m_enabled = e;}
	Direction direction() const {return m_direction;}
	void setDirection(Direction dir) {m_direction = dir;}
	unsigned int iMin() const {return m_iMin;}
	unsigned int iMax() const {return m_iMax;}
	unsigned int jMin() const {return m_jMin;}
	unsigned int jMax() const {return m_jMax;}
	unsigned int kMin() const {return m_kMin;}
	unsigned int kMax() const {return m_kMax;}
	void setIMin(unsigned int val) {m_iMin = val;}
	void setIMax(unsigned int val) {m_iMax = val;}
	void setJMin(unsigned int val) {m_jMin = val;}
	void setJMax(unsigned int val) {m_jMax = val;}
	void setKMin(unsigned int val) {m_kMin = val;}
	void setKMax(unsigned int val) {m_kMax = val;}
	void setActor(vtkActor* actor);
	vtkAlgorithmOutput* getOutputPort();
	void handleStandardItemChange() override;
	void innerUpdateZScale(double scale) override;
	vtkExtractGrid* filter() const {return m_filter;}
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
