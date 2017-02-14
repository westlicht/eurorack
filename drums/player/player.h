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

#ifndef DRUMS_PLAYER_PLAYER_H_
#define DRUMS_PLAYER_PLAYER_H_

#include "stmlib/stmlib.h"
#include "stmlib/utils/dsp.h"

#include "drums/gate_processor.h"
#include "drums/modulations/multistage_envelope.h"
#include "drums/player/svf.h"
#include "drums/resources.h"

namespace drums {

class Player {
 public:
  Player() { }
  ~Player() { }

  void Init();
  int16_t ProcessSingleSample(uint8_t control) IN_RAM;

  void Configure(uint16_t* parameter, ControlMode control_mode) {
    if (control_mode == CONTROL_MODE_HALF) {
      set_frequency(parameter[0]);
      set_decay(parameter[1]);
      set_noise(0);
      set_filter(0x7fff);
    } else {
      set_frequency(parameter[0]);
      set_decay(parameter[1]);
      set_noise(parameter[2]);
      set_filter(parameter[3]);
    }
  }

  void set_sample(uint8_t index);

  void set_frequency(uint16_t frequency) {
    increment_ = stmlib::Interpolate88(lut_pow2, frequency) >> 2;
  }

  void set_decay(uint16_t decay) {
    decay_scale_ = stmlib::Interpolate88(lut_pow2, 0xffff - decay) >> 1;
    decay_scale_ = (decay_scale_ * decay_scale_) >> 8;
    decay_scale_ = (decay_scale_ * decay_scale_) >> 8;
  }

  void set_noise(uint16_t noise) {
    noise_level_ = noise;
  }

  void set_filter(uint16_t filter) {
    const uint16_t mid = 0x8000;
    if (filter < mid) {
      svf_.set_mode(SVF_MODE_LP);
      svf_.set_frequency(filter << 1);
    } else {
      svf_.set_mode(SVF_MODE_HP);
      svf_.set_frequency((filter - mid) << 1);
    }

    filter_mix_ = abs(int32_t(filter - mid));
    filter_mix_ = std::min(int32_t(0x1fff), std::max(int32_t(0), filter_mix_ - 0x1fff));
    filter_mix_ <<= 3;
  }

 private:
  Svf svf_;
  const int8_t* waveform_;
  int32_t phase_;
  int32_t length_;
  int32_t increment_;
  int32_t decay_scale_;
  int32_t noise_level_;
  int32_t filter_mix_;
  bool active_;

  DISALLOW_COPY_AND_ASSIGN(Player);
};

}  // namespace drums

#endif  // DRUMS_PLAYER_PLAYER_H_
