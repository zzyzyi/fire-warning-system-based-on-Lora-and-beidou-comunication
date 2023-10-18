#include<WiFi.h>
#include<WebServer.h>
#include <string.h>
#include "Arduino.h"
#include "LoRa_E22.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "bdrd_esp32.h"
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include  <time.h>
#include "PubSubClient.h"
#include "OneButton.h"
#include <SPI.h>
#include <mySD.h>


//LED引脚定义
#define INPUT_PIN1 19
#define OUTPUT_PIN1 23
#define DEBUG_PIN 27
#define Buzzer_PIN1 32
#define TOUCH_PIN 33
//onenet
#define mqtt_pubid "610279"         //产品ID
#define mqtt_devid "1099722645"     //设备ID
#define mqtt_password "CSTX"        //鉴权信息


//定义串口实例
HardwareSerial LoraSerial1(1);
//lora实例对象
LoRa_E22 e22ttl(&LoraSerial1);
//本地服务器实例对象
WebServer server(80);
//实例化一个OneButton对象
OneButton button(0, true);
//创建一个WIFI连接客户端-onenet
WiFiClient espClient;
//创建一个PubSub客户端, 传入创建的WIFI客户端-onenet
PubSubClient client(espClient);
//创建一个文件对象
ext::File root;
// 初始化显示屏对象，采用系统默认I2C接口：SCL-22、SDA-21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//WIFI用户名密码
const char* ssid = "Admin";
const char* password = "123456789";

//onenet
int god = 0;
const char *mqtt_server = "xxx.xxx.xxx.xxx"; //onenet 的 IP地址
const int port = xxxx;                     //端口号
 
//本地服务器网页内容
String web_head = "<!DOCTPYE html><html><head><meta charset='utf-8'></head><body>";
String web_tail = "</body></html>";
String SD_data_Directory = "";
String SD_data = "";

//传感器数据容器
const int node_number_max = 7;        //最大可接受传感器数量   max=5999   但本代码的string和int转换仅支持到10，如需增加请自行跳转至下方函数增加
String DATA_name[7] = {"记录次数","节点名称","烟雾","光照","湿度","温度","警告"};//{"times","num","smog","light","humi","temp","worning"}; 
String DATA = "";              // 接收数据容器
String DATA_temporary = "";    // 临时接收数据容器
String DATA_send = "";         // 北斗发送数据容器
String DATA_onenet = "";       // onenet发送数据容器
String DATA_parse[6];          // 传感器解析数据容器
String DATA_parse_all[node_number_max*6];     // 传感器解析数据容器//由于实际节点数受限，暂时认为节点数不大于2，支持自由改动，需要变更get_each_data_12()以及set_DATA_parse_all()函数中的数量即可
int DATA_worning[node_number_max];//报警指示以及初始化
int DATA_worning_past = 0;     // 过去一段时间里是否触发过报警

//传感器节点过去的10条数据以及当前位置指针
String DATA_past[node_number_max][10];
int DATA_past_indicator[node_number_max];
int each_times[node_number_max];   // 记录次数

int number = 0;                    // 当前访问传感器的序号
int alive_name[node_number_max];   // 存活的传感器节点
int alive_indicator = 0;           // 存活传感器节点指针
bool each_node_state[node_number_max];   // 各个传感器节点的开启状态
int read_name[node_number_max];    // 判断当前节点是否正在被访问，0：无访问；1：正在访问数据；2：客户端强行关停设备

//北斗发射
int start_read = 0;                // 开始访问节点标识
char targetID[] = "15950077";      // 目标端ID号字符串
char msgType = '1';                // 短报文类型1，汉字


