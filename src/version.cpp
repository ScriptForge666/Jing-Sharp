#include "version.h"
#include <chrono>
#include <string>
#include <sstream>
#include <format>

std::string getVersion() {
    return std::string(ProjectName) + " version " + std::string(ProjectVersion);
}

int getYear() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto dp = floor<days>(now);
    year_month_day ymd{dp};
    return int(ymd.year());
}

std::string getCopyright() {
    std::ostringstream oss;
    oss << "Copyright " << getYear() << " " << ProjectContributor
        << "\nLicensed under the " << ProjectLicense;
    return oss.str();
}