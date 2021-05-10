// Copyright 2021 The Rebooters
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
// limitations under the License.#include <memory>

#include <string>
#include <vector>

#include "blackboard/BlackBoardNode.hpp"
#include "blackboard/BlackBoardClient.hpp"

#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto client = blackboard::BlackBoardClient::make_shared();

  auto node = rclcpp::Node::make_shared("move_param_node");

  node->declare_parameter("waypoints");
  node->declare_parameter("waypoint_coords");
  node->declare_parameter("rooms_colors");

  if (node->has_parameter("waypoints")) {
    std::vector<std::string> wp_names;

    node->get_parameter_or("waypoints", wp_names, {});

    for (auto & wp : wp_names) {
      node->declare_parameter("waypoint_coords." + wp);

      std::vector<double> coords;
      if (node->get_parameter_or("waypoint_coords." + wp, coords, {})) {
        geometry_msgs::msg::PoseStamped wp_pose;
        wp_pose.header.frame_id = "/map";
        // wp_pose.header.stamp = now();
        wp_pose.pose.position.x = coords[0];
        wp_pose.pose.position.y = coords[1];
        wp_pose.pose.position.z = coords[2];
        wp_pose.pose.orientation.x = 0.0;
        wp_pose.pose.orientation.y = 0.0;
        wp_pose.pose.orientation.z = 0.0;
        wp_pose.pose.orientation.w = 1.0;

        auto entry = blackboard::Entry<geometry_msgs::msg::PoseStamped>::make_shared(wp_pose);
        client->add_entry(wp, "location", entry->to_base());
      }

      node->declare_parameter("rooms_colors." + wp);

      std::string color;
      if (node->get_parameter_or("rooms_colors." + wp, color, {})) {
        RCLCPP_INFO(node->get_logger(), "Color %s", color);
        auto entry_color = blackboard::Entry<std::string>::make_shared(color);
        client->add_entry(wp, "color", entry_color->to_base());
      }
    }
  }

  return 0;
}
