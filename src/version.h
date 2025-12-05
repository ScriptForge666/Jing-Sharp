#pragma once
#include <string>
#include <string_view>

inline constexpr std::string_view ProjectName = "peculiarc";
inline constexpr std::string_view ProjectVersion = "0.9.14";
inline constexpr std::string_view ProjectContributor = "Scriptforge";
inline constexpr std::string_view ProjectLicense = "Apache License 2.0";

std::string getVersion();
std::string getCopyright();
int getYear();