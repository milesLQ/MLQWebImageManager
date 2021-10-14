//
//  MLQSchemeHandler.m
//  MLQWebImageManager
//
//  Created by qianlei on 2021/10/14.
//

#import "MLQSchemeHandler.h"
#import "MLQWebImageManager.h"

NSString *imageURLScheme = @"mlqimageurlscheme";   //约定的拦截scheme

@implementation MLQSchemeHandler 

#pragma mark - WKURLSchemeHandler

/// WKWebView 开始加载自定义scheme的资源
- (void)webView:(nonnull WKWebView *)webView startURLSchemeTask:(nonnull id<WKURLSchemeTask>)urlSchemeTask  API_AVAILABLE(ios(11.0)) {

    // 判断开启URL拦截
    NSString *originalURL = urlSchemeTask.request.URL.absoluteString;
    NSURLComponents *componets = [[NSURLComponents alloc] initWithString:originalURL];
       
    if ([componets.scheme isEqualToString:imageURLScheme]) {
        NSURL *realWebPUrl = componets.URL;
        BOOL isNeedRequest = NO;
        __block NSURLQueryItem *_Nonnull targetQueryItem;
        [componets.queryItems enumerateObjectsUsingBlock:^(NSURLQueryItem *_Nonnull obj, NSUInteger idx, BOOL *_Nonnull stop) {
            if ([obj.name isEqualToString:@"url"]) {
                targetQueryItem = obj;
                *stop = YES;
            }
        }];
        
        //MLQImageURLScheme://webp/url?url=完整URL
        if ([componets.URL.lastPathComponent isEqualToString:@"url"]) {
            isNeedRequest = YES;
            //真实地址
            realWebPUrl = [NSURL URLWithString:targetQueryItem.value];
            
            SDWebImageOptions options = SDWebImageRetryFailed | SDWebImageAvoidDecodeImage;
            
            [[MLQWebImageManager sharedManager] loadImageWithURL:realWebPUrl options:options progress:nil completed:^(NSData * _Nullable data, NSError * _Nullable error, SDImageCacheType cacheType, BOOL finished, NSURL * _Nullable imageURL) {
                // 回传图片的 NSData数据 给H5页面
                [self responseWithUrlSchemeTask:urlSchemeTask mimeType:nil data:data error:error];
                
            }];
            return;
        }
        
    }
    
    NSError *error = [NSError errorWithDomain:@"schemeUnMatch" code:9527 userInfo:nil];
    [self responseWithUrlSchemeTask:urlSchemeTask mimeType:nil data:[NSData new] error:error];
}

- (void)webView:(nonnull WKWebView *)webView stopURLSchemeTask:(nonnull id<WKURLSchemeTask>)urlSchemeTask API_AVAILABLE(ios(11.0)) {
    
}

#pragma mark - Private

// 返回数据给H5页面 (提供我们本地下载好的图片资源数据)
- (void)responseWithUrlSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask
                         mimeType:(NSString *)mimeType
                             data:(NSData *)data
                            error:(NSError *)error API_AVAILABLE(ios(11.0)) {
    if (!urlSchemeTask || !urlSchemeTask.request || !urlSchemeTask.request.URL) {
        return;
    }
    
    if (error) {
        [urlSchemeTask didFailWithError:error];
    } else {
        NSURLResponse *response = [[NSURLResponse alloc] initWithURL:urlSchemeTask.request.URL
                                                            MIMEType:mimeType
                                               expectedContentLength:data.length
                                                    textEncodingName:nil];
        [urlSchemeTask didReceiveResponse:response];
        [urlSchemeTask didReceiveData:data ?: [NSData new]];
        [urlSchemeTask didFinish];
    }
}

@end
