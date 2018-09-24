#ifndef BEEP_H
#define BEEP_H

#include <cinttypes>
#include <alsa/asoundlib.h>
#include <cmath>

class Beep
{
public:
    Beep();
    void play(double freq, std::uint32_t duration);
    ~Beep();

private:
    int rc;
    int size;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    char *buffer;
    inline std::int16_t gen_sin(size_t sample, double freq) { return (int16_t) ((((std::uint16_t)-1)/2.0)*sin(freq*sample*2*M_PI)); }
};

#endif // BEEP_H
