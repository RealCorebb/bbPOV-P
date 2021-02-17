
#include <WiFi.h>

const char *ssid = "Hollyshit_A";
const char *password = "00197633";

WiFiServer server; //声明服务器对象

#include <JPEGDEC.h>
JPEGDEC jpeg;

int JPEGDraw(JPEGDRAW *pDraw)
{
  // do nothing
  return 1; // continue decode
} 

void setup()
{
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); //关闭STA模式下wifi休眠，提高响应速度
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());

    server.begin(22333); //服务器启动监听端口号22333
}

uint8_t streamBuffer[15*1024];
void loop()
{
    WiFiClient client = server.available(); //尝试建立客户对象
    if (client) //如果当前客户可用
    {
        Serial.println("[Client connected]");
        while (client.connected()) //如果客户端处于连接状态
        {
            if (client.available()) //如果有可读数据
            {
                String buff = client.readStringUntil('\r');
                int len = buff.toInt();
                client.readBytes(streamBuffer,len);                    
                if(streamBuffer[len-2]==0xFF && streamBuffer[len-1]==0xD9){   //JPG结尾
                  if (jpeg.openRAM(streamBuffer, len, JPEGDraw)) {
                    Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
                    jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
                      if (jpeg.decode(0,0,0)) { // full sized decode
                      }
                      jpeg.close();
                    }
                  }
            }
        }
      //  client.stop(); //结束当前连接:
      //  Serial.println("[Client disconnected]");
    }
}
