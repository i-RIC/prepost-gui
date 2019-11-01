#include "animationcontroller.h"
#include "../main/iricmainwindow.h"

#include <guibase/irictoolbar.h>
#include <guibase/timeformat/timeformatutil.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/slowmotionspeededitdialog.h>

#include <QAction>
#include <QDateTime>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QSlider>
#include <QTimer>

AnimationController::AnimationController(iRICMainWindow *parent) :
	m_followLastStep {true},
	m_animationActions {new AnimationController::AnimationActions(this)},
	m_animationMenu {nullptr},
	m_animationToolBar {nullptr},
	m_parent {parent},
	m_stepMode {StepMode::Time},
	m_timer {new QTimer(this)},
	m_currentStepIndex {0},
	m_runMode {NotRunning},
	m_stepInterval {0.01}
{
	m_animationActions->actionToggleFollowLastStep->setChecked(m_followLastStep);
	setupConnections();
	setup(SolverDefinition::NoIteration);
	// interval used in fast animation = 10 ms.
	// default interval in slow motion animation = 500 ms.
	m_timer->setSingleShot(true);

	connect(m_timer, SIGNAL(timeout()), this, SLOT(handleTimerTimeout()));
}

bool AnimationController::followLastStep() const
{
	return m_followLastStep;
}

unsigned int AnimationController::currentStepIndex() const
{
	return m_currentStepIndex;
}

QMenu* AnimationController::animationMenu() const
{
	return m_animationMenu;
}

QToolBar* AnimationController::animationToolBar() const
{
	return m_animationToolBar;
}

int AnimationController::stepCount() const
{
	if (m_stepMode == StepMode::Time) {
		return static_cast<int>(m_timeSteps.size());
	} else {
		return static_cast<int>(m_iterationSteps.size());
	}
}

void AnimationController::setStepCount(int count)
{
	// update the slider.
	m_slider->setMinimum(0);
	if (count == 0) {
		m_slider->setMaximum(0);
		setCurrentStepIndex(0);
		m_slider->setDisabled(true);
		return;
	}
	m_slider->setEnabled(true);
	m_slider->setMaximum(count - 1);

	int tickInterval = count / 20;
	if (tickInterval == 0) {tickInterval = 1;}
	m_slider->setTickInterval(tickInterval);

	unsigned int lastIndex;
	lastIndex = static_cast<unsigned int>(count - 1);
	if (m_currentStepIndex > lastIndex) {
		if (m_followLastStep) {
			setCurrentStepIndex(lastIndex);
		} else {
			setCurrentStepIndex(0);
		}
		return;
	}
	if (m_followLastStep && (m_currentStepIndex != lastIndex)) {
		setCurrentStepIndex(lastIndex);
	}
}

void AnimationController::setupConnections()
{
	connect(m_animationActions->actionStepFirst, SIGNAL(triggered()), this, SLOT(stepFirst()));
	connect(m_animationActions->actionStepBackward, SIGNAL(triggered()), this, SLOT(stepBackward()));
	connect(m_animationActions->actionStepForward, SIGNAL(triggered()), this, SLOT(stepForward()));
	connect(m_animationActions->actionStepLast, SIGNAL(triggered()), this, SLOT(stepLast()));
	connect(m_animationActions->actionStartAnimation, SIGNAL(triggered()), this, SLOT(startAnimation()));
	connect(m_animationActions->actionStopAnimation, SIGNAL(triggered()), this, SLOT(stopAnimation()));
	connect(m_animationActions->actionToggleFollowLastStep, SIGNAL(triggered()), this, SLOT(toggleFollowLastStep()));
	connect(m_animationActions->actionEditSpeed, SIGNAL(triggered()), this, SLOT(editSlowmotionSpeed()));
}

void AnimationController::setupToolBar()
{
	m_animationToolBar = new iRICToolBar(tr("Animation ToolBar"), m_parent);
	m_animationToolBar->addAction(m_animationActions->actionStepFirst);
	m_animationToolBar->addAction(m_animationActions->actionStepBackward);
	m_animationToolBar->addAction(m_animationActions->actionStartAnimation);
	m_animationToolBar->addAction(m_animationActions->actionStepForward);
	m_animationToolBar->addAction(m_animationActions->actionStepLast);
	m_animationToolBar->addSeparator();
	m_animationToolBar->addAction(m_animationActions->actionToggleFollowLastStep);
	m_animationToolBar->addAction(m_animationActions->actionEditSpeed);
	m_animationToolBar->addSeparator();

	m_slider = new QSlider(m_animationToolBar);
	m_slider->setOrientation(Qt::Horizontal);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setMaximumWidth(SLIDERWIDTH_MAX);
	m_slider->setTracking(false);
	m_animationToolBar->addWidget(m_slider);
	connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMove(int)));
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(handleSlideValueChange(int)));

	m_currentLabel = new QLabel(m_animationToolBar);
	m_currentLabel->setMargin(4);
	m_animationToolBar->addWidget(m_currentLabel);
	clearSteps();
}

