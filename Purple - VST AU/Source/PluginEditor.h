/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

    -Purple Effects Plugin - Mark Matamoros Fall 2015
    -This is a guitar (bass and electric) effects plugin with tremolo, two gain stages, and stereo width.
    -Referenced files/sites:
        -++delay.jucer - Tom Erbe
        -stereo width: http://musicdsp.org/showArchiveComment.php?ArchiveID=173
 
  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class MmAssig3AudioProcessorEditor  : public AudioProcessorEditor,
                                      public Timer,
                                      private Slider::Listener
{
public:
    MmAssig3AudioProcessorEditor (MmAssig3AudioProcessor&);
    ~MmAssig3AudioProcessorEditor();
    
    //==============================================================================
    void timerCallback() override;
    MmAssig3AudioProcessor* getProcessor() const
    {return static_cast<MmAssig3AudioProcessor*>(getAudioProcessor());}
    
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MmAssig3AudioProcessor& processor;
        
    //sliders
    Slider gainOne;
    Slider gainTwo;
    Slider rate;
    Slider lfoDepth;
    Slider stereoWidth;
    Slider input;
    Slider output;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MmAssig3AudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
