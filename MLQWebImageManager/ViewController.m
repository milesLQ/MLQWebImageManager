//
//  ViewController.m
//  MLQWebImageManager
//
//  Created by qianlei on 2021/10/14.
//

#import "ViewController.h"
#import "MLQSchemeHandler.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Test wkwebview image load";
    [self.view setBackgroundColor:UIColor.whiteColor];
    
    
    // 初始化webViewConfiguration
    WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];

    // 需要通过configuration 注册
    if (@available(iOS 11.0, *)) {
        [configuration setURLSchemeHandler:
         [[MLQSchemeHandler alloc] init] forURLScheme:imageURLScheme];
    }

    // 使用注册好的configuration 初始化 WKWebView
    WKWebView *webView =
        [[WKWebView alloc] initWithFrame:self.view.frame configuration:configuration];
    webView.backgroundColor = UIColor.whiteColor;
    [self.view addSubview:webView];
    
    // WKWebview加载 H5测试页面;
    // 首次、首次下载图片可能会比较慢、比较慢
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"html"];
    NSURL *pathUrl = [NSURL fileURLWithPath:filePath];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:pathUrl cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:20];
    [webView loadRequest:urlRequest];
    
    
    UIButton *nextPage = [UIButton buttonWithType:UIButtonTypeSystem];
    nextPage.backgroundColor = UIColor.yellowColor;
    [nextPage setTitle:@"点击加载一个新页面，图片直接读取缓存" forState:UIControlStateNormal];
    [nextPage setTitleColor:UIColor.redColor forState:UIControlStateNormal];
    nextPage.frame = CGRectMake(40, 520, 290, 70);
    [nextPage addTarget:self action:@selector(nextPageEvent) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:nextPage];
}

- (void)nextPageEvent {
    ViewController *vc = [ViewController new];
    [self.navigationController pushViewController:vc animated:YES];
}

@end
