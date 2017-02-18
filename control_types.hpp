/*!
 * Control types declared here.
 *  
 * 
 * 
 */

#ifndef CONTROL_INFO_H_INCLUDED
#define CONTROL_INFO_H_INCLUDED

#include <string>
#include <functional>
#include <type_traits>
#include <limits>


class TControlType {
  public:
    const std::string &MetaType; ///< It will be written to meta/type
    const std::string &DefaultName; ///< Default name for control if you don't want to think out it by youself.
    const std::string &DefaultDescription; ///< Default description for control if you don't want to think out it by youself.

    /*!
     * Different controls may have different correct values.
     * This method allows to understand if value is correct for this type of control.
     * \param [in] value - Value that should be validated.
     * \return True is value is correct, false otherwise.
     */
    virtual bool ValidateValue(const std::string &value);
    /*!
     * Method allows to publish meta info.
     * 
     * Usage:
     *     controlType->PublishMetaInfo([](const std::string &relativeTopic, const std::string &value) {
     *         publishToMqtt("/device/mega_switch/controls/switch" + topic, value); 
     *     });
     */
    virtual void PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher);
    /*!
     * Method allows to delete meta info added by this control type.
     * 
     * Usage:
     *     controlType->DeleteMetaInfo([](const std::string &relativeTopic) {
     *         deleteFromMqtt("/device/mega_switch/controls/switch" + topic); 
     *     });
     */
    virtual void DeleteMetaInfo(std::function<void(const std::string &)> deleter);

    /*!
     * I hope you never use this constructor, there is a lot of pretty subclasses =)
     * 
     * Parameters are quite obvious. It's references to some data.
     */
    TControlType(const std::string &metaType, const std::string &defaultName, const std::string &defaultDescription);

    virtual ~TControlType();

  protected:
    template <typename TDerived>
    class TControlTypeAutocreate;

    static bool CheckStringIfFloat(const std::string &value, double *floatPointer = nullptr);
    static bool CheckStringIfInteger(const std::string &value, int *floatPointer = nullptr);

  public:

    struct TSwitch;
    struct TAlarm;
    struct TPushButton;
    struct TRange;
    struct TColorRgb;
    struct TGeneric;
    struct TTemperature;
    struct THumidity;
    struct TAtmosphericPressure;
    struct TPrecipitationRate;
    struct TWindSpeed;
    struct TPower;
    struct TPowerConsumption;
    struct TVoltage;
    struct TWaterFlow;
    struct TWaterTotalConsumption;
    struct TResistance;
    struct TGasConcentration;
    struct THeatPower;
    struct THeatEnergy;

};


/*!
 * Some specific type traits to parse structures - subclasses of TControlTypeAutocreate.
 */
namespace NControlInfoTypeTraits {

template <typename T>
constexpr auto has_type_field(int n) -> decltype( bool(typename T::TValueType()) ){
    return true;
}

template <typename T>
constexpr bool has_type_field(char n) {
    return false;
}

template <typename T, bool hasType = has_type_field<T>(0)>
struct extract_type {
    typedef void TValueType;
};

template <typename T>
struct extract_type<T, true> {
    typedef typename T::TValueType TValueType;
};





template <typename T>
constexpr auto has_maximum_field(int n) -> decltype(bool(int(T::Maximum))){
    return true;
}

template <typename T>
constexpr bool has_maximum_field(char n) {
    return false;
}

template <typename T, bool hasMaximum = has_maximum_field<T>(0)>
struct extract_maximum {
    static constexpr double Maximum = std::numeric_limits<double>::infinity();
};

template <typename T>
struct extract_maximum<T, true> {
    static constexpr decltype(T::Maximum) Maximum = T::Maximum;
};





template <typename T>
constexpr auto has_minimum_field(int n) -> decltype(bool(int(T::Minimum))){
    return true;
}

template <typename T>
constexpr bool has_minimum_field(char n) {
    return false;
}

template <typename T, bool hasMinimum = has_minimum_field<T>(0)>
struct extract_minimum {
    static constexpr double Minimum = -std::numeric_limits<double>::infinity();
};

template <typename T>
struct extract_minimum<T, true> {
    static constexpr decltype(T::Minimum) Minimum = T::Minimum;
};


};

/*!
 * The powerful intermediate class, which enables simple creation of control types.
 * 
 * Usage:
 * \code
 *     // Note that all variables' and typedefs' names mentioned in this example
 *     // are used by TControlTypeAutocreate for default creation and validating.
 *     //                   V         <- the same types ->          V
 *     struct TControlType::TSwitch : public TControlTypeAutocreate<TSwitch> {
 *         static constexpr TCStringConst
 *             MetaTypeCString =           "switch",  // Here you set fields
 *             DefaultNameCString =        "switch",  // references to which
 *             DefaultDescriptionCString = "Switch";  // will go to TControlType constructor.
 *
 *         // Next fields used by default validator in TControlTypeAutocreate.
 *         // You are free to omit all of them or any of last two.
 *         typedef int TValueType;           // Type of value (though stored as string). May be int, float (=double), void (no check).
 *         static constexpr int Minimum = 0; // Minimum of correct values
 *         static constexpr int Maximum = 1; // Maximum of correct values
 *     };
 * \endcode
 */
