# encoding=gbk

import cv2
import numpy as np

def get_hsv_value(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:  # 鼠标左键点击
        hsv_value = hsv[y, x]  # 获取 (x, y) 位置的 HSV 值
        print(f"点击位置 ({x}, {y}) 的 HSV 值: {hsv_value}")




'''
tips:
1.camera
2.rgb_get
3.边界修改 v范围更小，加膨胀轮数 来更好get亮点定位



'''
#path
#cap = cv2.VideoCapture(r'C:\D\works\ai\trafficlight_ide\color-main\video\move_green.avi')
cap = cv2.VideoCapture(0)
num=0
while True:
    num+=1
    ret,frame = cap.read()
    if ret == False:
        break
    frame = cv2.resize(frame,(1920,1080))
    f2=frame.copy();
    hsv=cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)


    '''
    cv2.namedWindow('Image')
    cv2.setMouseCallback('Image', get_hsv_value)
    while True:
        cv2.imshow('Image', frame)
        if cv2.waitKey(1) & 0xFF == 27: # 按下 ESC 键退出
            raise
    '''
    

    
    if num==10:
        hsv=cv2.cvtColor(f2,cv2.COLOR_BGR2HSV)
        cv2.namedWindow('Image')
        cv2.setMouseCallback('Image', get_hsv_value)
        while True:
            cv2.imshow('Image', frame)
            if cv2.waitKey(1) & 0xFF == 27: # 按下 ESC 键退出
                raise
      


    #get 亮点
    lower_v=np.array([0,0,250])
    upper_v=np.array([255,255,255])

    mask_v=cv2.inRange(hsv,lowerb=lower_v,upperb=upper_v)
    mask_v=cv2.medianBlur(mask_v, 7)


    kernel_dilation = np.ones((7, 7), np.uint8)
    mask_v = cv2.dilate(mask_v, kernel_dilation, iterations=4)
    #加上饱和度提取？？

    #red识别周围的红色
    lower_hsv_red = np.array([168,230,170])
    upper_hsv_red = np.array([179,255,255])
    mask_red=cv2.inRange(hsv,lowerb=lower_hsv_red,upperb=upper_hsv_red)
    red_blur_ = cv2.medianBlur(mask_red, 7)
    red_blur=cv2.bitwise_and(red_blur_,mask_v)#



    #yellow
    lower_hsv_yellow=np.array([8,230,170])
    upper_hsv_yellow=np.array([20,255,255])
    mask_yellow=cv2.inRange(hsv,lowerb=lower_hsv_yellow,upperb=upper_hsv_yellow);
    yellow_blur_=cv2.medianBlur(mask_yellow,7);
    yellow_blur=cv2.bitwise_and(yellow_blur_,mask_v);



    #green
    lower_hsv_green = np.array([45,205,120])
    upper_hsv_green = np.array([90,255,255])
    mask_green = cv2.inRange(hsv,lowerb=lower_hsv_green,upperb=upper_hsv_green)
    green_blur_ = cv2.medianBlur(mask_green, 7)
    green_blur=cv2.bitwise_and(green_blur_,mask_v);





    #轮廓识别
    contours1, hierarchy1 = cv2.findContours(green_blur, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE) # 轮廓检测 green灯
    contours2, hierarchy2 = cv2.findContours(red_blur, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE) # 轮廓检测 red灯
    contours3, hierarchy3 = cv2.findContours(yellow_blur, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)#yellow
    #初处理red green

    
    







    for cnt in contours1:
        
        (x, y, w, h) = cv2.boundingRect(cnt) # 该函数返回矩阵四个点
        if w*h < 50*50: # 忽略20*20的框
            continue
        cv2.rectangle(f2, (x, y), (x + w, y + h), (255, 255, 255), 2) # 将检测到的颜色框起来
        cv2.putText(f2, 'green_light', (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.6,(0, 255, 0) , 2)


    for cnt in contours2:
        (x, y, w, h) = cv2.boundingRect(cnt) # 该函数返回矩阵四个点
        if w*h < 50*50: # 忽略20*20的框
            continue
        cv2.rectangle(f2, (x, y), (x + w, y + h), (255, 0, 0), 2) # 将检测到的颜色框起来
        cv2.putText(f2, 'red_light', (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.6,(0, 0, 255) , 2)


    for cnt in contours3:
        (x, y, w, h) = cv2.boundingRect(cnt) # 该函数返回矩阵四个点
        if w*h < 50*50: # 忽略20*20的框
            continue
        cv2.rectangle(f2, (x, y), (x + w, y + h), (255, 0, 255), 2) # 将检测到的颜色框起来
        cv2.putText(f2, 'yellow_light', (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.6,(0,255,255) , 2)





    cv2.imshow('frame',f2)
    red_blur = cv2.resize(red_blur,(300,200))
    #red_blur_ = cv2.resize(red_blur_,(300,200))
    green_blur = cv2.resize(green_blur,(300,200))
    #green_blur_ = cv2.resize(green_blur_,(300,200))
    yellow_blur=cv2.resize(yellow_blur,(300,200))
    #yellow_blur_=cv2.resize(yellow_blur_,(300,200))
    mask_v=cv2.resize(mask_v,(300,200))

    cv2.imshow('red_masked',red_blur)
    #cv2.imshow('red_nonmask',red_blur_)
    cv2.imshow('green_masked',green_blur)
    #cv2.imshow('green_nonmask',green_blur_)
    cv2.imshow('yellow_masked',yellow_blur)
    #cv2.imshow('yellow_nonmask',yellow_blur_)
    cv2.imshow('mask_v',mask_v)




    c = cv2.waitKey(10)
    if c==27:
        break



