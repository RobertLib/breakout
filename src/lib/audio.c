#include "audio.h"

#define SAMPLE_RATE 44100
#define SFX_TRACK_POOL 10

static MIX_Audio *sfxAudio[SFX_COUNT];
static MIX_Audio *musicAudio[MUSIC_COUNT];

static MIX_Track *musicTrack;
static MIX_Track *sfxTracks[SFX_TRACK_POOL];
static int nextSfxTrack;

static MusicTrack currentMusic = MUSIC_NONE;

static float musicVolume = 0.7f;
static float sfxVolume = 0.8f;

// ---------------------------------------------------------------------------
// Synth core
// ---------------------------------------------------------------------------

typedef enum Wave
{
  W_SINE,
  W_SQUARE,
  W_SAW,
  W_TRI,
  W_NOISE
} Wave;

static unsigned int noiseState = 0x12345678;

static float noiseSample(void)
{
  noiseState ^= noiseState << 13;
  noiseState ^= noiseState >> 17;
  noiseState ^= noiseState << 5;
  return ((noiseState & 0xffffff) / 8388608.0f) - 1.0f;
}

// Render one tone into the float buffer.
// f0 slides to f1 over the duration. Envelope: linear attack, exponential decay.
static void tone(float *buf, int bufLen, double start, double dur,
                 float f0, float f1, Wave wave, float vol,
                 float attack, float decay, float vibrato)
{
  int s0 = (int)(start * SAMPLE_RATE);
  int n = (int)(dur * SAMPLE_RATE);
  double phase = 0;

  for (int i = 0; i < n; i++)
  {
    int idx = s0 + i;
    if (idx < 0 || idx >= bufLen)
    {
      continue;
    }

    float t = i / (float)SAMPLE_RATE;
    float progress = i / (float)n;
    float freq = f0 + (f1 - f0) * progress;

    if (vibrato > 0)
    {
      freq *= 1.0f + vibrato * sinf(t * 6.0f * 2.0f * SDL_PI_F);
    }

    phase += 2.0 * SDL_PI_D * freq / SAMPLE_RATE;

    float sample = 0;
    switch (wave)
    {
    case W_SINE:
      sample = sinf((float)phase);
      break;
    case W_SQUARE:
      // Soft square: rounded edges, less harsh than a hard clip
      sample = tanhf(3.0f * sinf((float)phase));
      break;
    case W_SAW:
    {
      float p = (float)(phase / (2.0 * SDL_PI_D));
      sample = 2.0f * (p - floorf(p)) - 1.0f;
      sample = tanhf(1.5f * sample);
      break;
    }
    case W_TRI:
      sample = (2.0f / SDL_PI_F) * asinf(sinf((float)phase));
      break;
    case W_NOISE:
      sample = noiseSample();
      break;
    }

    float env = 1.0f;
    if (attack > 0 && t < attack)
    {
      env = t / attack;
    }
    env *= expf(-decay * t);

    // Short fade-out at the very end to avoid clicks
    int left = n - i;
    if (left < 220)
    {
      env *= left / 220.0f;
    }

    buf[idx] += sample * vol * env;
  }
}

static float midiFreq(int midi)
{
  return 440.0f * powf(2.0f, (midi - 69) / 12.0f);
}

// Drums
static void drumKick(float *buf, int bufLen, double start, float vol)
{
  tone(buf, bufLen, start, 0.16, 130, 38, W_SINE, vol, 0.002f, 16.0f, 0);
}

static void drumSnare(float *buf, int bufLen, double start, float vol)
{
  tone(buf, bufLen, start, 0.14, 0, 0, W_NOISE, vol * 0.8f, 0.001f, 24.0f, 0);
  tone(buf, bufLen, start, 0.08, 200, 160, W_TRI, vol * 0.5f, 0.001f, 30.0f, 0);
}

static void drumHat(float *buf, int bufLen, double start, float vol)
{
  tone(buf, bufLen, start, 0.035, 0, 0, W_NOISE, vol, 0.001f, 90.0f, 0);
}

