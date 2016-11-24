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

enum SinkType : int
{
    UndefinedSinkType,
    CanFrameSinkType,
    CanSignalSinkType
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


class  CanFrameSink : public Sink<CanFrameSinkType>
{
public:
    static const SinkType sinkType {CanFrameSinkType};

    virtual bool FrameIn(const CanFrame& frame) = 0;
};

struct CanFrameSource : public Source<SourceType::CanFrame, CanFrameSink>
{
};

template<typename A>
struct Component
{
    Component()
    {
//        (processArguments<A>(), ...);
        processArguments<A>();
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

    template<typename S>
    S& operator|(S& obj)
    {
//        (v.push_back(args), ...);
//        for()

//        self.connect()
    }

protected:
    typedef std::map<SourceType, std::vector<std::experimental::any> > ConnectionMap;
    ConnectionMap mConnection;

    template<typename S>
    void processArguments()
    {
//        mSinkVector.push_back(S::sinkType);
        mSinkVector.push_back(S::sinkType);

    }

//    typedef std::vector<SinkType> SinkVector;
//    SinkVector mSinkVector;

    std::vector<SinkType> mSinkVector;

    typedef std::vector<SourceType> SourceVector;
    SourceVector mSrcVector;
};

struct CanDevice : public Component<CanFrameSink>
{
};

//class CanFilter : public Component, public CanFrameSink
//{
//public:
//    std::vector<eSourceType> getSourceTypes() const
//    {
//        return {{CanFrameSourceType}};
//    }

//    std::vector<eSinkType> getSinkTypes() const
//    {
//        return {{CanFrameSinkType}};
//    }

//    std::experimental::any getCompSink(eSourceType src)
//    {
//        switch(src)
//        {
//        case CanFrameSourceType:
//            return this;

//        default:
//            return std::experimental::any();
//        }
//    }

//    // CanFrameSink->CanFrameSource handling
//    bool FrameIn(const CanFrame& frame)
//    {
//        // do some filtering and send to registered sinks

//        for(const auto &v:mConnection[CanFrameSourceType])
//        {
//            auto iface = std::experimental::any_cast<CanFrameSink*>(v);

//            iface->FrameIn(frame);
//        }

//        return true;
//    }

//};

//class CanRawView : public Component, public CanFrameSink
//{
//public:
//    std::vector<eSourceType> getSourceTypes() const
//    {
//        return {};
//    }

//    std::vector<eSinkType> getSinkTypes() const
//    {
//        return {{CanFrameSinkType}};
//    }

//    std::experimental::any getCompSink(eSourceType src)
//    {
//        switch(src)
//        {
//        case CanFrameSourceType:
//            return this;

//        default:
//            return std::experimental::any();
//        }
//    }

//    // CanFrameSink handling
//    bool FrameIn(const CanFrame& frame)
//    {
//        (void) frame;

//        //display in widnow(frame);
//        return true;
//    }
//};


#endif // __COMPONENT_H
