#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "AudioManager.h"

class PhaseVocoderApplication  : public JUCEApplication
{
public:
    PhaseVocoderApplication () = default;
    ~PhaseVocoderApplication () = default;

    void initialise (const String& commandLine)
    {
        audioManager = new AudioManager (commandLine);
        mainWindow = new MainAppWindow (audioManager);
    }

    void shutdown ()
    {
        mainWindow = 0;
        audioManager = 0;
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

    void anotherInstanceStarted (const String& /*commandLine*/)
    {
    }

private:
    ScopedPointer<AudioManager> audioManager;
    ScopedPointer<MainAppWindow> mainWindow;
};

START_JUCE_APPLICATION(PhaseVocoderApplication)
