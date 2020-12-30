/*
 * Copyright (C) 2020 Jordan Hendl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   RenderPass.h
 * Author: Jordan Hendl
 *
 * Created on December 28, 2020, 10:34 AM
 */

#ifndef KGL_VKG_RENDERPASS_H
#define KGL_VKG_RENDERPASS_H

/** Vulkan forward declares.
 */
namespace vk
{
  template <typename BitType, typename MaskType>
  class Flags ;

       class Framebuffer           ;
       class RenderPass            ;
  enum class ImageLayout           ;
  enum class AttachmentStoreOp     ;
  enum class AttachmentLoadOp      ;
  enum class Format                ;
  enum class ImageUsageFlagBits    ;
  enum class SampleCountFlagBits   ;
  enum class PipelineStageFlagBits ;
  enum class AccessFlagBits        ;
  enum class PipelineBindPoint     ;
}

namespace kgl
{
  namespace vkg
  {
    class Device ;
    
    /** Class to abstract a vulkan render pass.
     */
    class RenderPass
    {
      public:
        
        /** Alias for Vulkan Sample flags.
         */
        using SampleFlags = ::vk::Flags<::vk::SampleCountFlagBits, unsigned> ;
        
        /** Default constructor. 
         */
        RenderPass() ;
        
        /** Copy Constructor. Copies the input's data into this object.
         * @param render_pass The object to copy.
         */
        RenderPass( const RenderPass& render_pass ) ;
        
        /** Deconstructor. Releases allocated data.
         */
        ~RenderPass() ;
        
        /** Assignment operator. Assigns this object to the input.
         * @param render_pass The render pass to assign this object to.
         * @return Reference to this object after assignment.
         */
        RenderPass& operator=( const RenderPass& render_pass ) ;
        
        /** Method to initialize this object & Generate a vulkan render pass.
         * @param device The Library device to use for the operation.
         */
        void initalize( const kgl::vkg::Device& device ) ;
        
        /** Method to retrieve the const pointer to an array of framebuffers.
         * @see numFramebuffers for the amount of framebuffers are in this array.
         * @return The const pointer to the start of this object's framebuffer array.
         */
        const ::vk::Framebuffer* framebuffers() const ;
        
        /** Method to retrieve a const-reference to this object's internal vulkan handle.
         * @return Const-reference to this object's internal vulkan handle.
         */
        const ::vk::RenderPass& pass() const ;

        /** Method to retrieve the amount of framebuffers created by this renderpass.
         * @return The number of framebuffers created by this object.
         */
        unsigned numFramebuffers() const ;
        
        /** Method to set the number of framebuffers for this object to generate on initialization.
         * @param count The number of framebuffers to create on initialization.
         */
        void setNumFramebuffers( unsigned count ) ;
        
        /** Method to set the number of attachments to this render pass.
         * @param count The amount of attachments to generate for this renderpass.
         */
        void setNumAttachments( unsigned count ) ;
        
        /** Method to set the number of subpasses to this render pass.
         * @param count The number of subpasses to generate for this render pass.
         */
        void setNumSubpasses( unsigned count ) ;
        
        /** Method to set the number of vulkan dependancies for this subpass
         * @param count The number of vulkan dependancies to generate for this render pass.
         */
        void setNumDependancies( unsigned count ) ;
        
        /** Method to set the width to use for generating framebuffer.
         * @param width The width in pixels to use for framebuffer generation.
         */
        void setFramebufferWidth( unsigned width ) ;
        
        /** Method to set the height to use for generating framebuffer.
         * @param height The height in pixels to use for framebuffer generation.
         */
        void setFramebufferHeight( unsigned height ) ;
        
        /** Method to set the number of layers to use for generating framebuffer.
         * @param layers The number of layers to use for framebuffer generation.
         */
        void setFramebufferLayers( unsigned layers ) ;
        
        /** Method to set the number of attachment samples.
         * @param flags The number of samples to use for the selected attachment.
         * @param idx The index of attachment to apply the parameter to.
         */
        void setAttachmentNumSamples( const ::vk::SampleCountFlagBits& flags, unsigned idx = 0 ) ;
        
        /** Method to set the load operation of the attachment at the specified index.
         * @param op The operation to use for loading the attachment.
         * @param idx The number of attachment to apply the load operation to.
         */
        void setAttachmentLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx = 0  ) ;
        
