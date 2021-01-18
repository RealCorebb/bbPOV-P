#Based on homemadegarbage.com
#Thanks for the knowledge
#Developed by Corebb

import numpy as np
import os
import math
from PIL import Image
from PIL import ImageGrab
from datetime import datetime
import cv2

#配置
#Frame = 5 #指定帧数量
NUMPIXELS = 80 #单边LED数量
Div = 360 #1圈分割数
Bright = 100 #輝度
Led0Bright = 3 #中心LEDの輝度 [%]
i = 0


 

#画像変換関数
def polarConv(imgOrgin, frame):
    h = imgOrgin.height #帧尺寸
    w = imgOrgin.width
    #画像縮小
   # imgRedu = imgOrgin.resize((math.floor((NUMPIXELS * 2 -1)/h *w), NUMPIXELS * 2 -1))
   # imgRedu = cv2.cvtColor(np.array(imgRedu), cv2.COLOR_BGR2GRAY)
    #polar_image = cv2.warpPolar(imgRedu,(NUMPIXELS , Div ), (w/2,h/2) ,min(h, w) / 2, 0)
    #cv2.imwrite('sb.jpg',polar_image)
    dateTimeObj = datetime.now()
    print(dateTimeObj)        
 

    
    
while 1:
    #捕捉屏幕
    frame = ImageGrab.grab(bbox=(100,100,160,160)) 
    polarConv(frame, i)
    #frame.save('1.bmp')
    

    