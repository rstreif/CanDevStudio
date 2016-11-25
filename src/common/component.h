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

template<SinkType _sinkType>
struct Sink : public SinkTag
{
    static const SinkType sinkType = _sinkType;
};

struct SourceTag {};

template<SourceType _sourceType, typename... CompSink>
struct Source : public SourceTag
{
    static const SourceType sourceType {_sourceType};

    virtual std::vector<SinkType> getCompatibleSinks()
    {
        return { (CompSink::sinkType)... };
    }
};


class  CanFrameSink : public Sink<SinkType::CanFrame>
{
public:
    virtual bool FrameIn(const CanFrame& frame) = 0;
};

struct CanFrameSource : public Source<SourceType::CanFrame, CanFrameSink>
{
};

template<typename... A>
struct Component
{
    Component()
    {
        (processArguments<A>(), ...);
    }

//    virtual ~Component();

//    virtual const std::vector<SourceType> getSourceTypes() const
//    {
//        return mSrcVector;
//    }

//    virtual const std::vector<SinkType> getSinkTypes() const
//    {
//        return mSinkVector;
//    }

//    virtual bool connect(SourceType src, Component *sink)
//    {
//        auto srcTypes = getSourceTypes();

//        if(std::find(srcTypes.begin(), srcTypes.end(), src) != srcTypes.end())
//        {
//            auto sinkInterface = sink->getCompSink(src);

//            if(!sinkInterface.empty())
//            {
//                mConnection[src].push_back(sinkInterface);
//                return true;
//            }
//        }

//        //TODO: Not supported

//        return false;
//    }

protected:
    typedef std::map<SourceType, std::vector<std::experimental::any> > ConnectionMap;
    ConnectionMap mConnection;

    template<typename S>
    void processArguments()
    {
        if constexpr(std::is_base_of<SinkTag, S>::value)
        {
            mSinkVector.push_back(static_cast<SinkType>(S::sinkType));
        }
        else if constexpr(std::is_base_of<SourceTag, S>::value)
        {
            mSrcVector.push_back(static_cast<SourceType>(S::sourceType));
        }
    }

    typedef std::vector<SinkType> SinkVector;
    SinkVector mSinkVector;

    typedef std::vector<SourceType> SourceVector;
    SourceVector mSrcVector;
};

struct CanDevice : public Component<CanFrameSink, CanFrameSink, CanFrameSource>
{
};


#endif // __COMPONENT_H
