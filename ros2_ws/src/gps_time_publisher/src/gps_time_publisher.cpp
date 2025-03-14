#include "rclcpp/rclcpp.hpp"
#include "inertial_sense_ros2/msg/gps.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

class GPSTimePublisher : public rclcpp::Node
{
private:
    void gps_callback(const inertial_sense_ros2::msg::GPS::SharedPtr msg)
    {
        // Extract integer and fractional parts
        int64_t int_sec = msg->header.stamp.sec;
        int64_t int_nsec = msg->header.stamp.nanosec;
        double gps_time = int_sec + int_nsec * 1e-9;  // Full precision GPS time

        // Convert to UTC time struct
        std::time_t time_t_gps = static_cast<std::time_t>(int_sec);
        std::tm *utc_time = std::gmtime(&time_t_gps);

        // Log values for debugging
        RCLCPP_DEBUG(this->get_logger(), "gps_time: %.9f", gps_time);
        // RCLCPP_INFO(this->get_logger(), "time_t_gps: %ld", time_t_gps);

        // Format as NMEA $GPZDA with millisecond precision
        std::ostringstream nmea_sentence;
        nmea_sentence << "$GPZDA,"
                    << std::setw(2) << std::setfill('0') << utc_time->tm_hour
                    << std::setw(2) << std::setfill('0') << utc_time->tm_min
                    << std::setw(2) << std::setfill('0') << utc_time->tm_sec
                    << "." << std::setw(3) << std::setfill('0') << (int_nsec / 1000000) // Milliseconds
                    << ","
                    << std::setw(2) << std::setfill('0') << utc_time->tm_mday << ","
                    << std::setw(2) << std::setfill('0') << (utc_time->tm_mon + 1) << ","
                    << utc_time->tm_year + 1900 << ",00,00*00\r\n";

        std::string nmea_str = nmea_sentence.str();

        // Write to serial port
        if (serial_stream_.is_open())
        {
            serial_stream_ << nmea_str;
            serial_stream_.flush();
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Serial port not open!");
        }
    }


    rclcpp::Subscription<inertial_sense_ros2::msg::GPS>::SharedPtr subscription_;
    std::ofstream serial_stream_;

public:
    GPSTimePublisher() : Node("gps_time_publisher")
    {
        subscription_ = this->create_subscription<inertial_sense_ros2::msg::GPS>(
            "/inertial_sense/gps1/pos_vel", 10, std::bind(&GPSTimePublisher::gps_callback, this, std::placeholders::_1)
        );

        // Open virtual serial port
        serial_stream_.open("/dev/tnt1", std::ios::out | std::ios::binary);
        if (!serial_stream_.is_open())
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open /dev/tnt1");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Opened /dev/tnt1 successfully");
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
