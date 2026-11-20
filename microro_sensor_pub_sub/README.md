# Micro-ROS Sensor & Actuator Node (ESP32 + Arduino + PlatformIO)

English | [Tiếng Việt](#tiếng-viet)

## Overview
- **Publisher**: Reads a simulated sensor value and publishes to `/sensor_data`
  (`sensor_msgs/Float32MultiArray`) at **10 Hz**.
- **Subscriber**: Subscribes to `/led_control` (`std_msgs/Bool`) and turns the
  built-in LED (GPIO 2) on/off.
- **Transport**: WiFi UDP to a micro-ROS Agent running on your PC.
- **Board**: ESP32 DevKit (PlatformIO, Arduino framework).
- **Host**: Ubuntu 22.04.

## Prerequisites on Ubuntu 22.04 (PC)

### 1. Install ROS2 Humble
```bash
# Set locale
sudo apt update && sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# Add ROS2 repository
sudo apt install -y software-properties-common curl
sudo add-apt-repository universe
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Install ROS2 Humble Desktop
sudo apt update
sudo apt install -y ros-humble-desktop python3-rosdep2
sudo rosdep init
rosdep update
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```
### 2. Install micro-ROS tools
```bash
sudo apt install -y python3-pip
pip3 install -U colcon-common-extensions ros-humble-rmw-cyclonedds-cpp
pip3 install -U empy==3.3.4 lark==1.1.5 argparse numpy
ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh
source ~/micro_ros_ws/install/setup.bash
```

### 3. Start the micro-ROS Agent (on PC)
The agent bridges WiFi UDP <-> ROS2. Run **one** terminal:
```bash
source /opt/ros/humble/setup.bash
source ~/micro_ros_ws/install/setup.bash
ros2 run micro_ros_agent udp4 --port 8888 -v6
```
Leave this running. The ESP32 will connect to this agent.

## Project Structure

```
microro_sensor_pub_sub/
├── platformio.ini          # PlatformIO project config
├── src/
│   └── main.cpp            # ESP32 application code
└── README.md               # This file
```

### platformio.ini
Targets `esp32dev`, Arduino framework, upload to `/dev/ttyUSB0`.  
Change `upload_port`/`monitor_port` to match your ESP32 USB port
(check `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`).

### src/main.cpp
Key constants you **must edit** before building:
```cpp
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define AGENT_IP      "192.168.1.100"   /* PC IP running the agent */
#define AGENT_PORT    8888
```
Get your PC IP with `hostname -I`. Ensure the ESP32 and PC are on the
**same WiFi network**.

## Build & Flash (on Ubuntu)

1. Open the project folder in VS Code with the **PlatformIO** extension installed.
2. Edit `src/main.cpp` with your WiFi credentials and PC IP (`hostname -I`).
3. Connect the ESP32 via USB.
4. Build:
   ```bash
   pio run
   ```
5. Upload:
   ```bash
   pio run --target upload
   ```
6. Open serial monitor:
   ```bash
   pio device monitor
   ```
   You should see: `WiFi connected! IP: ...` and `micro-ROS node ready.`

## Testing (PC side)

Open **two** new terminals on the PC (source ROS2 in each):

### Terminal A — echo sensor data
```bash
source /opt/ros/humble/setup.bash
ros2 topic echo /sensor_data
```
Expected output: messages arriving every ~100 ms with `[voltage, angle, time]`.

### Terminal B — publish LED control
```bash
source /opt/ros/humble/setup.bash
ros2 topic pub /led_control std_msgs/msg/Bool "{data: true}"
ros2 topic pub /led_control std_msgs/msg/Bool "{data: false}"
```
Watch the ESP32 built-in LED (GPIO 2) turn on/off and check the serial monitor
for `[SUB] ...` logs.

## Troubleshooting

| Problem | Likely cause | Fix |

---

## Tiếng Việt

### Tổng quan
- **Publisher**: Đọc giá trị cảm biến mô phỏng và publish lên `/sensor_data`
  (`sensor_msgs/Float32MultiArray`) tần số **10 Hz**.
- **Subscriber**: Đăng ký `/led_control` (`std_msgs/Bool`) và bật/tắt LED đen
  (GPIO 2).
- **Giao tiếp**: WiFi UDP đến micro-ROS Agent chạy trên PC.
- **Board**: ESP32 DevKit (PlatformIO, framework Arduino).
- **Máy chủ**: Ubuntu 22.04.

### Điều kiện trước trên Ubuntu 22.04 (PC)

#### 1. Cài ROS2 Humble
```bash
# Thiết lập locale
sudo apt update && sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# Thêm kho ROS2
sudo apt install -y software-properties-common curl
sudo add-apt-repository universe
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Cài ROS2 Humble Desktop
sudo apt update
sudo apt install -y ros-humble-desktop python3-rosdep2
sudo rosdep init
rosdep update
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

#### 2. Cài công cụ micro-ROS
```bash
sudo apt install -y python3-pip
pip3 install -U colcon-common-extensions ros-humble-rmw-cyclonedds-cpp
pip3 install -U empy==3.3.4 lark==1.1.5 argparse numpy
ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh
source ~/micro_ros_ws/install/setup.bash
```

#### 3. Chạy micro-ROS Agent (trên PC)
Agent nối WiFi UDP <-> ROS2. Chạy trong **một** terminal:
```bash
source /opt/ros/humble/setup.bash
source ~/micro_ros_ws/install/setup.bash
ros2 run micro_ros_agent udp4 --port 8888 -v6
```
Để terminal này chạy. ESP32 sẽ kết nối đến agent này.

### Cấu trúc dự án PlatformIO

```
microro_sensor_pub_sub/
├── platformio.ini          # Cấu hình dự án PlatformIO
├── src/
│   └── main.cpp            # Mã ứng dụng ESP32
└── README.md               # Tệp này
```

#### platformio.ini
Nhắm đến `esp32dev`, framework Arduino, upload đến `/dev/ttyUSB0`.  
Thay `upload_port`/`monitor_port` cho khớp với cổng USB của ESP32
(kiểm tra `ls /dev/ttyUSB*` hoặc `ls /dev/ttyACM*`).

#### src/main.cpp
Các hằng số bạn **phải sửa** trước khi nén/build:
```cpp
#define WIFI_SSID     "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"
#define AGENT_IP      "192.168.1.100"   /* IP của PC chạy agent */
#define AGENT_PORT    8888
```
Lấy IP PC bằng lệnh `hostname -I`. Đảm bảo ESP32 và PC nằm trong **mạng WiFi
giống nhau**.

### Build & Nén firmware (trên Ubuntu)

1. Mở thư mục dự án trong VS Code đã cài tiện ích **PlatformIO**.
2. Sửa `src/main.cpp` với thông tin WiFi và IP PC.
3. Cắm ESP32 qua USB.
4. Build:
   ```bash
   pio run
   ```
5. Upload:
   ```bash
   pio run --target upload
   ```
6. Mở serial monitor:
   ```bash
   pio device monitor
   ```
   Bạn sẽ thấy: `WiFi connected! IP: ...` và `micro-ROS node ready.`

### Kiểm thử (bên PC)

Mở **hai** terminal mới trên PC (source ROS2 trong mỗi terminal):

#### Terminal A — xem dữ liệu cảm biến
```bash
source /opt/ros/humble/setup.bash
ros2 topic echo /sensor_data
```
Kết quả mong đợi: tin nhắn đến mỗi ~100 ms với mảng `[điện áp, góc, thời gian]`.

#### Terminal B — điều khiển LED
```bash
source /opt/ros/humble/setup.bash
ros2 topic pub /led_control std_msgs/msg/Bool "{data: true}"
ros2 topic pub /led_control std_msgs/msg/Bool "{data: false}"
```
Quản sát LED đen trên ESP32 (GPIO 2) bật/tắt và kiểm tra log serial
`[SUB] ...`.

### Khắc phục sự cố

| Vấn đề | Nguyên nhân có thể | Cách khắc phục |
|--------|-------------------|----------------|
| Không kết nối WiFi | Sai SSID/Mật khẩu | Sửa `src/main.cpp`, nén lại |
| Node chưa sẵn sàng / không thấy topic | Sai AGENT_IP hoặc agent chưa chạy | Đặt IP PC, khởi động `micro_ros_agent udp4 --port 8888` |
| Không thấy topic | Tường lửa chặn UDP 8888 | `sudo ufw allow 8888/udp` |
| Lỗi build | Thiếu thư viện micro-ROS | Đảm bảo PlatformIO đã tải `micro_ros_arduino` |

|---------|--------------|-----|
| WiFi not connecting | Wrong SSID/Password | Edit `src/main.cpp`, re-upload |
| Node not ready / no topics | Wrong AGENT_IP or agent not running | Set PC IP, start `micro_ros_agent udp4 --port 8888` |
| Topics not visible | Firewall blocking UDP 8888 | `sudo ufw allow 8888/udp` |
| Build fails | Missing micro-ROS lib | Ensure PlatformIO downloaded `micro_ros_arduino` |