/********基础程序********/
//led亮0.3s
void led_on(int led_on_pin){
  digitalWrite(led_on_pin, HIGH);
  delay(300);
  digitalWrite(led_on_pin, LOW);
}
//按键
void click(){
  Serial.println("click");
  DATA_worning_past = 1;
  digitalWrite(Buzzer_PIN1, HIGH);
  delay(60000);
  DATA_worning_past = 0;
}
//int---->string
String number_to_number_str(int number_change){
  if(number_change==1){
    return "1";
  }else if(number_change==2){
    return "2";
  }else if(number_change==3){
    return "3";
  }else if(number_change==4){
    return "4";
  }else if(number_change==5){
    return "5";
  }else if(number_change==6){
    return "6";
  }else if(number_change==7){
    return "7";
  }else if(number_change==8){
    return "8";
  }else if(number_change==9){
    return "9";
  }else if(number_change==10){
    return "10";
  }else if(number_change==0){
    return "0";
  }
}
//string---->int
int number_str_to_number(String number_change){
  if(number_change=="1"){
    return 1;
  }else if(number_change=="2"){
    return 2;
  }else if(number_change=="3"){
    return 3;
  }else if(number_change=="4"){
    return 4;
  }else if(number_change=="5"){
    return 5;
  }else if(number_change=="6"){
    return 6;
  }else if(number_change=="7"){
    return 7;
  }else if(number_change=="8"){
    return 8;
  }else if(number_change=="9"){
    return 9;
  }else if(number_change=="10"){
    return 10;
  }else if(number_change=="0"){
    return 0;
  }
}
//OLED显示屏刷新显示
void page1(int a, int b, String ug8_data) {
  u8g2.setFont(u8g2_font_timR18_tf);    // 配置ASCII字体字号
  u8g2.setFontPosTop();                 // 文字向上对齐
  u8g2.setCursor(5, 12);
  u8g2.print("Fire Wall");
  u8g2.setCursor(a,b);                 // 文字起始位置为（0，20）坐标处
  u8g2.print(ug8_data);  // 将超声波测距的数值小数点1位四舍五入后显示
}

/********本地服务器********/
//本地服务器回调函数
void handleRoot(){
  String HTML = "<!DOCTPYE html><html><head><meta charset='UTF-8'><style>\
  .button {background-color: #a54b4a;border: none;color: white;padding: 20px 40px;text-align: center;text-decoration: none;display:block;margin:0 auto;font-size: 150%;cursor: pointer;border-radius: 5px;}\
  .p {font-family: '仿宋';font-weight: bold;font-size: 300%;font-style: italic;display: display: block; text-align: center;}\
  </style></head><body>\
  <p align='center' class='p'><b>“防火墙”森林防火预警系统首页<b><br /></p>\
  <a align='center' href='http://192.168.43.53/memory' class='button'>过去记录</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=1&state=on' class='button'>节点1  开</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=1&state=off' class='button'>节点1  关</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=2&state=on' class='button'>节点2  开</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=2&state=off' class='button'>节点2  关</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=3&state=on' class='button'>节点3  开</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=3&state=off' class='button'>节点3  关</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=4&state=on' class='button'>节点4  开</a><br /><br />\
  <a align='center' href='http://192.168.43.53/handle?node=4&state=off' class='button'>节点4  关</a><br /><br />\
  <a align='center' href='http://192.168.43.53/serverSD' class='button'>SD卡数据记录</a><br /><br />\
  <a align='center' href='http://192.168.43.53/journal' class='button'>日志</a><br /><br />" + web_tail;
  server.send(200,"text/html",HTML);
}
//本地服务器记忆回调函数
void handleRoot_memory(){
  String HTML = web_head + "<p align='center' font-size='140%'>";
  for(int j=1;j<node_number_max+1;j++){
    HTML += "<br />-----------------------------<b>node" + number_to_number_str(j) + "<b>-----------------------------<br />";
    for(int i = 0;i<10&&DATA_past[j-1][i] != "";i++){
      HTML += DATA_past[j-1][i] + "<br />";
    }
  }
  HTML += "</p>" + web_tail;
  server.send(200,"text/html",HTML);
}
//本地服务器控制回调函数
void handle(){
  String state1 = server.arg("node");
  String state2 = server.arg("state");

  if(state2 == "on"){
    node_on(number_str_to_number(state1));
  }else if(state2 == "off"){ 
    node_off(number_str_to_number(state1));
  }

  server.send(200,"text/html","<p align='center'>node <b>" + state1 + "<b> is " + state2 + "." + "</p>");
}
//本地服务器SD卡数据函数
void server_SD_data(){
  int wait_time = 0;
  int state_SD_touch = 0;
  String inlimited = "非法访问";
  while(touchRead(TOUCH_PIN)>30&&wait_time<50){
    delay(50);
    wait_time++;
  }
  if(wait_time>=48){state_SD_touch = 1;}
  if(state_SD_touch == 0){
    sd_read_file("data.txt");
    server.send(200,"text/html","<!DOCTPYE html><html><head><meta charset='utf-8'></head><body><p align='center'>" + SD_data + "</p></body></html>");
  }else{
    server.send(200,"text/html","<!DOCTPYE html><html><head><meta charset='utf-8'></head><body><p align='center'><b>" + inlimited + "<b></p></body></html>");
    // 刷新OLED屏幕界面显示
    u8g2.firstPage();
    do
    {
      page1(5, 40, "limited");
    }while(u8g2.nextPage());
  }
}
//本地服务器SD卡数据函数
void server_SD_journal(){
  int wait_time = 0;
  int state_SD_touch = 0;
  String inlimited = "非法访问";
  while(touchRead(TOUCH_PIN)>30&&wait_time<50){
    delay(50);
    wait_time++;
  }
  if(wait_time>=48){state_SD_touch = 1;}
  if(state_SD_touch == 0){
    sd_read_file("jou.txt");
    server.send(200,"text/html","<!DOCTPYE html><html><head><meta charset='utf-8'></head><body><p align='center'>" + SD_data + "</p></body></html>");
  }else{
    server.send(200,"text/html","<!DOCTPYE html><html><head><meta charset='utf-8'></head><body><p align='center'><b>" + inlimited + "<b></p></body></html>");
    // 刷新OLED屏幕界面显示
    u8g2.firstPage();
    do
    {
      page1(5, 40, "limited");
    }while(u8g2.nextPage());
  }
}
//本地服务器的客户端控制模块开启
void node_on(int which){
  read_name[which-1] = 0;
  each_node_state[which-1] = true;
  // 刷新OLED屏幕界面显示
  u8g2.firstPage();
  do
  {
    page1(5, 40, "on");
  }while(u8g2.nextPage());
}
//本地服务器的客户端控制模块关闭
void node_off(int which){
  read_name[which-1] = 2;
  each_node_state[which-1] = false;
  // 刷新OLED屏幕界面显示
  u8g2.firstPage();
  do
  {
    page1(5, 40, "off");
  }while(u8g2.nextPage());
}

