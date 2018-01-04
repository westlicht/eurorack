#ifndef CHORDS_GENERATOR_H_
#define CHORDS_GENERATOR_H_

#include "stmlib/stmlib.h"

// #include "stmlib/algorithms/pattern_predictor.h"
// #include "stmlib/utils/ring_buffer.h"

#include "chords/analog_oscillator.h"
#include "chords/chord.h"


// #define WAVETABLE_HACK

namespace chords {

enum GeneratorWaveform {
  GENERATOR_WAVEFORM_SINE,
  GENERATOR_WAVEFORM_TRIANGLE,
  GENERATOR_WAVEFORM_SAW,
  GENERATOR_WAVEFORM_SQUARE,
  GENERATOR_WAVEFORM_LAST,
};

enum ControlBitMask {
  CONTROL_WAVEFORM = 1<<0,
  CONTROL_GATE = 1<<1,
  CONTROL_CLOCK = 1<<2,
  CONTROL_WAVEFORM_RISING = 1<<3,
  CONTROL_GATE_RISING = 1<<4,
  CONTROL_GATE_FALLING = 1<<5,
  CONTROL_CLOCK_RAISING = 1<<6,
};

// enum GeneratorRange {
//   GENERATOR_RANGE_HIGH,
//   GENERATOR_RANGE_MEDIUM,
//   GENERATOR_RANGE_LOW
// };

// enum GeneratorMode {
//   GENERATOR_MODE_AD,
//   GENERATOR_MODE_LOOPING,
//   GENERATOR_MODE_AR,
// };

// enum ControlBitMask {
//   CONTROL_FREEZE = 1,
//   CONTROL_GATE = 2,
//   CONTROL_CLOCK = 4,
//   CONTROL_CLOCK_RISING = 8,
//   CONTROL_GATE_RISING = 16,
//   CONTROL_GATE_FALLING = 32
// };

// enum FlagBitMask {
//   FLAG_END_OF_ATTACK = 1,
//   FLAG_END_OF_RELEASE = 2
// };

// struct GeneratorSample {
//   uint16_t unipolar;
//   int16_t bipolar;
//   uint8_t flags;
// };

// const size_t kNumBlocks = 2;
// const size_t kBlockSize = 16;
const size_t kNumBlocks = 4;
const size_t kBlockSize = 24;
const size_t kNumOscillators = 4;


// struct FrequencyRatio {
//   uint32_t p;
//   uint32_t q;
// };

class Generator {
 public:
  Generator() { }
  ~Generator() { }
  
  void Init();

  ChordQuality quality() const {
    return quality_;
  }

  void set_quality(ChordQuality quality) {
    quality_ = quality;
  }

  ChordInversion inversion() const {
    return inversion_;
  }

  void set_inversion(ChordInversion inversion) {
    inversion_ = inversion;
  }

  ChordVoicing voicing() const {
    return voicing_;
  }

  void set_voicing(ChordVoicing voicing) {
    voicing_ = voicing;
  }

  void set_pitch(int16_t pitch) {
    pitch_ = pitch; 
  }

  GeneratorWaveform waveform() const {
    return waveform_;
  }

  void set_waveform(GeneratorWaveform waveform) {
    waveform_ = waveform;
  }

  void next_waveform() {
    set_waveform(GeneratorWaveform((int(waveform()) + 1) % GENERATOR_WAVEFORM_LAST));
  }

  // void set_range(GeneratorRange range) {
  //   ClearFilterState();
  //   range_ = range;
  //   clock_divider_ = range_ == GENERATOR_RANGE_LOW ? 4 : 1;
  // }
  
  // void set_mode(GeneratorMode mode) {
  //   mode_ = mode;
  //   if (mode_ == GENERATOR_MODE_LOOPING) {
  //     running_ = true;
  //   }
  // }
  
