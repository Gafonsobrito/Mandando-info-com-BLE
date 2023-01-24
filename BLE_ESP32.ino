//Bibliotecas ESP32-BLE.
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *characteristicTX; //Característica resposável por mandar informação.

//UUID das características.
#define SERVICE_UUID            "4ac8a682-9736-4e5d-932b-e9b31405049d"
//#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b3140504bb"
#define CHARACTERISTIC_UUID_TX  "4ac8a682-9736-4e5d-932b-e9b31405049c"

bool deviceConnected = false; //Variável que confere se o ESP32 está conectado ao dsipositívo.

int pinPot = 36; //Variável que recebe informação.  
float value;

class ServerCallbacks: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    deviceConnected = true;
  };
  void onDosconnection(BLEServer* pServer){
    deviceConnected = false;
  }
};

void setup(){ 

  Serial.begin(115200);
  
  pinMode(pinPot, INPUT);

  BLEDevice::init("Test ESP32-BLE"); //Nome do dispositivo bluetooth.
  BLEServer *server = BLEDevice::createServer(); //Cria um BLE server.
  server->setCallbacks(new ServerCallbacks()); //Define o callback do server.
  BLEService *service = server->createService(SERVICE_UUID); //Cria o BLE service.
  
  //Cria a característica do BLE que envia os dados.
  characteristicTX = service->createCharacteristic(
                       CHARACTERISTIC_UUID_TX,
                       BLECharacteristic::PROPERTY_NOTIFY
                     );
  characteristicTX->addDescriptor(new BLE2902());
 
  service->start(); //Inicia o serviço.
  server->getAdvertising()->start(); //Faz os dispositivos verem o ESP32.

  Serial.println("Esperando algum dispositivo conectar...");
}

void loop(){

  if(deviceConnected){
    value = analogRead(pinPot)*3.3/4095;
    char cvalue[3];
    dtostrf(value, 1, 2,  cvalue);
    characteristicTX->setValue(cvalue);
    characteristicTX->notify();
    }
    delay(1);
}
