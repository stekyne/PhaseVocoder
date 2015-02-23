#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"

class PhaseVocoderApplication  : public JUCEApplication
{
public:
    PhaseVocoderApplication () {}

    ~PhaseVocoderApplication () {}

    void initialise (const String& commandLine)
    {
        mainWindow = new MainAppWindow;
    }

    void shutdown ()
    {
        mainWindow = 0;
    }

    void systemRequestedQuit ()
    {
        quit ();
    }

    const String getApplicationName ()
    {
        return "PhaseVocoder";
    }

    const String getApplicationVersion ()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed ()
    {
        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
    }

private:
    ScopedPointer<MainAppWindow> mainWindow;
};

START_JUCE_APPLICATION(PhaseVocoderApplication)
