#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import "hap.h"




/**
This object represents a frame, and holds all the values necessary to decode a hap frame from AVFoundation as a CMSampleBufferRef to DXT data.  Instances of this class is not intended to be reusable: this is just a simple holder, the backend wants to release it as soon as possible.		*/
@interface HapDecoderFrame : NSObject	{
	CMSampleBufferRef		hapSampleBuffer;	//	RETAINED
	OSType					codecSubType;	//	kHapCodecSubType, etc.
	NSSize					imgSize;	//	the size of the hap frame
	
	void					*dxtData;	//	NOT RETAINED.  when you decode the contents of 'hapSampleBuffer', the resulting DXT data is written here.
	size_t					dxtMinDataSize;
	size_t					dxtDataSize;	//	the size in bytes of the memory available at dxtData
	OSType					dxtPixelFormat;	//	kHapCVPixelFormat_RGB_DXT1, etc.  populated from the sample buffer.
	NSSize					dxtImgSize;	//	the size of the dxt image (the image size is this or smaller, dxt's have to be a multiple of 4)
	enum HapTextureFormat	dxtTextureFormat;	//	this value is 0 until the frame has been decoded
	
	id						userInfo;	//	RETAINED, arbitrary ptr used to keep a piece of user-specified data with the frame
	
	BOOL					decoded;	//	when decoding is complete, this is set to YES.
}

/**
Calls "initEmptyWithHapSampleBuffer:", then allocates a CFDataRef and sets that as the empty frame's "dxtData".  If you don't specify a HapDecoderFrameAllocBlock in your AVPlayerItemHapDXTOutput, this is how the frames are created.
@param sb A CMSampleBufferRef containing video data compressed using the hap codec.
*/
- (id) initWithHapSampleBuffer:(CMSampleBufferRef)sb;
/**
Returns an "empty" decoder frame- all the fields except "dxtData" and "dxtDataSize" are populated.  You MUST populate the dxtData and dxtDataSize fields before you can return (or decode) the frame!  "dxtMinDataSize" and the other fields are valid as soon as this returns, so you can query the properties of the frame and allocate memory of the appropriate length.
@param sb A CMSampleBufferRef containing video data compressed using the hap codec.
*/
- (id) initEmptyWithHapSampleBuffer:(CMSampleBufferRef)sb;

/// The CMSampleBufferRef containing video data compressed using the Hap codec, returned from an AVSampleBufferGenerator
@property (readonly) CMSampleBufferRef hapSampleBuffer;
/// The codec subtype of the video data in this frame.  Either kHapCodecSubType, kHapAlphaCodecSubType, or kHapYCoCgCodecSubType (defined in "HapCodecSubTypes.h")
@property (readonly) OSType codecSubType;
/// The size of the image being returned.  Note that the dimensions of the DXT buffer may be higher (multiple-of-4)- these are the dimensions of the final decoded image.
@property (readonly) NSSize imgSize;
/// If you're manually allocating HapDecoderFrame instances with a HapDecoderFrameAllocBlock, you must use this property to provide a pointer to the buffer of memory into which this framework can decode the hap frame into DXT data.  If you're just retrieving HapDecoderFrame instances from an AVPlayerItemHapDXTOutput, this is a weak ptr to the memory containing the DXT data, ready for upload to a GL texture.
@property (assign,readwrite,setter=setDXTData:) void* dxtData;
/// The minimum amount of memory required to contain a DXT-compressed image with dimensions of "imgSize".  If you're using a HapDecoderFrameAllocBlock, the blocks of memory assigned to "dxtData" must be at least this large.
@property (readonly) size_t dxtMinDataSize;
/// If you're using a HapDecoderFrameAllocBlock, in addition to providing memory for the dxtData block, you must also tell the frame how much memory you've allocated.
@property (assign,readwrite,setter=setDXTDataSize:) size_t dxtDataSize;
/// The pixel format of the DXT frame, and is either 'kHapCVPixelFormat_RGB_DXT1' (if the video frame used the hap codec), 'kHapCVPixelFormat_RGBA_DXT5' (if it used the "hap alpha" codec), or 'kHapCVPixelFormat_YCoCg_DXT5' (if it used the "hap Q" codec).  These values are defined in PixelFormats.h
@property (readonly) OSType dxtPixelFormat;
/// The size of the DXT frame, in pixels.  This may be larger tha the "imgSize".
@property (readonly) NSSize dxtImgSize;
/// The format of the GL texture, suitable for passing on to GL commands (
@property (readonly) enum HapTextureFormat dxtTextureFormat;
/// A nondescript, retained, (id) that you can use to retain an arbitrary object with this frame (it will be freed when the frame is deallocated).  If you're using a HapDecoderFrameAllocBlock to allocate memory for frames created by an AVPlayerItemHapDXTOutput and you want to retain a resource with the decoded frame, this is a good way to do it.
@property (retain,readwrite) id userInfo;
//	Returns YES when the frame has been decoded
@property (assign,readwrite) BOOL decoded;

//	The AVPlayerItemHapDXTOutput calls this method, you don't have to and probably shouldn't.  It just decodes the hap frame in the sample buffer into the dxtData memory buffer.
- (void) _decode;

@end