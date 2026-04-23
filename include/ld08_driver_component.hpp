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

#ifndef LD08_DRIVER_COMPONENT_HPP_
#define LD08_DRIVER_COMPONENT_HPP_

#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

#include "cmd_interface_linux.hpp"
#include "lipkg.hpp"

namespace ld08_driver
{

class LD08DriverComponent : public rclcpp::Node
{
public:
  explicit LD08DriverComponent(const rclcpp::NodeOptions & options);
  ~LD08DriverComponent() = default;

private:
  void timer_callback();

  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr lidar_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<LiPkg> pkg_;
  std::unique_ptr<CmdInterfaceLinux> cmd_port_;

  std::string frame_id_;
};

}  // namespace ld08_driver

#endif  // LD08_DRIVER_COMPONENT_HPP_