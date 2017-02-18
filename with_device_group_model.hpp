/*!
 *  This file represents relatively complex model of WB devices hierarchy
 *  I. e. Driver -> DeviceGroup -> Device -> Control
 *  Also it is an example of creating hierarchies
 * 
 */

// include headers with declarations

// Firstly declare all classes of hierarchy
class TDriverDgm;
class TDeviceGroupDgm;
class TDeviceDgm;
class TControlDgm;

/*!
 *  Class of driver in with-device-group-model
 *  How to read name: T - type, Driver - driver, Dgm - DGM - device group model
 */
class TDriverDgm : public NWbMqtt::TDriverBase<TDriverDgm, TDeviceGroupDgm> {
  protected:
    virtual void TryDelegateMessageToChildren(const TMessage &message) {
        // replaces "/${child->Name}_" (bash style) with "/"
        // not cut just "/${child->Name}" as by default
    }
  public:
  
    /*! 
     * Constructor.
     */
    TDriverDgm(const std::string &driverName);
    /*!
     *  Destructor
     */
    virtual ~TDriverDgm();
};

/*!
 *  Class of group of devices in with-device-group-model
 *  How to read name: T - type, Driver - driver, Dgm - DGM - device group model
 */
class TDeviceGroupDgm : public NWbMqtt::THierarchyMemberBase<TDriverDgm, TDeviceGroupDgm, TDeviceDgm> {
  public:
    virtual void SendParsed(const std::string &parsedRadioMessage);
    /*! 
     * Constructor.
     */
    TDeviceGroupDgm(TDriverDgm *driver, const std::string &groupName);
    virtual ~TDeviceGroupDgm();
};


/*!
 *  Class of devices in with-device-group-model
 *  How to read name: T - type, Driver - driver, Dgm - DGM - device group model
 */
class TDeviceDgm : public NWbMqtt::TDeviceBase<TDeviceGroupDgm, TDeviceDgm, TControlDgm> {
  protected:
    virtual TControlDgm *CreateChildByName(const String &nameOfChild) override;
  public:
    /*! 
     * Constructor.
     */
    TDeviceDgm(TDeviceGroupDgm *group, const std::string &deviceName);
    virtual ~TDeviceDgm();
};

/*!
 *  Class of controls in with-device-group-model
 *  How to read name: T - type, Driver - driver, Dgm - DGM - device group model
 */
class TControlDgm : public NWbMqtt::TControlBase<TDeviceDgm, TControlDgm> {
  public:    
    /*! 
     * Constructor.
     */
    template <typename ...Args>
    TControlDgm(TDeviceDgm *device, Args... args): TControlBase<TDeviceDgm>(device, args...) {}
    virtual ~TControlDgm();
};

// usage begin


class TOregonDeviceGroupDgm : public TDeviceGroupDgm {
    // ...
  public:
    virtual void SendParsed(const std::string &parsedRadioMessage) override {
        // tokenize parsed radio message and do updates to mqtt
    }
    TOregonDeviceGroupDgm(TDriverDgm *driver): TDeviceGroupDgm(driver, "oregon") {
       // add controls 
    }
    // ...
};

class TNooliteDeviceGroupDgm : public TDeviceGroupDgm {
    // ...
  public:
    
    virtual void SendParsed(const std::string &parsedRadioMessage) override {
        // tokenize parsed radio message and do updates to mqtt
    }
    TNooliteDeviceGroupDgm(TDriverDgm *driver): TDeviceGroupDgm(driver, "noolite") {
       // add controls 
    }
    // ...
};

 
class TOregonTemperatureSensorDeviceDgm : public TDeviceDgm {
    ...
};

class TNooliteRemoteDeviceDgm : public TDeviceDgm {
    ...
};

void usage_1() {
    TDriverDgm *driver = new TDriverDgm("rfsniffer");
    driver->EmplaceChild<TNooliteDeviceGroupDgm>();
    (*driver)["noolite"].EmplaceChild<TDeviceDgm>("rx_1234");
    (*driver)["noolite"].EmplaceChild<TDeviceDgm>("rx_ab7f");
    (*driver)["noolite"].EmplaceChild<TNooliteRemoteDeviceDgm>("remote");
    
    TOregonDeviceGroupDgm oregonRef = *driver->EmplaceChild<TOregonDeviceGroupDgm>();
    
    oregonRef.EmplaceChild<TOregonTemperatureSensorDeviceDgm>("123");
    oregonRef.EmplaceChild<TDeviceDgm>("123");
    oregonRef["123"]["temperature"] = "17.0";
    oregonRef["123"]["temperature"].As<float>() = 17.0;
}


// usage end


