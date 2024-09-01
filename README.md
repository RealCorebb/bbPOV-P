# bbPOV-P  [English](https://github.com/RealCorebb/bbPOV-P/blob/main/README_EN.md "English")
A new milestone?  
# ⭐ 100 Stars!!! Thank You!!!

🔗[PCB工程文件地址](https://oshwhub.com/Corebb/bbpov-mcu_copy_copy_copy "PCB工程文件地址")  
😄[3D模型、更详细的教程](https://www.afdian.com/@kuruibb "3D模型、更详细的教程")  
🐧QQ群（仅供交流）：647186542  
▶️视频(Video):[Youtube](https://www.youtube.com/watch?v=HpYd48YgSek&t=5s "Youtube")[ Bilibili](https://www.bilibili.com/video/BV1Wy4y1a7t6 " Bilibili")  
# 禁止搬运到Gitee  
![image](https://github.com/RealCorebb/bbPOV-P/blob/main/IMG/logo.jpg?raw=true)
新的自我突破
# 目录结构：
**API** 包含取模软件、串流软件，都在NewConvert目录下，其它为一些早期的版本  
**Arduino--HardwareTest** 开发过程中用来测试LED是否正常、硬件性能的一些硬件测试的软件，最终的主程序中用不到  
**Arduino--bbPOV-P** 主程序  
# 依赖库 
除了[NeoPixelBus](https://github.com/RealCorebb/NeoPixelBus "NeoPixelBus")需要用我修改过的版本以外，其它的一般找对应名字就能找到
# 备选LED芯片
APA102-2020(好像有的版本IC芯片比较小，PWM速度不够)                          价格￥0.67  
HD107S-2020（查看了Datasheet，应该是无论5050还是2020版本都有着27khz的PWM）   价格￥0.80  
LC8822-2020（~~有的地方说是26khz的PWM，但Datasheet里没看到，不清楚~~已发现其就是APA107）           价格￥0.68  
LC8823-2020（~~规格跟HD107S相似，但找不到购买地址~~已发现其就是HD107S又名NS107S）
![image](https://github.com/RealCorebb/bbPOV-V3/blob/main/IMG/LED_Chips.jpg?raw=true)  
