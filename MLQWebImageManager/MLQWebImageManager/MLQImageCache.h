//
//  MLQImageCache.h
//  MLQWebImageManager
//
//  Created by qianlei on 2021/10/14.
//

#import <Foundation/Foundation.h>
#import <SDWebImage/SDWebImageCompat.h>
#import <SDWebImage/SDWebImageDefine.h>
#import <SDWebImage/SDImageCacheConfig.h>
#import <SDWebImage/SDImageCacheDefine.h>
#import <SDWebImage/SDMemoryCache.h>
#import <SDWebImage/SDDiskCache.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^MLQImageCacheQueryCompletionBlock)(NSData * _Nullable data, SDImageCacheType cacheType);

/**
 * MLQImageCache maintains a memory cache and a disk cache. Disk cache write operations are performed
 * asynchronous so it doesn’t add unnecessary latency to the UI.
 */
@interface MLQImageCache : NSObject

#pragma mark - Properties

/**
 *  Cache Config object - storing all kind of settings.
 *  The property is copy so change of current config will not accidentally affect other cache's config.
 */
@property (nonatomic, copy, nonnull, readonly) SDImageCacheConfig *config;

/**
 * The memory cache implementation object used for current image cache.
 * By default we use `SDMemoryCache` class, you can also use this to call your own implementation class method.
 * @note To customize this class, check `SDImageCacheConfig.memoryCacheClass` property.
 */
@property (nonatomic, strong, readonly, nonnull) id<SDMemoryCache> memoryCache;

/**
 * The disk cache implementation object used for current image cache.
 * By default we use `SDMemoryCache` class, you can also use this to call your own implementation class method.
 * @note To customize this class, check `SDImageCacheConfig.diskCacheClass` property.
 * @warning When calling method about read/write in disk cache, be sure to either make your disk cache implementation IO-safe or using the same access queue to avoid issues.
 */
@property (nonatomic, strong, readonly, nonnull) id<SDDiskCache> diskCache;

/**
 *  The disk cache's root path
 */
@property (nonatomic, copy, nonnull, readonly) NSString *diskCachePath;

/**
 *  The additional disk cache path to check if the query from disk cache not exist;
 *  The `key` param is the image cache key. The returned file path will be used to load the disk cache. If return nil, ignore it.
 *  Useful if you want to bundle pre-loaded images with your app
 */
@property (nonatomic, copy, nullable) SDImageCacheAdditionalCachePathBlock additionalCachePathBlock;

#pragma mark - Singleton and initialization

/**
 * Returns global shared cache instance
 */
@property (nonatomic, class, readonly, nonnull) MLQImageCache *sharedImageCache;

/**
 * Control the default disk cache directory. This will effect all the SDImageCache instance created after modification, even for shared image cache.
 * This can be used to share the same disk cache with the App and App Extension (Today/Notification Widget) using `- [NSFileManager.containerURLForSecurityApplicationGroupIdentifier:]`.
 * @note If you pass nil, the value will be reset to `~/Library/Caches/com.hackemist.SDImageCache`.
 * @note We still preserve the `namespace` arg, which means, if you change this property into `/path/to/use`,  the `SDImageCache.sharedImageCache.diskCachePath` should be `/path/to/use/default` because shared image cache use `default` as namespace.
 * Defaults to nil.
 */
@property (nonatomic, class, readwrite, null_resettable) NSString *defaultDiskCacheDirectory;

/**
 * Init a new cache store with a specific namespace
 * The final disk cache directory should looks like ($directory/$namespace). And the default config of shared cache, should result in (~/Library/Caches/com.hackemist.SDImageCache/default/)
 *
 * @param ns The namespace to use for this cache store
 */
- (nonnull instancetype)initWithNamespace:(nonnull NSString *)ns;

/**
 * Init a new cache store with a specific namespace and directory.
 * The final disk cache directory should looks like ($directory/$namespace). And the default config of shared cache, should result in (~/Library/Caches/com.hackemist.SDImageCache/default/)
 *
 * @param ns        The namespace to use for this cache store
 * @param directory Directory to cache disk images in
 */
- (nonnull instancetype)initWithNamespace:(nonnull NSString *)ns
                       diskCacheDirectory:(nullable NSString *)directory;