// Simple feedback delay — instant depth for melodic content
static void applyEcho(float *buf, int n, double delaySec, float feedback)
{
  int d = (int)(delaySec * SAMPLE_RATE);

  for (int i = d; i < n; i++)
  {
    buf[i] += buf[i - d] * feedback;
  }
}

// Convert float buffer to a MIX_Audio (16-bit mono), with soft limiting
static MIX_Audio *finishBuffer(float *buf, int n)
{
#ifdef DEBUG
  {
    float peak = 0, sum = 0;
    for (int i = 0; i < n; i++)
    {
      float a = fabsf(buf[i]);
      if (a > peak) peak = a;
      sum += buf[i] * buf[i];
    }
    printf("audio buffer: %.2fs peak=%.2f rms=%.3f\n",
           n / (float)SAMPLE_RATE, peak, sqrtf(sum / n));
  }
#endif

  Sint16 *pcm = (Sint16 *)SDL_malloc((size_t)n * sizeof(Sint16));

  if (pcm == NULL)
  {
    SDL_free(buf);
    return NULL;
  }

  for (int i = 0; i < n; i++)
  {
    float v = tanhf(buf[i] * 1.2f);
    pcm[i] = (Sint16)(v * 32000.0f);
  }

  SDL_AudioSpec spec = {SDL_AUDIO_S16, 1, SAMPLE_RATE};
  MIX_Audio *audio = MIX_LoadRawAudio(mixer, pcm, (size_t)n * sizeof(Sint16), &spec);

  if (audio == NULL)
  {
    fprintf(stderr, "Unable to load synthesized audio! SDL_mixer Error: %s\n", SDL_GetError());
  }

  SDL_free(pcm);
  SDL_free(buf);

  return audio;
}

static float *newBuffer(double seconds, int *outLen)
{
  int n = (int)(seconds * SAMPLE_RATE);
  float *buf = (float *)SDL_calloc((size_t)n, sizeof(float));

  if (buf == NULL)
  {
    fprintf(stderr, "Audio buffer allocation failed.\n");
    exit(1);
  }

  *outLen = n;
  return buf;
}

// ---------------------------------------------------------------------------
// Sound effects
// ---------------------------------------------------------------------------

