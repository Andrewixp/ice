// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEPY_TYPES_H
#define ICEPY_TYPES_H

#include <Operation.h>
#include <Util.h>
#include <Ice/Stream.h>

namespace IcePy
{

const int TYPE_BOOL         = 0;
const int TYPE_BYTE         = 1;
const int TYPE_SHORT        = 2;
const int TYPE_INT          = 3;
const int TYPE_LONG         = 4;
const int TYPE_FLOAT        = 5;
const int TYPE_DOUBLE       = 6;
const int TYPE_STRING       = 7;
const int TYPE_OBJECT       = 8;
const int TYPE_OBJECT_PROXY = 9;

class ExceptionInfo;
typedef IceUtil::Handle<ExceptionInfo> ExceptionInfoPtr;
typedef std::vector<ExceptionInfoPtr> ExceptionInfoList;

class ClassInfo;
typedef IceUtil::Handle<ClassInfo> ClassInfoPtr;
typedef std::vector<ClassInfoPtr> ClassInfoList;

//
// This class is raised as an exception when object marshaling needs to be aborted.
//
class AbortMarshaling
{
};

typedef std::map<PyObject*, Ice::ObjectPtr> ObjectMap;

//
// The delayed nature of class unmarshaling in the Ice protocol requires us to
// handle unmarshaling using a callback strategy. An instance of UnmarshalCallback
// is supplied to each type's unmarshal() member function. For all types except
// classes, the callback is invoked with the unmarshaled value before unmarshal()
// returns. For class instances, however, the callback may not be invoked until
// the stream's finished() function is called.
//
class UnmarshalCallback : public IceUtil::Shared
{
public:

    virtual ~UnmarshalCallback();

    //
    // The unmarshaled() member function receives the unmarshaled value. The
    // last two arguments are the values passed to unmarshal() for use by
    // UnmarshalCallback implementations.
    //
    virtual void unmarshaled(PyObject*, PyObject*, void*) = 0;
};
typedef IceUtil::Handle<UnmarshalCallback> UnmarshalCallbackPtr;

//
// Base class for type information.
//
class TypeInfo : public UnmarshalCallback
{
public:

    virtual std::string getId() const = 0;

    virtual bool validate(PyObject*) = 0;

    virtual void unmarshaled(PyObject*, PyObject*, void*); // Default implementation is assert(false).

    virtual void destroy();

protected:

    TypeInfo();

public:

    //
    // The marshal and unmarshal functions can raise Ice exceptions, and may raise
    // AbortMarshaling if an error occurs.
    //
    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*) = 0;
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*) = 0;
};
typedef IceUtil::Handle<TypeInfo> TypeInfoPtr;

//
// Primitive type information.
//
class PrimitiveInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);

    enum Kind
    {
        KindBool,
        KindByte,
        KindShort,
        KindInt,
        KindLong,
        KindFloat,
        KindDouble,
        KindString
    };

    Kind kind;
};
typedef IceUtil::Handle<PrimitiveInfo> PrimitiveInfoPtr;

//
// Enum information.
//
typedef std::vector<PyObjectHandle> EnumeratorList;

class EnumInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);

    std::string id;
    EnumeratorList enumerators;
    PyObjectHandle pythonType;
};
typedef IceUtil::Handle<EnumInfo> EnumInfoPtr;

class DataMember : public UnmarshalCallback
{
public:

    virtual void unmarshaled(PyObject*, PyObject*, void*);

    std::string name;
    TypeInfoPtr type;
};
typedef IceUtil::Handle<DataMember> DataMemberPtr;
typedef std::vector<DataMemberPtr> DataMemberList;

//
// Struct information.
//
class StructInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);

    virtual void destroy();

    std::string id;
    DataMemberList members;
    PyObjectHandle pythonType;
};
typedef IceUtil::Handle<StructInfo> StructInfoPtr;

//
// Sequence information.
//
class SequenceInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);
    virtual void unmarshaled(PyObject*, PyObject*, void*);

    virtual void destroy();

    std::string id;
    TypeInfoPtr elementType;
};
typedef IceUtil::Handle<SequenceInfo> SequenceInfoPtr;

//
// Dictionary information.
//
class DictionaryInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);
    virtual void unmarshaled(PyObject*, PyObject*, void*);

    virtual void destroy();

    class KeyCallback : public UnmarshalCallback
    {
    public:

        virtual void unmarshaled(PyObject*, PyObject*, void*);

        PyObjectHandle key;
    };
    typedef IceUtil::Handle<KeyCallback> KeyCallbackPtr;

    std::string id;
    TypeInfoPtr keyType;
    TypeInfoPtr valueType;
};
typedef IceUtil::Handle<DictionaryInfo> DictionaryInfoPtr;

