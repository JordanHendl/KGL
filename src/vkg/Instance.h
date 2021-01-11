#ifndef KGL_VKG_INSTANCE_H
#define KGL_VKG_INSTANCE_H

/** Forward declared vulkan objects.
 */
namespace vk
{
  class Instance       ;
  class PhysicalDevice ;
}

namespace kgl
{
  namespace vkg
  {
    /** Class to handle creation & management of a Vulkan instance.
     */
    class Instance
    {
      public:
        
        /** Enum of the type of output levels.
         */
        enum class DebugOutputLevel
        {
          Quiet,
          Normal,
          Verbose,
        };
        
        /** The severity of vulkan instance debug events to output.
         */
        enum class DebugSeverity
        {
          None,
          All,
          WError,
          ErrorsOnly,
        };

        /** Default constructor.
         */
        Instance() ;
        
        /** Copy constructor. Copies the input instance data into this instance.
         */
        Instance( const Instance& instance ) ;
        
        /** Default Deconstructor.
         */
        ~Instance() ;
        
        /** Equals operator to set this object's instance equal to the input parameter's.
         * @return A reference to this object after the copy.
         */
        Instance& operator=( const Instance& instance ) ;

        /** Conversion operator for a vulkan instance, so this object can be used in the place of that as well.
         */
        operator const ::vk::Instance&() const ;
        
        /** Conversion operator for a vulkan instance, so this object can be used in the place of that as well.
         */
        operator ::vk::Instance() const ;
        
        /** Conversion operator for a vulkan instance, so this object can be used in the place of that as well.
         */
        operator const ::vk::Instance&() ;
        
        /** Conversion operator for a vulkan instance, so this object can be used in the place of that as well.
         */
        operator ::vk::Instance() ;

        /** Method to initialize this Vulkan instance using the input application name.
         * @param application_name The name of this application.
         */
        void initialize() ;
        
        /** Method to set the application name.
         * @param application_name The name of this application.
         */
        void setApplicationName( const char* app_name ) ;

        /** Method to set the level of debug output for this instance.
         * @param level The level of debug output to use for outputting info.
         */
        void setDebugOutputLevel( Instance::DebugOutputLevel level ) ;
        
        /** Method to set the severity of errors allowed to be outputted by this instance.
         * @param severity The severity of errors allowed by this instance.
         */
        void setDebugOutputType( Instance::DebugSeverity severity ) ;

        /** Method to set the application version.
         * @param major The major version number.
         * @param minor The minor version number.
         * @param revision The revision.
         */
        void setApplicationVersion( unsigned major, unsigned minor, unsigned revision ) ;
        
        /** Method to set the Vulkan API version to use.
         * @param major The major version number.
         * @param minor The minor version number.
         * @param revision The revision.
         */
        void setApiVersion( unsigned major, unsigned minor, unsigned revision ) ;
        
        /** Method to clear this instance and free all data.
         */
        void reset() ;
        
        /** Method to retrieve whether or not this object is initialized.
         * @return Whether or not this object is initialized.
         */
        bool isInitialized() const ;

        /** Method to retrieve the underlying Vulkan Instance of this object.
         * @return 
         */
        const ::vk::Instance& instance() const ;
        
        /** Method to add a extention name to be loaded by this instance.
         * @param extension_name The name of the extension to load when this object is initialized.
         */
        void addExtension( const char* extension_name ) ;
        
        /** Method to add a validation layer to this instance upon it's creation.
         * @param layer_name The name of the validation layer to use. If it is invalid, it is skipped.
         */
        void addValidationLayer( const char* layer_name ) ;
        
        /** Method to retrieve the number of physical devices found by the vulkan instance.
         * @return The number of physical GPU devices detected by the Vulkan API.
         */
        unsigned numDevices() const ;

        /** Method to retrieve the Vulkan Physical device of the specified ID.
         * @param id The ID of physical device to return. If not valid, returns ID 0.
         * @return The physical device matching the input ID.
         */
        const ::vk::PhysicalDevice& device( unsigned id = 0 ) ;

        /** Enables debug callbacks for Vulkan Errors.
         */
        void setDebug( bool flag ) ;
      private:
        
        /** Forward declared object containing this object's internal data.
         */
        struct InstanceData* instance_data ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return A reference to this object's internal data.
         */
        InstanceData& data() ;
        
        /** Method to retrieve a reference to this object's internal data.
         * @return A reference to this object's internal data.
         */
        const InstanceData& data() const ;
    };
  }
}

#endif /* INSTANCE_H */