static MIX_Audio *makeSfx(Sfx which)
{
  float *buf;
  int n;

  switch (which)
  {
  case SFX_BALL_PADDLE:
    buf = newBuffer(0.09, &n);
    tone(buf, n, 0, 0.07, 320, 210, W_TRI, 0.55f, 0.001f, 26.0f, 0);
    tone(buf, n, 0, 0.04, 640, 500, W_SINE, 0.2f, 0.001f, 40.0f, 0);
    break;

  case SFX_BALL_WALL:
    buf = newBuffer(0.06, &n);
    tone(buf, n, 0, 0.05, 190, 150, W_SINE, 0.4f, 0.001f, 40.0f, 0);
    break;

  case SFX_BRICK_HIT:
    buf = newBuffer(0.07, &n);
    tone(buf, n, 0, 0.05, 480, 430, W_SQUARE, 0.3f, 0.001f, 45.0f, 0);
    tone(buf, n, 0, 0.02, 0, 0, W_NOISE, 0.12f, 0.001f, 90.0f, 0);
    break;

  case SFX_BRICK_BREAK:
    buf = newBuffer(0.12, &n);
    tone(buf, n, 0, 0.09, 720, 380, W_SQUARE, 0.35f, 0.001f, 30.0f, 0);
    tone(buf, n, 0, 0.05, 0, 0, W_NOISE, 0.22f, 0.001f, 50.0f, 0);
    break;

  case SFX_EXPLOSION:
    buf = newBuffer(0.55, &n);
    tone(buf, n, 0, 0.5, 0, 0, W_NOISE, 0.55f, 0.002f, 9.0f, 0);
    tone(buf, n, 0, 0.4, 100, 34, W_SINE, 0.7f, 0.002f, 9.0f, 0);
    break;

  case SFX_GOLD:
    buf = newBuffer(0.3, &n);
    tone(buf, n, 0.0, 0.12, 880, 880, W_SINE, 0.3f, 0.002f, 14.0f, 0);
    tone(buf, n, 0.07, 0.18, 1318, 1318, W_SINE, 0.3f, 0.002f, 12.0f, 0);
    tone(buf, n, 0.13, 0.15, 1760, 1760, W_SINE, 0.2f, 0.002f, 12.0f, 0);
    break;

  case SFX_CRYSTAL:
    buf = newBuffer(0.7, &n);
    tone(buf, n, 0.0, 0.6, 1046, 1046, W_SINE, 0.3f, 0.003f, 5.0f, 0.004f);
    tone(buf, n, 0.05, 0.55, 1568, 1568, W_SINE, 0.22f, 0.003f, 5.0f, 0.004f);
    tone(buf, n, 0.1, 0.5, 2093, 2093, W_SINE, 0.13f, 0.003f, 6.0f, 0.004f);
    break;

  case SFX_POWERUP_GOOD:
  {
    buf = newBuffer(0.4, &n);
    const int notes[4] = {72, 76, 79, 84}; // C5 E5 G5 C6
    for (int i = 0; i < 4; i++)
    {
      tone(buf, n, i * 0.07, 0.12, midiFreq(notes[i]), midiFreq(notes[i]),
           W_SQUARE, 0.22f, 0.002f, 16.0f, 0);
    }
    break;
  }

  case SFX_POWERUP_BAD:
    buf = newBuffer(0.35, &n);
    tone(buf, n, 0.0, 0.15, 420, 280, W_SAW, 0.3f, 0.002f, 10.0f, 0);
    tone(buf, n, 0.14, 0.2, 280, 150, W_SAW, 0.3f, 0.002f, 10.0f, 0);
    break;

  case SFX_SHOOT:
    buf = newBuffer(0.14, &n);
    tone(buf, n, 0, 0.12, 950, 180, W_SAW, 0.3f, 0.001f, 16.0f, 0);
    break;

  case SFX_ENEMY_HIT:
    buf = newBuffer(0.28, &n);
    tone(buf, n, 0, 0.22, 340, 70, W_SQUARE, 0.35f, 0.001f, 12.0f, 0);
    tone(buf, n, 0, 0.12, 0, 0, W_NOISE, 0.2f, 0.001f, 22.0f, 0);
    break;

  case SFX_LIFE_LOST:
  {
    buf = newBuffer(0.9, &n);
    const int notes[3] = {69, 65, 62}; // A4 F4 D4
    for (int i = 0; i < 3; i++)
    {
      tone(buf, n, i * 0.18, 0.3, midiFreq(notes[i]), midiFreq(notes[i]),
           W_TRI, 0.35f, 0.004f, 7.0f, 0.006f);
    }
    tone(buf, n, 0.5, 0.35, 80, 45, W_SINE, 0.4f, 0.01f, 8.0f, 0);
    break;
  }

  case SFX_LEVEL_COMPLETE:
  {
    buf = newBuffer(1.4, &n);
    const int notes[4] = {72, 76, 79, 84};
    for (int i = 0; i < 4; i++)
    {
      tone(buf, n, i * 0.11, 0.16, midiFreq(notes[i]), midiFreq(notes[i]),
           W_SQUARE, 0.25f, 0.002f, 10.0f, 0);
    }
    // Closing chord
    tone(buf, n, 0.5, 0.8, midiFreq(72), midiFreq(72), W_TRI, 0.22f, 0.01f, 4.0f, 0.004f);
    tone(buf, n, 0.5, 0.8, midiFreq(76), midiFreq(76), W_TRI, 0.18f, 0.01f, 4.0f, 0.004f);
    tone(buf, n, 0.5, 0.8, midiFreq(79), midiFreq(79), W_TRI, 0.18f, 0.01f, 4.0f, 0.004f);
    tone(buf, n, 0.5, 0.8, midiFreq(88), midiFreq(88), W_SINE, 0.14f, 0.01f, 4.0f, 0.004f);
    break;
  }

  case SFX_MENU_MOVE:
    buf = newBuffer(0.05, &n);
    tone(buf, n, 0, 0.04, 700, 700, W_SINE, 0.3f, 0.001f, 50.0f, 0);
    break;

  case SFX_MENU_SELECT:
    buf = newBuffer(0.15, &n);
    tone(buf, n, 0, 0.06, 520, 520, W_SQUARE, 0.25f, 0.001f, 20.0f, 0);
    tone(buf, n, 0.05, 0.09, 780, 780, W_SQUARE, 0.25f, 0.001f, 18.0f, 0);
    break;

  case SFX_SHIELD:
    buf = newBuffer(0.16, &n);
    tone(buf, n, 0, 0.14, 240, 520, W_TRI, 0.4f, 0.002f, 14.0f, 0);
    break;

  case SFX_CATCH:
    buf = newBuffer(0.1, &n);
    tone(buf, n, 0, 0.08, 620, 300, W_TRI, 0.35f, 0.001f, 20.0f, 0);
    break;

  case SFX_GAME_OVER:
  {
    buf = newBuffer(2.4, &n);
    const int melody[5] = {76, 72, 69, 64, 57}; // E5 C5 A4 E4 A3
    for (int i = 0; i < 5; i++)
    {
      tone(buf, n, i * 0.28, 0.5, midiFreq(melody[i]), midiFreq(melody[i]),
           W_TRI, 0.3f, 0.006f, 4.5f, 0.005f);
    }
    // Final minor chord
    tone(buf, n, 1.4, 0.9, midiFreq(45), midiFreq(45), W_SAW, 0.16f, 0.02f, 3.0f, 0);
    tone(buf, n, 1.4, 0.9, midiFreq(57), midiFreq(57), W_TRI, 0.2f, 0.02f, 3.0f, 0.004f);
    tone(buf, n, 1.4, 0.9, midiFreq(60), midiFreq(60), W_TRI, 0.18f, 0.02f, 3.0f, 0.004f);
    tone(buf, n, 1.4, 0.9, midiFreq(64), midiFreq(64), W_TRI, 0.18f, 0.02f, 3.0f, 0.004f);
    applyEcho(buf, n, 0.22, 0.25f);
    break;
  }

  case SFX_WIN:
  {
    buf = newBuffer(2.8, &n);
    const int melody[6] = {72, 76, 79, 84, 79, 84}; // triumphant C major runs
    for (int i = 0; i < 6; i++)
    {
      tone(buf, n, i * 0.16, 0.24, midiFreq(melody[i]), midiFreq(melody[i]),
           W_SQUARE, 0.22f, 0.003f, 8.0f, 0);
    }
    tone(buf, n, 1.1, 1.2, midiFreq(72), midiFreq(72), W_TRI, 0.22f, 0.02f, 2.5f, 0.005f);
    tone(buf, n, 1.1, 1.2, midiFreq(76), midiFreq(76), W_TRI, 0.2f, 0.02f, 2.5f, 0.005f);
    tone(buf, n, 1.1, 1.2, midiFreq(79), midiFreq(79), W_TRI, 0.2f, 0.02f, 2.5f, 0.005f);
    tone(buf, n, 1.1, 1.2, midiFreq(88), midiFreq(88), W_SINE, 0.16f, 0.02f, 2.5f, 0.005f);
    applyEcho(buf, n, 0.19, 0.28f);
    break;
  }

  default:
    buf = newBuffer(0.05, &n);
    break;
  }

  return finishBuffer(buf, n);
}

