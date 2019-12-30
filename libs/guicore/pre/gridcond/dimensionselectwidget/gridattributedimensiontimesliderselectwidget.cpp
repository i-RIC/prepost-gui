#include "../base/gridattributedimensioncontainer.h"
#include "gridattributedimensiontimesliderselectwidget.h"

#include <guibase/irictoolbar.h>
#include <misc/slowmotionspeededitdialog.h>

#include <QAction>
#include <QDateTime>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QSlider>
#include <QTextStream>
#include <QTimer>
#include <QTimeZone>
#include <QVariant>

GridAttributeDimensionTimeSliderSelectWidget::GridAttributeDimensionTimeSliderSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent) :
	GridAttributeDimensionSelectWidget(container, parent)
{
	m_animationActions = new GridAttributeDimensionTimeSliderSelectWidget::AnimationActions(this);
	m_runMode = NotRunning;
	m_slowInterval = 0.01;
	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_isTime = (container->name() == "Time");

	connect(m_animationActions->actionStepFirst, SIGNAL(triggered()), this, SLOT(stepFirst()));
	connect(m_animationActions->actionStepBackward, SIGNAL(triggered()), this, SLOT(stepBackward()));
	connect(m_animationActions->actionStepForward, SIGNAL(triggered()), this, SLOT(stepForward()));
	connect(m_animationActions->actionStepLast, SIGNAL(triggered()), this, SLOT(stepLast()));
	connect(m_animationActions->actionStartAnimation, SIGNAL(triggered()), this, SLOT(startSlowmotionAnimation()));
	connect(m_animationActions->actionEditSpeed, SIGNAL(triggered()), this, SLOT(editSlowmotionSpeed()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(handleTimerTimeout()));

	iRICToolBar* toolbar = new iRICToolBar(this);
	m_titleLabel = new QLabel(toolbar);
	m_titleLabel->setText(container->caption());
	toolbar->addWidget(m_titleLabel);

	toolbar->addAction(m_animationActions->actionStepFirst);
	toolbar->addAction(m_animationActions->actionStepBackward);
	toolbar->addAction(m_animationActions->actionStartAnimation);
	toolbar->addAction(m_animationActions->actionStepForward);
	toolbar->addAction(m_animationActions->actionStepLast);
	toolbar->addSeparator();
	toolbar->addAction(m_animationActions->actionEditSpeed);
	toolbar->addSeparator();

	m_slider = new QSlider(toolbar);
	m_slider->setOrientation(Qt::Horizontal);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setMaximumWidth(SLIDERWIDTH_MAX);
	m_slider->setTracking(false);
	toolbar->addWidget(m_slider);

	connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMove(int)));
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(handleSlideValueChange(int)));

	m_currentLabel = new QLabel(toolbar);
	m_currentLabel->setMargin(4);
	m_currentLabel->setText("2015/1/1");
	toolbar->addWidget(m_currentLabel);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(toolbar);
	setLayout(layout);

	setBackgroundRole(QPalette::ButtonText);
	setMaximumWidth(450);
	applyValues();
}

GridAttributeDimensionTimeSliderSelectWidget::~GridAttributeDimensionTimeSliderSelectWidget()
{

}

void GridAttributeDimensionTimeSliderSelectWidget::doApplyValues()
{
	m_slider->setEnabled(true);
	m_slider->setMinimum(0);
	if (container()->count() == 0) {
		m_slider->setMaximum(0);
		m_slider->setEnabled(false);
	}
	m_slider->setMaximum(container()->count() - 1);
	setCurrentIndex(container()->currentIndex());
}

QString GridAttributeDimensionTimeSliderSelectWidget::stepLabel(int index) const
{
	QVariant value = container()->variantValue(index);
	if (m_isTime) {
		double dblDatetime = value.toDouble();
		int intDateTime = static_cast<int>(dblDatetime);
		QDateTime dateTime = QDateTime::fromTime_t(intDateTime, Qt::UTC);
		int msec = static_cast<int>((dblDatetime - intDateTime) * 1000);

		QString ret;
		QTextStream out(&ret);
		out << dateTime.toString("yyyy-MM-dd HH:mm:ss");
		if (msec != 0) {
			out << ".";
			out.setFieldWidth(3);
			out.setFieldAlignment(QTextStream::AlignRight);
			out.setPadChar('0');
			out << msec;
		}
		return ret;
	} else {
		return value.toString();
	}
}

QString GridAttributeDimensionTimeSliderSelectWidget::currentStepLabel() const
{
	if (container()->count() == 0) {return "";}
	return stepLabel(currentStepIndex());
}

void GridAttributeDimensionTimeSliderSelectWidget::stepForward()
{
	if (container()->count() == 0) {return;}
	if (container()->currentIndex() == container()->count() - 1) { return; }
	setCurrentIndex(container()->currentIndex() + 1);
}

void GridAttributeDimensionTimeSliderSelectWidget::stepBackward()
{
	if (container()->currentIndex() == 0) { return; }
	setCurrentIndex(container()->currentIndex() - 1);
}

void GridAttributeDimensionTimeSliderSelectWidget::stepFirst()
{
	if (container()->currentIndex() == 0) { return; }
	setCurrentIndex(0);
}

void GridAttributeDimensionTimeSliderSelectWidget::stepLast()
{
	if (container()->count() == 0) { return; }
	if (container()->currentIndex() == container()->count() - 1) { return; }
	setCurrentIndex(container()->count() - 1);
}

