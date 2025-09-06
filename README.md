# Smart Dehydrator

An intelligent food dehydrator system built with ESP32 that provides automated temperature control, safety monitoring, and remote operation capabilities through WiFi connectivity.

![Organic Farm](Organic%20Farm.png)

## Features

### 🌡️ **Temperature Control**
- Precise temperature monitoring using DS18B20 and DHT22 sensors
- Automatic heating control to maintain desired temperature
- Overheating protection (automatically shuts down at 70°C)
- Real-time temperature display on LCD

### 🛡️ **Safety Features**
- **Gas Detection**: MQ2 sensor for detecting harmful gases
- **Automatic Ventilation**: Servo-controlled vents for air circulation
- **Emergency Shutdown**: Immediate system shutdown on gas detection
- **Overheating Protection**: Automatic cooling when temperature exceeds safe limits

### 📱 **Remote Control & Monitoring**
- WiFi-enabled web interface for remote operation
- Set temperature and duration from any device
- Real-time status updates on LCD display
- SMS notifications for process completion and alerts

### ⏰ **Smart Timing**
- NTP time synchronization for accurate timing
- Automatic start/stop based on set duration
- Countdown display showing remaining time
- Process completion notifications

### 💨 **Environmental Control**
- Inside and outside humidity monitoring
- Automatic fan control for optimal air circulation
- Temperature-based ventilation control
- Humidity-based cooling system activation

## Hardware Requirements

### **Main Components**
- **ESP32 Development Board** - Main microcontroller
- **DS18B20 Temperature Sensor** - Internal temperature monitoring
- **DHT22 Sensors (x2)** - External temperature and humidity monitoring
- **MQ2 Gas Sensor** - Safety gas detection
- **16x2 LCD Display (I2C)** - Status display
- **GSM Module** - SMS notifications
- **Servo Motor** - Ventilation control
- **Relay Modules** - Heater and fan control

### **Additional Components**
- Heating element
- Cooling fan
- LEDs for status indication
- Buzzer for audio alerts
- Resistors and connecting wires
- Power supply (appropriate for your heating element)

### **Pin Configuration**
```
MQ2 Gas Sensor    → Pin 25
Fan Control       → Pin 26
Heater Control    → Pin 15
DS18B20 (OneWire) → Pin 13
DHT22 #1          → Pin 14
DHT22 #2          → Pin 4
Status LED 1      → Pin 18
Status LED 2      → Pin 19
Buzzer            → Pin 23
Servo Motor       → Pin 27
GSM Module RX     → Pin 17
GSM Module TX     → Pin 16
LCD (I2C)         → SDA/SCL pins
```

## Software Requirements

### **Arduino Libraries**
Install the following libraries through Arduino IDE Library Manager:

```
- OneWire
- DallasTemperature
- Wire
- LiquidCrystal_I2C
- DHT sensor library
- WiFi (ESP32)
- WebServer (ESP32)
- ESP32Servo
- SoftwareSerial
```

### **Development Environment**
- Arduino IDE (1.8.x or newer)
- ESP32 Board Package for Arduino IDE

## Installation & Setup

### 1. **Hardware Assembly**
- Connect all components according to the pin configuration
- Ensure proper power supply for heating elements
- Secure all connections and test continuity

### 2. **Software Configuration**
1. Open `dehydedrator.ino` in Arduino IDE
2. Update WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Update SMS phone number:
   ```cpp
   mySerial.println("AT+CMGS=\"+YOUR_PHONE_NUMBER\"");
   ```
4. Compile and upload to ESP32

### 3. **Initial Setup**
1. Power on the system
2. Wait for WiFi connection (IP address will display on LCD)
3. Allow 20 seconds for MQ2 sensor warm-up
4. System is ready for operation

## Usage Instructions

### **Web Interface Control**
1. Connect to the same WiFi network as the dehydrator
2. Open web browser and navigate to the IP address shown on LCD
3. Send HTTP GET request with parameters:
   ```
   http://[ESP32_IP_ADDRESS]/?data1=[TEMPERATURE]&data2=[DURATION_MINUTES]
   ```
   
   **Example:**
   ```
   http://192.168.1.100/?data1=50&data2=120
   ```
   (Sets temperature to 50°C for 120 minutes)

