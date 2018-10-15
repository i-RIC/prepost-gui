#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <guicore/solverdef/solverdefinition.h>
#include <guicore/base/animationcontrollerinterface.h>

#include <QList>
#include <QObject>

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
		Running,
		SlowRunning
	};

public:
	const static int SLIDERWIDTH_MAX = 100;
	/// constructor
	AnimationController(QWidget* parent);
	/// Current setting of followLastStep.
	bool followLastStep() {return m_followLastStep;}
	/// Returns the index of current step
	unsigned int currentStepIndex() {return m_currentStepIndex;}
	/// Returns the label of current index
	const QString& currentStep() {return m_stepList[m_currentStepIndex];}
	/// Animation menu
	QMenu* animationMenu() {return m_animationMenu;}
	/// Animation toolbar
	QToolBar* animationToolBar() {return m_animationToolBar;}
	/// Setup menus, toolbar for the specified iteration type.
	void setup(SolverDefinition::IterationType iType);
	/// Setup animation menu.
	void setupMenu();
	/// Setup animation toolbar.
	void setupToolBar();
	/// Clear Steps data.
	void clearSteps();
	/// Returns the step list.
	const QList<QString>& stepList() {return m_stepList;}

public slots:
	/// Step to next step
	void stepForward();
	/// Step to previous step
	void stepBackward();
	/// Step to the first step
	void stepFirst();
	/// Step to the last step
	void stepLast();
	/// Start animation
	void startAnimation();
	/// Start animation in slowmotion
	void startSlowmotionAnimation();
	/// Stop animation
	void stopAnimation();
	/// Turn on/off the followLastStep flag
	void toggleFollowLastStep();
	/// Turn on/off the followLastStep flag
	void toggleFollowLastStep(bool follow);
	/// Show Dialog to edit slowmotion speed
	void editSlowmotionSpeed();
	/// Update the list of step
	void updateStepList(const QList<QString>& steps);
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
	void setupConnections();
	void disableSteppingActions(bool disable = true);
	void animationStep();
	void updateStartButtonIcon();
	/// Update the time (or iteration) step label.
	void updateStepLabel(const QString& label);
	bool m_followLastStep;

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
	AnimationActions* m_animationActions;
	QMenu* m_animationMenu;
	QToolBar* m_animationToolBar;
	QWidget* m_parent;
	QSlider* m_slider;
	QLabel* m_currentLabel;
	QList<QString> m_stepList;
	QTimer* m_timer;
	unsigned int m_currentStepIndex;
	RunMode m_runMode;
	double m_stepInterval;
	double m_fastInterval;
	double m_slowInterval;

	bool m_timeouted;
	bool m_rendered;
};

#endif // ANIMATIONCONTROLLER_H
