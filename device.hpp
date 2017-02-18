/*!
 * TDevice is declared here
 * Synonyms in documentation: 
 *   1) Device, сущность устройство, устройство
 *   2) parent, device holder (usually equal to Driver)
 * 
 */
#pragma once

#include "driver.h"
#include "control.h"

 
namespace NWbMqtt {

template <typename TDeviceHolder_, typename TDevice_, typename TChild_>
class TDeviceBase : public THierarchyMemberBase<TDeviceHolder_, TDevice_, TChild_> {
  public:
    /*!
     * Supposed to be Driver type or DeviceGroup type
     */
    typedef TDeviceHolder_ TDeviceHolder;
    /*!
     * Supposed to be Control type or ControlGroup type
     */
    typedef TChild_ TChild;
    typedef TDevice_ TDevice;
    typedef TDeviceHolder_ TDeviceHolder;
  protected:

    /// human-readable description
    std::string DeviceDescription;

  public:
    // standart realisation is used
    //virtual void MessageToParent(const TMessage &message) override;
    
    // standart realisation is used    
    //virtual void ProcessMessageFromParent(const TMessage &message) override;
    
    /*!
     * Device constructor
     * \param [in] parent - pointer to parent in hierarchy
     * \param [in] deviceName - name of device
     * \param [in] deviceDescription - description of device
     */
    TDeviceBase(TParent *parent, const std::string &deviceName, const std::string &deviceDescription = "");
    virtual ~TDeviceBase();
}


}
