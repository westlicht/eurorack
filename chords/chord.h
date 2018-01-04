
#ifndef CHORDS_CHORD_H_
#define CHORDS_CHORD_H_

#include <algorithm>

namespace chords {

enum ChordQuality {
  CHORD_QUALITY_MAJOR7,
  CHORD_QUALITY_MINOR7,
  CHORD_QUALITY_DOM7,
  CHORD_QUALITY_HALF_DIM,
  CHORD_QUALITY_LAST,
};

enum ChordInversion {
  CHORD_INVERSION_ROOT,
  CHORD_INVERSION_FIRST,
  CHORD_INVERSION_SECOND,
  CHORD_INVERSION_THIRD,
  CHORD_INVERSION_LAST,
};

enum ChordVoicing {
  CHORD_VOICING_CLOSE,
  CHORD_VOICING_DROP_2,
  CHORD_VOICING_DROP_3,
  CHORD_VOICING_SPREAD,
  CHORD_VOICING_LAST,
};

class Chord {
public:
  static void notes(ChordQuality quality, ChordInversion inversion, ChordVoicing voicing, int16_t notes[4]) {
    switch (quality) {
    case CHORD_QUALITY_MAJOR7:
      notes[0] = 0;
      notes[1] = 4;
      notes[2] = 7;
      notes[3] = 11;
      break;
    case CHORD_QUALITY_MINOR7:
      notes[0] = 0;
      notes[1] = 3;
      notes[2] = 7;
      notes[3] = 10;
      break;
    case CHORD_QUALITY_DOM7:
      notes[0] = 0;
      notes[1] = 4;
      notes[2] = 7;
      notes[3] = 10;
      break;
    case CHORD_QUALITY_HALF_DIM:
      notes[0] = 0;
      notes[1] = 3;
      notes[2] = 6;
      notes[3] = 10;
      break;
    default:
      break;
    }

    switch (inversion) {
    case CHORD_INVERSION_ROOT:
      break;
    case CHORD_INVERSION_FIRST:
      notes[0] += 12;
      break;
    case CHORD_INVERSION_SECOND:
      notes[0] += 12;
      notes[1] += 12;
      break;
    case CHORD_INVERSION_THIRD:
      notes[0] += 12;
      notes[1] += 12;
      notes[2] += 12;
      break;
    default:
      break;
    }

    std::sort(&notes[0], &notes[4]);

    switch (voicing) {
    case CHORD_VOICING_CLOSE:
      break;
    case CHORD_VOICING_DROP_2:
      notes[1] -= 12;
      break;
    case CHORD_VOICING_DROP_3:
      notes[2] -= 12;
      break;
    case CHORD_VOICING_SPREAD:
      notes[0] -= 12;
      notes[3] += 12;
      break;
    default:
      break;
    }
  }
};

} // namespace chords

#endif // CHORDS_CHORD_H_
