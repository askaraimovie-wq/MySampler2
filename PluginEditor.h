#pragma once

#include "PluginProcessor.h"

class SamplerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SamplerAudioProcessorEditor (SamplerAudioProcessor&);
    ~SamplerAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    SamplerAudioProcessor& proc;

    juce::TextButton loadButton { "Load Sample" };
    juce::Label sampleLabel;
    Knob knobs[5];
    juce::MidiKeyboardComponent keyboard;
    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerAudioProcessorEditor)
};