void AnimationController::stepForward()
{
	if (m_currentStepIndex == stepCount() - 1) {return;}
	setCurrentStepIndex(m_currentStepIndex + 1);
}

void AnimationController::stepBackward()
{
	if (m_currentStepIndex == 0) {return;}
	setCurrentStepIndex(m_currentStepIndex - 1);
}

void AnimationController::stepFirst()
{
	if (m_currentStepIndex == 0) {return;}
	setCurrentStepIndex(0);
}

void AnimationController::stepLast()
{
	if (m_currentStepIndex == stepCount() - 1) {return;}
	setCurrentStepIndex(stepCount() - 1);
}

void AnimationController::startAnimation()
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

	// start animation!
	animationStep();
}
void AnimationController::stopAnimation()
{
	m_runMode = NotRunning;

	updateStartButtonIcon();
	disableSteppingActions(false);
}

void AnimationController::toggleFollowLastStep()
{
	toggleFollowLastStep(! m_followLastStep);
}

void AnimationController::toggleFollowLastStep(bool follow)
{
	m_followLastStep = follow;
	m_animationActions->actionToggleFollowLastStep->setChecked(m_followLastStep);
	if (m_followLastStep) {
		unsigned int lastIndex = static_cast<unsigned int>(stepCount() - 1);
		setCurrentStepIndex(lastIndex);
	}
}

void AnimationController::setup(SolverDefinition::IterationType iType)
{
	m_currentStepIndex = 0;
	delete m_animationMenu;
	m_animationMenu = nullptr;

	delete m_animationToolBar;
	m_animationToolBar = nullptr;

	if (iType == SolverDefinition::NoIteration) {
		// no menu, toolbar for animations.
		return;
	}
	if (iType == SolverDefinition::TimeIteration) {
		setupMenu();
		setupToolBar();
		return;
	}
	if (iType == SolverDefinition::ConvergenceIteration) {
		setupMenu();
		setupToolBar();
		return;
	}
}

void AnimationController::setupMenu()
{
	m_animationMenu = new QMenu(tr("&Animation"), m_parent);
	m_animationMenu->addAction(m_animationActions->actionStartAnimation);
	m_animationMenu->addAction(m_animationActions->actionToggleFollowLastStep);
	m_animationMenu->addSeparator();
	m_animationMenu->addAction(m_animationActions->actionStepFirst);
	m_animationMenu->addAction(m_animationActions->actionStepBackward);
	m_animationMenu->addAction(m_animationActions->actionStepForward);
	m_animationMenu->addAction(m_animationActions->actionStepLast);
	m_animationMenu->addSeparator();
	m_animationMenu->addAction(m_animationActions->actionEditSpeed);
}

AnimationController::AnimationActions::AnimationActions(QObject* /* parent*/)
{
	// setup actions
	actionStepBackward = new QAction(AnimationController::tr("Step &Backward"), this);
	actionStepBackward->setIcon(QIcon(":/images/iconAnimationStepBack.png"));

	actionStepForward = new QAction(AnimationController::tr("Step &Forward"), this);
	actionStepForward->setIcon(QIcon(":/images/iconAnimationStepForward.png"));

	actionStepFirst = new QAction(AnimationController::tr("&Reset"), this);
	actionStepFirst->setIcon(QIcon(":/images/iconAnimationReset.png"));

	actionStepLast = new QAction(AnimationController::tr("Go to &Last Step"), this);
	actionStepLast->setIcon(QIcon(":/images/iconAnimationLast.png"));

	actionStartAnimation = new QAction(AnimationController::tr("&Start/Stop Animation"), this);
	actionStartAnimation->setIcon(QIcon(":/images/iconAnimationRun.png"));

	actionStopAnimation = new QAction(AnimationController::tr("S&top Animation"), this);
	actionStopAnimation->setIcon(QIcon(":/images/iconAnimationStop.png"));

	actionToggleFollowLastStep = new QAction(AnimationController::tr("Toggle &following last step"), this);
	actionToggleFollowLastStep->setIcon(QIcon(":/images/iconAnimationFollowLastStep.png"));
	actionToggleFollowLastStep->setCheckable(true);

	actionEditSpeed = new QAction(AnimationController::tr("&Animation Speed Setting..."), this);
	actionEditSpeed->setIcon(QIcon(":/images/iconAnimationSpeedEdit.png"));
}