  // void set_pitch(int16_t pitch) {
  //   if (sync_) {
  //     ComputeFrequencyRatio(pitch);
  //   }
  //   pitch += (12 << 7) - (60 << 7) * static_cast<int16_t>(range_);
  //   if (range_ == GENERATOR_RANGE_LOW) {
  //     pitch -= (12 << 7);  // One extra octave of super LF stuff!
  //   }
  //   pitch_ = pitch;
  // }
  
  // void set_shape(int16_t shape) {
  //   shape_ = shape;
  // }

  // void set_slope(int16_t slope) {
  //   slope_ = slope;
  // }

  // void set_smoothness(int16_t smoothness) {
  //   smoothness_ = smoothness;
  // }
  
  // void set_frequency_ratio(FrequencyRatio ratio) {
  //   frequency_ratio_ = ratio;
  // }
  
  // void set_sync(bool sync) {
  //   if (!sync_ && sync) {
  //     pattern_predictor_.Init();
  //   }
  //   sync_ = sync;
  //   sync_edges_counter_ = 0;
  // }
  
  // inline GeneratorMode mode() const { return mode_; }
  // inline GeneratorRange range() const { return range_; }
  // inline bool sync() const { return sync_; }
  
  // inline const GeneratorSample& Process(uint8_t control) {
  //   input_samples_[playback_block_][current_sample_] = control;
  //   const GeneratorSample& out = output_samples_[playback_block_][current_sample_];
  //   current_sample_ = current_sample_ + 1;
  //   if (current_sample_ >= kBlockSize) {
  //     current_sample_ = 0;
  //     playback_block_ = (playback_block_ + 1) % kNumBlocks;
  //   }
  //   return out;
  // }
  
  inline bool writable_block() const {
    return render_block_ != playback_block_;
  }

  inline void Process(uint8_t control_bits) {
    ProcessControlBits(control_bits);
    while (render_block_ != playback_block_) {
      RenderBlock();
    }
  }

  inline int16_t GetSample() {
    int16_t sample = -audio_samples_[playback_block_][current_sample_];

    current_sample_ = current_sample_ + 1;
    if (current_sample_ >= kBlockSize) {
      current_sample_ = 0;
      playback_block_ = (playback_block_ + 1) % kNumBlocks;
    }

    return sample;
  }

  // inline void Process() {
  //   while (render_block_ != playback_block_) {
  //     uint8_t* in = input_samples_[render_block_];
  //     GeneratorSample* out = output_samples_[render_block_];
  // #ifndef WAVETABLE_HACK
  //     if (range_ == GENERATOR_RANGE_HIGH) {
  //       ProcessAudioRate(in, out, kBlockSize);
  //     } else {
  //       ProcessControlRate(in, out, kBlockSize);
  //     }
  //     ProcessFilterWavefolder(out, kBlockSize);
  // #else
  //     ProcessWavetable(in, out, kBlockSize);
  // #endif
  //     render_block_ = (render_block_ + 1) % kNumBlocks;
  //   }
  // }
  
  // uint32_t clock_divider() const {
  //   return clock_divider_;
  // }

 private:
  inline AnalogOscillatorShape oscillator_shape() const {
    switch (waveform_) {
    case GENERATOR_WAVEFORM_SINE:
      return OSC_SHAPE_SINE;
    case GENERATOR_WAVEFORM_TRIANGLE:
      return OSC_SHAPE_TRIANGLE;
    case GENERATOR_WAVEFORM_SAW:
      return OSC_SHAPE_SAW;
    case GENERATOR_WAVEFORM_SQUARE:
      return OSC_SHAPE_SQUARE;
    default:
      return OSC_SHAPE_SINE;
    }
  }

  void ProcessControlBits(uint8_t control_bits);
  void RenderBlock();

  ChordQuality quality_;
  ChordInversion inversion_;
  ChordVoicing voicing_;
  int16_t pitch_;
  GeneratorWaveform waveform_;

  size_t current_sample_;
  volatile size_t playback_block_;
  volatile size_t render_block_;
  int16_t audio_samples_[kNumBlocks][kBlockSize];
  uint8_t sync_samples_[kBlockSize];