        /** Method to set the store operation of the attachment at the specified index.
         * @param op The operation to use for store the attachment.
         * @param idx The number of attachment to apply the store operation to.
         */
        void setAttachmentStoreOp( const ::vk::AttachmentStoreOp& op, unsigned idx = 0 ) ;
        
        /** Method to set the stencil load operation of the attachment at the specified index.
         * @param op The operation to use for loading the stencil attachment.
         * @param idx The number of attachment to apply the stencil load operation to.
         */
        void setAttachmentStencilLoadOp( const ::vk::AttachmentLoadOp& op, unsigned idx = 0 ) ;
        
        /** Method to set the stencil store operation of the attachment at the specified index.
         * @param op The operation to use for storing the stencil attachment.
         * @param idx The number of attachment to apply the stencil store operation to.
         */
        void setAttachmentStencilStoreOp( const ::vk::AttachmentStoreOp& op, unsigned idx = 0 ) ;
        
        /** Method to set the initial layout of the attachment at the specified index.
         * @param layout The initial layout to use for the attachment.
         * @param idx The number of attachment to apply layout to.
         */
        void setAttachmentInitialLayout( const ::vk::ImageLayout& layout, unsigned idx = 0 ) ;
        
        /** Method to set the final layout of the attachment at the specified index.
         * @param layout The final layout to use for the attachment.
         * @param idx The number of attachment to apply layout to.
         */
        void setAttachmentFinalLayout( const ::vk::ImageLayout& layout, unsigned idx = 0 ) ;
        
        /** Method to set the attachment format for the attachment at the specified index.
         * @param format The format to use for the specified attachment.
         * @param idx The index of attachment to apply the format to.
         */
        void setAttachmentFormat( const ::vk::Format& format, unsigned idx = 0 ) ;
        
        /** Method to set the layout of the attachment reference at the specified index.
         * @param layout The layout to apply to the reference.
         * @param idx The index of reference to apply the layout to.
         */
        void setReferenceLayout( const ::vk::ImageLayout& layout, unsigned idx = 0 ) ;
        
        /** Method to set the pipeline bind point of the specified subpass.
         * @param point The pipeline bind point to apply to the subpass.
         * @param idx The index of subpass to apply the bind point to.
         */
        void setSubpassPipelineBindPoint( const ::vk::PipelineBindPoint& point, unsigned idx = 0 ) ;
        
        /** Method to set the source for the specified dependency.
         * @param src The source index to use for the dependency.
         * @param idx The index of dependency to apply this to.
         */
        void setDependancySrc( unsigned src, unsigned idx = 0 ) ;
        
        /** Method to set the source for the specified dependency.
         * @param src The source index to use for the dependency.
         * @param idx The index of dependency to apply this to.
         */
        void setDependancyDst( unsigned dst, unsigned idx = 0 ) ;
        
        /** Method to set the source stage for the specified dependency.
         * @param src The source pipeline stage flag to use for the dependency.
         * @param idx The index of dependency to apply the flag to.
         */
        void setDependancySrcStage( const ::vk::PipelineStageFlagBits& src, unsigned idx = 0 ) ;
        
        /** Method to set the destination stage for the specified dependency.
         * @param dst The destination pipeline stage flag to use for the dependency.
         * @param idx The index of dependency to apply the flag to.
         */
        void setDependancyDstStage( const ::vk::PipelineStageFlagBits& dst, unsigned idx = 0 ) ;
        
        /** Method to set the source access flag for the specified dependency.
         * @param src The source access flag to use for the dependency.
         * @param idx The index of dependency to apply the flag to.
         */
        void setDependancySrcAccess( const ::vk::AccessFlagBits& src, unsigned idx = 0 ) ;
        
        /** Method to set the destination access flag for the specified dependency.
         * @param src The destination access flag to use for the dependency.
         * @param idx The index of dependency to apply the flag to.
         */
        void setDependancyDstAccess( const ::vk::AccessFlagBits& dst, unsigned idx = 0 ) ;

      private:
        
        /** Forward declared structure to contain this object's internal data.
         */
        struct RenderPassData *pass_data ;
        
        
        /** Method to retrieve a reference to this object's internal data.
         * @return Reference to this object's internal data.
         */
        RenderPassData& data() ;

        
        /** Method to retrieve a const-reference to this object's internal data.
         * @return Const-reference to this object's internal data.
         */
        const RenderPassData& data() const ;
    };
  }
}
#endif
