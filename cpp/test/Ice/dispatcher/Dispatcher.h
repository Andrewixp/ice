// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef DISPATCHER_I_H
#define DISPATCHER_I_H

#include <IceUtil/Thread.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <Ice/Dispatcher.h>

#include <deque>

class Dispatcher : public Ice::Dispatcher, IceUtil::Thread, IceUtil::Monitor<IceUtil::Mutex>
{
public:

    Dispatcher();
    virtual void dispatch(const Ice::DispatcherCallPtr&, const Ice::ConnectionPtr&);

    
    void run();
    static void terminate();
    static bool isDispatcherThread();

private:

    static Dispatcher* _instance;
    std::deque<Ice::DispatcherCallPtr> _calls;
    bool _terminated;
};

#endif