/********LORA********/
//搜寻当前存活的节点数量
void check_how_much(){
  //置空所有传感器节点
  memset(alive_name,0,sizeof(alive_name));
  for(int j=0;j<node_number_max;j++){
    each_node_state[j] = false;
  }
  number = 0;
  alive_indicator = 0;
  //判断第一台设备是否存在
  e22ttl.sendMessage("alive");
  for(int i = 0;i<(10+node_number_max*5);i++){
    delay(50);
    if (e22ttl.available()>0){
      ResponseContainer rc = e22ttl.receiveMessage();

      if (rc.status.code!=1){
		    rc.status.getResponseDescription();
      }else{
        if(read_name[number_str_to_number(rc.data)-1]!=2){
          alive_name[alive_indicator] = number_str_to_number(rc.data);
          each_node_state[alive_name[alive_indicator]-1] = true;
          alive_indicator++;
          number++;
        }
      }
    }
  }
}
//LoRa接受传感器数据
void get_Data(int member){
  //访问指定传感器节点
  e22ttl.sendMessage(number_to_number_str(member));

  //等待指定节点的回复
  int available_time = 0;
  while(e22ttl.available()==0&&available_time<30){delay(100);available_time++;}
  if(available_time>29){
    Serial.println("no signal");
    //写入SD卡中
    String journal = number_to_number_str(member) + " no signal<br />";
    sd_write_file("jou.txt", journal);
  }


  //开始接受并处理数据至DATA容器
  if (e22ttl.available()>1) {
	  //接受数据
	  ResponseContainer rc = e22ttl.receiveMessage();
	  // 判断数据的状态是否正常
	  if (rc.status.code!=1){
		  rc.status.getResponseDescription();
	  }else{
		  //接收数据至DATA，并做解析
      DATA = "";
      DATA = rc.data;
      DATA_onenet = DATA;
      //拆分数据
      get_each_data(DATA);

      //报警蜂鸣器
      if(DATA_parse[5] == "1" && DATA_worning_past == 0){
        digitalWrite(Buzzer_PIN1, LOW);
      }

      //数据合并至DATA临时容器中
      DATA_temporary = "";
      DATA_temporary += (DATA_name[0] + ":" + each_times[member-1] + ",");
      for(int i =1;i<6;i++){
        DATA_temporary += (DATA_name[i] + ":" + DATA_parse[i-1] + ",");
      }
      DATA_temporary += (DATA_name[6] + ":" + DATA_parse[5]);
      each_times[member-1] ++;

      //写入记忆容器中
      wright_10_memory(member,DATA_temporary);

      //写入SD卡中
      DATA_temporary += "<br />";
      sd_write_file("data.txt", DATA_temporary);

      //北斗发送容器
      DATA_send += (DATA_name[1] + ":" + DATA_parse[0] + "," + DATA_name[6] + ":" + DATA_parse[5] + ";");

      //接受成功标识
      led_on(INPUT_PIN1);

      //回应节点check
      e22ttl.sendMessage("recive");
      led_on(OUTPUT_PIN1);
	  }
    //清空lora接收端缓存
    while (e22ttl.available()>1) {}
  }
}