// ---------------------------------------------------------------------------
// Music
// ---------------------------------------------------------------------------

// Chord quality: semitone offset of the third
typedef struct ChordDef
{
  int root;  // MIDI
  int third; // 3 = minor, 4 = major
} ChordDef;

static void padChord(float *buf, int n, double start, double dur, ChordDef c, float vol)
{
  tone(buf, n, start, dur, midiFreq(c.root), midiFreq(c.root), W_TRI, vol, 0.4f, 0.6f, 0.003f);
  tone(buf, n, start, dur, midiFreq(c.root + c.third), midiFreq(c.root + c.third), W_TRI, vol * 0.8f, 0.5f, 0.6f, 0.003f);
  tone(buf, n, start, dur, midiFreq(c.root + 7), midiFreq(c.root + 7), W_TRI, vol * 0.8f, 0.5f, 0.6f, 0.003f);
}

static MIX_Audio *makeMenuMusic(void)
{
  const double bpm = 90.0;
  const double beat = 60.0 / bpm;
  const int bars = 8;
  const double total = bars * 4 * beat;

  int n;
  float *buf = newBuffer(total, &n);

  const ChordDef prog[4] = {{57, 3}, {53, 4}, {48, 4}, {55, 4}}; // Am F C G

  for (int bar = 0; bar < bars; bar++)
  {
    ChordDef c = prog[bar % 4];
    double t0 = bar * 4 * beat;

    // Warm pad
    padChord(buf, n, t0, 4 * beat, c, 0.07f);

    // Soft bass on beats 1 and 3
    tone(buf, n, t0, 1.6 * beat, midiFreq(c.root - 12), midiFreq(c.root - 12),
         W_SINE, 0.2f, 0.01f, 1.2f, 0);
    tone(buf, n, t0 + 2 * beat, 1.6 * beat, midiFreq(c.root - 12), midiFreq(c.root - 12),
         W_SINE, 0.17f, 0.01f, 1.2f, 0);

    // Dreamy eighth-note arpeggio
    const int arpOffsets[8] = {12, 19, 24, 24 + 3, 24, 19, 12, 19};
    for (int i = 0; i < 8; i++)
    {
      int offset = arpOffsets[i];
      if (offset == 27 && c.third == 4)
      {
        offset = 28; // match major chords
      }
      tone(buf, n, t0 + i * 0.5 * beat, 0.45 * beat,
           midiFreq(c.root + offset), midiFreq(c.root + offset),
           W_SINE, 0.11f, 0.004f, 5.0f, 0);
    }

    // Occasional high sparkle
    if (bar % 2 == 1)
    {
      tone(buf, n, t0 + 3 * beat, 1.5 * beat, midiFreq(c.root + 31), midiFreq(c.root + 31),
           W_SINE, 0.05f, 0.05f, 2.0f, 0.006f);
    }
  }

  applyEcho(buf, n, 0.5 * beat, 0.3f);

  return finishBuffer(buf, n);
}

