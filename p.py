from PIL import Image
import os

def GetPalette(f):
    palette = set()
    img = Image.open(f)
    w, h = img.size
    if img.mode != "RGBA":
        img = img.convert("RGBA")
    
    pa = img.load()
    for x in range(w):
        for y in range(h):
            palette.add(pa[x,y])

    img.close() 
    return palette



def ConvertToPalette(palette, image_filepath):
    palette = list(palette)
    img = Image.open(image_filepath)
    w, h = img.size
    if img.mode != "RGBA":
        img = img.convert("RGBA")


    pa = img.load()
    for x in range(w):
        for y in range(h):
            dis = 255**2 * 4
            dis_idx = 0
            pp = pa[x, y]
            if pp[3] == 0:
                continue

            for i, p in enumerate(palette):
                d = (pp[0] - p[0]) ** 2
                d += (pp[1] - p[1]) ** 2
                d += (pp[2] - p[2]) ** 2
                d += (pp[3] - p[3]) ** 2

                if d < dis:
                    dis_idx = i
                    dis = d
            pa[x, y] = palette[dis_idx]
    
    fp, fe = os.path.splitext(image_filepath)
    img.save(fp + "-2" + fe)


palette = GetPalette("./palette.png")

ConvertToPalette(palette, "./banner.png")

"""
f = "./data/image/animation/blood/%i.png"
for i in xrange(100):
    if not os.path.exists(f % i):
        continue
    ConvertToPalette(palette, f % i)
    """
