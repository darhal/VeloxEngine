#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
    class RenderDevice;

	struct SemaphoreDeleter
	{
		void operator()(class Semaphore* semaphore);
	};

	class Semaphore : public Utils::RefCounterEnabled<Semaphore, SemaphoreDeleter, HandleCounter>
	{
	public:
        enum {
            BINARY = 0,
            TIMELINE,
        };

        Semaphore(RenderDevice& device, VkSemaphore semaphore, uint64 initialValue = 0) :
            device(device), semaphore(semaphore), initialValue(initialValue),
            tempValue(initialValue), clean(true)
        {};

		~Semaphore();

		FORCEINLINE VkSemaphore GetApiObject() const { return semaphore; }

		void SetNoClean() { clean = false; };

        void Signal(uint64 signalValue);

        void Wait(uint64 waitValue = 0);

        uint64 GetCurrentCounterValue() const;

       void Reset();

        uint8 GetType() const { return uint8(initialValue ? TIMELINE : BINARY); }

        uint64 IncrementTempValue(uint64 amount = 1) { tempValue += amount; return tempValue; }

        uint64 GetTempValue() const { return tempValue; }
	private:
        RenderDevice& device;
        VkSemaphore   semaphore;
        uint64        initialValue;
        uint64        tempValue;   // the feature value the semaphore will be in
        bool          clean;

		friend struct SemaphoreDeleter;
        friend class RenderDevice;
	};

	using SemaphoreHandle = Handle<Semaphore>;
}

TRE_NS_END