/********数据容器存储与解析********/
//解析传感器数据
void get_each_data(String data){
  String recvMsg(data);
  char *p=(char*)recvMsg.c_str();
  char ch[100];
  int i = 0;
  strcpy(ch,p); 
  char *result = NULL;
  DATA_parse[i] = strtok(ch,",");
  while (DATA_parse[i] != NULL&&i<5){
    i++;
    DATA_parse[i] = strtok(NULL, ",");
  }
  i = 0;
}
//解析传感器全部数据
void get_each_data_12(int number_get_12_data, String data){
  String recvMsg(data);
  char *p=(char*)recvMsg.c_str();
  char ch[100];
  int i = (number_get_12_data-1)*6;

  strcpy(ch,p); 
  char *result = NULL;
  DATA_parse_all[i] = strtok(ch,",");
  while (DATA_parse_all[i] != NULL&&i<((node_number_max*6)+5)){
    i++;
    DATA_parse_all[i] = strtok(NULL, ",");
  }
  i = 0;
}
//重置传感器节点全部数据
void set_DATA_parse_all(){
  for (int i = 0; i<(node_number_max*6); i++){
    DATA_parse_all[i] = "-";
  }
}
//记录传感器过去10条信息
void wright_10_memory(int which_one,String data_memory){
  DATA_past[which_one-1][DATA_past_indicator[which_one-1]] = data_memory;
  DATA_past_indicator[which_one-1]++;
  if(DATA_past_indicator[which_one-1]>9){DATA_past_indicator[which_one-1]=0;}
}

/********SD卡数据存储********/
//目录文件写入
void sd_write_file(String file_name, String file_data) {
  root = SD.open(file_name.c_str(), FILE_WRITE);
 /*如果成功打开 - > root！= NULL 然后写字符串“Hello world！”*/ 
  if (root) {
    root.println(file_data.c_str());
    root.flush();
   /*关闭文件 */
    root.close();
  } else {
    /* 如果文件打开错误，则打印错误 */
    Serial.println("error opening " + file_name);
  }
}
//目录文件读取
void sd_read_file(String file_name) {
  SD_data = "";
  root = SD.open(file_name.c_str());
  if (root) {    
    /* 从文件中读取，直到其中没有其他内容 */
    while (root.available()) {
      SD_data += (char)root.read();
    }
    root.close();
  } else {
    Serial.println("error opening " + file_name);
  }
}

/********北斗发射********/
//北斗发射程序
void send_to_beidou(String sendmsg3){
  int result = sendSMS(targetID, msgType, getWCharString((char*)sendmsg3.c_str()));
  Serial.println(getWCharString((char*)sendmsg3.c_str()));
    if (0 == result) {
      // 发送成功
      Serial.println("Send Seccess");   // 调试串口输出“发送成功”调试信息
      led_on(DEBUG_PIN);
    } else {
      // 发送失败
      showError(result);              // 调试串口输出错误提示信息
    }
}

/********Onenet********/
//收到主题下发的回调, 注意这个回调要实现三个形参 1:topic 主题, 2: payload: 传递过来的信息 3: length: 长度
void callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  //for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  //}
  Serial.println();
  
  if ((char)payload[0] == '0') {   //
    god=0;
  }
  if ((char)payload[0] == '1') { //
    god=1;
  }
  else{}
  
}
//Onenet重连函数
void clientReconnect() {
  while (!client.connected())  //再重连客户端
  {
    Serial.print("reconnect MQTT...");
    client.setServer(mqtt_server, port);
    if (client.connect(mqtt_devid, mqtt_pubid, mqtt_password)) {
      Serial.println("connected");
      client.setCallback(callback);
    } else {
      Serial.print("failed");
      Serial.print(client.state());
      Serial.print("try again in 3 sec");
      delay(3000);
    }
  }
}
//Onenet数据发送函数
void pubMQTTmsg_more_name_value(String sendmsg_in){
 
 
    long lastMsg = 0;
    int sendlen=1000;//28 //数组大小
    char msg[sendlen+22]; //数组大小
    char tmp[sendlen];
    char d[3];
    String sendmsg=sendmsg_in;
    sendmsg.toCharArray(tmp, sendlen);
    //Serial.println(tmp);
    
    uint16_t streamLen= strlen(tmp);
    d[0]='\x03';
    d[1] = (streamLen >> 8);
    d[2] = (streamLen & 0xFF);
    snprintf(msg,sizeof(msg),"%c%c%c%s",d[0],d[1],d[2],tmp);
    
    client.publish("$dp", (uint8_t*)msg,streamLen+3,false);
  
    //发送数据到主题
    //delay(500);
}