template <typename TDerived>
class TControlType::TControlTypeAutocreate : public TControlType {
  public:
    typedef const char *TCStringConst;

    virtual bool ValidateValue(const std::string &value) override {
        typedef typename NControlInfoTypeTraits::extract_type<TDerived>::TValueType TValueType;
        static_assert(
            std::is_same<TValueType, float>::value ||
            std::is_same<TValueType, double>::value ||
            std::is_same<TValueType, int>::value ||
            std::is_same<TValueType, void>::value,
            "Unsupported type tag"
        );
        auto minimum = NControlInfoTypeTraits::extract_minimum<TDerived>::Minimum;
        auto maximum = NControlInfoTypeTraits::extract_maximum<TDerived>::Maximum;
        if (std::is_same<TValueType, float>::value || std::is_same<TValueType, double>::value) {
            double floatValue;
            if (!CheckStringIfFloat(value, &floatValue))
                return false;
            if (NControlInfoTypeTraits::has_minimum_field<TDerived>(0) && minimum > floatValue)
                return false;
            if (NControlInfoTypeTraits::has_maximum_field<TDerived>(0) && maximum < floatValue)
                return false;
        }

        if (std::is_same<TValueType, int>::value) {
            int integerValue;
            if (!CheckStringIfInteger(value, &integerValue))
                return false;
            if (NControlInfoTypeTraits::has_minimum_field<TDerived>(0) && minimum > integerValue)
                return false;
            if (NControlInfoTypeTraits::has_maximum_field<TDerived>(0) && maximum < integerValue)
                return false;
        }
        return true;
    }

    const static std::string
        MetaTypeString,
        DefaultNameString,
        DefaultDescriptionString;
    TControlTypeAutocreate():
        TControlType(MetaTypeString, DefaultNameString, DefaultDescriptionString)
    {}


  protected:
    ~TControlTypeAutocreate() {};
};

template <typename TDerived>
const std::string TControlType::TControlTypeAutocreate<TDerived>::MetaTypeString = TDerived::MetaTypeCString;

template <typename TDerived>
const std::string TControlType::TControlTypeAutocreate<TDerived>::DefaultNameString = TDerived::DefaultNameCString;

template <typename TDerived>
const std::string TControlType::TControlTypeAutocreate<TDerived>::DefaultDescriptionString = TDerived::DefaultDescriptionCString;


struct TControlType::TSwitch : public TControlTypeAutocreate<TSwitch> {
    static constexpr TCStringConst
        MetaTypeCString =           "switch",
        DefaultNameCString =        "switch",
        DefaultDescriptionCString = "Switch";

    typedef int TValueType;
    static constexpr int Minimum = 0;
    static constexpr int Maximum = 1;
};

struct TControlType::TAlarm : public TControlTypeAutocreate<TAlarm> {
    static constexpr TCStringConst
        MetaTypeCString =           "alarm",
        DefaultNameCString =        "alarm",
        DefaultDescriptionCString = "Alarm";

    typedef int TValueType;
    static constexpr int Minimum = 0;
    static constexpr int Maximum = 1;
};

struct TControlType::TPushButton : public TControlTypeAutocreate<TPushButton> {
    static constexpr TCStringConst
        MetaTypeCString =           "pushbutton",
        DefaultNameCString =        "pushbutton",
        DefaultDescriptionCString = "Button";

    typedef int TValueType;
    static constexpr int Minimum = 1;
    static constexpr int Maximum = 1;
};


struct TControlType::TRange : public TControlTypeAutocreate<TRange> {
    static constexpr TCStringConst
        MetaTypeCString =           "range",
        DefaultNameCString =        "range",
        DefaultDescriptionCString = "Range";

    virtual bool ValidateValue(const std::string &value);
    virtual void PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher);
    virtual void DeleteMetaInfo(std::function<void(const std::string &)> deleter) override;

    TRange(int maximum);

  protected:
    int Maximum;
};


struct TControlType::TColorRgb : public TControlTypeAutocreate<TColorRgb> {
    static constexpr TCStringConst
        MetaTypeCString =           "rgb",
        DefaultNameCString =        "rgb",
        DefaultDescriptionCString = "Range";

    virtual bool ValidateValue(const std::string &value) override;
};



struct TControlType::TGeneric : public TControlTypeAutocreate<TGeneric> {
    static constexpr TCStringConst
        MetaTypeCString =           "generic",
        DefaultNameCString =        "value",
        DefaultDescriptionCString = "Value";

