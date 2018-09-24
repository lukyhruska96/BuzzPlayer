#include "beep.h"

#include <stdexcept>
#include <iostream>
#include <chrono>

using namespace std::chrono;

Beep::Beep()
{
    /* Open PCM device for playback. */
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        throw std::runtime_error("Unable to open device");
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* 44100 bits/second sampling rate (CD quality) */
    val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    /* Set period size to 32 frames. */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        snd_pcm_close(handle);
        throw std::runtime_error("Unable to set HW parameters");
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    snd_pcm_hw_params_get_period_time(params, &val, &dir);
}

void Beep::play(double freq, std::uint32_t duration)
{
    std::cout << "playing: " << freq << " for " << duration << "." << std::endl;
    milliseconds before = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    snd_pcm_wait(handle, -1);
    duration -= (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - before).count();
    long loops = duration * 1000 / val;

    size_t i = 0;

    while (loops > 0) {
      loops--;
      for(int j = 0; j<frames; j++){
          std::int16_t tmp = gen_sin(i, freq/val);
          buffer[j*4+1] = tmp >> 8;
          buffer[j*4+3] = tmp >> 8;
          buffer[j*4] = (char) tmp;
          buffer[j*4+2] = (char) tmp;
          i++;
      }
      rc = snd_pcm_writei(handle, buffer, frames);
      if (rc == -EPIPE) {
        /* EPIPE means underrun */
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(handle);
      }
      if (rc < 0)
         frames = snd_pcm_recover(handle, frames, 0);
      if (rc < 0) {
        fprintf(stderr,
                "error from writei: %s\n",
                snd_strerror(rc));
      } else if (rc != (int)frames) {
        fprintf(stderr,
                "short write, write %d frames\n", rc);
      }
    }
}

Beep::~Beep()
{
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
}
