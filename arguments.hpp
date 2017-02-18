/*!
 * Some classes of arguments and small auxiliary classes declared here
 */


namespace NWbMqtt {

/*!
 * \brief Use this type for THierarchyMemberBase template arguments
 * 
 * This type is as a flag, showing that there will be a value field with accessors.
 */
class TArgHavingValue {};



/*!
 * \brief Use this type for THierarchyMemberBase template arguments as type of child or parent.
 * 
 * This type is as a flag, showing that there will not be a child or parent.
 */
class TArgNoChildOrParent {};


}
