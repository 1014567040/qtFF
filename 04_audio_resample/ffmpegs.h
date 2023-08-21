#ifndef FFMPEGS_H
#define FFMPEGS_H
extern "C"{
    #include<libavformat/avformat.h>
}

typedef struct{
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
}ResampleAudioSpec;

class FFmpegs
{
public:
    FFmpegs();
    static void resampleAudio(const char *inFilename,
                              int inSampleRate,
                              AVSampleFormat inSampleFmt,
                              int inChLayout,
                              const char *outFilename,
                              int outSampleRate,
                              AVSampleFormat outSampleFmt,
                              int outChLayout);

    static void resampleAudio(ResampleAudioSpec &in,
                              ResampleAudioSpec &out);
};

#endif // FFMPEGS_H
