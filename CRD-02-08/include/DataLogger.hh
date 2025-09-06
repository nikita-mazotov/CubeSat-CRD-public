// Auxilliary class - opens and writes csv 
// N. Mazotov, Yale Cubesat, 31/07/2025

#include "G4Threading.hh"

#ifndef B1_DATALOGGER_HH
#define B1_DATALOGGER_HH


#include <fstream>
    #include <chrono>
    #include <ctime>
    #include <sstream>
    #include <iomanip>
    #include <string>

namespace B1 {

class CsvLogger {
public:
    CsvLogger(const std::string& baseName) {
        filename_ = GetTimestampedFilename(baseName);
        ofs_.open(filename_, std::ios::out);
    }

    ~CsvLogger() {
        if (ofs_.is_open()) ofs_.close();
    }

    bool IsOpen() const { return ofs_.is_open(); }

    template<typename T>
    void WriteRow(const std::initializer_list<T>& fields) {
        std::ostringstream line;
        for (auto it = fields.begin(); it != fields.end(); ++it) {
            if (it != fields.begin()) line << ",";
            line << *it;
        }
        if (ofs_.is_open()) {
            ofs_ << line.str() << "\n";
            ofs_.flush();
        }
    }

    const std::string& GetFilename() const { return filename_; }

private:
    std::string GetTimestampedFilename(const std::string& baseName) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << baseName << "_";
        ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S") << ".csv";
        return ss.str();
    }

    std::string filename_;
    std::ofstream ofs_;
};

}  // namespace B1

#endif  // B1_DATALOGGER_HH