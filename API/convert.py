#Based on homemadegarbage.com
#Thanks for the knowledge
#Developed by Corebb

import numpy as np
import os
import math
from PIL import Image
 
#配置
#Frame = 5 #指定帧数量
NUMPIXELS = 80 #单边LED数量
Div = 320#1圈分割数
Bright = 70 #輝度
Led0Bright = 15 #中心LEDの輝度 [%]

gif_file_name = "magiccool.gif"
im = Image.open(gif_file_name)
print(im.is_animated)
print(im.n_frames)
Frame=im.n_frames


 

#画像変換関数
def polarConv(imgOrgin, frame):
    
    h = imgOrgin.height #帧尺寸
    w = imgOrgin.width
 
    #画像縮小
    imgRedu = imgOrgin.resize((math.floor((NUMPIXELS * 2 -1)/h *w), NUMPIXELS * 2 -1))
    #imgRedu.save(str(frame)+'.png')   #输出缩小后的原图像
    imgArray=np.array(imgRedu)
    #縮小画像中心座標
    h2 = imgRedu.height
    w2 = imgRedu.width
    wC = math.floor(w2 / 2)
    hC = math.floor(h2 / 2)
 
    #極座標変換画像準備
    imgPolar = Image.new('RGB', (NUMPIXELS, Div))
    
 
    #極座標変換
    for j in range(0, Div):
        for i in range(0, hC+1):
            #座標色取得
            rP = int(imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 0] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100)
            gP = int(imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 1] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100)
            bP = int(imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 2] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100)
            imgPolar.putpixel((i,j), (rP, gP, bP))

    imgPolar.save(str(frame)+'.jpg',quality=90, optimize=True) #输出极坐标变换后的图像
            
 

    
    
for i in range(Frame):
    #输出每一帧
    frame = im.convert('RGBA') #如果是RGB的话，有的透明背景GIF不兼容
    polarConv(frame, i)
    if i != Frame-1:
        im.seek(im.tell()+1)

    