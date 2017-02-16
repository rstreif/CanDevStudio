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

template<typename... A>
struct Component
{
    Component()
    {
        (processArguments<A>(), ...);
    }

    virtual ~Component() { } 
        
    template <typename F>
    bool connect(SourceType src, F &func)
    {
        return connect(src, mElements, func);
    }

    std::vector<SourceType> getSources() const
    {
        return getSources(mElements);
    }
   
protected:
    template<typename S>
    void processArguments()
    {
        // Will fail to compile if not all elements are unique
        std::get<S>(mElements);

        if constexpr(std::is_base_of<SourceTag, S>::value)
        {
            mSources.push_back(S::type);
        }
        else if constexpr(std::is_base_of<SinkTag, S>::value)
        {
            mSinks.push_back(S::type);
        }
        else
        {
            // Causes compilation error. Only Sources and Sinks are accepted.
            std::get<Component>(mElements);
        }
    }

    template<typename F, typename T, int size = std::tuple_size<T>::value>
    bool connect(SourceType src, T &t, F &func)
    {
        auto &e = std::get<size - 1>(t);  

        if constexpr(std::is_base_of<SourceTag, std::decay_t<decltype(e)> >::value)
        {
            if (e.type == src)
            {
                e.mDataOut.push_back(func);
                return true;
            }
        } 
        
        if constexpr(size > 1)
        {
            return connect<F, T, size - 1>(src, t, func);
        }

        return false;
    }

    std::tuple<A...> mElements;
    std::vector<SinkType> mSinks;
    std::vector<SourceType> mSources;
};

struct CanDevice : public Component<CanFrameSink, CanFrameSource>
{
    void test()
    {
        CanFrameInterface b;
        
        std::cout << "aaaaa " << connect(SourceType::Undefined, b) << std::endl;
        std::cout << "aaaaa " << connect(SourceType::CanFrame, b) << std::endl;
        std::cout << "aaaaa " << connect(SourceType::CanFrame, b) << std::endl;

        std::cout << "bbb " << std::get<CanFrameSource>(mElements).mDataOut.size() << std::endl;
    }
};


#endif // __COMPONENT_H
