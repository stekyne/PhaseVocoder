#include "MainWindow.h"
#include "AudioManager.h"
#include "PVMainComponent.h"

//==============================================================================
MainAppWindow::MainAppWindow ()
    : DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                      Colours::lightgrey,
                      DocumentWindow::allButtons)
{
	AudioManager::getInstance();

	setResizeLimits (620, 400, 8192, 8192);
	setResizable (true, false);
	setContentOwned (new PVMainComponent, false);
    centreWithSize (620, 400);
    setVisible (true);
}

MainAppWindow::~MainAppWindow ()
{
	AudioManager::deleteInstance ();
	clearContentComponent ();
}

void MainAppWindow::closeButtonPressed ()
{
    JUCEApplication::getInstance()->systemRequestedQuit ();
}
