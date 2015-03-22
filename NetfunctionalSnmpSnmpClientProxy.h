//
//  ComSnmpProxy.h
//  snmp
//
//  Created by Ken Bantoft on 8/28/11.
//  Copyright 2011 __NetFunctional Inc.__. All rights reserved.
//

#import "TiProxy.h"
#import "netsnmp_adapter.c"

@interface NetfunctionalSnmpSNMPClientProxy : TiProxy {
    netsnmp_session *session;
    int timeout_;
    
    BOOL hasOnload;
    BOOL hasOnerror;
}

@property (nonatomic, assign) netsnmp_session *session;
@property (nonatomic, retain) NSString* responseText;
@property (nonatomic, assign) int timeout;

-(void)setOnload:(KrollCallback *)callback;
- (NSString *)errorText;

@end
