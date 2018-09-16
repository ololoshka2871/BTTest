/*

 SPBTLE_SensorDemo

 This sketch provides a default example how to use BLE with:
  - Discovery L475VG IoT board

 For the Sensor Service sketch, 3 services are started : Acc, Environnemental and Time.
 For testing the sketch, you can download on the playstore the "BlueNRG"
 application provided by STMICROELECTRONICS.
 Launch the application and enable Bluetooth on your smartphone. Connect it to
 the BLueNRG device. You will see all the services, you can click on each one.

 You can choose the bluetooth MAC address of the device by configuring SERVER_BDADDR.

 Accelerometer values are updated on user action (press user button).
 Environnemental values (Temperature, humidity and pressure) are updated each seconds.
 Each minute a notification is sent to the user and seconds can be read.

 */

#include <SPI.h>
#include <SPBTLE_RF.h>
#include <sensor_service.h>

#include "DMAHalper.h"

#define PIN_BLE_SPI_MOSI   (PB15)
#define PIN_BLE_SPI_MISO   (PB14)
#define PIN_BLE_SPI_SCK    (PB13)

#define PIN_BLE_SPI_nCS    (PB12)
#define PIN_BLE_SPI_RESET  (PA8)
#define PIN_BLE_SPI_IRQ    (PB6)

#define USER_BTN    (PB5)

// Configure BTLE_SPI
SPIClass BTLE_SPI(SPI2, PIN_BLE_SPI_MOSI, PIN_BLE_SPI_MISO, PIN_BLE_SPI_SCK);

// Configure BTLE pins
SPBTLERFClass BTLE(&BTLE_SPI, PIN_BLE_SPI_nCS, PIN_BLE_SPI_IRQ, PIN_BLE_SPI_RESET);

const char *name = "BlueNRG";
uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x03};

AxesRaw_t axes_data;
uint32_t previousSecond = 0;

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 *
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(AxesRaw_t* p_axes)
{
  /* Check if the user has pushed the button */
  if(digitalRead(USER_BTN) == RESET)
  {
    while (digitalRead(USER_BTN) == RESET);

    /* Update acceleration data */
    p_axes->AXIS_X += 100;
    p_axes->AXIS_Y += 100;
    p_axes->AXIS_Z += 100;
    SensorService.Acc_Update(p_axes);
  }
}


void _setup() {
  int ret;

  //Serial.begin(9600);


  //
  BTLE_SPI.begin();
  BTLE_SPI.beginTransaction({8000000, MSBFIRST, SPI_MODE0}); // настрокай SPI
  // Прерыванию на ножку PIN_BLE_SPI_IRQ будет назначен приоритет 6 внутри BTLE.begin(), нужно чтобы у DMA был выше
  FixSPI_DMA_IRQ_Prio(&BTLE_SPI.spiHandle, 5);

  pinMode(PIN_BLE_SPI_IRQ, INPUT_PULLDOWN); // Написано, что нужно подтянуть к 0

  if(BTLE.begin() == SPBTLERF_ERROR)
  {
    //Serial.println("Bluetooth module configuration error!");
    while(1);
  }

  // вот тут внутри ставится PIN-код, по дуфолту он равен 123456
  if(SensorService.begin(name, SERVER_BDADDR))
  {
    //Serial.println("Sensor service configuration error!");
    while(1);
  }

  /* Configure the User Button in GPIO Mode */
  pinMode(USER_BTN, INPUT_PULLUP);

  ret = SensorService.Add_Acc_Service();

  if(ret == BLE_STATUS_SUCCESS){
    //Serial.println("Acc service added successfully.");
  } else {
    //Serial.println("Error while adding Acc service.");
  }

  ret = SensorService.Add_Environmental_Sensor_Service();

  if(ret == BLE_STATUS_SUCCESS) {
    //Serial.println("Environmental Sensor service added successfully.");
  } else {
    //Serial.println("Error while adding Environmental Sensor service.");
  }

  randomSeed(/*analogRead(A0)*/0);

  /* Instantiate Timer Service with two characteristics:
   * - seconds characteristic (Readable only)
   * - minutes characteristics (Readable and Notifiable )
   */
  ret = SensorService.Add_Time_Service();

  if(ret == BLE_STATUS_SUCCESS) {
    //Serial.println("Time service added successfully.");
  } else {
    //Serial.println("Error while adding Time service.");
  }
}

void _loop() {
  BTLE.update();

  if(SensorService.isConnected() == TRUE)
  {
    //Update accelerometer values
    User_Process(&axes_data);

    //Update time
    SensorService.Update_Time_Characteristics();

    if((millis() - previousSecond) >= 1000)
    {
      //Update environnemental data
      //Data are set with random values but can be replace with data from sensors.
      previousSecond = millis();
      SensorService.Temp_Update(random(-100,400));
      SensorService.Press_Update(random(95000,105000));
      SensorService.Humidity_Update(random(0,100));
    }
  }
  else
  {
    //Keep the Bluetooth module in discoverable mode
    SensorService.setConnectable();
  }
}