### **API Endpoints**

#### **Start Dehydration Process**
- **URL**: `http://[ESP32_IP]/`
- **Method**: GET
- **Parameters**:
  - `data1`: Target temperature in Celsius (°C)
  - `data2`: Duration in minutes
- **Response**: "Data received and processed"

#### **Example Usage**
```bash
curl "http://192.168.1.100/?data1=45&data2=180"
```

### **Operation Flow**
1. **Gas Safety Check**: System checks for gas leaks
2. **Parameter Setting**: Receive temperature and duration via web interface
3. **Heating Phase**: Maintain target temperature
4. **Monitoring**: Continuous temperature and humidity monitoring
5. **Completion**: Automatic shutdown with SMS notification
6. **Cooling**: Automatic cooling if needed based on environmental conditions

## Safety Features

### **Gas Detection**
- Continuous monitoring with MQ2 sensor
- Immediate heater shutdown on gas detection
- Automatic ventilation activation
- Visual and audio alerts

### **Temperature Safety**
- Maximum temperature limit: 70°C
- Automatic shutdown on overheating
- Emergency cooling activation
- SMS alert for overheating conditions

### **Environmental Protection**
- Humidity-based cooling control
- Temperature differential monitoring
- Automatic ventilation adjustment

## Status Indicators

### **LCD Display Messages**
- `Connected to WiFi` - System connected to network
- `Gas checking...` - MQ2 sensor warming up
- `Gas detected!!!` - Safety alert
- `Process starting...` - Dehydration beginning
- `Heating to: XX.X` - Current target temperature
- `Heater on/off` - Heating element status
- `Waiting until: HH:MM:SS` - Time remaining
- `Finishing time reached!` - Process completed
- `Over heating` - Safety shutdown
- `Process cool down...` - Cooling phase
- `Process over.....` - Final completion

### **LED & Audio Indicators**
- **LED 1**: Process completion indicator
- **LED 2**: Overheating warning
- **Buzzer**: Audio alerts for completion and emergencies

## SMS Notifications

The system sends SMS alerts for:
- Process completion: "process over"
- Overheating condition: "Over heating..."

## Troubleshooting

### **Common Issues**

#### **WiFi Connection Problems**
- Check SSID and password in code
- Ensure ESP32 is within WiFi range
- Verify network supports 2.4GHz (ESP32 limitation)

#### **Temperature Reading Errors**
- Check DS18B20 wiring and power supply
- Verify OneWire bus connection
- Ensure proper pull-up resistor (4.7kΩ)

#### **LCD Display Issues**
- Verify I2C address (default: 0x27)
- Check SDA/SCL connections
- Test with I2C scanner if needed

#### **SMS Not Sending**
- Check GSM module power and antenna
- Verify SIM card and network coverage
- Confirm phone number format
- Check GSM module AT commands

#### **Sensor Calibration**
- DHT22 sensors may need 5-10 seconds between readings
- MQ2 requires 20-second warm-up period
- DS18B20 should be properly sealed if used in humid environment

### **Safety Checks**
- Always test gas detection before operation
- Verify temperature readings with external thermometer
- Ensure proper ventilation in installation area
- Check all electrical connections before powering on

## Technical Specifications

- **Operating Voltage**: 3.3V (ESP32) + appropriate voltage for peripherals
- **Temperature Range**: 0°C to 70°C (safety limited)
- **Humidity Monitoring**: 0-100% RH
- **WiFi Standard**: 802.11 b/g/n (2.4GHz)
- **Time Accuracy**: NTP synchronized
- **SMS Capability**: GSM/2G networks

## License

This project is open-source. Please refer to the license file for terms of use.

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## Support

For technical support or questions, please open an issue in this repository.

---

**⚠️ Safety Warning**: This device controls heating elements and should only be operated by qualified individuals. Always follow electrical safety guidelines and local regulations when building and operating this system.