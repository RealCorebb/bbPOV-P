#Based on homemadegarbage.com
#Thanks for the knowledge
#Developed by Corebb

import numpy as np
import os
import math
from PIL import Image
import base64
from io import BytesIO
import flask
from flask import request,json,make_response
import gzip
app = flask.Flask(__name__)
data = []

#画像変換関数
def polarConv(imgOrgin, frame,NUMPIXELS,Div):
    
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
            rP = imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 0]
                     
            gP = imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 1]
                     
            bP = imgArray[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 2]
            imgPolar.putpixel((i,j), (rP, gP, bP))
    buffered = BytesIO()
    imgPolar.save(buffered,format="BMP") #输出极坐标变换后的图像
    data.append(base64.b64encode(buffered.getvalue()).decode('utf-8'))        

@app.route('/', methods=['POST'])
def convert():
    data.clear()
    gif_file_name = request.files['img']
    NUMPIXELS = int(request.form['NUMPIXELS']) #半径灯条数
    Div = int(request.form['Div']) #1圈分割数
    im = Image.open(gif_file_name)
    print(im.is_animated)
    print(im.n_frames)
    Frame=im.n_frames
    for i in range(Frame):
        #输出每一帧
        frame = im.convert('RGBA') #如果是RGB的话，有的透明背景GIF不兼容
        polarConv(frame, i, NUMPIXELS, Div)
        if i != Frame-1:
            im.seek(im.tell()+1)
    content = gzip.compress(json.dumps(data).encode('utf8'),5)    
    response = make_response(content)
    response.headers['Content-Type'] = 'application/json'
    response.headers['Content-length'] = len(content)
    response.headers['Content-Encoding'] = 'gzip'
    return response
app.run()






 


 

    

    