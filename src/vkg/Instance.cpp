#include "Instance.h"
#include "Vulkan.h"
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <iostream>

#define KGL_MAJOR_VERSION 0 
#define KGL_MINOR_VERSION 2 
#define KGL_REVISION      0

namespace kgl
{
  namespace vkg
  {
    /** Static variables for debug output.
     */
    static Instance::DebugOutputLevel output_level    = Instance::DebugOutputLevel::Normal ;
    static Instance::DebugSeverity    output_severity = Instance::DebugSeverity::WError    ;

    /** Method to convert a instance debug flag to vulkan flag.
     * @param level The flag to convert.
     * @return The Vulkan severity flag.
     */
    static vk::DebugUtilsMessageSeverityFlagsEXT toFlags( Instance::DebugSeverity level ) ;
    
    /** Method to convert a instance debug flag to vulkan flag.
     * @param level The flag to convert.
     * @return The Vulkan type flag.
     */
    static vk::DebugUtilsMessageTypeFlagsEXT toFlags( Instance::DebugOutputLevel type ) ;
    
    static  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData )
    {
      const vk::DebugUtilsMessageSeverityFlagsEXT severity = static_cast<vk::DebugUtilsMessageSeverityFlagsEXT>( messageSeverity ) ;
      const vk::DebugUtilsMessageTypeFlagsEXT     type     = static_cast<vk::DebugUtilsMessageTypeFlagsEXT>    ( messageType     ) ;
      
      if( output_severity != Instance::DebugSeverity::None && output_level != Instance::DebugOutputLevel::Quiet )
      {
        if( toFlags( output_level ) & type && toFlags( output_severity ) & severity ) 
        {
          std::cout << "\n" ;
          std::cout << "--  KGL::VKG Instance Debug"                  << "\n" ;
          std::cout << "--  Type    : "  << vk::to_string( type     ) << "\n" ;
          std::cout << "--  Severity: "  << vk::to_string( severity ) << "\n" ;
          std::cout << "---> Message: " << pCallbackData->pMessage    << "\n" ;
        }
      }
      pUserData = pUserData ;
      return VK_FALSE;
    }
    
    struct Version
    {
      unsigned major    ;
      unsigned minor    ;
      unsigned revision ;
    };

    struct InstanceData
    {
      typedef std::vector<std::string>          List       ;
      typedef std::vector<const char*>          CharList   ;
      typedef std::vector<::vk::PhysicalDevice> DeviceList ;
      
      ::vk::Instance instance     ;
      Version        app_version  ;
      Version        api_version  ;
      std::string    app_name     ;
      List           ext_list     ;
      List           layer_list   ;
      DeviceList     physical_dev ;
      bool           debug        ;

      /** Default constructor.
       */
      InstanceData() ;
              
      /** Method to create the vulkan application info.
       * @return A Filled out Vulkan Application Info.
       */
      ::vk::ApplicationInfo makeAppInfo() const ;
      
      /** Method to create Vulkan debug info object.
       * @return A Filled out Vulkan Debug info object.
       */
      ::vk::DebugUtilsMessengerCreateInfoEXT makeDebugInfo() const ;
      
      /** Method to retrieve a filtered list of available extensions.
       * @return A Valid list of extensions.
       */
      List filterExtensions() const ;
      
      /** Method to retrieve a filtered list of available validation layers.
       * @return A valid list of validiation layers.
       */
      List filterValidationLayers() const ;
      
      /** Method to convert a list of strings to list of char* for vulkan objects.
       * @param list The list to convert.
       * @return A List of const char* pointing to string list's strings.
       */
      CharList listToCharList( List& list ) ;
    };
    
    
    InstanceData::InstanceData()
    {
      app_version = { 0, 0, 1 }        ;
      api_version = { 1, 2, 0 }        ;
      app_name    = "KGL_DEFAULT_NAME" ;
      debug       = true               ;
    }
    
