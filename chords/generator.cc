// Copyright 2013 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
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
// Tidal generator.

#include "chords/generator.h"

#include <algorithm>
#include <cmath>

// #include "stmlib/utils/dsp.h"

// #include "chords/resources.h"

namespace chords {

using namespace std;
using namespace stmlib;

void Generator::Init() {
  for (size_t i = 0; i < kNumBlocks; ++i) {
    std::fill(&audio_samples_[i][0], &audio_samples_[i][kBlockSize], 0);
  }
  std::fill(&sync_samples_[0], &sync_samples_[kBlockSize], 0);
  playback_block_ = kNumBlocks / 2;
  render_block_ = 0;
  current_sample_ = 0;

  waveform_ = GENERATOR_WAVEFORM_SQUARE;

  for (size_t i = 0; i < kNumOscillators; ++i) {
    AnalogOscillator& osc = oscillators_[i];
    osc.Init();
    osc.set_shape(OSC_SHAPE_SINE);
    osc.set_pitch(5000 + 1000 * i);
  }

}

void Generator::ProcessControlBits(uint8_t control_bits) {
  if (control_bits & CONTROL_WAVEFORM_RISING) {
    next_waveform();
  }
}

void Generator::RenderBlock() {
  static int16_t temp_buffer[kBlockSize];

  int16_t notes[4];
  Chord::notes(quality_, inversion_, voicing_, notes);

  AnalogOscillatorShape shape = oscillator_shape();

  uint8_t* sync_buffer = sync_samples_;
  int16_t* render_buffer = audio_samples_[render_block_];

  std::fill(&render_buffer[0], &render_buffer[kBlockSize], 0);

  for (size_t i = 0; i < kNumOscillators; ++i) {
    AnalogOscillator& osc = oscillators_[i];
    osc.set_shape(shape);
    osc.set_pitch(pitch_ + (1 << 7) * notes[i]);
    osc.Render(sync_buffer, temp_buffer, NULL, kBlockSize);

    for (size_t j = 0; j < kBlockSize; ++j) {
      // render_buffer[j] += std::max(-8191, temp_buffer[j] >> 2);
      // render_buffer[j] = std::max(-32767l, std::min(32768l, int32_t(render_buffer[j]) + (temp_buffer[j] / 8)));
      // render_buffer[j] = std::max(-32767l, std::min(32768l, int32_t(temp_buffer[j])));
      render_buffer[j] += temp_buffer[j] >> 3;
    }
  }

  render_block_ = (render_block_ + 1) % kNumBlocks;
}


}  // namespace chords
