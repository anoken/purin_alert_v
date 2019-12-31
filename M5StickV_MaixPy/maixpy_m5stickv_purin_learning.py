## Copyright (c) 2019 aNoken

## Push A Button is Initialize.
## Push B Button is View Switch.

import image, lcd, sensor,gc
import KPU as kpu
from fpioa_manager import fm,board_info
from Maix import GPIO
from machine import UART


def m5stickv_init():
    lcd.init()
    lcd.rotation(2)
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA) #QVGA=320x240
    sensor.set_windowing((224, 224))
    sensor.run(1)

m5stickv_init()

fm.register(board_info.BUTTON_A, fm.fpioa.GPIO1)
but_a=GPIO(GPIO.GPIO1, GPIO.IN, GPIO.PULL_UP)

fm.register(board_info.BUTTON_B, fm.fpioa.GPIO2)
but_b = GPIO(GPIO.GPIO2, GPIO.IN, GPIO.PULL_UP)

fm.register(35, fm.fpioa.UART2_TX, force=True)
fm.register(34, fm.fpioa.UART2_RX, force=True)
uart_Port = UART(UART.UART2, 115200,8,0,0, timeout=1000, read_buf_len= 4096)


task = kpu.load(0x200000)
#task = kpu.load("mbnet751.kmodel")#Within_SDCard
print(kpu.memtest())

set=kpu.set_layers(task,29)

but_a_pressed = 0
but_b_pressed = 0

master_data = []
for i in range(768):
    master_data.append(0)
w_data=0.99

cap_data = []
for i in range(768):
    cap_data.append(0)
cap_w_data=0.5

dummyImage = image.Image()
dummyImage = dummyImage.resize(32, 24)
kpu_dat = dummyImage.to_grayscale(1)

dummyImage2 = image.Image()
dummyImage2 = dummyImage2.resize(32, 24)
kpu_dat2 = dummyImage2.to_grayscale(1)

view_mode=0

img = sensor.snapshot()
fmap = kpu.forward(task, img)
plist=fmap[:]
for i in range(768):
    master_data[i] =plist[i]

while(True):
    img = sensor.snapshot()
    fmap = kpu.forward(task, img)

    plist=fmap[:]

    ##Weighted update filter
    for i in range(768):
        cap_data[i] =cap_w_data*cap_data[i]+ (1.0-cap_w_data)*plist[i]

    for row in range(32):
        for col in range(24):
            kpu_dat[24*row+col] = int(cap_data[row*24+col]*100)

    dist = 0

    for i in range(768):
        dist = dist + (cap_data[i]-master_data[i])**2

    if dist < 100:
        for i in range(768):
            master_data[i] =w_data*master_data[i]+ (1.0-w_data)*cap_data[i]

    if dist < 200:
        img.draw_rectangle(2,2,220,220,color = (0, 0, 255),thickness=5)
    else:
        img.draw_rectangle(2,2,220,220,color = (255, 0, 0),thickness=5)

#    SendUART
    data_str=str(int(dist))+"\n"
    uart_Port.write(data_str)

    if but_a.value() == 0 and but_a_pressed == 0:
        for i in range(768):
            master_data[i] =cap_data[i]
        but_a_pressed=1
    if but_a.value() == 1 and but_a_pressed == 1:
        but_a_pressed=0
    if but_b.value() == 0 and but_b_pressed == 0:
        view_mode+=1
        but_b_pressed=1
    if but_b.value() == 1 and but_b_pressed == 1:
        but_b_pressed=0
    lcd.draw_string(0, 0, "%.0f "%(dist), lcd.RED, lcd.BLACK)
    if view_mode==0:
        img2=img.resize(110,110)
        img3=kpu_dat.resize(110,110)
        lcd.display(img2,oft=(0,16))
        lcd.display(img3,oft=(130,16))

    elif view_mode==1:
        for row in range(32):
            for col in range(24):
                kpu_dat2[24*row+col] = int(master_data[row*24+col]*100)
        img2=kpu_dat2.resize(110,110)
        img3=kpu_dat.resize(110,110)

        if dist < 200:
            img2.draw_rectangle(2,2,108,108,color = (255),thickness=5)
        else:
            img2.draw_rectangle(2,2,108,108,color = (100),thickness=5)

        lcd.display(img2,oft=(0,16))
        lcd.display(img3,oft=(130,16))

    elif view_mode==2:
        if dist < 200:
            img.draw_rectangle(1,46,222,132,color = (0, 0, 255),thickness=5)
        else:
            img.draw_rectangle(1,46,222,132,color = (255, 0, 0),thickness=5)
        lcd.display(img)

    elif view_mode==3:
        lcd.clear()
        view_mode=0