// Shared game-track builder; tracks differ in tempo, key, melody, lead
// timbre and drive.
typedef struct MelodyNote
{
  float startBeat; // within a 4-bar phrase (0..16)
  float durBeats;
  int midi;
} MelodyNote;

static MIX_Audio *makeGameMusic(double bpm, const ChordDef *prog,
                                const MelodyNote *melody, int melodyLen,
                                int melodyTranspose, Wave leadWave,
                                bool driving)
{
  const double beat = 60.0 / bpm;
  const int bars = 16;
  const double total = bars * 4 * beat;

  int n;
  float *buf = newBuffer(total, &n);

  for (int bar = 0; bar < bars; bar++)
  {
    ChordDef c = prog[bar % 4];
    double t0 = bar * 4 * beat;

    // Bass: octave pump (8ths) or driving 16ths
    if (driving)
    {
      for (int i = 0; i < 16; i++)
      {
        int note = c.root - 12 + ((i % 4 == 2) ? 12 : 0);
        float accent = (i % 4 == 0) ? 1.0f : 0.7f;
        tone(buf, n, t0 + i * 0.25 * beat, 0.2 * beat,
             midiFreq(note), midiFreq(note), W_SAW, 0.085f * accent, 0.002f, 9.0f, 0);
      }
    }
    else
    {
      for (int i = 0; i < 8; i++)
      {
        int note = c.root - 12 + ((i % 2 == 1) ? 12 : 0);
        tone(buf, n, t0 + i * 0.5 * beat, 0.4 * beat,
             midiFreq(note), midiFreq(note), W_SAW, 0.09f, 0.002f, 7.0f, 0);
      }
    }

    // Drums
    for (int b = 0; b < 4; b++)
    {
      drumKick(buf, n, t0 + b * beat, 0.5f);
      drumHat(buf, n, t0 + (b + 0.5) * beat, 0.10f);
      if (driving)
      {
        drumHat(buf, n, t0 + (b + 0.25) * beat, 0.05f);
        drumHat(buf, n, t0 + (b + 0.75) * beat, 0.05f);
      }
    }
    drumSnare(buf, n, t0 + 1 * beat, 0.3f);
    drumSnare(buf, n, t0 + 3 * beat, 0.3f);

    // Off-beat chord stabs
    tone(buf, n, t0 + 1.5 * beat, 0.2 * beat, midiFreq(c.root + 12), midiFreq(c.root + 12),
         W_SQUARE, 0.05f, 0.002f, 14.0f, 0);
    tone(buf, n, t0 + 1.5 * beat, 0.2 * beat, midiFreq(c.root + 12 + c.third), midiFreq(c.root + 12 + c.third),
         W_SQUARE, 0.04f, 0.002f, 14.0f, 0);
    tone(buf, n, t0 + 3.5 * beat, 0.2 * beat, midiFreq(c.root + 19), midiFreq(c.root + 19),
         W_SQUARE, 0.05f, 0.002f, 14.0f, 0);
  }

  // Lead melody: a 4-bar phrase repeated; second half plays an octave up, quieter
  for (int phrase = 0; phrase < 4; phrase++)
  {
    double phraseStart = phrase * 4 * 4 * beat;
    int transpose = (phrase >= 2) ? 12 : 0;
    float vol = (phrase >= 2) ? 0.05f : 0.07f;

    for (int i = 0; i < melodyLen; i++)
    {
      const MelodyNote *m = &melody[i];
      int midi = m->midi + melodyTranspose + transpose;
      tone(buf, n, phraseStart + m->startBeat * beat, m->durBeats * beat * 0.92,
           midiFreq(midi), midiFreq(midi),
           leadWave, vol, 0.008f, 2.2f, 0.004f);
    }
  }

  applyEcho(buf, n, 0.375 * beat, 0.22f);

  return finishBuffer(buf, n);
}

