import tkinter as tk
from tkinter import ttk
from tkinter import *
import numpy as np
import os
import math
from mss import mss
import time
from threading import Thread
import cv2
import time
import tkinter.font as font

#配置
NUMPIXELS = 80 #单边LED数量
Div = 320 #1圈分割数
Bright = 70 #輝度
Led0Bright = 15 #中心LEDの輝度 [%]
last_time = time.time()
MAX_FPS=30
MIN_FRAME_TIME=1/MAX_FPS

#Global
running = False
posX = 0
posY = 0
posX2 = 160
posY2 = 160

def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

def polarConv(imgOrgin):
    h = imgOrgin.height #帧尺寸
    w = imgOrgin.width
    #画像縮小
    imgRedu = cv2.resize(np.array(imgOrgin),(math.floor((NUMPIXELS * 2 -1)/h *w), NUMPIXELS * 2 -1))
    polar_image = cv2.warpPolar(imgRedu,(NUMPIXELS , Div ), (imgRedu.shape[1]/2,imgRedu.shape[0]/2) ,min(imgRedu.shape[0], imgRedu.shape[1]) / 2, 0)
    hsv = cv2.cvtColor(polar_image,cv2.COLOR_BGR2HSV)
    for i in range(NUMPIXELS):                #亮度处理
        hsv[:,i,2] = hsv[:,i,2] * ((Led0Bright + i * ((Bright-Led0Bright)/NUMPIXELS)) / 100)
    final = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)
    #cv2.imwrite(str(time.time_ns())+'.jpg',final,[int(cv2.IMWRITE_JPEG_QUALITY), 95])
def capture():
    global posX
    global posY
    global posX2
    global posY2
    global last_time
    while running:
            #捕捉屏幕
            start = time.time()
            frame = mss().grab({'top': posY, 'left': posX, 'width': posX2-posX, 'height': posY2-posY})
            polarConv(frame)          
            print('FPS:'+str(int(1 / (time.time()-last_time))))
            last_time=time.time()
            time.sleep(max(0, MIN_FRAME_TIME - (time.time() - start)))
def startCapture():
    global running
    alpha = 0
    running=True
    t = Thread (target = capture)
    t.start()
def stopCapture():
    global running
    running=False
class Main(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        icon = """    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
        """
        iconImg=PhotoImage(data=icon) 
        self.wm_iconphoto(True, iconImg)
        self.title('bbPOV-P Sender')
        self.configure(background='#222222')
        self.resizable(0,0)
        w = 200
        h = 100
        ws = self.winfo_screenwidth()
        hs = self.winfo_screenheight()
        x = (ws/2) - (w/2)
        y = (hs/2) - (h/2)
        self.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self.floater = CaptureArea(self)
        ip = tk.Entry(self,justify='center',bg="#222222",highlightbackground="white",fg="white")
        ip.insert(0,'IP地址')
        buttonFont = font.Font(family="微软雅黑",size=16)
        start = tk.Button(self, text="开始", command=startCapture , bg="green",fg="white")
        stop = tk.Button(self, text="停止", command=stopCapture , bg="red",fg="white")
        start['font'] = buttonFont
        stop['font'] = buttonFont
        ip.pack(pady=10,anchor="center")
        start.pack(padx=10,pady=5,ipadx=10,ipady=5,side="left")
        stop.pack(padx=10,pady=5,ipadx=10,ipady=5,side="left")
class ResizingCanvas(tk.Canvas):
    def __init__(self, parent, **kwargs):
        tk.Canvas.__init__(self, parent, **kwargs)
        self.bind("<Configure>", self.on_resize)
        self.height = self.winfo_reqheight()
        self.width = self.winfo_reqwidth()

    def on_resize(self,event):
        # determine the ratio of old width/height to new width/height
        wscale = event.width/self.width
        hscale = event.height/self.height
        self.width = event.width
        self.height = event.height
        # rescale all the objects
        self.scale("all", 0, 0, wscale, hscale)
        
class CaptureArea(tk.Toplevel):
    def __init__(self, *args, **kwargs):
        tk.Toplevel.__init__(self, *args, **kwargs)
        self.overrideredirect(1) 
        self.attributes("-alpha", 0.3)
        self.attributes("-transparentcolor", 'purple')
        self.attributes("-topmost", True)
        myCanvas = ResizingCanvas(self, bg="purple", height=300, width=300,highlightthickness=8, highlightbackground="red")
        myCanvas.pack(fill=tk.BOTH, expand=tk.YES)
        self.move = tk.Label(self, bitmap="gray25")
        self.move.place(x=0,y=0,height=8,width=8)
        self.move.bind("<ButtonPress-1>", self.start_move)
        self.move.bind("<ButtonRelease-1>", self.stop_move)
        self.move.bind("<B1-Motion>", self.do_move)
        
        self.grip = ttk.Sizegrip(self)
        self.grip.place(height=8,width=8,relx=1.0, rely=1.0, anchor="se")
        self.grip.bind("<B1-Motion>", self.OnMotion)

        
    def OnMotion(self, event):
        global posX2
        global posY2
        x1 = self.winfo_pointerx()
        y1 = self.winfo_pointery() 
        x0 = self.winfo_rootx()
        y0 = self.winfo_rooty()
        posX2 = x0+self.winfo_width()-8
        posY2 = y0+self.winfo_height()-8
        self.geometry("%sx%s" % ((x1-x0),(y1-y0)))
        return
    def start_move(self, event):
        self.x = event.x
        self.y = event.y

    def stop_move(self, event):
        self.x = None
        self.y = None

    def do_move(self, event):
        global posX
        global posY
        global posX2
        global posY2
        deltax = event.x - self.x
        deltay = event.y - self.y
        x = self.winfo_x() + deltax
        y = self.winfo_y() + deltay
        posX = self.winfo_rootx()+8
        posY = self.winfo_rooty()+8
        posX2 = x+self.winfo_width()-8
        posY2 = y+self.winfo_height()-8
        self.geometry(f"+{x}+{y}")
app=Main()
app.mainloop()