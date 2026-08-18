#pragma once

#include "MainAudio.h"

struct SplitClipResult
{
    juce::File firstFile;
    double firstOffset;
    Fade::Data firstFadeInfo;
    juce::File secondFile;
    double secondOffset;
    Fade::Data secondFadeInfo;
};

class AudioClipSplitter
{
public:
    AudioClipSplitter() = default;
    ~AudioClipSplitter() = default;

    static SplitClipResult splitClipFile(const juce::File& originalFile, float ratio, double originalOffset,
                                         const juce::File& audioDir,
                                         const Fade::ClipData& splitClipFadeData);

private:
    static void writeToFile(juce::AudioFormatReader& reader, const juce::AudioFormatManager& formatManager,
                            const juce::File& destFile, int numOfSamplesToWrite, int readerOffsetInSamples);

    static juce::String getBaseName(const juce::String& fileName);
    static juce::String findNextAvailableName(const juce::String& baseName, const juce::String& extension,
                                              const juce::File& audioDir);
    static void generateSplitFileNames(const juce::File& originalFile, const juce::File& audioDir,
                                       juce::String& firstFileName, juce::String& secondFileName);
};