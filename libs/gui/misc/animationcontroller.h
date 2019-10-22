#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <guicore/solverdef/solverdefinition.h>
#include <guicore/base/animationcontrollerinterface.h>

#include <QList>
#include <QObject>

class iRICMainWindow;

class QAction;
class QLabel;
class QMenu;
class QSlider;
class QTimer;
class QToolBar;
class QWidget;

/// AnimationContoller controls which timestep (or iteration process) to
/// display on post-processing windows.
class AnimationController : public AnimationControllerInterface
{
	Q_OBJECT

private:
	enum RunMode {
		NotRunning,
		Running
	};
	enum StepMode {
		Time,
		Iteration,
	};

public:
	const static int SLIDERWIDTH_MAX = 100;
	/// constructor
	AnimationController(iRICMainWindow* parent);
	/// Current setting of followLastStep.
	bool followLastStep() const;
	/// Returns the index of current step
	unsigned int currentStepIndex() const;
	/// Animation menu
	QMenu* animationMenu() const;
	/// Animation toolbar
	QToolBar* animationToolBar() const;
	/// Setup menus, toolbar for the specified iteration type.
	void setup(SolverDefinition::IterationType iType);
	/// Setup animation menu.
	void setupMenu();
	/// Setup animation toolbar.
	void setupToolBar();
	/// Clear Steps data.
	void clearSteps();

	void updateLabelAndPostWindows();

public slots:
	/// Step to next step
	void stepForward();
	/// Step to previous step
	void stepBackward();
	/// Step to the first step
	void stepFirst();
	/// Step to the last step
	void stepLast();
	/// Start animation in slowmotion
	void startAnimation();
	/// Stop animation
	void stopAnimation();
	/// Turn on/off the followLastStep flag
	void toggleFollowLastStep();
	/// Turn on/off the followLastStep flag
	void toggleFollowLastStep(bool follow);
	/// Show Dialog to edit slowmotion speed
	void editSlowmotionSpeed();
	/// Update the list of step
	void updateTimeSteps(const QList<double>& steps);
	void updateIterationSteps(const QList<int>& steps);
	void setCurrentStepIndex(unsigned int i) override;
	void handleRenderingEnded();

private slots:
	void handleSliderMove(int val);
	void handleSliderRelease();
	void handleSlideValueChange(int val);
	void handleTimerTimeout();

signals:
	void indexChanged(unsigned int index);

private:
	int stepCount() const;
	void setStepCount(int count);
	void setupConnections();
	void disableSteppingActions(bool disable = true);
	void animationStep();
	void updateStartButtonIcon();
	/// Update the time (or iteration) step label.
	void updateStepLabel(int step);

public:
	/// Menu that has actions related to animation control
	class AnimationActions : public QObject
	{
	public:
		/// Constructor
		AnimationActions(QObject* parent);
		QAction* actionStepBackward;
		QAction* actionStepForward;
		QAction* actionStepFirst;
		QAction* actionStepLast;
		QAction* actionStartAnimation;
		QAction* actionStopAnimation;
		QAction* actionEditSpeed;
		QAction* actionToggleFollowLastStep;
	};

private:
	bool m_followLastStep;
	AnimationActions* m_animationActions;
	QMenu* m_animationMenu;
	QToolBar* m_animationToolBar;
	iRICMainWindow* m_parent;
	QSlider* m_slider;
	QLabel* m_currentLabel;
	StepMode m_stepMode;
	QList<double> m_timeSteps;
	QList<int> m_iterationSteps;
	QTimer* m_timer;
	unsigned int m_currentStepIndex;
	RunMode m_runMode;
	double m_stepInterval;

	bool m_timeouted;
	bool m_rendered;
};

#endif // ANIMATIONCONTROLLER_H
