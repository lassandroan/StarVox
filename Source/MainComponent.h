#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

struct MainContentComponent : juce::AudioAppComponent,
                             juce::MultiTimer,
                            juce::KeyListener
{

    enum TimerIds
    {
        Frame,
        Pitch
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

    int pitchCurrent;

    int pitchControl;

    float RMSTest;

    //===========================================================================

    juce::Random rand;

    juce::LinearSmoothedValue<float> interpolation;

    juce::Array<juce::Point<int>> stars;

    juce::Rectangle<int> coin;

    juce::Rectangle<int> player;

    int score;

};

#endif  // MAINCOMPONENT_H_INCLUDED