// Chord progressions (i VI III VII in four minor keys)
static const ChordDef progAm[4] = {{57, 3}, {53, 4}, {48, 4}, {55, 4}}; // Am F C G
static const ChordDef progEm[4] = {{52, 3}, {48, 4}, {55, 4}, {50, 4}}; // Em C G D
static const ChordDef progDm[4] = {{50, 3}, {46, 4}, {53, 4}, {48, 4}}; // Dm Bb F C
static const ChordDef progCm[4] = {{48, 3}, {44, 4}, {51, 4}, {46, 4}}; // Cm Ab Eb Bb

// Four melody shapes, written in A minor and transposed per track.
// "Anthem": stepwise and singable
static const MelodyNote melodyAnthem[] = {
    // Bar 1
    {0.0f, 1.0f, 69}, {1.0f, 0.5f, 72}, {1.5f, 0.5f, 74}, {2.0f, 1.0f, 76}, {3.0f, 0.5f, 74}, {3.5f, 0.5f, 72},
    // Bar 2
    {4.0f, 1.5f, 69}, {5.5f, 0.5f, 67}, {6.0f, 2.0f, 69},
    // Bar 3
    {8.0f, 1.0f, 72}, {9.0f, 0.5f, 74}, {9.5f, 0.5f, 76}, {10.0f, 1.0f, 79}, {11.0f, 1.0f, 76},
    // Bar 4
    {12.0f, 1.5f, 74}, {13.5f, 0.5f, 72}, {14.0f, 2.0f, 69}};