/**
 * Init a new cache store with a specific namespace, directory and config.
 * The final disk cache directory should looks like ($directory/$namespace). And the default config of shared cache, should result in (~/Library/Caches/com.hackemist.SDImageCache/default/)
 *
 * @param ns          The namespace to use for this cache store
 * @param directory   Directory to cache disk images in
 * @param config      The cache config to be used to create the cache. You can provide custom memory cache or disk cache class in the cache config
 */
- (nonnull instancetype)initWithNamespace:(nonnull NSString *)ns
                       diskCacheDirectory:(nullable NSString *)directory
                                   config:(nullable SDImageCacheConfig *)config NS_DESIGNATED_INITIALIZER;

#pragma mark - Cache paths

/**
 Get the cache path for a certain key
 
 @param key The unique image cache key
 @return The cache path. You can check `lastPathComponent` to grab the file name.
 */
- (nullable NSString *)cachePathForKey:(nullable NSString *)key;

#pragma mark - Store Ops

/**
 * Asynchronously store an image into memory and disk cache at the given key.
 *
 * @param imageData           The image to store
 * @param key             The unique image cache key, usually it's image absolute URL
 * @param completionBlock A block executed after the operation is finished
 */
- (void)storeImageData:(nullable NSData *)imageData
                forKey:(nullable NSString *)key
            completion:(nullable SDWebImageNoParamsBlock)completionBlock;

/**
 * Asynchronously store an image into memory and disk cache at the given key.
 *
 * @param imageData           The image to store
 * @param key             The unique image cache key, usually it's image absolute URL
 * @param toDisk          Store the image to disk cache if YES. If NO, the completion block is called synchronously
 * @param completionBlock A block executed after the operation is finished
 * @note If no image data is provided and encode to disk, we will try to detect the image format (using either `sd_imageFormat` or `SDAnimatedImage` protocol method) and animation status, to choose the best matched format, including GIF, JPEG or PNG.
 */
- (void)storeImageData:(nullable NSData *)imageData
                forKey:(nullable NSString *)key
                toDisk:(BOOL)toDisk
            completion:(nullable SDWebImageNoParamsBlock)completionBlock;

/**
 * Synchronously store image into memory cache at the given key.
 *
 * @param imageData  The image to store
 * @param key    The unique image cache key, usually it's image absolute URL
 */
- (void)storeImageDataToMemory:(nullable NSData *)imageData
                        forKey:(nullable NSString *)key;

/**
 * Synchronously store image data into disk cache at the given key.
 *
 * @param imageData  The image data to store
 * @param key        The unique image cache key, usually it's image absolute URL
 */
- (void)storeImageDataToDisk:(nullable NSData *)imageData
                      forKey:(nullable NSString *)key;


#pragma mark - Contains and Check Ops

/**
 *  Asynchronously check if image exists in disk cache already (does not load the image)
 *
 *  @param key             the key describing the url
 *  @param completionBlock the block to be executed when the check is done.
 *  @note the completion block will be always executed on the main queue
 */
- (void)diskImageExistsWithKey:(nullable NSString *)key completion:(nullable SDImageCacheCheckCompletionBlock)completionBlock;

/**
 *  Synchronously check if image data exists in disk cache already (does not load the image)
 *
 *  @param key             the key describing the url
 */
- (BOOL)diskImageDataExistsWithKey:(nullable NSString *)key;

#pragma mark - Query and Retrieve Ops

/**
 * Synchronously query the image data for the given key in disk cache. You can decode the image data to image after loaded.
 *
 *  @param key The unique key used to store the wanted image
 *  @return The image data for the given key, or nil if not found.
 */
- (nullable NSData *)diskImageDataForKey:(nullable NSString *)key;

/**
 * Asynchronously query the image data for the given key in disk cache. You can decode the image data to image after loaded.
 *
 *  @param key The unique key used to store the wanted image
 *  @param completionBlock the block to be executed when the query is done.
 *  @note the completion block will be always executed on the main queue
 */
- (void)diskImageDataQueryForKey:(nullable NSString *)key completion:(nullable SDImageCacheQueryDataCompletionBlock)completionBlock;

/**
 * Asynchronously queries the cache with operation and call the completion when done.
 *
 * @param key       The unique key used to store the wanted image. If you want transformed or thumbnail image, calculate the key with `SDTransformedKeyForKey`, `SDThumbnailedKeyForKey`, or generate the cache key from url with `cacheKeyForURL:context:`.
 * @param doneBlock The completion block. Will not get called if the operation is cancelled
 *
 * @return a NSOperation instance containing the cache op
 */
