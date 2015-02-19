/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  25 Mar 2011 11:58:07am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_PVMAINCOMPONENT_PVMAINCOMPONENT_DFB0B5DE__
#define __JUCER_HEADER_PVMAINCOMPONENT_PVMAINCOMPONENT_DFB0B5DE__

//[Headers]     -- You can add your own extra header files here --

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioManager.h"


class WaveformDisplay	:	public Component,
							public ChangeListener
{
public:
	WaveformDisplay( AudioFormatManager* const format_manager )
        :	thumbnailCache( 5 ),
			thumbnail( 512, *format_manager, thumbnailCache )
    {
        startTime = endTime = position = 0;
        thumbnail.addChangeListener (this);

		audio_manager = AudioManager::getInstance();
    }

    ~WaveformDisplay()
    {
        thumbnail.removeChangeListener (this);
    }

	void mouseUp( const MouseEvent &e )
	{
		/** When the user releases the mouse button, get the playback position at
			this point and tell the audio manager */
		const double playbackLength		 = audio_manager->getPlaybackLength();
		const double newPlaybackPosition = ((double)e.x / getWidth()) * playbackLength;

		if( newPlaybackPosition >= 0.0 &&
			newPlaybackPosition < playbackLength )
		{
			audio_manager->setPosition( newPlaybackPosition );
		}
	}

	void setFile( const File& file )
    {
        thumbnail.setSource( new FileInputSource( file ) );
        startTime = 0;
		position = 0;
        endTime = thumbnail.getTotalLength();
		repaint();
    }

	void setPosition( double pos )
	{
		position = pos;
		repaint();
	}

	void paint( Graphics& g )
    {
		/** Background */
		g.fillAll( Colours::whitesmoke );

		/** Waveform */
        g.setColour( Colour(0xFF8FBCC7) );

        if( thumbnail.getTotalLength() > 0 )
        {
            thumbnail.drawChannels( g, getLocalBounds().reduced( 2, 2 ),
                                    startTime, endTime, 1.0f );
        }
        else
        {
            g.setFont( 14.0f );
            g.drawFittedText( "(No audio file selected)", 0, 0, getWidth(), getHeight(),
                              Justification::centred, 2 );
        }

		/** Draw position */
		g.setColour( Colours::black );
		float pos = 0;
		if( thumbnail.getTotalLength() > 0 )
		{
			pos = (float)((position / endTime) * (float)getWidth());
			g.drawLine( pos, 0.0f, pos, getHeight(), 1.0f );
		}

		g.drawText( String( position, 2 ) + " s",
					pos + 3, 0, 50, getHeight()-2,
					Justification::bottomLeft, false );

		/** If the user is dragging to select a new position, draw a
			transparent marker to help them see */
		if( isMouseButtonDown() && (thumbnail.getTotalLength() > 0) )
		{
			g.setColour( Colours::black );
			g.setOpacity( 0.25f );

			/** Only interested in the x position */
			const float x = getMouseXYRelative().getX();

			/** Make sure its in range so we don't set the playback to something that,
				doesn't make sense */
			if( x > 0 && x < getWidth() )
			{
				/** Get the position in seconds  */
				const double pos = (float)((x / (float)getWidth()) * endTime );

				g.drawLine( x, 0.0f, x, getHeight(), 1.0f );
				g.drawText( String( pos, 2 ) + " s",
							x + 3, 1, 50, getHeight()-2,
							Justification::topLeft, false );
			}
		}

		/** Draw border */
		g.setOpacity( 1.0f );
		g.setColour( Colours::darkgrey );
		g.drawRect( 0, 0, getWidth(), getHeight(), 2 );
    }

    void changeListenerCallback( ChangeBroadcaster* )
    {
        // this method is called by the thumbnail when it has changed, so we should repaint it..
        repaint();
    }

private:
	AudioThumbnailCache thumbnailCache;
    AudioThumbnail		thumbnail;

    double startTime, endTime, position;

	AudioManager *audio_manager;
};
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PVMainComponent  : public Component,
                         public FilenameComponentListener,
                         public Timer,
                         public ButtonListener,
                         public SliderListener
{
public:
    //==============================================================================
    PVMainComponent ();
    ~PVMainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void filenameComponentChanged( FilenameComponent* );
	void timerCallback();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioManager *audio_manager;

	ScopedPointer <AudioFormatManager> format_manager;
	ScopedPointer <FilenameComponent> fileChooser;
	ScopedPointer <WaveformDisplay> waveform_display;

    //[/UserVariables]

    //==============================================================================
    Label* header_label1;
    GroupComponent* details_group1;
    Label* cpu_label1;
    TextButton* settings_btn1;
    TextButton* play_btn1;
    TextButton* stop_btn1;
    Slider* playback_pos_slider1;
    Label* label;
    Slider* vol_slider1;
    Label* label2;
    Slider* pitch_slider1;
    Label* label3;
    Slider* time_slider1;
    ToggleButton* repeat_btn1;
    ToggleButton* phaselock_btn1;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    PVMainComponent (const PVMainComponent&);
    const PVMainComponent& operator= (const PVMainComponent&);
};


#endif   // __JUCER_HEADER_PVMAINCOMPONENT_PVMAINCOMPONENT_DFB0B5DE__
