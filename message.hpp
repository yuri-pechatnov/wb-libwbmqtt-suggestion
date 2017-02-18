/*!
 *  Simplified message
 */


namespace NWbMqtt {
    

/*!
 *  It's just a note about class.
 *  Wrapper over std::string.
 *  It will include parsing methods, formating, transformation to and from popular types
 *  (it allows to encapsulate a lot of functions).
 *  Also it unifies code style (classes naming).
 * 
 *  Free converting to and from std::string will be included.
 * 
 *  There is a prototype in rfsniffer /libs/libutils/strutils.h
 * 
 */
class TString : public std::string {};
 
 
/*!
 *  Topic looks like "/devices/noolite/state"
 * 
 *  TTopic allows to do string operations safely, without
 *  occasions like "//devices/noolite//state"
 * 
 *  Also will be enabled simple parsing of topics
 */
class TTopic : public TString {
    ...
};


/*!
 * Types of messages
 * divides in DOWN_* and UP_* depending on direction of message
 */
enum TMessageType {
    /// (DOWN) This is for messages coming from mqtt  
    DOWN_VALUE_CHANGE,        
    /// (UP) This is a message to be published in mqtt    
    UP_PUBLISH,           
    /// (UP) This is a message to remove topic from mqtt 
    UP_REMOVE,            
    /// (UP) This is a message to suscribe to topic
    UP_SUBSCRIBE,         
    /// (UP) This is a message to unsubscrive from topic
    UP_UNSUBSCRIBE    
};

/*!
 *  Class for messages
 *  It's simple enough, I suppose
 */
class TMessage {
  protected:
    /// Topic of message. Logically you should not modify it after creating
    TString Topic;
    
    /// Type of message. Logically you should not modify it after creating
    TMessageType Type;
  public:
    /// Information load of message.
    TString Value;
  
    /// Getter for message type
    TMessageType GetType();
    
    /*!
     * Method mostly for debug
     * prints message
     */
    TString ToString();
    
    /// constructor from structure defined in mosquitto library
    TMessage(const mosquitto_message *message);
    
    /*!
     *  Direction "Down".
     *  Creates message that will be sent (Driver->Device->Control)
     */
    static TMessage CreateIncomeMessage(const TString &topic, const TString &value);
    /*!
     *  Direction "Up".
     *  Create messages that will be sent (Control->Device->Driver)
     * 
     *  Message to publish something
     */
    static TMessage CreatePublishMessage(const TString &topic, const TString &value);
    /*!
     *  Direction "Up".
     *  Create messages that will be sent (Control->Device->Driver)
     * 
     *  Message to remove topic
     */
    static TMessage CreateRemoveMessage(const TString &topic);
    /*!
     *  Direction "Up".
     *  Create messages that will be sent (Control->Device->Driver)
     * 
     *  Message to subscrive to topic
     */
    static TMessage CreateSubscribeMessage(const TString &topic);
    /*!
     *  Direction "Up".
     *  Create messages that will be sent (Control->Device->Driver)
     * 
     *  Message to unsubscribe from topic
     */
    static TMessage CreateUnsubscribeMessage(const TString &topic);
    
  private:
    /// Constructor used by static CreateSomethingMessage()
    TMessage(TMessageType type, const TString &topic, const TString &value);
};




/*!
 *  Thread-safe container for messages
 * 
 */
class TAsyncMessagesContainer {};

}