- (nullable NSOperation *)queryCacheOperationForKey:(nullable NSString *)key done:(nullable SDImageCacheQueryCompletionBlock)doneBlock;

/**
 * Asynchronously queries the cache with operation and call the completion when done.
 *
 * @param key       The unique key used to store the wanted image. If you want transformed or thumbnail image, calculate the key with `SDTransformedKeyForKey`, `SDThumbnailedKeyForKey`, or generate the cache key from url with `cacheKeyForURL:context:`.
 * @param queryCacheType Specify where to query the cache from. By default we use `.all`, which means both memory cache and disk cache. You can choose to query memory only or disk only as well. Pass `.none` is invalid and callback with nil immediately.
 * @param doneBlock The completion block. Will not get called if the operation is cancelled
 *
 * @return a NSOperation instance containing the cache op
 */
- (nullable NSOperation *)queryCacheOperationForKey:(nullable NSString *)key cacheType:(SDImageCacheType)queryCacheType done:(nullable SDImageCacheQueryCompletionBlock)doneBlock;

/**
 * Synchronously query the memory cache.
 *
 * @param key The unique key used to store the image
 * @return The image for the given key, or nil if not found.
 */
- (nullable NSData *)imageDataFromMemoryCacheForKey:(nullable NSString *)key;

/**
 * Synchronously query the disk cache.
 *
 * @param key The unique key used to store the image
 * @return The image for the given key, or nil if not found.
 */
- (nullable NSData *)imageDataFromDiskCacheForKey:(nullable NSString *)key;

/**
 * Synchronously query the cache (memory and or disk) after checking the memory cache.
 *
 * @param key The unique key used to store the image
 * @return The image for the given key, or nil if not found.
 */
- (nullable NSData *)imageDataFromCacheForKey:(nullable NSString *)key;


#pragma mark - Remove Ops

/**
 * Asynchronously remove the image from memory and disk cache
 *
 * @param key             The unique image cache key
 * @param completion      A block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(nullable NSString *)key withCompletion:(nullable SDWebImageNoParamsBlock)completion;

/**
 * Asynchronously remove the image from memory and optionally disk cache
 *
 * @param key             The unique image cache key
 * @param fromDisk        Also remove cache entry from disk if YES. If NO, the completion block is called synchronously
 * @param completion      A block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(nullable NSString *)key fromDisk:(BOOL)fromDisk withCompletion:(nullable SDWebImageNoParamsBlock)completion;

/**
 Synchronously remove the image from memory cache.
 
 @param key The unique image cache key
 */
- (void)removeImageFromMemoryForKey:(nullable NSString *)key;

/**
 Synchronously remove the image from disk cache.
 
 @param key The unique image cache key
 */
- (void)removeImageFromDiskForKey:(nullable NSString *)key;

#pragma mark - Cache clean Ops

/**
 * Synchronously Clear all memory cached images
 */
- (void)clearMemory;

/**
 * Asynchronously clear all disk cached images. Non-blocking method - returns immediately.
 * @param completion    A block that should be executed after cache expiration completes (optional)
 */
- (void)clearDiskOnCompletion:(nullable SDWebImageNoParamsBlock)completion;

/**
 * Asynchronously remove all expired cached image from disk. Non-blocking method - returns immediately.
 * @param completionBlock A block that should be executed after cache expiration completes (optional)
 */
- (void)deleteOldFilesWithCompletionBlock:(nullable SDWebImageNoParamsBlock)completionBlock;

#pragma mark - Cache Info

/**
 * Get the total bytes size of images in the disk cache
 */
- (NSUInteger)totalDiskSize;

/**
 * Get the number of images in the disk cache
 */
- (NSUInteger)totalDiskCount;

/**
 * Asynchronously calculate the disk cache's size.
 */
- (void)calculateSizeWithCompletionBlock:(nullable SDImageCacheCalculateSizeBlock)completionBlock;

@end

/**
 * MLQImageCache is the built-in image cache implementation for web image manager. It adopts `SDImageCache` protocol to provide the function for web image manager to use for image loading process.
 */
@interface MLQImageCache (MLQImageCache) <SDImageCache>

@end
NS_ASSUME_NONNULL_END