// "Rush": tense sixteenth-flavored runs
static const MelodyNote melodyRush[] = {
    // Bar 1
    {0.0f, 0.5f, 74}, {0.5f, 0.5f, 77}, {1.0f, 0.5f, 76}, {1.5f, 0.5f, 74}, {2.0f, 1.0f, 69}, {3.0f, 0.5f, 74}, {3.5f, 0.5f, 72},
    // Bar 2
    {4.0f, 1.0f, 70}, {5.0f, 0.5f, 69}, {5.5f, 0.5f, 67}, {6.0f, 2.0f, 69},
    // Bar 3
    {8.0f, 0.5f, 74}, {8.5f, 0.5f, 76}, {9.0f, 0.5f, 77}, {9.5f, 0.5f, 79}, {10.0f, 1.0f, 81}, {11.0f, 1.0f, 77},
    // Bar 4
    {12.0f, 0.5f, 76}, {12.5f, 0.5f, 74}, {13.0f, 1.0f, 72}, {14.0f, 2.0f, 74}};

// "Drift": long floating notes, wide intervals
static const MelodyNote melodyDrift[] = {
    // Bar 1
    {0.0f, 2.0f, 76}, {2.0f, 1.0f, 74}, {3.0f, 1.0f, 72},
    // Bar 2
    {4.0f, 2.0f, 69}, {6.0f, 1.0f, 71}, {7.0f, 1.0f, 72},
    // Bar 3
    {8.0f, 2.0f, 77}, {10.0f, 1.0f, 76}, {11.0f, 1.0f, 72},
    // Bar 4
    {12.0f, 1.5f, 74}, {13.5f, 0.5f, 71}, {14.0f, 2.0f, 69}};

// "Riff": syncopated and punchy
static const MelodyNote melodyRiff[] = {
    // Bar 1
    {0.0f, 0.5f, 69}, {0.5f, 0.5f, 69}, {1.0f, 1.0f, 72}, {2.0f, 0.5f, 69}, {2.5f, 0.5f, 67}, {3.0f, 1.0f, 64},
    // Bar 2
    {4.0f, 0.5f, 65}, {4.5f, 0.5f, 67}, {5.0f, 1.0f, 69}, {6.0f, 2.0f, 72},
    // Bar 3
    {8.0f, 0.5f, 74}, {8.5f, 0.5f, 72}, {9.0f, 0.5f, 71}, {9.5f, 0.5f, 72}, {10.0f, 1.0f, 74}, {11.0f, 1.0f, 76},
    // Bar 4
    {12.0f, 1.0f, 72}, {13.0f, 0.5f, 71}, {13.5f, 0.5f, 67}, {14.0f, 2.0f, 69}};

typedef struct GameTrackDef
{
  double bpm;
  const ChordDef *prog;
  const MelodyNote *melody;
  int melodyLen;
  int transpose; // semitones applied to the melody to match the key
  Wave leadWave;
  bool driving;
} GameTrackDef;

#define MELODY(m) m, (int)(sizeof(m) / sizeof(m[0]))

// Two tracks per world; each world has its own key and mood, the pair
// within a world differs in melody, lead timbre and tempo
static const GameTrackDef gameTracks[WORLD_COUNT * GAME_TRACKS_PER_WORLD] = {
    // World 1 — A minor, upbeat
    {128.0, progAm, MELODY(melodyAnthem), 0, W_SQUARE, false},
    {122.0, progAm, MELODY(melodyDrift), 0, W_TRI, false},
    // World 2 — E minor, brighter and busier
    {132.0, progEm, MELODY(melodyRiff), 7, W_SQUARE, false},
    {134.0, progEm, MELODY(melodyAnthem), -5, W_SAW, true},
    // World 3 — D minor, tense and driving
    {140.0, progDm, MELODY(melodyRush), 0, W_SQUARE, true},
    {144.0, progDm, MELODY(melodyDrift), 5, W_SAW, true},
    // World 4 — C minor, relentless
    {148.0, progCm, MELODY(melodyRiff), 3, W_SAW, true},
    {152.0, progCm, MELODY(melodyRush), -2, W_SQUARE, true}};

