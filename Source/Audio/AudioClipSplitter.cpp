#include "AudioClipSplitter.h"

SplitClipResult AudioClipSplitter::splitClipFile(const juce::File& originalFile, const float ratio,
                                                 const double originalOffset, const juce::File& audioDir)
{
    assert(audioDir.exists());

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    const std::unique_ptr<juce::AudioFormatReader> reader{formatManager.createReaderFor(originalFile)};
    assert(reader);

    const auto totalSamples{reader->lengthInSamples};
    const auto firstSamplesToWrite{static_cast<int>(totalSamples * ratio)};
    const auto secondSamplesToWrite{static_cast<int>(totalSamples - firstSamplesToWrite)};
    assert((firstSamplesToWrite + secondSamplesToWrite) == totalSamples);

    juce::String firstDestFileName, secondDestFileName;
    generateSplitFileNames(originalFile, audioDir, firstDestFileName, secondDestFileName);
    assert(!audioDir.getChildFile(firstDestFileName).existsAsFile() &&
           !audioDir.getChildFile(secondDestFileName).existsAsFile());

    const juce::File firstDestFile{audioDir.getFullPathName() + "/" + firstDestFileName};
    const juce::File secondDestFile{audioDir.getFullPathName() + "/" + secondDestFileName};

    writeToFile(*reader, formatManager, firstDestFile, firstSamplesToWrite, 0);
    writeToFile(*reader, formatManager, secondDestFile, secondSamplesToWrite, firstSamplesToWrite);

    const double secondSplitOffset{static_cast<double>(firstSamplesToWrite) / reader->sampleRate};

    return SplitClipResult{firstDestFile, originalOffset, secondDestFile, originalOffset + secondSplitOffset};
}

void AudioClipSplitter::writeToFile(juce::AudioFormatReader& reader, const juce::AudioFormatManager& formatManager,
                                    const juce::File& destFile, const int numOfSamplesToWrite,
                                    const int readerOffsetInSamples)
{
    // TODO: Think about reserving memory for split clip in chunks, not all at once as below
    juce::AudioBuffer<float> buffer{static_cast<int>(reader.numChannels), numOfSamplesToWrite};
    reader.read(&buffer, 0, numOfSamplesToWrite, readerOffsetInSamples, true, true);

    auto outputStream = destFile.createOutputStream();
    assert(outputStream);

    const auto fileExtension = destFile.getFileExtension();
    auto* writer = formatManager.findFormatForFileExtension(fileExtension)
                       ->createWriterFor(outputStream.release(),
                                         reader.sampleRate,
                                         reader.numChannels,
                                         static_cast<int>(reader.bitsPerSample),
                                         reader.metadataValues,
                                         0);
    assert(writer);

    const std::unique_ptr<juce::AudioFormatWriter> formatWriter{writer};
    formatWriter->writeFromAudioSampleBuffer(buffer, 0, numOfSamplesToWrite);
}

juce::String AudioClipSplitter::getBaseName(const juce::String& fileName)
{
    const int splitPos = fileName.lastIndexOf("_split");
    if(splitPos == -1)
        return fileName;

    return fileName.substring(0, splitPos);
}

juce::String AudioClipSplitter::findNextAvailableName(const juce::String& baseName, const juce::String& extension,
                                                      const juce::File& audioDir)
{
    static int counter = 1;
    while(true)
    {
        const juce::String testFileName = baseName + "_split" + juce::String(counter++) + extension;
        if(!audioDir.getChildFile(testFileName).existsAsFile())
            return testFileName;
    }
}

void AudioClipSplitter::generateSplitFileNames(const juce::File& originalFile, const juce::File& audioDir,
                                               juce::String& firstFileName, juce::String& secondFileName)
{
    const juce::String originalName = originalFile.getFileNameWithoutExtension();
    const juce::String extension = originalFile.getFileExtension() == ".mp3" ? ".wav" : originalFile.getFileExtension();
    const juce::String baseName = getBaseName(originalName);

    firstFileName = findNextAvailableName(baseName, extension, audioDir);
    secondFileName = findNextAvailableName(baseName, extension, audioDir);
}