typedef std::vector<TypeInfoPtr> TypeInfoList;

//
// Operation information.
//
const int OP_NORMAL      = 0;
const int OP_NONMUTATING = 1;
const int OP_IDEMPOTENT  = 2;

class ParamInfo : public UnmarshalCallback
{
public:

    virtual void unmarshaled(PyObject*, PyObject*, void*);

    TypeInfoPtr type;
};
typedef IceUtil::Handle<ParamInfo> ParamInfoPtr;
typedef std::vector<ParamInfoPtr> ParamInfoList;

class OperationInfo : public Operation
{
public:

    virtual PyObject* invoke(const Ice::ObjectPrx&, const Ice::CommunicatorPtr&, PyObject*);
    virtual bool dispatch(PyObject*, const std::vector<Ice::Byte>&, std::vector<Ice::Byte>&, const Ice::Current&);

    bool checkDispatchException(std::vector<Ice::Byte>&, const Ice::CommunicatorPtr&);

    void unmarshalException(const std::vector<Ice::Byte>&, const Ice::CommunicatorPtr&);

    //
    // Verify that the given Python exception is legal to be thrown from this operation.
    //
    bool validateException(PyObject*) const;

    std::string name;
    int mode;
    ParamInfoList inParams;
    ParamInfoList outParams;
    ParamInfoPtr returnType;
    ExceptionInfoList exceptions;
};
typedef IceUtil::Handle<OperationInfo> OperationInfoPtr;

//
// Class information.
//
typedef std::map<std::string, OperationInfoPtr> OperationInfoMap;

class ClassInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);

    virtual void destroy();

    OperationInfoPtr findOperation(const std::string&) const;
    bool hasOperations() const;

    std::string id;
    bool isInterface;
    ClassInfoPtr base;
    ClassInfoList interfaces;
    DataMemberList members;
    OperationInfoMap operations;
    PyObjectHandle pythonType;
};

//
// Proxy information.
//
class ProxyInfo : public TypeInfo
{
public:

    virtual std::string getId() const;

    virtual bool validate(PyObject*);

    virtual void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    virtual void unmarshal(const Ice::InputStreamPtr&, const UnmarshalCallbackPtr&, PyObject*, void*);

    std::string id;
    ClassInfoPtr _class;
    PyObjectHandle pythonType;
};
typedef IceUtil::Handle<ProxyInfo> ProxyInfoPtr;

//
// Exception information.
//
class ExceptionInfo : public IceUtil::Shared
{
public:

    void marshal(PyObject*, const Ice::OutputStreamPtr&, ObjectMap*);
    PyObject* unmarshal(const Ice::InputStreamPtr&);

    std::string id;
    ExceptionInfoPtr base;
    DataMemberList members;
    bool usesClasses;
    PyObjectHandle pythonType;
};

//
// ObjectWriter wraps a Python object for marshaling.
//
class ObjectWriter : public Ice::ObjectWriter
{
public:

    ObjectWriter(const ClassInfoPtr&, PyObject*, ObjectMap*);
    ~ObjectWriter();

    virtual void write(const Ice::OutputStreamPtr&) const;

private:

    ClassInfoPtr _info;
    PyObject* _object;
    ObjectMap* _map;
};

//
// ObjectReader unmarshals the state of an Ice object.
//
class ObjectReader : public Ice::ObjectReader
{
public:

    ObjectReader(PyObject*, const ClassInfoPtr&);
    ~ObjectReader();

    virtual void read(const Ice::InputStreamPtr&, bool);

    PyObject* getObject() const; // Borrowed reference.

private:

    PyObject* _object;
    ClassInfoPtr _info;
};
typedef IceUtil::Handle<ObjectReader> ObjectReaderPtr;

TypeInfoPtr getTypeInfo(const std::string&);
ExceptionInfoPtr getExceptionInfo(const std::string&);

bool initTypes(PyObject*);

}

extern "C" PyObject* Ice_defineEnum(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineStruct(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineSequence(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineDictionary(PyObject*, PyObject*);
extern "C" PyObject* Ice_declareProxy(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineProxy(PyObject*, PyObject*);
extern "C" PyObject* Ice_declareClass(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineClass(PyObject*, PyObject*);
extern "C" PyObject* Ice_defineException(PyObject*, PyObject*);

#endif
