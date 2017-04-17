#include "MainComponent.h"

#include <math.h>

#include "../Lib/fftsg_h.c"

MainContentComponent::MainContentComponent()
{
    setSize (768, 768);
    setAudioChannels (1, 0);

    addKeyListener(this);
    setWantsKeyboardFocus(true);

    coin.setBounds(0, 0, 16, 16);
    player.setBounds(25, 384-32, 32, 32);

    playerVelocity = 0.0f;

    pitchTest = 0;

    score = 0;

    interpolation.reset(300, 0.25);

}

MainContentComponent::~MainContentComponent()
{
    shutdownAudio();
}

void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    startTimer(FRAME_TIMER, 1000/60);
    startTimer(OUTPUT_TIMER, 250);

    currentBlockSize = samplesPerBlockExpected;
    currentSampleRate = sampleRate;

}

void MainContentComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{

    RMSTest = bufferToFill.buffer->getRMSLevel(0, 0, currentBlockSize);

    double signal[currentBlockSize * 2];

    for (int i = 0; i < currentBlockSize; ++i)
    {
        double window = 0.50 * (1 - cos((2 * M_PI * i) / (currentBlockSize  - 1)));
        double sample  = (double)bufferToFill.buffer->getSample(0, i);

        signal[i] = window * sample;
        signal[i + currentBlockSize] = 0.0;
    }

    rdft(currentBlockSize * 2, 1, signal);

    double spectrum[currentBlockSize / 2];
    for (int i = 0; i < (currentBlockSize / 2) - 1; ++i)
    {
        double re = signal[2 * i];
        double im = signal[2 * i + 1];
        spectrum[i] = sqrt(re * re + im * im);
    }

    int numHarmonics = 5;
    int minIndex = 1;
    int maxIndex = (currentBlockSize / 2) / numHarmonics;
    int maxLocation = minIndex;

    for (int j = minIndex; j <= maxIndex; ++j)
    {
        for (int i = 1; i <= numHarmonics; ++i)
        {
            spectrum[j] *= spectrum[j * i];
        }
        if (spectrum[j] > spectrum[maxLocation])
        {
            maxLocation = j;
        }
    }

    pitchTestIndex = (maxLocation * currentSampleRate / currentBlockSize) / 2;

}

void MainContentComponent::releaseResources()
{

}

bool MainContentComponent::keyPressed(const juce::KeyPress& key, juce::Component* origin)
{

    return true;

}

void MainContentComponent::timerCallback(int timerID)
{

    if (timerID == OUTPUT_TIMER)
    {
        printf("%f, %d\n", RMSTest, pitchTestIndex);
        return;
    }

    juce::Random rand;

    if (RMSTest > 0.05)
    {
        float diff = (float)pitchTestIndex / 2000.0f;
        interpolation.setValue((768.0f - 64.0f) - ceil((768.0f - 64.0f) * diff));
    }
    else
    {
        interpolation.setValue(768 - 64);
    }

    player.setY(interpolation.getNextValue());

    coin.translate(-5, 0);

    if (player.intersects(coin))
    {
        coin.setPosition(768, rand.nextInt(768 - 32) + 16);
        score++;
    }
    else if (coin.getX() <= -16)
    {
        coin.setPosition(768, rand.nextInt(768 - 32) + 16);
    }

    repaint();

}

void MainContentComponent::paint(juce::Graphics& g)
{

    int width = getWidth();
    int height = getHeight();

    g.fillAll (juce::Colours::white);

    g.setFont(48);
    g.setColour(juce::Colours::grey);
    g.drawText(juce::String(score), 0, 0, width, height, juce::Justification::centred);

    int cX = coin.getX();
    int cY = coin.getY();
    int cW = coin.getWidth();
    int cH = coin.getHeight();

    g.setColour(juce::Colours::cadetblue);
    g.fillRect(cX, cY, cW, cH);

    int pX = player.getX();
    int pY = player.getY();
    int pW = player.getWidth();
    int pH = player.getHeight();

    g.setColour(juce::Colours::red);
    juce::Path p;
    p.startNewSubPath(pX, pY);
    p.addTriangle(pX, pY, pX, pY + pH, pX + pW, pY + pH/2);
    g.fillPath(p);

}

juce::Component* createMainContentComponent()     { return new MainContentComponent(); }