void GridAttributeDimensionTimeSliderSelectWidget::startSlowmotionAnimation()
{
	if (m_runMode != NotRunning) {
		// It is already running.
		// user wants to stop running.
		stopAnimation();
		return;
	}
	// stop animation first.
	stopAnimation();
	m_runMode = Running;
	updateStartButtonIcon();
	disableSteppingActions();

	// set the interval to the slow value.
	m_stepInterval = m_slowInterval;
	// start animation!
	animationStep();
}

void GridAttributeDimensionTimeSliderSelectWidget::stopAnimation()
{
	m_runMode = NotRunning;

	updateStartButtonIcon();
	disableSteppingActions(false);
}

void GridAttributeDimensionTimeSliderSelectWidget::editSlowmotionSpeed()
{
	SlowmotionSpeedEditDialog dialog(this);
	dialog.setInterval(m_slowInterval);
	if (QDialog::Accepted == dialog.exec()) {
		m_slowInterval = dialog.interval();
	}
}

void GridAttributeDimensionTimeSliderSelectWidget::doSetCurrentIndex(int index)
{
	if (container()->currentIndex() != index) {
		container()->setCurrentIndex(index);
	}

	m_slider->setValue(index);
	if (container()->count() == 0) {
		updateStepLabel("");
	} else {
		updateStepLabel(currentStepLabel());
	}
	m_rendered = true;
	if (m_runMode == Running && m_timeouted) {
		// rendering consumed more time then the timer
		// interval.
		// wait just a little more.
		m_timer->setInterval(10);
		m_timer->start();
	}
}

void GridAttributeDimensionTimeSliderSelectWidget::updateStepLabel(const QString& label)
{
	m_currentLabel->setText(label);
}

void GridAttributeDimensionTimeSliderSelectWidget::handleSliderMove(int val)
{
	if (val < container()->count()) {
		updateStepLabel(stepLabel(val));
	} else {
		updateStepLabel("");
	}
}

void GridAttributeDimensionTimeSliderSelectWidget::handleSliderRelease()
{
	container()->setCurrentIndex(m_slider->value());
}

void GridAttributeDimensionTimeSliderSelectWidget::handleSlideValueChange(int val)
{
	handleSliderMove(val);
	handleSliderRelease();
}

void GridAttributeDimensionTimeSliderSelectWidget::disableSteppingActions(bool disable)
{
	m_animationActions->actionStepFirst->setDisabled(disable);
	m_animationActions->actionStepBackward->setDisabled(disable);
	m_animationActions->actionStepForward->setDisabled(disable);
	m_animationActions->actionStepLast->setDisabled(disable);
}

void GridAttributeDimensionTimeSliderSelectWidget::updateStartButtonIcon()
{
	QIcon icon;
	if (m_runMode == NotRunning) {
		icon = QIcon(":/images/iconAnimationRun.png");
	} else if (m_runMode == Running) {
		icon = QIcon(":/images/iconAnimationStop.png");
	}
	m_animationActions->actionStartAnimation->setIcon(icon);
}

void GridAttributeDimensionTimeSliderSelectWidget::handleTimerTimeout()
{
	// timer is timed out.
	m_timeouted = true;
	if (m_rendered) {
		// time outed, and rendering are finished too. then, step next.
		animationStep();
	}
}

void GridAttributeDimensionTimeSliderSelectWidget::animationStep()
{
	// if the user stopped running, finish.
	if (m_runMode == NotRunning) {return;}
	// if it reached the last step, stop running.
	if (container()->currentIndex() == container()->count() - 1) {
		stopAnimation();
		return;
	}
	// change flag values.
	m_timeouted = false;
	m_rendered = false;
	// set up timer.
	m_timer->setInterval(static_cast<int>(1000 * m_stepInterval));
	// start timer
	m_timer->start();
	// start rendering.
	stepForward();
}

int GridAttributeDimensionTimeSliderSelectWidget::currentStepIndex() const
{
	return container()->currentIndex();
}

GridAttributeDimensionTimeSliderSelectWidget::AnimationActions::AnimationActions(QObject* /* parent*/)
{
	// setup actions
	actionStepBackward = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("Step &Backward"), this);
	actionStepBackward->setIcon(QIcon(":/images/iconAnimationStepBack.png"));

	actionStepForward = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("Step &Forward"), this);
	actionStepForward->setIcon(QIcon(":/images/iconAnimationStepForward.png"));

	actionStepFirst = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("&Reset"), this);
	actionStepFirst->setIcon(QIcon(":/images/iconAnimationReset.png"));

	actionStepLast = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("Go to &Last Step"), this);
	actionStepLast->setIcon(QIcon(":/images/iconAnimationLast.png"));

	actionStartAnimation = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("&Start/Stop Animation"), this);
	actionStartAnimation->setIcon(QIcon(":/images/iconAnimationRun.png"));

	actionStopAnimation = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("S&top Animation"), this);
	actionStopAnimation->setIcon(QIcon(":/images/iconAnimationStop.png"));

	actionEditSpeed = new QAction(GridAttributeDimensionTimeSliderSelectWidget::tr("&Animation Speed Setting..."), this);
	actionEditSpeed->setIcon(QIcon(":/images/iconAnimationSpeedEdit.png"));
}
