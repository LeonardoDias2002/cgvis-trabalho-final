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

def gen_golf_hit():
    sr = 44100
    length = int(sr * 0.15) # 150ms
    samples = []
    for i in range(length):
        t = i / sr
        
        # Thump (low freq)
        thump_env = math.exp(-t * 80)
        thump = math.sin(2 * math.pi * 150 * t) * thump_env
        
        # Click (high freq noise)
        click_env = math.exp(-t * 300)
        click = random.uniform(-1, 1) * click_env
        
        # Wood resonance (mid freq)
        wood_env = math.exp(-t * 50)
        wood = math.sin(2 * math.pi * 800 * t) * wood_env * 0.5
        
        # Combine
        s = thump * 0.5 + click * 0.3 + wood * 0.2
        samples.append(s)
        
    save_wav('data/hit.wav', samples)

gen_golf_hit()
