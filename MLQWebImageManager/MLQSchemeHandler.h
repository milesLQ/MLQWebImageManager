//
//  MLQSchemeHandler.h
//  MLQWebImageManager
//
//  Created by qianlei on 2021/10/14.
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *imageURLScheme;   //约定的拦截scheme


@interface MLQSchemeHandler : NSObject <WKURLSchemeHandler>

@end

NS_ASSUME_NONNULL_END