/********启动前配置********/
void setup() {
  // 初始化OLED显示屏，并刷新界面
  u8g2.setI2CAddress(0x3C*2);
  u8g2.begin();
  u8g2.firstPage();
  do
  {
    page1(5, 12, "Fire Wall");
  }while(u8g2.nextPage());
  u8g2.enableUTF8Print();
  
  //数据容器初始化
  memset(DATA_worning,0,sizeof(DATA_worning));
  memset(DATA_past,0,sizeof(DATA_past));
  memset(DATA_past_indicator,0,sizeof(DATA_past_indicator));
  memset(each_times,0,sizeof(each_times));
  memset(read_name,0,sizeof(read_name));

  Serial.begin(115200);
  LoraSerial1.begin(115200,SERIAL_8N1,5,18);
  Serial2.begin(115200);

  //连接WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  // 刷新OLED屏幕界面显示
  u8g2.firstPage();
  do
  {
    page1(5, 40, "WIFI...");
  }while(u8g2.nextPage());
  //等待WIFI连接
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  Serial.println("WIFI连接正常");
  //服务器IP地址
  Serial.println(WiFi.localIP());
  delay(100);

  //配置服务器
  server.on("/",handleRoot);
  server.on("/handle",handle);
  server.on("/serverSD",server_SD_data);
  server.on("/journal",server_SD_journal);
  server.begin();

  //初始化SD卡
  Serial.print("Initializing SD card");
  // 刷新OLED屏幕界面显示
  u8g2.firstPage();
  do
  {
    page1(5, 40, "SD...");
  }while(u8g2.nextPage());
  while (!SD.begin(15, 13, 12, 14)) {
    Serial.println(".");
    delay(50);
  }
  Serial.println("initialization done.");

  //led以及蜂鸣器初始化
  pinMode(INPUT_PIN1, OUTPUT);
  pinMode(OUTPUT_PIN1, OUTPUT);
  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(Buzzer_PIN1, OUTPUT);
  digitalWrite(INPUT_PIN1, LOW);
  digitalWrite(OUTPUT_PIN1, LOW);
  digitalWrite(DEBUG_PIN, LOW);
  digitalWrite(Buzzer_PIN1, HIGH);
  
  ///按键初始化
  button.attachClick(click);

  //LoRa初始化
  e22ttl.begin();

  //多线程监听对本地服务器访问程序的配置
  xTaskCreate(
    taskOne,   /* Task function. */
    "TaskOne", /* String with name of task. */
    10000,     /* Stack size in bytes. */
    NULL,      /* Parameter passed as input of the task */
    1,         /* Priority of the task. */
    NULL
  );     /* Task handle. */

  xTaskCreate(
    taskTwo,   /* Task function. */
    "TaskTwo", /* String with name of task. */
    10000,     /* Stack size in bytes. */
    NULL,      /* Parameter passed as input of the task */
    1,         /* Priority of the task. */
    NULL
  );     /* Task handle. */
  
  xTaskCreate(
    taskThree,   /* Task function. */
    "TaskThree", /* String with name of task. */
    10000,     /* Stack size in bytes. */
    NULL,      /* Parameter passed as input of the task */
    1,         /* Priority of the task. */
    NULL
  );     /* Task handle. */

  /*Onenet链接*/ 
  client.setServer(mqtt_server, port);                    //设置客户端连接的服务器,连接Onenet服务器, 使用6002端口
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password);  //客户端连接到指定的产品的指定设备.同时输入鉴权信息
  if (client.connected()) {
    Serial.println("OneNet is connected!");  //判断以下是不是连好了.
    //设置好客户端收到信息是的回调
    client.setCallback(callback); //订阅命令下发主题
    //tim1.attach(5, sendTempAndHumi);  //定时每5秒调用一次发送数据函数sendTempAndHumi
  }
}

