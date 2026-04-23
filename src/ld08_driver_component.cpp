// Copyright 2021 ROBOTIS CO., LTD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: LD Robot, Will Son

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "rclcpp_components/register_node_macro.hpp"
#include "../include/ld08_driver_component.hpp"

namespace ld08_driver
{

LD08DriverComponent::LD08DriverComponent(const rclcpp::NodeOptions & options)
: Node("laser_scan_publisher", options)
{
  declare_parameter<std::string>("frame_id", "base_scan");
  declare_parameter<std::string>("namespace", "");

  frame_id_ = get_parameter("frame_id").as_string();
  const std::string name_space = get_parameter("namespace").as_string();
  if (!name_space.empty()) {
    frame_id_ = name_space + "/" + frame_id_;
  }

  pkg_ = std::make_unique<LD08_LiPkg>();
  cmd_port_ = std::make_unique<CmdInterfaceLinux>(8);

  std::vector<std::pair<std::string, std::string>> device_list;
  cmd_port_->GetCmdDevices(device_list);

  std::string port_name;
  for (const auto & dev : device_list) {
    std::cout << dev.first << "    " << dev.second << std::endl;
    if (strstr(dev.second.c_str(), "CP2102")) {
      port_name = dev.first;
    }
  }

  if (port_name.empty()) {
    RCLCPP_ERROR(get_logger(), "Can't find LDS-02 device. Component will not publish.");
    return;
  }

  RCLCPP_INFO(get_logger(), "Found LDS-02 on port: %s", port_name.c_str());

  LiPkg * raw_pkg = pkg_.get();
  cmd_port_->SetReadCallback(
    [raw_pkg](const char * byte, size_t len) {
      if (raw_pkg->Parse(reinterpret_cast<const uint8_t *>(byte), len)) {
        raw_pkg->AssemblePacket();
      }
    });

  if (!cmd_port_->Open(port_name)) {
    RCLCPP_ERROR(get_logger(), "Failed to open port %s", port_name.c_str());
    return;
  }

  RCLCPP_INFO(get_logger(), "LDS-02 started successfully");

  lidar_pub_ = create_publisher<sensor_msgs::msg::LaserScan>(
    "scan", rclcpp::QoS(rclcpp::SensorDataQoS()));

  timer_ = create_wall_timer(
    std::chrono::milliseconds(10),
    std::bind(&LD08DriverComponent::timer_callback, this));
}

void LD08DriverComponent::timer_callback()
{
  if (pkg_->IsFrameReady()) {
    pkg_->setStamp(now());
    pkg_->setFrameId(frame_id_);
    lidar_pub_->publish(pkg_->GetLaserScan());
    pkg_->ResetFrameReady();
  }
}

}  // namespace ld08_driver

RCLCPP_COMPONENTS_REGISTER_NODE(ld08_driver::LD08DriverComponent)