    vk::DebugUtilsMessageSeverityFlagsEXT toFlags( Instance::DebugSeverity level )
    {
      using flag = vk::DebugUtilsMessageSeverityFlagBitsEXT ;
      switch( level )
      {
        case Instance::DebugSeverity::ErrorsOnly : return flag::eError                  ;
        case Instance::DebugSeverity::WError     : return flag::eWarning | flag::eError ;
        default : return flag::eInfo ;
      }
    }
    
    vk::DebugUtilsMessageTypeFlagsEXT toFlags( Instance::DebugOutputLevel type )
    {
      using flag = vk::DebugUtilsMessageTypeFlagBitsEXT ;
      switch( type )
      {
        case Instance::DebugOutputLevel::Normal  : return flag::eGeneral                                      ;
        case Instance::DebugOutputLevel::Verbose : return flag::eValidation | flag::eGeneral | flag::eGeneral ;
        default : return flag::eGeneral ;
      }
    }
    
    ::vk::ApplicationInfo InstanceData::makeAppInfo() const
    {
      ::vk::ApplicationInfo info ;
      
      constexpr auto engine_version = VK_MAKE_VERSION( KGL_MAJOR_VERSION, KGL_MINOR_VERSION, KGL_REVISION                           ) ;
      auto           app_version    = VK_MAKE_VERSION( this->app_version.major, this->app_version.minor, this->app_version.revision ) ;
      auto           api_version    = VK_MAKE_VERSION( this->api_version.major, this->api_version.minor, this->api_version.revision ) ;

      info.setPApplicationName  ( this->app_name.c_str() ) ;
      info.setPEngineName       ( "KGL"                  ) ;
      info.setApplicationVersion( app_version            ) ;
      info.setApiVersion        ( api_version            ) ;
      info.setEngineVersion     ( engine_version         ) ;
      
      return info ;
    }
    
    ::vk::DebugUtilsMessengerCreateInfoEXT InstanceData::makeDebugInfo() const
    {
      const auto severity_bits = ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError  | ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning ;
      const auto message_type  = ::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     | ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance ;
      ::vk::DebugUtilsMessengerCreateInfoEXT info ;

      if( this->debug )
      {
        info.setMessageSeverity( severity_bits ) ;
        info.setMessageType    ( message_type  ) ;
        info.setPfnUserCallback( debugCallback ) ;
      }
      
      return info ;
    }
    
    InstanceData::List InstanceData::filterExtensions() const
    {
      List                                   list                 ;
      std::vector<::vk::ExtensionProperties> available_extentions ;
      
      available_extentions = ::vk::enumerateInstanceExtensionProperties() ;
      
      for( const auto& ext : available_extentions )
      {
        for( const auto& requested : this->ext_list )
        {
          if( std::string( ext.extensionName.data() ) == std::string( requested ) )
          {
            list.push_back( std::string( ext.extensionName.data() ) ) ;
          }
        }
      }
      
      return list ;
    }
    
    InstanceData::List InstanceData::filterValidationLayers() const
    {
      List                               list             ;
      std::vector<::vk::LayerProperties> available_layers ;
      
      if( this->debug )
      {
        available_layers = ::vk::enumerateInstanceLayerProperties() ;
        
        for( const auto& prop : available_layers )
        {
          for( const auto& requested : this->layer_list )
          {
            if( std::string( prop.layerName.data() ) == std::string( requested ) )
            {
              list.push_back( std::string( prop.layerName.data() ) ) ;
            }
          }
        }
      }
      
      return list ;
    }
    
    InstanceData::CharList InstanceData::listToCharList( List& list )
    {
      InstanceData::CharList char_list ;
      
      for( auto &str : list )
      {
        char_list.push_back( str.c_str() ) ;
      }
      
      return char_list ;
    }

    Instance::Instance()
    {
      this->instance_data = new InstanceData() ;
    }