/********主函数-四线程********/
//本地服务器监听
void loop() {
  // 服务器监听响应
  server.handleClient();
}
//访问节点-线程1
void taskOne(void *parameter){
  while(1){
    //判断存活数
    check_how_much();
    Serial.println(number);
    // 刷新OLED屏幕界面显示
    u8g2.firstPage();
    do
    {
      page1(5, 40, "alive:" + number_to_number_str(number));
    }while(u8g2.nextPage());

    DATA_onenet = "";
    set_DATA_parse_all();
    
    for(int i = 1;i<number+1;i++){
      //if(i>1){DATA_onenet += ",";}
      if(read_name[alive_name[i-1]-1]!=2)read_name[alive_name[i-1]-1]=1;

      //节点上传数据
      get_Data(alive_name[i-1]);
      get_each_data_12(alive_name[i-1],DATA_onenet);
      delay(300);
    }
    //停止对节点的访问
    for(int i = 0;i<node_number_max&&alive_name[i] != 0;i++){
      if(read_name[alive_name[i]-1]!=2)read_name[alive_name[i]-1]=0;
    }
    start_read = 0;

    /*发送数据*/
    //判断当前WIFI是否正常连接
    if(!WiFi.isConnected()){ 
      //连接WIFI
      //初始化
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid,password);
      //等待WIFI连接
      while(WiFi.status() != WL_CONNECTED){
        delay(80);
        Serial.print(".");
      }
      //服务器IP地址
      //Serial.print("\nIP地址为: ");
      //Serial.println(WiFi.localIP());
      //delay(500);
      Serial.print("WIFI连接正常");
    }
    //判断onenet客户端是否正常连接
    while (!client.connected()){ 
       clientReconnect();
       delay(80);
    }
    String msg1=String("")+"{"    
              +"\"smog1\":"+ DATA_parse_all[1]+","
              +"\"illu1\":"+ DATA_parse_all[2]+","
              +"\"humi1\":"+ DATA_parse_all[3]+","
              +"\"temp1\":"+ DATA_parse_all[4]+","
              +"\"warning1\":"+ DATA_parse_all[5]+","
              +"\"state1\":"+ each_node_state[0]+","
              +"\"smog2\":"+ DATA_parse_all[7]+"," 
              +"\"illu2\":"+ DATA_parse_all[8]+","
              +"\"humi2\":"+ DATA_parse_all[9]+","
              +"\"temp2\":"+ DATA_parse_all[10]+","
              +"\"warning2\":"+ DATA_parse_all[11]+","
              +"\"state2\":"+each_node_state[1]      
              +"}";
    String msg2=String("")+"{"        
              +"\"smog3\":"+ DATA_parse_all[13]+","
              +"\"illu3\":"+ DATA_parse_all[14]+","
              +"\"humi3\":"+ DATA_parse_all[15]+","
              +"\"temp3\":"+ DATA_parse_all[16]+","
              +"\"warning3\":"+ DATA_parse_all[17]+","
              +"\"state3\":"+each_node_state[2]+","
              +"\"smog4\":"+ DATA_parse_all[19]+"," 
              +"\"illu4\":"+ DATA_parse_all[20]+","
              +"\"humi4\":"+ DATA_parse_all[21]+","
              +"\"temp4\":"+ DATA_parse_all[22]+","
              +"\"warning4\":"+ DATA_parse_all[23]+","
              +"\"state4\":"+each_node_state[3]      
              +"}";
    pubMQTTmsg_more_name_value(msg1);
    pubMQTTmsg_more_name_value(msg2);
    client.loop(); //客户端循环检测
              
    Serial.print("public the data:");
    Serial.println(msg1);
    Serial.println(msg2);

    //进入下一次数据访问
    delay(2500);
    DATA_send = "";
    start_read = 1;
  }
}
//北斗发射-线程2
void taskTwo(void *parameter){
  while(1){
    delay(60010);
    while(start_read == 1){delay(50);}
    if(DATA_send != ""){
      Serial.println("send to beidou");
      send_to_beidou(DATA_send);
    }else{
      Serial.println("空数据");
      send_to_beidou("NONE DATA");
    }
  }
}
//按键检测-线程3
void taskThree(void *parameter){
  while(1){
    button.tick();
    delay(50);
  }
}
