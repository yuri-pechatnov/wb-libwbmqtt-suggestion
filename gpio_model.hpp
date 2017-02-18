/*!
 *  This file represents a possible gpio driver WB-devices hierarchy
 *  I. e. Driver -> Device -> Control
 */

//include headers with declarations

// Firstly declare all classes of hierarchy
class TDriverGpio;
class TDeviceGpio;
class TControlGpio;

/*!
 *  Class of driver in simple model
 *  How to read name: T - type, Driver - driver, Gpio - GPIO 
 */
class TDriverGpio : public NWbMqtt::TDriverBase<TDriverGpio, TDeviceGpio> {
  public:
    int Main() {
        // read configs
        
        TDeviceGpio &gpios = *EmplaceChild<TDeviceGpio>("gpios");
        
        for (auto controlInfo : controlsInfo)
            gpios.EmplaceChild<TControlGpio>(controlInfo.path, ...);
        
        while (true) {
            WaitAndProcessMessages(1000);
            for (auto gpioControl : gpios.Children)
                gpioControl->Loop();
        }
    }
    /*! 
     * Constructor.
     */
    TDriverGpio() : TDriverBase<TDriverGpio, TDeviceGpio>("wb-gpio") {}
    /*!
     *  Destructor
     */
    virtual ~TDriverGpio();
};

/*!
 *  Class of devices in simple model
 *  How to read name: T - type, Device - device
 */
class TDeviceGpio : public NWbMqtt::TDeviceBase<TDriverGpio, TDeviceGpio, TControlGpio> {
  public:
    /*! 
     * Constructor.
     */
    TDeviceGpio(TDriver *parent, const std::string &deviceName) : TDeviceBase<TDriverGpio, TDeviceGpio, TControlGpio>(parent, deviceName) {}
    virtual ~TDeviceGpio();
};


/*!
 *  Class of controls in simple model
 *  How to read name: T - type, Control - control
 */
class TControlGpio : public NWbMqtt::TControlBase<TDeviceGpio, TControlGpio> {
    int fd;
  public: 
    virtual void OnValueChange(const TTopic &topic, const String &newValue) override {
        Value = newValue;
        int val = GetValue<int>();
        if (write(fd, sizeof(val), &val) ...) // not successful
            SetError("Problem on ...");
        else
            CleanError();
    }
  
    void Loop() {
        int val;
        if (read(fd, sizeof(val), &val) ...) // not successful
            SetError("Problem on ...");
        else {
            this->As<int>() = val;
            CleanError();
        }
    }
    
    /*! 
     * Constructor.
     */
    template <typename ...Args>
    TControlGpio(TDevice *parent, std::string pathToSysfs, Args... args): TControlBase<TDeviceGpio>(parent, args...) {
        fd = open(pathToSysfs.c_str());
    }
    virtual ~TControlGpio() {
        if (fd != -1)
            close(fd);
    }
};

 

void main() {
    TDriverGpio driver("wb-gpio");
    return driver.Main();
}


// usage end
