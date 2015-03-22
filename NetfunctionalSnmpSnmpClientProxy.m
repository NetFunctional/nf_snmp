//
//  ComSnmpProxy.m
//  snmp
//
//  Created by Ken Bantoft on 8/28/11.
//  Copyright 2011 NetFunctional Inc.. All rights reserved.
//

#import "NetfunctionalSnmpSnmpClientProxy.h"
#import "TiUtils.h"

#define kMibsDirectoryName @"mibs"

@implementation NetfunctionalSnmpSNMPClientProxy

//@synthesize host, community, mib;
@synthesize session, responseText, timeout = timeout_;

- (void)dealloc
{
    [responseText release];
    
    [super dealloc];
}

/**
 * Opening the SNMP session
 */
-(void)open:(id)args
{
    // retrieving properties values
    NSString *host_ = [self valueForKey:@"hostname"];
    NSString *community_ = [self valueForKey:@"community"];
    NSString *sTimeout = [self valueForKey:@"time_out"];
    NSString *version_ = [self valueForKey:@"version"];
    NSUInteger timeout = [sTimeout integerValue];
    //NSLog(@"timeout value: %@", sTimeout);
    
    // setting mibs directories (as module asset or project resource) and adding them
    NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
    
    NSString *modulePath = [NSString stringWithFormat:@"modules/%@/%@",@"netfunctional.snmp", kMibsDirectoryName];
    NSString *mibsModulePath = [NSString stringWithFormat:@"%@/%@", bundlePath, modulePath];
    
    NSString *appceleratorPath = kMibsDirectoryName;
    NSString *mibsAppceleratorPath = [NSString stringWithFormat:@"%@/%@", bundlePath, appceleratorPath];
    
    NSString *mibsDirectoryPath = [NSString stringWithFormat:@"%@:%@", mibsModulePath, mibsAppceleratorPath];
    
    snmp_mod_add_mibdir([mibsDirectoryPath UTF8String]);
    
    // opening the SNMP session
    session = snmp_mod_open([host_ UTF8String], [community_ UTF8String],  [version_ UTF8String], timeout);
    
    // fire the "onload" or "onerror" callbacks
    if ([[self errorText] length])
    {
        //NSLog(@"found error: %@", [self errorText]);
        [self fireCallback:@"onerror" withArg:[NSDictionary dictionaryWithObject:[self errorText] forKey:@"message"] withSource:self];
    }
    else
    {
        [self fireCallback:@"onload" withArg:[NSDictionary dictionaryWithObject:@"SNMP session successfully opened" forKey:@"message"] withSource:self];
    }
}

/**
 * Performing a GET request
 * @param mib The name of the mib
 */
- (id)fetch:(id)args
{
    NSString *mib = [self valueForKey:@"mib"];
    
    // forwarding the "GET" request to the NETSNMP C library
    const char *result = snmp_mod_get(session, [mib UTF8String]);
    //printf("result: %s", result);
    
    NSString *sResult = nil;
    if (result != NULL)
    {
        sResult = [NSString stringWithUTF8String:result];
    }
    
    // fire the "onerror" callback if an error is found
    if ([[self errorText] length])
    {
        [self fireCallback:@"onerror" withArg:[NSDictionary dictionaryWithObject:[self errorText] forKey:@"message"] withSource:self];
    }
    
    return sResult;
}



/**
 * Performing a GET request
 * @param mib The name of the mib
 */
- (id)fetchnext:(id)args
{
    NSString *mib = [self valueForKey:@"mib"];
    
    // forwarding the "GETNEXT" request to the NETSNMP C library
    const char *result = snmp_mod_getnext(session, [mib UTF8String]);
    //printf("result: %s", result);
    
    NSString *sResult = nil;
    if (result != NULL)
    {
        sResult = [NSString stringWithUTF8String:result];
    }
    
    // fire the "onerror" callback if an error is found
    if ([[self errorText] length])
    {
        [self fireCallback:@"onerror" withArg:[NSDictionary dictionaryWithObject:[self errorText] forKey:@"message"] withSource:self];
    }
    
    return sResult;
}


/**
 * Performing a WALK request
 */
- (NSArray *)walk:(id)args
{
    NSString *baseOid = [self valueForKey:@"mib"];
    
    char **mib_strings = snmp_mod_walk(session, [baseOid UTF8String]);
    
    NSMutableArray *mibStrings = [NSMutableArray array];
    
    int i=0;
    while (mib_strings[i])
    {
        char *cString = mib_strings[i];
        NSString *mibString = [NSString stringWithUTF8String:cString];
        [mibStrings addObject:mibString];
        
        i++;
    }
    
    // deallocate C mibs array
    i=0;
    while (mib_strings[i])
    {
        free(mib_strings[i]);
        i++;
    }
    free(mib_strings);
    
    // fire the "onerror" callback if an error is found
    if ([[self errorText] length])
    {
        [self fireCallback:@"onerror" withArg:[NSDictionary dictionaryWithObject:[self errorText] forKey:@"message"] withSource:self];
    }
    
    return mibStrings;
}

- (BOOL)write:(NSArray *)args
{
    NSString *mib = [TiUtils stringValue:[args objectAtIndex:0]];
    if (!mib) mib = [self valueForKey:@"mib"];
    NSString *type = [TiUtils stringValue:[args objectAtIndex:1]];
    NSString *value = [TiUtils stringValue:[args objectAtIndex:2]];
    NSLog(@"test: %@, %@, %@", mib, type, value);
    char *result = snmp_mod_set(session, [mib UTF8String], [type characterAtIndex:0], [value UTF8String]);
    
    BOOL success = NO;
    if (result != NULL)
    {
        success = YES;
    }
    
    // fire the "onerror" callback if an error is found
    if ([[self errorText] length])
    {
        [self fireCallback:@"onerror" withArg:[NSDictionary dictionaryWithObject:[self errorText] forKey:@"message"] withSource:self];
    }
    
    return success;
}

/**
 * Closing the SNMP session
 */
- (void)close:(id)args
{
    snmp_mod_close(session);
}

/**
 * Setting the "onload" callback function
 */
-(void)setOnload:(KrollCallback *)callback
{
	hasOnload = [callback isKindOfClass:[KrollCallback class]];
	[self setCallback:callback forKey:@"onload"];
}

/**
 * Setting the "onerror" callback function
 */
-(void)setOnerror:(KrollCallback *)callback
{
	hasOnerror = [callback isKindOfClass:[KrollCallback class]];
	[self setCallback:callback forKey:@"onerror"];
}

/**
 * Getting the error message as an Objective-C "NSString" variable (instead of a C "char *")
 */
- (NSString *)errorText
{
    return (errorText == NULL) ? nil : [NSString stringWithUTF8String:errorText];
}

/**
 * Custom setter for the timeout property
 */
- (void)setTimeout:(int)timeout
{
    if (session && timeout > 0)
    {
        session->timeout = timeout;
    }
}

@end
