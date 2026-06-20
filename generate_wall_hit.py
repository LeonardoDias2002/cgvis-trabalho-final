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

def gen_wall_hit():
    sr = 44100
    length = int(sr * 0.05) # 50ms - very short click
    samples = []
    for i in range(length):
        t = i / sr
        
        # Click (high freq noise)
        click_env = math.exp(-t * 600)
        click = random.uniform(-1, 1) * click_env
        
        # Hard surface resonance
        wood_env = math.exp(-t * 100)
        wood = math.sin(2 * math.pi * 1200 * t) * wood_env * 0.5
        
        # Combine
        s = click * 0.5 + wood * 0.8
        samples.append(s)
        
    save_wav('data/wall.wav', samples)

gen_wall_hit()
