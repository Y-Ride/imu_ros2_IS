#include "rclcpp/rclcpp.hpp"
#include "inertial_sense_ros2/msg/gps.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

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
        // std::ostringstream nmea_sentence;
        // nmea_sentence << "$GPZDA,"
        //             << std::setw(2) << std::setfill('0') << utc_time->tm_hour
        //             << std::setw(2) << std::setfill('0') << utc_time->tm_min
        //             << std::setw(2) << std::setfill('0') << utc_time->tm_sec
        //             << "." << std::setw(3) << std::setfill('0') << (int_nsec / 1000000) // Milliseconds
        //             << ","
        //             << std::setw(2) << std::setfill('0') << utc_time->tm_mday << ","
        //             << std::setw(2) << std::setfill('0') << (utc_time->tm_mon + 1) << ","
        //             << utc_time->tm_year + 1900 << ",00,00*00\r\n";

        // std::string nmea_str = nmea_sentence.str();

        // Convert lat/lon to NMEA format
        auto convert_to_nmea = [](double deg, bool is_lat)
        {
            char hemi = (is_lat ? (deg >= 0 ? 'N' : 'S') : (deg >= 0 ? 'E' : 'W'));
            deg = std::abs(deg);
            int d = static_cast<int>(deg);
            double m = (deg - d) * 60;
            char buffer[16];
            if (is_lat)
                std::snprintf(buffer, sizeof(buffer), "%02d%07.4f", d, m);
            else
                std::snprintf(buffer, sizeof(buffer), "%03d%07.4f", d, m);
            return std::make_pair(std::string(buffer), hemi);
        };

        auto [lat_str, ns] = convert_to_nmea(msg->latitude, true);
        auto [lon_str, ew] = convert_to_nmea(msg->longitude, false);

        // Format time string hhmmss.ss
        char time_str[16];
        std::snprintf(time_str, sizeof(time_str), "%02d%02d%02d.%02d", utc_time->tm_hour, utc_time->tm_min, utc_time->tm_sec, (int)(int_nsec / 10000000));

        // Format date string ddmmyy
        char date_str[16];
        std::snprintf(date_str, sizeof(date_str), "%02d%02d%02d", utc_time->tm_mday, utc_time->tm_mon + 1, utc_time->tm_year % 100);

        // Assemble GPRMC (speed and track = 0.0)
        std::ostringstream gprmc;
        gprmc << "$GPRMC," << time_str << ",A," << lat_str << "," << ns << ","
              << lon_str << "," << ew << ",0.0,0.0," << date_str << ",,,";

        // Compute checksum
        std::string gprmc_body = gprmc.str().substr(1); // Excludes the initial '$'
        unsigned char checksum = 0;
        for (char c : gprmc_body)
            checksum ^= c;

        // Append checksum
        gprmc << "*" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)checksum << "\r\n";
        std::string nmea_str = gprmc.str();



        // Write to serial port
        // if (serial_stream_.is_open())
        // {
        //     serial_stream_ << nmea_str;
        //     serial_stream_.flush();
        // }
        // else
        // {
        //     RCLCPP_ERROR(this->get_logger(), "Serial port not open!");
        // }
        if (serial_fd_ >= 0)
        {
            ssize_t result = write(serial_fd_, nmea_str.c_str(), nmea_str.length());
            if (result < 0)
            {
                if (errno == EAGAIN)
                {
                    RCLCPP_WARN(this->get_logger(), "Write skipped: no reader connected to /dev/tnt0");
                }
                else
                {
                    RCLCPP_ERROR(this->get_logger(), "Write error: %s", strerror(errno));
                }
            }
        }
    }


    rclcpp::Subscription<inertial_sense_ros2::msg::GPS>::SharedPtr subscription_;
    std::ofstream serial_stream_;
    int serial_fd_ = -1;

public:
    GPSTimePublisher() : Node("gps_time_publisher")
    {
        subscription_ = this->create_subscription<inertial_sense_ros2::msg::GPS>(
            "/inertial_sense/gps1/pos_vel", 10, std::bind(&GPSTimePublisher::gps_callback, this, std::placeholders::_1)
        );

        // Open virtual serial port
        // serial_stream_.open("/dev/tnt1", std::ios::out | std::ios::binary);
        // if (!serial_stream_.is_open())
        // {
        //     RCLCPP_ERROR(this->get_logger(), "Failed to open /dev/tnt1");
        // }
        // else
        // {
        //     RCLCPP_INFO(this->get_logger(), "Opened /dev/tnt1 successfully");
        // }
        serial_fd_ = open("/dev/tnt1", O_WRONLY | O_NONBLOCK);
        if (serial_fd_ < 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open /dev/tnt1: %s", strerror(errno));
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Opened /dev/tnt1 successfully");
        }

    }

    ~GPSTimePublisher()
    {
        if (serial_fd_ >= 0)
        {
            close(serial_fd_);
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
