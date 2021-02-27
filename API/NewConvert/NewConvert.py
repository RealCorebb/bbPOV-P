from dearpygui.core import *
from dearpygui.simple import *
import numpy as np
import os
import math
import time
import cv2
from dearpygui.core import *
from dearpygui.simple import *

#配置
NUMPIXELS = 80 #单边LED数量
Div = 320 #1圈分割数
Bright = 60 #輝度
Led0Bright = 15 #中心LEDの輝度 [%]
last_time = time.time()

#Global
gammaCorrection = False
identity = np.arange(256, dtype = np.dtype('uint8'))
gammatable = np.array([0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,
    3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6,
    6,   7,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  11,  11,  11,
    12,  12,  13,  13,  14,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,
    19,  20,  20,  21,  22,  22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,
    29,  30,  30,  31,  32,  33,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,
    41,  42,  43,  43,  44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  53,  54,
    55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  71,
    72,  73,  74,  75,  76,  77,  78,  80,  81,  82,  83,  84,  86,  87,  88,  89,
    91,  92,  93,  94,  96,  97,  98,  100, 101, 102, 104, 105, 106, 108, 109, 110,
    112, 113, 115, 116, 118, 119, 121, 122, 123, 125, 126, 128, 130, 131, 133, 134,
    136, 137, 139, 140, 142, 144, 145, 147, 149, 150, 152, 154, 155, 157, 159, 160,
    162, 164, 166, 167, 169, 171, 173, 175, 176, 178, 180, 182, 184, 186, 187, 189,
    191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
    223, 225, 227, 229, 231, 233, 235, 238, 240, 242, 244, 246, 248, 251, 253, 255])
lut = np.dstack((identity, identity, gammatable))
media_path = ""
save_path = ""

def polarConv(imgOrgin,outputName):
    global gammaCorrection
    h = imgOrgin.shape[0] #帧尺寸
    w = imgOrgin.shape[1]
    #画像縮小 
    imgRedu = cv2.resize(imgOrgin,(math.floor((NUMPIXELS * 2 -1)/h *w), NUMPIXELS * 2 -1))
    #顺时针旋转90度 因为下一步的极坐标转换的0度是正东方向，而我们的POV正北方向为0度
    imgRedu = cv2.rotate(imgRedu,cv2.ROTATE_90_CLOCKWISE)  
    polar_image = cv2.warpPolar(imgRedu,(NUMPIXELS , Div ), (imgRedu.shape[1]/2,imgRedu.shape[0]/2) ,min(imgRedu.shape[0], imgRedu.shape[1]) / 2, 0)
    for i in range(NUMPIXELS):  #亮度处理
         polar_image[:,i,0] = polar_image[:,i,0] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100
         polar_image[:,i,1] = polar_image[:,i,1] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100 
         polar_image[:,i,2] = polar_image[:,i,2] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100 
        #polar_image[:,i,2] = hsv[:,i,2] * ((100 - Led0Bright) / NUMPIXELS * i + Led0Bright) / 100 * Bright /100
    if(gammaCorrection):
        polar_image = cv2.LUT(polar_image, lut)
    cv2.imwrite(outputName+'.jpg',polar_image,[int(cv2.IMWRITE_JPEG_QUALITY), 100]+[int(cv2.IMWRITE_JPEG_OPTIMIZE), True])
frameNum = 0
def start_convert():
    global frameNum
    startTime = time.time()
    if (media_path.endswith(".jpg") or media_path.endswith(".png") or media_path.endswith(".bmp")):
        frame = cv2.imread(media_path)
        polarConv(frame,save_path+'\\'+str(0))
    else:
        media = cv2.VideoCapture(media_path)
        while True:
            ret, frame = media.read()
            if ret == False:
                frameNum = 0
                break  
            polarConv(frame,save_path+'\\'+str(frameNum))
            frameNum=frameNum+1
    print(time.time() - startTime)        
           

def file_picker(sender, data):
    open_file_dialog(callback=apply_selected_file, extensions=".*,.gif,.jpg,.png,.mp4,.avi,.mov")

def directory_picker(sender, data):
    select_directory_dialog(callback=apply_selected_directory)
def apply_selected_directory(sender, data):
    global save_path
    log_debug(data)  # so we can see what is inside of data
    directory = data[0]
    folder = data[1]
    set_value("directory", directory)
    save_path = f"{directory}"
    print(save_path)
    
def apply_selected_file(sender, data):
    global media_path
    log_debug(data)  # so we can see what is inside of data
    directory = data[0]
    file = data[1]
    set_value("file_path", f"{directory}\\{file}")
    media_path = f"{directory}\\{file}"
    print(media_path)

def gamma_switch(sender,data):
    global gammaCorrection
    gammaCorrection = get_value("Gamma Correction")
    print(gammaCorrection)

with window("Main"):
    add_button("Select Image or Video", callback=file_picker)
    add_text("Input Path: ", color=[128, 255, 0])
    add_same_line()
    add_label_text("##filepath", source="file_path", color=[0, 255, 0])
    add_button("Select Output Folder", callback=directory_picker)
    add_text("Output Folder: " , color=[0, 255, 255])
    add_same_line()
    add_label_text("##dir", source="directory", color=[128, 255, 255])
    add_checkbox("Gamma Correction",default_value=gammaCorrection,callback=gamma_switch)
    add_button("Go!",callback=start_convert,width=100,height=50)
    add_text("Ctrl + Click to diretly edit:" , color=[200,150,255])
    add_slider_int("single-sided LEDs",default_value=NUMPIXELS,max_value=200) 
    add_slider_int("Divide",default_value=Div,max_value=1000)
    add_slider_int("Center Brightness",default_value=Led0Bright,max_value=100) 
    add_slider_int("Edge Brightness",default_value=Bright,max_value=100)    
    add_text("Author: Corebb" , color=[255,255,0])
    add_drawing("Drawing_1", width=256, height=256)
    add_same_line()
    add_drawing("Drawing_2", width=256, height=256)  
     
def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)
    
set_main_window_size(800, 800)  
set_main_window_title("bbPOV-P Converter") 
draw_image("Drawing_1", resource_path('data/Corebb.jpg'), [0, 0], pmax=[256,256], uv_min=[0, 0], uv_max=[1, 1], tag="image")
draw_image("Drawing_2", resource_path('data/logo.jpg'), [0, 0], pmax=[256,256], uv_min=[0, 0], uv_max=[1, 1], tag="image2")
set_theme("Cherry")
start_dearpygui(primary_window="Main")
