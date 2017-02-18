/*!
 * Implementation on non-template methods from control_types.h
 */
#include "control_info.h"

inline bool CheckIfValueIs01(const std::string &value) {
    return value.size() == 1 && (value[0] == '0' || value[0] == '1');
}

inline bool CheckIfValueIs1(const std::string &value) {
    return value.size() == 1 && value[0] == '1';
}

inline bool TControlType::CheckStringIfInteger(const std::string &value, int *integer) {
    int valueInteger, readLength;
    int status = sscanf(value.c_str(), "%d%n", &valueInteger, &readLength);
    if (integer != nullptr)
        *integer = valueInteger;
    return status == 1 && readLength == (int)value.length();
}

inline std::string IntegerToString(int value) {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return buffer;
}


bool TControlType::CheckStringIfFloat(const std::string &value, double *floatPointer) {
    double valueFloat;
    int readLength;
    int status = sscanf(value.c_str(), "%lf%n", &valueFloat, &readLength);
    if (floatPointer != nullptr)
        *floatPointer = valueFloat;
    return status == 1 && readLength == (int)value.length();
}



bool TControlType::ValidateValue(const std::string &value) { return true; }
void TControlType::PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher) {
    publisher("/meta/type", MetaType);
}
void TControlType::DeleteMetaInfo(std::function<void(const std::string &)> deleter) {
    deleter("/meta/type");
}

TControlType::TControlType(const std::string &metaType, const std::string &defaultName, const std::string &defaultDescription):
    MetaType(metaType), DefaultName(defaultName), DefaultDescription(defaultDescription)
{}

TControlType::~TControlType() {}





bool TControlType::TRange::ValidateValue(const std::string &value) {
    int valueInteger;
    if (!CheckStringIfInteger(value, &valueInteger))
        return false;
    else
        return 0 <= valueInteger && valueInteger <= Maximum;
}
void TControlType::TRange::PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher) {
    publisher("/meta/type", MetaType);
    publisher("/meta/max", IntegerToString(Maximum));
}
void TControlType::TRange::DeleteMetaInfo(std::function<void(const std::string &)> deleter) {
    deleter("/meta/type");
    deleter("/meta/max");
}

TControlType::TRange::TRange(int maximum): Maximum(maximum)
{
    if (maximum <= 1)
        throw std::invalid_argument("maximum must be greater then 1");
}




bool TControlType::TColorRgb::ValidateValue(const std::string &value) {
    int r, g, b, readLength;
    int status = sscanf(value.c_str(), "%d;%d;%d%n", &r, &g, &b, &readLength);
    return (!((r | g | b) & ~255) && status == 3 && readLength == (int)value.size());
}




void TControlType::TGeneric::PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher) {
    publisher("/meta/type", MetaType);
    publisher("/meta/units", Units);
}
void TControlType::TGeneric::DeleteMetaInfo(std::function<void(const std::string &)> deleter) {
    deleter("/meta/type");
    deleter("/meta/units");
}

TControlType::TGeneric::TGeneric(const std::string &units): Units(units)
{}