    typedef float TValueType;

    virtual void PublishMetaInfo(std::function<void(const std::string &, const std::string &)> publisher) override;
    virtual void DeleteMetaInfo(std::function<void(const std::string &)> deleter) override;

    TGeneric(const std::string &units);

  protected:
    std::string Units;
};





struct TControlType::TTemperature : public TControlTypeAutocreate<TTemperature> {
    static constexpr TCStringConst
        MetaTypeCString =           "temperature",
        DefaultNameCString =        "temperature",
        DefaultDescriptionCString = "Temperature";

    typedef float TValueType;
};


struct TControlType::THumidity : public TControlTypeAutocreate<THumidity> {
    static constexpr TCStringConst
        MetaTypeCString =           "rel_humidity",
        DefaultNameCString =        "humidity",
        DefaultDescriptionCString = "Relative humidity";

    typedef float TValueType;
    static constexpr float Minimum = 0;
    static constexpr float Maximum = 100;
};

struct TControlType::TAtmosphericPressure : public TControlTypeAutocreate<TAtmosphericPressure> {
    static constexpr TCStringConst
        MetaTypeCString =           "atmospheric_pressure",
        DefaultNameCString =        "pressure",
        DefaultDescriptionCString = "Atmospheric pressure";

    typedef float TValueType;
};


struct TControlType::TPrecipitationRate : public TControlTypeAutocreate<TPrecipitationRate> {
    static constexpr TCStringConst
        MetaTypeCString =           "rainfall",
        DefaultNameCString =        "rainfall",
        DefaultDescriptionCString = "Precipitation rate";

    typedef float TValueType;
};

struct TControlType::TWindSpeed : public TControlTypeAutocreate<TWindSpeed> {
    static constexpr TCStringConst
        MetaTypeCString =           "wind_speed",
        DefaultNameCString =        "wind_speed",
        DefaultDescriptionCString = "Wind speed";

    typedef float TValueType;
};




struct TControlType::TPower : public TControlTypeAutocreate<TPower> {
    static constexpr TCStringConst
        MetaTypeCString =           "power",
        DefaultNameCString =        "power",
        DefaultDescriptionCString = "Power";

    typedef float TValueType;
};

struct TControlType::TPowerConsumption : public TControlTypeAutocreate<TPowerConsumption> {
    static constexpr TCStringConst
        MetaTypeCString =           "power_consumption",
        DefaultNameCString =        "power_consumption",
        DefaultDescriptionCString = "Power consumption";

    typedef float TValueType;
};

struct TControlType::TVoltage : public TControlTypeAutocreate<TVoltage> {
    static constexpr TCStringConst
        MetaTypeCString =           "voltage",
        DefaultNameCString =        "voltage",
        DefaultDescriptionCString = "Voltage";

    typedef float TValueType;
};

struct TControlType::TWaterFlow : public TControlTypeAutocreate<TWaterFlow> {
    static constexpr TCStringConst
        MetaTypeCString =           "water_flow",
        DefaultNameCString =        "water_flow",
        DefaultDescriptionCString = "Water flow";

    typedef float TValueType;
};

struct TControlType::TWaterTotalConsumption : public TControlTypeAutocreate<TWaterTotalConsumption> {
    static constexpr TCStringConst
        MetaTypeCString =           "water_consumption",
        DefaultNameCString =        "water_consumption",
        DefaultDescriptionCString = "Water total consumption";

    typedef float TValueType;
};

struct TControlType::TResistance : public TControlTypeAutocreate<TResistance> {
    static constexpr TCStringConst
        MetaTypeCString =           "resistance",
        DefaultNameCString =        "resistance",
        DefaultDescriptionCString = "Resistance";

    typedef float TValueType;
};

struct TControlType::TGasConcentration : public TControlTypeAutocreate<TGasConcentration> {
    static constexpr TCStringConst
        MetaTypeCString =           "concentration",
        DefaultNameCString =        "concentration",
        DefaultDescriptionCString = "Gas concentration";

    typedef float TValueType;
    static constexpr float Minimum = 0;
};

struct TControlType::THeatPower : public TControlTypeAutocreate<THeatPower> {
    static constexpr TCStringConst
        MetaTypeCString =           "heat_power",
        DefaultNameCString =        "heat_power",
        DefaultDescriptionCString = "Heat power";

    typedef float TValueType;
    static constexpr float Minimum = 0;
};

struct TControlType::THeatEnergy : public TControlTypeAutocreate<THeatEnergy> {
    static constexpr TCStringConst
        MetaTypeCString =           "heat_energy",
        DefaultNameCString =        "heat_energy",
        DefaultDescriptionCString = "Heat energy";

    typedef float TValueType;
    static constexpr float Minimum = 0;
};

#endif // CONTROL_INFO_H_INCLUDED

