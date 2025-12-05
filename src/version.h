#include <string>
std::string ProjectName = "peculiarc";
std::string ProjectVersion = "0.9.14";
std::string getVersion() {
	return ProjectName + " version " + ProjectVersion;
}
std::string getCopyright(){
	return "Copyright 2025 Scriptforge\n" + getVersion() + "\nLicensed under the Apache License, Version 2.0";
}
