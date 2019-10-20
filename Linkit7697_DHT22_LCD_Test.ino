#include <DHT.h> //引用DHT溫溼度感測器涵式
#define TYPE DHT22 //選擇DHT溫溼度感測器型號
#include <LWiFi.h> //引用Wi-Fi涵式
#include "MCS.h" //引用聯發科平台MCS專用涵式
#include <LiquidCrystal_I2C.h>  //使用LCD模組涵式
//請注意Wi-Fi來源須為2.4GHz,5GHz不行使用
char _lwifi_ssid[] = "這裡輸入WIFI名稱"; //這裡輸入WIFI名稱
char _lwifi_pass[] = "這裡輸入WIFI密碼"; //這裡輸入WIFI密碼

MCSDevice mcs("這裡輸入設備ID", "這裡輸入設備鑰匙"); //這裡輸入設備ID以及設備鑰匙
MCSDisplayInteger M001_Humidity("M001_RH"); //M001號機濕度監控 資料通道ID
MCSDisplayInteger M001_Temperature("M001_Temp"); //M001號機溫度監控 資料通道ID
#define DHT22_PIN 2 //設定DHT溫溼度感測器插於7697腳位2號

DHT dht(DHT22_PIN, TYPE); //設定DHT溫溼度感測器

LiquidCrystal_I2C lcd(0x27,16,2); //LCD模組設定

float RH, Temp; //定義

void setup() {
	Serial.begin(9600); //設定IDE顯示視窗鮑率
	lcd.init(); //LCD設定
	lcd.backlight(); //LCD開啟背光
	mcs.addChannel(M001_Humidity);mcs.addChannel(M001_Temperature); //M001號機溫濕度 通道
	Serial.println("Wi-Fi 開始連線");
	while (WiFi.begin(_lwifi_ssid, _lwifi_pass) != WL_CONNECTED) { delay(1000); }
	Serial.println("Wi-Fi 連線成功");
	while(!mcs.connected()) { mcs.connect(); }
	Serial.println("MCS 連線成功");
}

// 這個"loop"涵式將不斷執行直到電源關閉或重置
void loop() {
	while (!mcs.connected()) { //當未與MCS連線成功時重新連線
    Serial.println("MCS 連線中斷...");
		mcs.connect(); //重新連線
		if (mcs.connected()) { Serial.println("MCS 已重新連線"); } //顯示發生過重新連線事件
	}
	mcs.process(100);
 
	RH = dht.readHumidity(); //讀取DHT感測器濕度
	Temp = dht.readTemperature(); //讀取DHT感測器攝氏溫度
	//float Temp_Fahrenheit = dht.readTemperature(true); //讀取DHT感測器華氏溫度

	//檢查DHT感測器接收是否成功(可自行擴充，例加else，如讀取失敗無法進入)
	if (isnan(RH) || isnan(Temp)) {
		Serial.println("DHT 感測器讀取失敗!");
		return;
	}
	
	M001_Humidity.set(RH); //M001號機濕度 資料發送
	M001_Temperature.set(Temp); //M001號機溫度 資料發送
	
	//IDE視窗顯示
	Serial.print("溫度:");Serial.print(Temp, 1);Serial.println(",");
	Serial.print("濕度:");Serial.print(RH, 1);Serial.println(",");
	delay(1000); //延遲等待1000ms
	
	//LCD顯示
	lcd.setCursor(0,0); //第一行第一位置定位
	lcd.print("Humidity:");lcd.print(RH);lcd.print("%");
	lcd.setCursor(0,1); //第二行第一位置定位
	lcd.print("Temp:");lcd.print(Temp);lcd.print((char) 0xDF);lcd.print("C");
}
