#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcellcontourdataitem.h"
#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcellcontourgroupdataitem_settingeditwidget.h"

#include <guibase/objectbrowserview.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectdefaultcolormapsettings.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/qscreenutil.h>
#include <misc/stringtool.h>
#include <misc/valuechangert.h>

#include <vtkActor2D.h>

Post3dWindowCellContourGroupDataItem::Post3dWindowCellContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_target {target},
	m_colorMapSetting {nullptr},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor2D(m_legendActor);

	auto gType = data()->gridType();
	SolverDefinitionGridOutput* output = nullptr;
	QRegExp re("^(.+) \\(magnitude\\)$");
	auto pos = re.indexIn(target.c_str());
	if (pos >= 0) {
		auto target2 = iRIC::toStr(re.cap(1)) + "X";
		output = gType->output(target2);
	} else {
		output = gType->output(target);
	}

	ColorMapSettingContainerI* cs = nullptr;
	auto defaultCs = projectData()->mainfile()->defaultColorMapSettings()->colorMap(target);
	if (defaultCs != nullptr) {
		cs = defaultCs->copy();
	} else {
		cs = output->createColorMapSettingContainer();
	}

	m_colorMapSetting->legendSetting()->imgSetting()->setActor(m_legendActor);
	m_colorMapSetting->legendSetting()->setTitle(data()->gridType()->outputCaption(target));
	m_colorMapSetting->setAutoValueRange(valueRange());

	m_standardItem->setText(gType->outputCaption(m_target));

	m_colorMapToolBarWidget->hide();
	m_colorMapToolBarWidget->setSetting(m_colorMapSetting);
	connect(m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(m_colorMapToolBarWidget->modifiedSetting(), m_colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	informSelection(dataModel()->graphicsView());
}

Post3dWindowCellContourGroupDataItem::~Post3dWindowCellContourGroupDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
	delete m_colorMapSetting;
}

void Post3dWindowCellContourGroupDataItem::update()
{
	updateActorSetting();
}

void Post3dWindowCellContourGroupDataItem::updateColorMapVisibility()
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);

	auto actor = m_colorMapSetting->legendSetting()->imgSetting()->actor();
	m_actor2DCollection->RemoveItem(actor);
	actor->VisibilityOff();

	bool visible = false;
	if (standardItem()->checkState() == Qt::Checked) {
		for (auto child : m_childItems) {
			visible = visible || child->standardItem()->checkState() == Qt::Checked;
		}
	}
	if (visible) {
		auto v = dataModel()->graphicsView();
		m_colorMapSetting->legendSetting()->imgSetting()->apply(v);
		m_actor2DCollection->AddItem(actor);
	}
}

const std::string& Post3dWindowCellContourGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowCellContourGroupDataItem::gatherActiveColorMapLegends(std::vector<ColorMapLegendSettingContainerI*>* legends)
{
	bool visible = false;
	if (standardItem()->checkState() == Qt::Checked) {
		for (auto child : m_childItems) {
			visible = visible || child->standardItem()->checkState() == Qt::Checked;
		}
	}
	if (! visible) {return;}

	legends->push_back(m_colorMapSetting->legendSetting());
}

void Post3dWindowCellContourGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowCellContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting->load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "RangeSetting") {
			QString label = childElem.attribute("label");
			auto item = new Post3dWindowCellContourDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			connect(item, &Post3dWindowCellContourDataItem::destroyed, this, &Post3dWindowCellContourGroupDataItem::updateActorSetting);

			m_childItems.push_back(item);
		}
	}

	updateActorSetting();
}

void Post3dWindowCellContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting->save(writer);

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowCellContourDataItem*>(child);
		writer.writeStartElement("RangeSetting");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowCellContourGroupDataItem::propertyDialog(QWidget* p)
{
	if (data() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Contour Setting (cell center) (%1)").arg(standardItem()->text()));
	dialog->resize(QScreenUtil::sizeWithinScreen(ColorMapSettingEditWidgetI::standardDialogSize(100)));

	return dialog;
}

void Post3dWindowCellContourGroupDataItem::doUpdateActorSetting()
{
	m_colorMapSetting->setAutoValueRange(valueRange());
	m_colorMapSetting->legendSetting()->imgSetting()->apply(dataModel()->graphicsView());

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowCellContourDataItem*> (child);
		item->update();
	}
	updateColorMapVisibility();
}

void Post3dWindowCellContourGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);
	updateColorMapVisibility();
}

const ValueRangeContainer& Post3dWindowCellContourGroupDataItem::valueRange() const
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->cellValueRange(m_target);
}

Post3dWindowZoneDataItem* Post3dWindowCellContourGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());
}

v4PostZoneDataContainer* Post3dWindowCellContourGroupDataItem::data() const
{
	return zoneDataItem()->v4DataContainer();
}

std::vector<Post3dWindowCellRangeSettingContainer> Post3dWindowCellContourGroupDataItem::ranges() const
{
	std::vector<Post3dWindowCellRangeSettingContainer> ret;

	for (auto item : m_childItems) {
		auto rangeItem = dynamic_cast<Post3dWindowCellContourDataItem*> (item);
		ret.push_back(rangeItem->setting());
	}

	return ret;
}

void Post3dWindowCellContourGroupDataItem::setRanges(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges)
{
	clearChildItemsAndUpdateItemMap();

	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (parent());
	int idx = 1;
	for (const auto& range : ranges) {
		auto label = tr("Range%1").arg(idx);
		auto item = new Post3dWindowCellContourDataItem(label, this);
		item->setSetting(range);
		item->updateZScale(tItem->zScale());
		connect(item, &Post3dWindowCellContourDataItem::destroyed, this, &Post3dWindowCellContourGroupDataItem::updateActorSetting);

		Qt::CheckState cs = Qt::Unchecked;
		if (range.enabled) {
			cs = Qt::Checked;
		}
		item->standardItem()->setCheckState(cs);

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowCellContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(this, event, v);
}

void Post3dWindowCellContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting->legendSetting()->imgSetting()->controller()->handleMousePressEvent(this, event, v);
}

void Post3dWindowCellContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(this, event, v);
}

void Post3dWindowCellContourGroupDataItem::handleStandardItemChange()
{
	updateActorSetting();
	Post3dWindowDataItem::handleStandardItemChange();
}

void Post3dWindowCellContourGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

bool Post3dWindowCellContourGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_colorMapToolBarWidget->setParent(toolBar);
	m_colorMapToolBarWidget->show();

	toolBar->addWidget(m_colorMapToolBarWidget);
	return true;
}
