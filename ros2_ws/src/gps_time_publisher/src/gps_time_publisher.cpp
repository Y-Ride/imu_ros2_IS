#include "rclcpp/rclcpp.hpp"
// #include "inertial_sense_ros2/msg/gps.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

class GPSTimePublisher : public rclcpp::Node
{
private:
    // void gps_callback(cont inertial_sense_msgs::msg::GPS::SharedPtr msg)
    // {
    //     //Convert GPS time to UNIX time
    //     double gps_time = msg->header.stamp.sec + msg->header.stamp.nanosec * 1e-9;

    //     // Convert to UTC time struct
    //     std::time_t time_t_gps = static_cast<std::time_t>(gps_time);
    //     std::tm *utc_time = std::gmtime(&time_t_gps);

    //     // Format as NMEA $GPZDA (UTC time + date)
    //     std::ostringstream nmea_sentence;
    //     nmea_sentence << "$GPZDA,"
    //                   << std::setw(2) << std::setfill('0') << utc_time->tm_hour
    //                   << std::setw(2) << utc_time->tm_min
    //                   << std::setw(2) << utc_time->tm_sec << ","
    //                   << std::setw(2) << utc_time->tm_mday << ","
    //                   << std::setw(2) << utc_time->tm_mon + 1 << ","
    //                   << utc_time->tm_year + 1900 << ",00,00*00\r\n";
        
    //     std::string nmea_str = nmea_sentence.str();

    //     // Write to serial port
    //     if (serial_stream_.is_open())
    //     {
    //         serial_stream_ << nmea_str;
    //         serial_stream_.flush();
    //         RCLCPP_INFO(this->get_logger(), "Sent NMEA: %s", nmea_str.c_str());
    //     }
    //     else
    //     {
    //         RCLCPP_ERROR(this->get_logger(), "Serial port not open!");
    //     }
    // }

    // rclcpp::Subscription<inertial_sense_msg::msg::GPS>::SharedPtr subscription_;
    std::ofstream serial_stream_;

public:
    GPSTimePublisher() : Node("gps_time_publisher")
    {
        // subscription_ = this->create_subscription<inertial_sense_msgs::msg::GPS>(
        //     "/gps1/pos_vel", 10, std::bind(&GPSTimePublisher::gps_callback, this, std::placehoders::_1)
        // );

        // Open virtual serial port
        serial_stream_.open("/dev/tnt1", std::ios::out | std::ios::binary);
        if (!serial_stream_.is_open())
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open /dev/tnt1");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Opened /dev/tnt1 successfully");

            // Write to serial port
            if (serial_stream_.is_open())
            {
                serial_stream_ << "nmea_str";
                serial_stream_.flush();
                RCLCPP_INFO(this->get_logger(), "Sent NMEA: %s", "nmea_str");
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(), "Serial port not open!");
            }
        }

    }
};


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GPSTimePublisher>());
    rclcpp::shutdown();
    return 0;
}
