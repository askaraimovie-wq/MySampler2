#include "PluginProcessor.h"
#include "PluginEditor.h"

SamplerAudioProcessor::SamplerAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    formatManager.registerBasicFormats();

    for (int i = 0; i < 16; ++i)
        synth.addVoice (new juce::SamplerVoice());
}

juce::AudioProcessorValueTreeState::ParameterLayout SamplerAudioProcessor::createLayout()
{
    using juce::ParameterID;
    using Range = juce::NormalisableRange<float>;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParameterID { "attack", 1 }, "Attack", Range (0.001f, 5.0f, 0.001f, 0.4f), 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParameterID { "decay", 1 }, "Decay", Range (0.001f, 5.0f, 0.001f, 0.4f), 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParameterID { "sustain", 1 }, "Sustain", Range (0.0f, 1.0f, 0.001f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParameterID { "release", 1 }, "Release", Range (0.001f, 8.0f, 0.001f, 0.4f), 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParameterID { "gain", 1 }, "Gain", Range (0.0f, 1.5f, 0.001f), 0.8f));

    return { params.begin(), params.end() };
}

bool SamplerAudioProcessor::loadSample (const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));

    if (reader == nullptr)
        return false;

    juce::BigInteger allNotes;
    allNotes.setRange (0, 128, true);

    // root note 60 = C3 in most DAWs (middle C); max sample length 30 seconds
    auto* sound = new juce::SamplerSound ("sample", *reader, allNotes, 60, 0.01, 0.1, 30.0);

    {
        const juce::SpinLock::ScopedLockType lock (synthLock);
        synth.clearSounds();
        synth.addSound (sound);
    }

    samplePath = file.getFullPathName();
    return true;
}

juce::String SamplerAudioProcessor::getSampleName() const
{
    return samplePath.isEmpty() ? juce::String ("No sample loaded")
                                : juce::File (samplePath).getFileName();
}

void SamplerAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

bool SamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // keys clicked on the on-screen keyboard
    keyboardState.processNextMidiBuffer (midi, 0, buffer.getNumSamples(), true);

    juce::ADSR::Parameters env;
    env.attack  = apvts.getRawParameterValue ("attack")->load();
    env.decay   = apvts.getRawParameterValue ("decay")->load();
    env.sustain = apvts.getRawParameterValue ("sustain")->load();
    env.release = apvts.getRawParameterValue ("release")->load();
    const float gain = apvts.getRawParameterValue ("gain")->load();

    {
        const juce::SpinLock::ScopedTryLockType lock (synthLock);

        if (lock.isLocked())
        {
            for (int i = 0; i < synth.getNumSounds(); ++i)
                if (auto* s = dynamic_cast<juce::SamplerSound*> (synth.getSound (i).get()))
                    s->setEnvelopeParameters (env);

            synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
        }
    }

    buffer.applyGain (gain);
}

juce::AudioProcessorEditor* SamplerAudioProcessor::createEditor()
{
    return new SamplerAudioProcessorEditor (*this);
}

void SamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("samplePath", samplePath, nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void SamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            apvts.replaceState (tree);

            const auto path = tree.getProperty ("samplePath").toString();

            if (path.isNotEmpty())
            {
                juce::File f (path);

                if (f.existsAsFile())
                    loadSample (f);
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerAudioProcessor();
}