    Instance::Instance( const Instance& instance )
    {
      this->instance_data = new InstanceData() ;
      
      *this = instance ;
    }

    Instance::~Instance()
    {
      delete this->instance_data ;
    }
    
    unsigned Instance::numDevices() const
    {
      return data().physical_dev.size() ;
    }

    Instance& Instance::operator=( const Instance& instance )
    {
      *this->instance_data = *instance.instance_data ;
      
      return *this ;
    }
    
    Instance::operator const ::vk::Instance&() const
    {
      return data().instance ;
    }    
    
    Instance::operator ::vk::Instance() const
    {
      return data().instance ;
    }
    
    Instance::operator const ::vk::Instance&()
    {
      return data().instance ;
    }
    
    Instance::operator ::vk::Instance()
    {
      return data().instance ;
    }
    
    void Instance::setDebugOutputLevel( Instance::DebugOutputLevel level )
    {
      kgl::vkg::output_level = level ;
    }
    
    void Instance::setDebugOutputType( Instance::DebugSeverity severity )
    {
      kgl::vkg::output_severity = severity ;
    }
        
    void Instance::setApplicationName( const char* app_name )
    {
      data().app_name = app_name ;
    }
    
    const ::vk::Instance& Instance::instance() const
    {
      return data().instance ;
    }

    void Instance::initialize()
    {
      ::vk::InstanceCreateInfo               info            ;
      ::vk::ApplicationInfo                  app_info        ;
      ::vk::DebugUtilsMessengerCreateInfoEXT debug_info      ;
      InstanceData::List                     ext_list        ;
      InstanceData::List                     layer_list      ;
      InstanceData::CharList                 ext_list_char   ;
      InstanceData::CharList                 layer_list_char ;
      
      app_info   = data().makeAppInfo()            ;
      debug_info = data().makeDebugInfo()          ;
      ext_list   = data().filterExtensions()       ;
      layer_list = data().filterValidationLayers() ;
      
      
      if( data().debug )
      {
        ext_list.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) ;
        info.setPNext( reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>( &debug_info ) ) ;
      }
      
      ext_list_char   = data().listToCharList( ext_list )   ;
      layer_list_char = data().listToCharList( layer_list ) ;

      info.setEnabledLayerCount      ( layer_list.size()      ) ;
      info.setPpEnabledLayerNames    ( layer_list_char.data() ) ;
      info.setEnabledExtensionCount  ( ext_list.size()        ) ;
      info.setPpEnabledExtensionNames( ext_list_char.data()   ) ;
      info.setPApplicationInfo       ( &app_info              ) ;
      
      data().instance     = ::vk::createInstance( info )               ;
      data().physical_dev = data().instance.enumeratePhysicalDevices() ;
      
      Vulkan::initialize( data().instance ) ;
    }

    void Instance::setApplicationVersion( unsigned major, unsigned minor, unsigned revision )
    {
      data().app_version = { major, minor, revision } ;
    }

    void Instance::setApiVersion( unsigned major, unsigned minor, unsigned revision )
    {
      data().api_version = { major, minor, revision } ;
    }

    void Instance::reset()
    {
      data().instance.destroy() ;
    }
    
    bool Instance::isInitialized() const
    {
      return data().instance ? true : false ;
    }

    void Instance::addExtension( const char* extension_name )
    {
      data().ext_list.push_back( extension_name ) ;
    }
    
    void Instance::addValidationLayer( const char* layer_name )
    {
      data().layer_list.push_back( layer_name ) ;
    }
    
    const ::vk::PhysicalDevice& Instance::device( unsigned id )
    {
      return id < data().physical_dev.size() ? data().physical_dev[ id ] : data().physical_dev[ 0 ] ;
    }
    
    InstanceData& Instance::data()
    {
      return *this->instance_data ;
    }

    const InstanceData& Instance::data() const
    {
      return *this->instance_data ;
    }
  }
}
