#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <vector>
#include <map>
#include <experimental/any>
#include <cstdint>
#include <algorithm>
#include <memory>

enum eSinkType
{
    CanFrameSink,
    CanSignalSink
};

enum eSourceType
{
    CanFrameSource,
    CanSignalSource,
};

struct CanFrame
{
    uint32_t id;
    std::vector<uint8_t> data;
    bool extended;
    bool error;
};

class ICanFrameSink
{
public:
    virtual bool FrameIn(const CanFrame& frame) = 0;
};

class Component
{
public:
//    virtual ~IComponent();

    virtual std::vector<eSourceType> getSourceTypes() const = 0;
    virtual std::vector<eSinkType> getSinkTypes() const = 0;
    virtual std::experimental::any getCompSink(eSourceType src) = 0;

    virtual bool connect(eSourceType src, Component *sink)
    {
        auto srcTypes = getSourceTypes();

        if(std::find(srcTypes.begin(), srcTypes.end(), src) != srcTypes.end())
        {
            auto sinkInterface = sink->getCompSink(src);

            if(!sinkInterface.empty())
            {
                mConnection[src].push_back(sinkInterface);
                return true;
            }
        }

        //TODO: Not supported

        return false;
    }

protected:
    std::map<eSourceType, std::vector<std::experimental::any> > mConnection;
};

class CanDevice : public Component, public ICanFrameSink
{
public:
    std::vector<eSourceType> getSourceTypes() const
    {
        return {{CanFrameSource}};
    }

    std::vector<eSinkType> getSinkTypes() const
    {
        return {{CanFrameSink}};
    }

    std::experimental::any getCompSink(eSourceType src)
    {
        switch(src)
        {
        case CanFrameSource:
            return this;

        default:
            return nullptr;
        }
    }

    // CanFrameSource handling
    void receiveThread()
    {
        while(true)
        {
            CanFrame frame;

            //receive_from_dev(&frame)

            for(const auto &v:mConnection[CanFrameSource])
            {
                auto iface = std::experimental::any_cast<ICanFrameSink*>(v);

                iface->FrameIn(frame);
            }
        }
    }

    // CanFrameSink handling
    bool FrameIn(const CanFrame& frame)
    {
        (void) frame;

        //send_to_device(frame);
        return true;
    }
};

class CanFilter : public Component, public ICanFrameSink
{
public:
    std::vector<eSourceType> getSourceTypes() const
    {
        return {{CanFrameSource}};
    }

    std::vector<eSinkType> getSinkTypes() const
    {
        return {{CanFrameSink}};
    }

    std::experimental::any getCompSink(eSourceType src)
    {
        switch(src)
        {
        case CanFrameSource:
            return this;

        default:
            return std::experimental::any();
        }
    }

    // CanFrameSink->CanFrameSource handling
    bool FrameIn(const CanFrame& frame)
    {
        // do some filtering and send to registered sinks

        for(const auto &v:mConnection[CanFrameSource])
        {
            auto iface = std::experimental::any_cast<ICanFrameSink*>(v);

            iface->FrameIn(frame);
        }

        return true;
    }

};

class CanRawView : public Component, public ICanFrameSink
{
public:
    std::vector<eSourceType> getSourceTypes() const
    {
        return {};
    }

    std::vector<eSinkType> getSinkTypes() const
    {
        return {{CanFrameSink}};
    }

    std::experimental::any getCompSink(eSourceType src)
    {
        switch(src)
        {
        case CanFrameSource:
            return this;

        default:
            return nullptr;
        }
    }

    // CanFrameSink handling
    bool FrameIn(const CanFrame& frame)
    {
        (void) frame;

        //display in widnow(frame);
        return true;
    }
};


#endif // __COMPONENT_H
