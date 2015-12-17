// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestI.h>
#include <TestCommon.h>
#include <Dispatcher.h>

DEFINE_TEST("server")

using namespace std;

int
run(int, char**, const Ice::CommunicatorPtr& communicator)
{
    communicator->getProperties()->setProperty("TestAdapter.Endpoints", getTestEndpoint(communicator, 0));
    communicator->getProperties()->setProperty("ControllerAdapter.Endpoints", getTestEndpoint(communicator, 1, "tcp"));
    communicator->getProperties()->setProperty("ControllerAdapter.ThreadPool.Size", "1");

    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
    Ice::ObjectAdapterPtr adapter2 = communicator->createObjectAdapter("ControllerAdapter");

    TestIntfControllerIPtr testController = ICE_MAKE_SHARED(TestIntfControllerI, adapter);

    adapter->add(ICE_MAKE_SHARED(TestIntfI), communicator->stringToIdentity("test"));
    adapter->activate();

    adapter2->add(testController, communicator->stringToIdentity("testController"));
    adapter2->activate();

    TEST_READY

    communicator->waitForShutdown();
    return EXIT_SUCCESS;
}

#ifdef ICE_CPP11_MAPPING
class DispatcherCall : public Ice::DispatcherCall
{
public:

    DispatcherCall(function<void ()> call) :
        _call(move(call))
    {
    }

    virtual void run()
    {
        _call();
    }

private:

    function<void ()> _call;
};
#endif

int
main(int argc, char* argv[])
{
#ifdef ICE_STATIC_LIBS
    Ice::registerIceSSL();
#endif
    int status;
    try
    {
        Ice::InitializationData initData;
        initData.properties = Ice::createProperties(argc, argv);

        //
        // Limit the recv buffer size, this test relies on the socket
        // send() blocking after sending a given amount of data.
        //
        initData.properties->setProperty("Ice.TCP.RcvSize", "50000");

#if defined(ICE_CPP11_MAPPING)
        Ice::DispatcherPtr dispatcher = new Dispatcher();
        initData.dispatcher = [=](function<void ()> call, const shared_ptr<Ice::Connection>& conn)
            {
                dispatcher->dispatch(new DispatcherCall(call), conn);
            };
#elif defined(ICE_CPP11_COMPILER)
        Ice::DispatcherPtr dispatcher = new Dispatcher();
        initData.dispatcher = Ice::newDispatcher(
            [=](const Ice::DispatcherCallPtr& call, const Ice::ConnectionPtr& conn)
                {
                    dispatcher->dispatch(call, conn);
                });
#else
        initData.dispatcher = new Dispatcher();
#endif
        Ice::CommunicatorHolder ich = Ice::initialize(argc, argv, initData);
        status = run(argc, argv, ich.communicator());
    }
    catch(const Ice::Exception& ex)
    {
        cerr << ex << endl;
        status = EXIT_FAILURE;
    }
    Dispatcher::terminate();
    return status;
}
