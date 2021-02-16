#include <WiFi.h>

const char *ssid = "Hollyshit_A";
const char *password = "00197633";

WiFiServer server; //声明服务器对象
uint8_t *streamBuffer;
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

    if(streamBuffer = (uint8_t(*)) calloc(10*1024,sizeof(uint8_t)))
    Serial.println("Alloc memory OK");
}

long lTime;
int bufferSize=0;
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
                if(bufferSize==0) lTime = micros();
                Serial.println(bufferSize);
                int Received = client.read(&streamBuffer[bufferSize],512);
               // Serial.println(Received);
                bufferSize+=Received;
               // Serial.println("memcpy Done");
                if(streamBuffer[bufferSize-2]==0xFF && streamBuffer[bufferSize-1]==0xD9){   //JPG结尾
                  Serial.println("END");
                  //Serial.printf("TCP Receiving time %d us\n", (int)micros() - lTime);
                  //Serial.println(bufferSize);
                  //client.write(streamBuffer, 512);
                  /*
                  if (jpeg.openRAM(streamBuffer, bufferSize, JPEGDraw)) {
                      
                    Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
                    jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
                      //lTime = micros();
                      if (jpeg.decode(0,0,0)) { // full sized decode
                        lTime = micros() - lTime;
                       // Serial.printf("Total time %d us\n", (int)lTime);
                      }
                      jpeg.close();
                    }*/
                    bufferSize=0;
                 // break;
                  }
            }
        }
      //  client.stop(); //结束当前连接:
      //  Serial.println("[Client disconnected]");
    }
}
