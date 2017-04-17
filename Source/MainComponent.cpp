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

    score = 0;

    pitchCurrent = 0;
    pitchLast = 0;

    interpolation.reset(200, 0.25);

    stars.resize(25);

    for (int i = 0; i < stars.size(); ++i)
    {
        juce::Point<int>& star = stars.getReference(i);
        star.setXY(rand.nextInt(768), rand.nextInt(768));
    }

}

MainContentComponent::~MainContentComponent()
{
    shutdownAudio();
}

void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    startTimer(1000/60);

    currentBlockSize = samplesPerBlockExpected;
    currentSampleRate = sampleRate;

}

void MainContentComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{

    RMSTest = bufferToFill.buffer->getRMSLevel(0, 0, currentBlockSize);

    double signal[currentBlockSize * 2];

    for (int i = 0; i < currentBlockSize; ++i)
    {
        double window  = 0.54 - 0.46 * (1 - cos((2 * M_PI * i) / (currentBlockSize  - 1)));
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

    int max2 = minIndex;
    int maxsearch = maxLocation * 3 / 4;
    for (int i = minIndex; i < maxsearch; ++i)
    {
        if (spectrum[i] > spectrum[max2])
        {
            max2 = i;
        }
    }
    if (abs(max2 * 2 - maxLocation) < 4)
    {
        if (spectrum[max2] / spectrum[maxLocation] > 0.2)
        {
            maxLocation = max2;
        }
    }

    pitchCurrent = (((maxLocation * currentSampleRate / currentBlockSize)) + pitchLast) / 2;
    pitchLast = pitchCurrent;

}

void MainContentComponent::releaseResources()
{

}

bool MainContentComponent::keyPressed(const juce::KeyPress& key, juce::Component* origin)
{

    return true;

}

void MainContentComponent::timerCallback()
{

    for (int i = 0; i < stars.size(); ++i)
    {
        juce::Point<int>& star = stars.getReference(i);

        star.addXY(-10, 0);

        if (star.getX() < 0)
        {
            star.setXY(769, rand.nextInt(768));
        }
    }

    if (RMSTest > 0.005)
    {
        if (pitchCurrent > 200)
        {
            printf("%d\n", pitchCurrent);
            float diff = fmin(1.0f, (float)pitchCurrent / 1000.0f);
            interpolation.setValue((768.0f - 64.0f) - ceil((768.0f - 64.0f) * diff));
        }
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

    g.fillAll (juce::Colours::black);

    g.setColour(juce::Colours::white);
    for (int i = 0; i < stars.size(); ++i)
    {
        juce::Point<int>& star = stars.getReference(i);
        g.setPixel(star.x, star.y);
    }

    g.setFont(48);
    g.setColour(juce::Colours::darkgrey);
    g.drawText(juce::String(score), 0, 0, width, height, juce::Justification::centred);

    int cX = coin.getX();
    int cY = coin.getY();
    int cW = coin.getWidth();
    int cH = coin.getHeight();

    g.setColour(juce::Colours::gold);
    g.fillEllipse(cX, cY, cW, cH);

    int pX = player.getX();
    int pY = player.getY();
    int pW = player.getWidth();
    int pH = player.getHeight();

    g.setColour(juce::Colours::cadetblue);
    juce::Path p;
    p.startNewSubPath(pX, pY);
    p.addTriangle(pX, pY, pX, pY + pH, pX + pW, pY + pH/2);
    g.fillPath(p);

}

juce::Component* createMainContentComponent()     { return new MainContentComponent(); }
