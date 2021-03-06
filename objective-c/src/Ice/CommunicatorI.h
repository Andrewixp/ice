// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <objc/Ice/Communicator.h>

#import <objc/Ice/LocalObject.h>

#import <Foundation/NSSet.h>

#include <Ice/Communicator.h>

@class ICEObjectAdapter;

@interface ICECommunicator : ICELocalObject<ICECommunicator>
{
    NSMutableDictionary* valueFactories_;
    NSMutableDictionary* objectFactories_;
    NSDictionary* prefixTable_;
    NSMutableDictionary* adminFacets_;
}
-(void)setup:(NSDictionary*)prefixTable;
-(Ice::Communicator*)communicator;
-(NSDictionary*)getPrefixTable;
@end
