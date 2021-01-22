import tkinter as tk
from tkinter import ttk
import numpy as np
import os
import math
from mss import mss
import time
from threading import Thread
import cv2
import time

#配置
NUMPIXELS = 80 #单边LED数量
Div = 320 #1圈分割数
Bright = 35 #輝度
Led0Bright = 15 #中心LEDの輝度 [%]


#Global
running = False
posX = 0
posY = 0
posX2 = 160
posY2 = 160

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
    cv2.imwrite(str(time.time_ns())+'.jpg',final)
def capture():
    global posX
    global posY
    global posX2
    global posY2
    while running:
            #捕捉屏幕
            last_time = time.time()
            frame = mss().grab({'top': posY, 'left': posX, 'width': posX2-posX, 'height': posY2-posY})
            polarConv(frame)
            print('fps: {0}'.format(1 / (time.time()-last_time)))
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
        self.title('bbPOV-P Sender')
        w = 300
        h = 100
        ws = self.winfo_screenwidth()
        hs = self.winfo_screenheight()
        x = (ws/2) - (w/2)
        y = (hs/2) - (h/2)
        self.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self.floater = CaptureArea(self)
        tk.Label(self, text="IP地址").pack()
        ip = tk.Entry(self)
        start = tk.Button(self, text="开始", command=startCapture)
        stop = tk.Button(self, text="停止", command=stopCapture)
        ip.pack()
        start.pack()
        stop.pack()
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