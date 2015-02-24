#ifndef WAVEFORMDISPLAYCOMPONENT_H_INCLUDED
#define WAVEFORMDISPLAYCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioManager.h"

class WaveformDisplay   :   public Component,
                            public ChangeListener
{
public:
    WaveformDisplay (AudioFormatManager* const format_manager)
        :   thumbnailCache (5),
            thumbnail (512, *format_manager, thumbnailCache)
    {
        startTime = endTime = position = 0;
        thumbnail.addChangeListener (this);

        audio_manager = AudioManager::getInstance ();
    }

    ~WaveformDisplay ()
    {
        thumbnail.removeChangeListener (this);
    }

    void mouseUp (const MouseEvent &e)
    {
        const double playbackLength = audio_manager->getPlaybackLength ();
        const double newPlaybackPosition = (e.x / (float)getWidth ()) * playbackLength;

        if (newPlaybackPosition >= 0.0 &&
            newPlaybackPosition < playbackLength)
        {
            audio_manager->setPosition (newPlaybackPosition);
        }
    }

    void setFile (const File& file)
    {
        thumbnail.setSource (new FileInputSource (file));
        startTime = 0;
        position = 0;
        endTime = thumbnail.getTotalLength ();
        repaint ();
    }

    void setPosition (double pos)
    {
        position = pos;
        repaint ();
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::whitesmoke);
        g.setColour (Colour (0xFF8FBCC7));

        if (thumbnail.getTotalLength () > 0)
        {
            thumbnail.drawChannels (g, getLocalBounds ().reduced (2, 2),
                                    startTime, endTime, 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No audio file selected)", 0, 0, getWidth (), getHeight (),
                              Justification::centred, 2);
        }

        // Draw position
        g.setColour (Colours::black);
        float pos = 0;
        if (thumbnail.getTotalLength () > 0)
        {
            pos = (float)((position / endTime) * (float)getWidth ());
            g.drawLine (pos, 0.0f, pos, (float)getHeight (), 1.0f);
        }

        g.drawText (String (position, 2) + " s",
                    (int)pos + 3, 0, 50, getHeight () - 2,
                    Justification::bottomLeft, false);

        // If the user is dragging to select a new position, draw a
        // transparent marker to help them see
        if (isMouseButtonDown () && (thumbnail.getTotalLength () > 0))
        {
            g.setColour (Colours::black);
            g.setOpacity (0.25f);

            // Only interested in the x position
            const auto x = getMouseXYRelative ().getX ();

            // Make sure its in range so we don't set the playback to something that,
            // doesn't make sense
            if (x > 0 && x < getWidth ())
            {
                const double pos = (float)((x / (float)getWidth ()) * endTime);

                g.drawLine (x, 0, x, getHeight (), 1.0f);
                g.drawText (String (pos, 2) + " s",
                            x + 3, 1, 50, getHeight () - 2,
                            Justification::topLeft, false);
            }
        }

        g.setOpacity (1.0f);
        g.setColour (Colours::darkgrey);
        g.drawRect (0, 0, getWidth (), getHeight (), 2);
    }

    void changeListenerCallback (ChangeBroadcaster*)
    {
        repaint ();
    }

private:
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    double startTime, endTime, position;
    AudioManager *audio_manager;
};

#endif
