#include "PluginEditor.h"

SamplerAudioProcessorEditor::SamplerAudioProcessorEditor (SamplerAudioProcessor& p)
    : AudioProcessorEditor (&p),
      proc (p),
      keyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    static const char* ids[]   = { "attack", "decay", "sustain", "release", "gain" };
    static const char* names[] = { "Attack", "Decay", "Sustain", "Release", "Gain" };

    addAndMakeVisible (loadButton);
    loadButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser> ("Choose a sample", juce::File(), "*.wav;*.aif;*.aiff;*.flac;*.ogg");

        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  auto file = fc.getResult();

                                  if (file.existsAsFile() && proc.loadSample (file))
                                      sampleLabel.setText (proc.getSampleName(), juce::dontSendNotification);
                              });
    };

    addAndMakeVisible (sampleLabel);
    sampleLabel.setText (proc.getSampleName(), juce::dontSendNotification);

    for (int i = 0; i < 5; ++i)
    {
        auto& k = knobs[i];

        k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        k.label.setText (names[i], juce::dontSendNotification);
        k.label.setJustificationType (juce::Justification::centred);

        addAndMakeVisible (k.slider);
        addAndMakeVisible (k.label);

        k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            proc.apvts, ids[i], k.slider);
    }

    addAndMakeVisible (keyboard);
    setSize (560, 340);
}

void SamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1e1f26));
}

void SamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);

    auto top = area.removeFromTop (30);
    loadButton.setBounds (top.removeFromLeft (130));
    top.removeFromLeft (10);
    sampleLabel.setBounds (top);

    area.removeFromTop (10);
    auto keyArea = area.removeFromBottom (90);
    keyboard.setBounds (keyArea);

    area.removeFromBottom (10);
    const int w = area.getWidth() / 5;

    for (auto& k : knobs)
    {
        auto col = area.removeFromLeft (w);
        k.label.setBounds (col.removeFromTop (20));
        k.slider.setBounds (col);
    }
}
