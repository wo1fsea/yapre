from PIL import Image
import os

def TrimImage(image_filepath, bbox):
    img = Image.open(image_filepath)
    if img.mode != "RGBA":
        img = img.convert("RGBA")
    
    new_img = img.crop(bbox)
    img.close()

    new_img.save(image_filepath)

def ConvertToSize(image_filepath, size):
    new_img = Image.new('RGBA', size, 0x45283c)
    img = Image.open(image_filepath)
    if img.mode != "RGBA":
        img = img.convert("RGBA")
    
    x = int((size[0] - img.size[0]) / 2)
    y = int((size[1] - img.size[1]) / 2)
    new_img.paste(img, (x, y, x+img.size[0], y+img.size[1]))
    
    
    new_img = new_img.resize((64, 64))
    new_img.save(image_filepath)


if __name__ == '__main__':
    img_file_name = "./wizard%i.png"
    for i in range(1024):
        r_path = img_file_name % i
        if os.path.exists(r_path):
            TrimImage(r_path, (8, 90, 127, 209))
            ConvertToSize(r_path, (128, 128))
