from PIL import Image

def fix(f):
    try:
        img = Image.open(f).convert('RGBA')
        data = img.getdata()
        target_color = (224, 104, 136)
        new_data = [(255, 255, 255, 0) if p[:3] == target_color else p for p in data]
        img.putdata(new_data)
        img.save(f)
        print('Fixed transparency for', f)
    except Exception as e:
        print("Error processing", f, ":", e)

fix('assets/textures/player.png')
fix('assets/textures/mobs.png')