  AnalogOscillator oscillators_[kNumOscillators];


  // // There are two versions of the rendering code, one optimized for audio, with
  // // band-limiting.
  // void ProcessAudioRate(const uint8_t* in, GeneratorSample* out, size_t size);
  // void ProcessControlRate(const uint8_t* in, GeneratorSample* out, size_t size);
  // void ProcessWavetable(const uint8_t* in, GeneratorSample* out, size_t size);
  // void ProcessFilterWavefolder(GeneratorSample* in_out, size_t size);

  // int32_t ComputeAntialiasAttenuation(
  //       int16_t pitch,
  //       int16_t slope,
  //       int16_t shape,
  //       int16_t smoothness) const;
  
  // inline void ClearFilterState() {
  //   uni_lp_state_[0] = uni_lp_state_[1] = 0;
  //   bi_lp_state_[0] = bi_lp_state_[1] = 0;
  // }

  // uint32_t ComputePhaseIncrement(int16_t pitch);
  // int16_t ComputePitch(uint32_t phase_increment);
  // int32_t ComputeCutoffFrequency(int16_t pitch, int16_t smoothness);
  // void ComputeFrequencyRatio(int16_t pitch);
  
  // inline int32_t NextIntegratedBlepSample(uint32_t t) const {
  //   if (t >= 65535) {
  //     t = 65535;
  //   }
  //   const int32_t t1 = t >> 1;
  //   const int32_t t2 = t1 * t1 >> 16;
  //   const int32_t t4 = t2 * t2 >> 16;
  //   return 12288 - t1 + (3 * t2 >> 1) - t4;
  // }

  // inline int32_t ThisIntegratedBlepSample(uint32_t t) const {
  //   if (t >= 65535) {
  //     t = 65535;
  //   }
  //   t = 65535 - t;
  //   const int32_t t1 = t >> 1;
  //   const int32_t t2 = t1 * t1 >> 16;
  //   const int32_t t4 = t2 * t2 >> 16;
  //   return 12288 - t1 + (3 * t2 >> 1) - t4;
  // }
  
  // GeneratorSample output_samples_[kNumBlocks][kBlockSize];
  // uint8_t input_samples_[kNumBlocks][kBlockSize];
  // size_t current_sample_;
  // volatile size_t playback_block_;
  // volatile size_t render_block_;
  
  // GeneratorMode mode_;
  // GeneratorRange range_;
  // GeneratorSample previous_sample_;
  // GeneratorSample buffer_[kBlockSize];
  
  // uint32_t clock_divider_;
  
  // uint16_t prescaler_;
  // int16_t pitch_;
  // int16_t previous_pitch_;
  // int16_t shape_;
  // int16_t slope_;
  // int32_t smoothed_slope_;
  // int16_t smoothness_;
  // int16_t attenuation_;
  
  // uint32_t phase_;
  // uint32_t phase_increment_;
  // uint16_t x_;
  // uint16_t y_;
  // uint16_t z_;
  // bool wrap_;
  
  // bool sync_;
  // FrequencyRatio frequency_ratio_;
  
  // // Time measurement and clock divider for PLL mode.
  // uint32_t sync_counter_;
  // uint32_t sync_edges_counter_;
  // uint32_t local_osc_phase_;
  // uint32_t local_osc_phase_increment_;
  // uint32_t target_phase_increment_;
  // uint32_t eor_counter_;
  
  // stmlib::PatternPredictor<32, 8> pattern_predictor_;
  
  // int64_t uni_lp_state_[2];
  // int64_t bi_lp_state_[2];
  
  // bool running_;
  
  // // Polyblep status.
  // int32_t next_sample_;
  // bool slope_up_;
  // uint32_t mid_point_;
  
  // static const FrequencyRatio frequency_ratios_[];
  // static const int16_t num_frequency_ratios_;
  
  DISALLOW_COPY_AND_ASSIGN(Generator);
};

}  // namespace chords

#endif  // CHORDS_GENERATOR_H_
