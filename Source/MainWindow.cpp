#include "MainWindow.h"
#include "AudioManager.h"
#include "PVMainComponent.h"

//==============================================================================
MainAppWindow::MainAppWindow (AudioManager& audioManager)
    : DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                      Colours::lightgrey,
                      DocumentWindow::allButtons)
{
	setResizeLimits (620, 400, 8192, 8192);
	setResizable (true, false);
    setContentOwned (new PVMainComponent (audioManager), false);
    centreWithSize (620, 400);
    setVisible (true);
}

MainAppWindow::~MainAppWindow ()
{
	clearContentComponent ();
}

void MainAppWindow::closeButtonPressed ()
{
    JUCEApplication::getInstance()->systemRequestedQuit ();
}
