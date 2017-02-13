#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <vector>
#include <map>
#include <experimental/any>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <tuple>
#include <type_traits>
#include <set>
#include <iostream>

enum class SinkType
{
    Undefined,
    CanFrame,
    CanSignal
};

enum class SourceType
{
    Undefined,
    CanFrame,
    CanSignal
};

struct CanFrame
{
    uint32_t id;
    std::vector<uint8_t> data;
    bool extended;
    bool error;
};

struct SinkTag {};

template<SinkType _sinkType, typename Interface>
struct Sink : public SinkTag
{
    static const SinkType sinkType = _sinkType;

    Interface mDataIn {nullptr};
};

struct SourceTag {};

template<SourceType _sourceType, typename Interface>
struct Source : public SourceTag
{
    static const SourceType sourceType {_sourceType};

    std::vector<Interface> mDataOut;
};

typedef std::function<bool(const CanFrame &)> CanFrameInterface;

class  CanFrameSink : public Sink<SinkType::CanFrame, CanFrameInterface>
{
};

struct CanFrameSource : public Source<SourceType::CanFrame, CanFrameInterface>
{
};

template<typename S, typename T, int size = std::tuple_size<T>::value>
decltype(auto) _findElement(S s, T &t, typename std::enable_if<size != 1 && std::is_base_of<SourceTag, typename std::tuple_element<size - 1, T>::type >::value>::type* = 0)
{
    auto &e = std::get<size - 1>(t);  

    if (e.sourceType == s)
    {
        return e;
    }

    return _findElement<S, T, size - 1>(s, t);
}

template<typename S, typename T, int size = std::tuple_size<T>::value>
decltype(auto) _findElement(S s, T &t, typename std::enable_if<size != 1 && std::is_base_of<SinkTag, typename std::tuple_element<size - 1, T>::type >::value>::type* = 0)
{
    auto &e = std::get<size - 1>(t);  

    if constexpr(std::is_same<S, SinkType>::value)
    {
        if(e.sinkType == s)
        {
            return e;
        }
    }
        
    return _findElement<S, T, size - 1>(s, t);
}

template<typename S, typename T, int size = std::tuple_size<T>::value>
decltype(auto) _findElement(S s, T &t, typename std::enable_if<size == 1 && std::is_base_of<SourceTag, typename std::tuple_element<0, T>::type >::value>::type* = 0)
{
    auto &e = std::get<0>(t);  

    if (e.sourceType == s)
    {
        return e;
    }

    throw std::runtime_error(std::string(typeid(T).name()) + " not found!");
    
    return _findElement<S, T, 0>(s, t);
}

template<typename S, typename T, int size = std::tuple_size<T>::value>
decltype(auto) _findElement(S s, T &t, typename std::enable_if<size == 1 && std::is_base_of<SinkTag, typename std::tuple_element<0, T>::type >::value>::type* = 0)
{
    auto &e = std::get<0>(t);  

    if constexpr(std::is_same<S, SinkType>::value)
    {
        if(e.sinkType == s)
        {
            return e;
        }
    }
        
    throw std::runtime_error(std::string(typeid(T).name()) + " not found!");
    
    return _findElement<S, T, 0>(s, t);
}

template<typename... A>
struct Component
{
    Component()
    {
        (processArguments<A>(), ...);
    }

    //virtual ~Component();

//    virtual const std::vector<SourceType> getSourceTypes() const
//    {
//        return mSrcVector;
//    }

//    virtual const std::vector<SinkType> getSinkTypes() const
//    {
//        return mSinkVector;
//    }

    //virtual bool connect(SourceType src, Component *sink)
    //{
        //if(std::find(mSrcVector.begin(), mSrcVector.end(), src) != mSrcVector.end())
        //{
            //auto sinkInterface = sink->getCompSink(src);

            //if(!sinkInterface.empty())
            //{
                //mConnection[src].push_back(sinkInterface);
                //return true;
            //}
        //}

        ////TODO: Not supported

        //return false;
    //}
    //
    
protected:
    typedef std::map<SourceType, std::vector<std::experimental::any> > ConnectionMap;
    ConnectionMap mConnection;


    decltype(auto) findElement(SourceType src)
    {
        return ::_findElement(src, mElements);
        //return src;
    }

    template<typename S>
    void processArguments()
    {
        if constexpr(std::is_base_of<SinkTag, S>::value)
        {
            auto result = mSinkVector.insert(static_cast<SinkType>(S::sinkType));

            if(!result.second) {
                throw std::runtime_error(std::string("Duplicate sink '") + typeid(S).name() + "' in '" + typeid(this).name() + "' class");
            }
        }
        else if constexpr(std::is_base_of<SourceTag, S>::value)
        {
            auto s = findElement(S::sourceType);

            auto result = mSrcVector.insert(static_cast<SourceType>(S::sourceType));
            
            if(!result.second) {
                 throw std::runtime_error(std::string("Duplicate source '") + typeid(S).name() + "' in '" + typeid(this).name() + "' class");
            }
        }
    }

    typedef std::set<SinkType> SinkVector;
    SinkVector mSinkVector;

    typedef std::set<SourceType> SourceVector;
    SourceVector mSrcVector;

    std::tuple<A...> mElements;
};

struct CanDevice : public Component<CanFrameSink, CanFrameSource>
{
};


#endif // __COMPONENT_H
