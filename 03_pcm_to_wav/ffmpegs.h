#ifndef FFMPEGS_H
#define FFMPEGS_H
#include<QObject>

typedef struct {
    uint8_t riffChunkId[4] = {'R', 'I', 'F', 'F'};
    uint32_t riffChunkDataSize;
    uint8_t format[4] = {'W', 'A', 'V', 'E'};

    uint8_t fmtChunkId[4] = {'f', 'm', 't', ' '};
    uint32_t fmtChunkDataSize = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    uint8_t dataChunkId[4] = {'d', 'a', 't', 'a'};
    uint32_t dataChunkDataSize;
} WAVHeader;
class FFmpegs
{
public:
    FFmpegs();
    static void pcm2wav(WAVHeader &header,
                        const char *pcmFilename,
                        const char *wavFilename);
};

#endif // FFMPEGS_H