void AnimationController::updateTimeSteps(const QList<double>& steps)
{
	m_stepMode = StepMode::Time;
	m_timeSteps = steps;
	setStepCount(steps.size());
}

void AnimationController::updateIterationSteps(const QList<int>& steps)
{
	m_stepMode = StepMode::Iteration;
	m_iterationSteps = steps;
	setStepCount(steps.size());
}

void AnimationController::setCurrentStepIndex(unsigned int i)
{
	m_currentStepIndex = i;
	m_slider->setValue(i);
	if (stepCount() == 0) {
		m_currentLabel->setText("");
	} else {
		updateStepLabel(m_currentStepIndex);
	}
}

void AnimationController::updateStepLabel(int step)
{
	if (m_stepMode == StepMode::Time) {
		if (step >= m_timeSteps.size()) {
			m_currentLabel->setText("---");
			return;
		}
		auto mainFile = m_parent->projectData()->mainfile();
		double time = m_timeSteps.at(step);
		auto zt = mainFile->zeroDateTime();
		auto f = mainFile->timeFormat();
		auto cf = mainFile->customTimeFormat();
		m_currentLabel->setText(TimeFormatUtil::formattedString(zt, time, f, cf));
	} else {
		if (step >= m_iterationSteps.size()) {
			m_currentLabel->setText("---");
			return;
		}
		QString labelStr("i = %1");
		m_currentLabel->setText(labelStr.arg(m_iterationSteps.at(step)));
	}
}

void AnimationController::handleSliderMove(int val)
{
	if (val < stepCount()) {
		updateStepLabel(val);
	} else {
		m_currentLabel->setText("");
	}
}

void AnimationController::handleSliderRelease()
{
	m_currentStepIndex = m_slider->value();
	emit indexChanged(m_currentStepIndex);
}

void AnimationController::handleSlideValueChange(int val)
{
	handleSliderMove(val);
	handleSliderRelease();
}

void AnimationController::disableSteppingActions(bool disable)
{
	m_animationActions->actionStepFirst->setDisabled(disable);
	m_animationActions->actionStepBackward->setDisabled(disable);
	m_animationActions->actionStepForward->setDisabled(disable);
	m_animationActions->actionStepLast->setDisabled(disable);
}

void AnimationController::handleRenderingEnded()
{
	m_rendered = true;
	if (m_timeouted) {
		// rendering consumed more time then the timer
		// interval.
		// wait just a little more.
		m_timer->setInterval(10);
		m_timer->start();
	}
}

void AnimationController::handleTimerTimeout()
{
	// timer is timed out.
	m_timeouted = true;
	if (m_rendered) {
		// time outed, and rendering of all post processors
		// are finished too. then, step next.
		animationStep();
	}
}

void AnimationController::animationStep()
{
	// if the user stopped running, finish.
	if (m_runMode == NotRunning) {return;}
	// if it reached the last step, stop running.
	if (m_currentStepIndex == stepCount() - 1) {
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

void AnimationController::updateStartButtonIcon()
{
	QIcon icon;
	if (m_runMode == NotRunning) {
		icon = QIcon(":/images/iconAnimationRun.png");
	} else if (m_runMode == Running) {
		icon = QIcon(":/images/iconAnimationStop.png");
	}
	m_animationActions->actionStartAnimation->setIcon(icon);
}

void AnimationController::editSlowmotionSpeed()
{
	SlowmotionSpeedEditDialog dialog(m_parent);
	dialog.setInterval(m_stepInterval);

	if (QDialog::Accepted == dialog.exec()) {
		m_stepInterval = dialog.interval();
	}
}

void AnimationController::clearSteps()
{
	m_slider->setMinimum(0);
	m_slider->setMaximum(0);
	m_slider->setDisabled(true);
	m_currentLabel->setText("");
}

void AnimationController::updateLabelAndPostWindows()
{
	updateStepLabel(m_currentStepIndex);
	emit indexChanged(m_currentStepIndex);
}
