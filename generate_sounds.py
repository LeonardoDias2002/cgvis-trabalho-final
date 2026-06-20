import wave, math, random, struct

def save_wav(filename, samples, sample_rate=44100):
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        f.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, s)) * 32767)) for s in samples))

def gen_hit():
    sr = 44100
    length = int(sr * 0.15)
    samples = []
    for i in range(length):
        env = math.exp(-i / (sr * 0.03))
        s = random.uniform(-1, 1) * env
        samples.append(s)
    save_wav('data/hit.wav', samples)

def gen_victory():
    sr = 44100
    length = int(sr * 1.5)
    samples = []
    freqs = [523.25, 659.25, 783.99, 1046.50] # C E G C
    for i in range(length):
        t = i / sr
        s = 0
        for j, f in enumerate(freqs):
            start = j * 0.15
            if t > start:
                env = math.exp(-(t - start) * 2)
                s += math.sin(2 * math.pi * f * (t - start)) * env * 0.3
        samples.append(s)
    save_wav('data/vitoria.wav', samples)

def gen_ambient():
    sr = 44100
    length = int(sr * 5.0) # 5 seconds loop
    samples = []
    for i in range(length):
        t = i / sr
        env = 0.5 + 0.3 * math.sin(2 * math.pi * 0.2 * t)
        s = random.uniform(-1, 1) * env * 0.05 # very quiet wind
        # add occasional bird chirp
        if t % 2.5 < 0.2:
            bird_env = math.exp(-(t % 2.5) * 20)
            s += math.sin(2 * math.pi * 2000 * t + math.sin(2 * math.pi * 50 * t)) * bird_env * 0.1
        samples.append(s)
    save_wav('data/ambiente.wav', samples)

def gen_menu():
    sr = 44100
    length = int(sr * 4.0)
    samples = []
    for i in range(length):
        t = i / sr
        chord_idx = int(t * 2) % 4
        # simple bossa progression Cmaj7 -> Dmin7 -> G7 -> Cmaj7
        chords = [
            [261.63, 329.63, 392.00, 493.88], # Cmaj7
            [293.66, 349.23, 440.00, 523.25], # Dm7
            [392.00, 493.88, 587.33, 698.46], # G7
            [261.63, 329.63, 392.00, 493.88]  # Cmaj7
        ]
        s = 0
        chord = chords[chord_idx]
        note_idx = int(t * 8) % 4
        env = math.exp(-(t * 8 - int(t * 8)) * 5)
        s += math.sin(2 * math.pi * chord[note_idx] * t) * env * 0.2
        # Add bass
        bass_env = math.exp(-(t * 2 - int(t * 2)) * 3)
        s += math.sin(2 * math.pi * chord[0] * 0.5 * t) * bass_env * 0.3
        samples.append(s)
    save_wav('data/menu.wav', samples)

gen_hit()
gen_victory()
gen_ambient()
gen_menu()