MusicTrack musicForLevel(int level)
{
  int world = worldForLevel(level);
  int variant = level % GAME_TRACKS_PER_WORLD;

  return (MusicTrack)(MUSIC_GAME_FIRST + world * GAME_TRACKS_PER_WORLD + variant);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void initializeAudio(void)
{
  if (mixer == NULL)
  {
    return;
  }

  for (int i = 0; i < SFX_COUNT; i++)
  {
    sfxAudio[i] = makeSfx((Sfx)i);
  }

  musicAudio[MUSIC_MENU] = makeMenuMusic();

  for (int i = 0; i < WORLD_COUNT * GAME_TRACKS_PER_WORLD; i++)
  {
    const GameTrackDef *def = &gameTracks[i];
    musicAudio[MUSIC_GAME_FIRST + i] = makeGameMusic(
        def->bpm, def->prog, def->melody, def->melodyLen,
        def->transpose, def->leadWave, def->driving);
  }

  musicTrack = MIX_CreateTrack(mixer);

  for (int i = 0; i < SFX_TRACK_POOL; i++)
  {
    sfxTracks[i] = MIX_CreateTrack(mixer);
  }
}

void destroyAudio(void)
{
  if (mixer == NULL)
  {
    return;
  }

  stopMusic();

  if (musicTrack != NULL)
  {
    MIX_DestroyTrack(musicTrack);
    musicTrack = NULL;
  }

  for (int i = 0; i < SFX_TRACK_POOL; i++)
  {
    if (sfxTracks[i] != NULL)
    {
      MIX_DestroyTrack(sfxTracks[i]);
      sfxTracks[i] = NULL;
    }
  }

  for (int i = 0; i < SFX_COUNT; i++)
  {
    if (sfxAudio[i] != NULL)
    {
      MIX_DestroyAudio(sfxAudio[i]);
      sfxAudio[i] = NULL;
    }
  }

  for (int i = 0; i < MUSIC_COUNT; i++)
  {
    if (musicAudio[i] != NULL)
    {
      MIX_DestroyAudio(musicAudio[i]);
      musicAudio[i] = NULL;
    }
  }
}

void playSfx(Sfx sfx)
{
  if (mixer == NULL || sfxAudio[sfx] == NULL || sfxVolume <= 0.001f)
  {
    return;
  }

  MIX_Track *track = sfxTracks[nextSfxTrack];
  nextSfxTrack = (nextSfxTrack + 1) % SFX_TRACK_POOL;

  // A mixer is not a promise of a track: MIX_CreateTrack() can fail on its own
  // in initializeAudio(), and every check up to here only asked about the mixer.
  if (track == NULL)
  {
    return;
  }

  MIX_SetTrackAudio(track, sfxAudio[sfx]);
  MIX_SetTrackGain(track, sfxVolume);
  MIX_PlayTrack(track, 0);
}

void playMusic(MusicTrack track)
{
  // musicTrack for the same reason playSfx() checks its own: it is a separate
  // MIX_CreateTrack() that can fail while the mixer itself is fine.
  if (mixer == NULL || musicTrack == NULL || track == MUSIC_NONE ||
      musicAudio[track] == NULL)
  {
    return;
  }

  if (currentMusic == track && MIX_TrackPlaying(musicTrack))
  {
    return;
  }

  MIX_StopTrack(musicTrack, 0);
  MIX_SetTrackAudio(musicTrack, musicAudio[track]);
  MIX_SetTrackGain(musicTrack, musicVolume);

  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
  MIX_PlayTrack(musicTrack, props);
  SDL_DestroyProperties(props);

  currentMusic = track;
}

void stopMusic(void)
{
  if (mixer == NULL || musicTrack == NULL)
  {
    return;
  }

  MIX_StopTrack(musicTrack, MIX_TrackMSToFrames(musicTrack, 300));
  currentMusic = MUSIC_NONE;
}

void setMusicVolume(float v)
{
  musicVolume = clamp(v, 0.0f, 1.0f);

  if (mixer != NULL && musicTrack != NULL)
  {
    MIX_SetTrackGain(musicTrack, musicVolume);
  }
}

void setSfxVolume(float v)
{
  sfxVolume = clamp(v, 0.0f, 1.0f);
}
