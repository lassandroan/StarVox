#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

struct MainContentComponent : juce::AudioAppComponent,
                             juce::MultiTimer,
                            juce::KeyListener
{

    enum TimerIDs
    {
        FRAME_TIMER,
        OUTPUT_TIMER
    };

    MainContentComponent();

    ~MainContentComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;

    bool keyPressed(const juce::KeyPress& key, juce::Component* origin) override;

    void timerCallback(int timerID) override;

    void paint (juce::Graphics& g) override;

    //===========================================================================

    int currentBlockSize;
    int currentSampleRate;

    double pitchTest;
    int pitchTestIndex;

    float RMSTest;

    //===========================================================================

    juce::KeyPress currentKeyPress;

    //===========================================================================

    juce::Rectangle<int> floor;


    juce::Rectangle<int> player;

    float playerVelocity;

};

#endif  // MAINCOMPONENT_H_INCLUDED
