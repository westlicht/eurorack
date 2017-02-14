// Copyright 2017 Simon Kallweit.
//
// Author: Simon Kallweit (simon.kallweit@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Sample player.

#include "drums/player/player.h"

#include <cstdio>

#include "stmlib/utils/dsp.h"
#include "stmlib/utils/random.h"

#include "drums/resources.h"

namespace drums {

using namespace stmlib;

static inline int32_t interpolate(const int8_t* table, uint32_t index) {
  int32_t a = table[index >> 8] << 8;
  int32_t b = table[(index >> 8) + 1] << 8;
  return a + ((b - a) * static_cast<int32_t>(index & 0xff) >> 8);
}

static inline int32_t crossfade(int32_t a, int32_t b, int32_t mix) {
  return ((a * (0xffff - mix)) >> 16) + ((b * mix) >> 16);
}

void Player::Init() {
  svf_.Init();
  svf_.set_mode(SVF_MODE_LP);
  svf_.set_resonance(0);

  active_ = false;
  increment_ = 256;
  decay_scale_ = 256;
}

int16_t Player::ProcessSingleSample(uint8_t control) {
  if (control & CONTROL_GATE_RISING) {
    active_ = true;
    phase_ = 0;
  }

  int32_t output = 0;
  if (active_) {
    // exponential envelope relative to sample length
    int32_t index = phase_ / (length_ / 0xffff); // TODO precompute this as shift
    index = (index * decay_scale_) >> 8;
    index = std::min(index, int32_t((LUT_ENV_EXPO_SIZE - 2) << 8));
    int32_t envelope = 0xffff - Interpolate88(lut_env_expo, index);

    // read sample waveform
    int32_t sample = interpolate(waveform_, phase_);

    // increment phase and stop playback at end
    phase_ += increment_;
    if (++phase_ >= length_) {
      active_ = false;
    }

    // mix white noise
    int32_t noise = Random::GetSample();
    output = crossfade(sample, noise, noise_level_);

    CLIP(output);
    // filtering
    // int32_t filtered = svf_.Process(output);
    // output = crossfade(output, filtered, filter_mix_);
    // CLIP(output);

    // apply envelope
    output = (output * envelope) >> 16;

    CLIP(output);
  }
  return output;
}

void Player::set_sample(uint8_t index) {
  switch (index) {
    case 0: waveform_ = wav_sample0; length_ = WAV_SAMPLE0_SIZE; break;
    case 1: waveform_ = wav_sample1; length_ = WAV_SAMPLE1_SIZE; break;
  }
  length_ = (length_ - 1) << 8;
}


}  // namespace drums
