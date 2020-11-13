#Based on homemadegarbage.com
#Thanks for the knowledge
#Developed by Corebb
import cv2
import os
import math
from PIL import Image
 
#配置
#Frame = 5 #指定帧数量
NUMPIXELS = 80 #单边LED数量
Div = 240 #1圈分割数
 

gif_file_name = "magic.gif"
im = Image.open(gif_file_name)
print(im.is_animated)

print(im.n_frames)
Frame=im.n_frames
gif = cv2.VideoCapture(gif_file_name)

 
file = open('graphics.bmp', 'w')

#画像変換関数
def polarConv(pic, frame):
    imgOrgin = cv2.imread(pic) #画像データ読み込み
    
    h, w, _ = imgOrgin.shape #画像サイズ取得
 
    #画像縮小
    imgRedu = cv2.resize(imgOrgin,(math.floor((NUMPIXELS * 2 -1)/h *w), NUMPIXELS * 2 -1))
    #cv2.imwrite(str(frame) + '-resize.jpg',imgRedu)
 
    #縮小画像中心座標
    h2, w2, _ = imgRedu.shape
    wC = math.floor(w2 / 2)
    hC = math.floor(h2 / 2)
 
    #極座標変換画像準備
    imgPolar = Image.new('RGB', (NUMPIXELS, Div))
    
 
    #極座標変換
    file.write('\t[\n')
    for j in range(0, Div):
        file.write('\t\t[')
        for i in range(0, hC+1):
            #座標色取得
            #参考：http://peaceandhilightandpython.hatenablog.com/entry/2016/01/03/151320
            rP = int(imgRedu[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 2])
                     
            gP = int(imgRedu[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 1])
                     
            bP = int(imgRedu[hC + math.ceil(i * math.cos(2*math.pi/Div*j)),
                         wC - math.ceil(i * math.sin(2*math.pi/Div*j)), 0])
            
            file.write('"%02X%02X%02X"' % (rP,gP,bP))
            if i != hC:
                file.write(',') 
            imgPolar.putpixel((i,j), (rP, gP, bP))
        if j == Div-1:
            file.write(']\n')
        else:
            file.write('],\n')
    imgPolar.save(str(frame)+'.bmp')
 
 
# スクリーンキャプチャを保存するディレクトリを生成
dir_name = "screen_caps"
if not os.path.exists(dir_name):
    os.mkdir(dir_name)
 

    
    
for i in range(Frame):
    is_success, frame = gif.read()
    
    # 画像ファイルに書き出す
    img_name = str(i) + ".png"
    img_path = os.path.join(dir_name, img_name)
    
    cv2.imwrite(img_path, frame)
    #変換
    polarConv(img_path, i)
    if i!= Frame-1:
        file.write('\t],\n')
    else:
        file.write('\t]\n')
    
 
file.write(']\n')
file.write('}')
file.close()