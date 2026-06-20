import wave, math, random, struct

def save_wav(filename, samples, sample_rate=44100):
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        # Normalize and clip
        max_val = max(0.001, max(abs(s) for s in samples))
        normalized = [s / max_val for s in samples]
        f.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, s)) * 32767)) for s in normalized))

def gen_rubber_bounce():
    sr = 44100
    length = int(sr * 0.15) # 150ms
    samples = []
    for i in range(length):
        t = i / sr
        
        # Low frequency punch (body of the bounce)
        env = math.exp(-t * 30)
        # Pitch drop (from ~150Hz to ~50Hz)
        freq = 50 + 100 * math.exp(-t * 100)
        punch = math.sin(2 * math.pi * freq * t) * env
        
        # Tiny bit of noise for the physical contact
        noise_env = math.exp(-t * 200)
        noise = random.uniform(-1, 1) * noise_env * 0.1
        
        samples.append(punch + noise)
        
    save_wav('data/wall.wav', samples)

gen_rubber_bounce()
