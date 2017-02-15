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
#include <boost/variant.hpp>
#include <utility>

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

template<SinkType sinkType, typename Interface>
struct Sink : public SinkTag
{
    constexpr static SinkType type {sinkType};

    Interface mDataIn {nullptr};
};

struct SourceTag {};

template<SourceType sourceType, typename Interface>
struct Source : public SourceTag
{
    constexpr static SourceType type {sourceType};

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
decltype(auto) _findElement(S s, T &t)
{
    auto &e = std::get<size - 1>(t);  

    if constexpr(std::is_same<std::decay_t<S>, std::decay_t<decltype(e.type)> >::value)
    {
        if (e.type == s)
        {
            return e;
        }
        else if constexpr(size > 1)
        {
            return _findElement<S, T, size - 1>(s, t);
        }
    } 
    else if constexpr(size > 1)
    {
        return _findElement<S, T, size - 1>(s, t);
    }

    static_assert(size != 0, "dupa");
}

template<typename... A>
struct Component
{
    Component()
    {
        (processArguments<A>(), ...);
    }

    //virtual ~Component();
   
protected:
    typedef std::map<SourceType, std::vector<std::experimental::any> > ConnectionMap;
    ConnectionMap mConnection;

    template<typename S>
    void processArguments()
    {
        if constexpr(std::is_base_of<SinkTag, S>::value)
        {
            auto result = mSinkVector.insert(S::type);

            if(!result.second) {
                throw std::runtime_error(std::string("Duplicate sink '") + typeid(S).name() + "' in '" + typeid(this).name() + "' class");
            }
        }
        else if constexpr(std::is_base_of<SourceTag, S>::value)
        {
            mMap.insert(std::make_pair(S::type, S()));

            auto result = mSrcVector.insert(S::type);
            
            if(!result.second) {
                 throw std::runtime_error(std::string("Duplicate source '") + typeid(S).name() + "' in '" + typeid(this).name() + "' class");
            }
        }
    }

    typedef std::set<SinkType> SinkVector;
    SinkVector mSinkVector;

    typedef std::set<SourceType> SourceVector;
    SourceVector mSrcVector;

    std::map<SourceType, boost::variant<A...>> mMap;
    boost::variant<A...> mElements;
};

struct CanDevice : public Component<CanFrameSink, CanFrameSource>
{
    void test()
    {
        for (auto i : mMap)
        {
            std::cout << "dupa " << static_cast<int>(i.first) << std::endl;
        }

        auto dupa = boost::get<CanFrameSource>(mMap[SourceType::CanFrame]);
    }
};


#endif // __COMPONENT_H
