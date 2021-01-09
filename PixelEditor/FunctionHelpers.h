#pragma once
#include <chrono>
#include <iostream>
#include <string>

namespace time_functions
{
	enum TimeType
	{
		ms, s, min
	};

	namespace delays
	{
		// Delay with arguments passed to function called
		//example use: time::delays::Delay<DemoClass, void, std::string>(time::s, 5, this, &DemoClass::functionToCall, "Delay with arguments");
		template <class ObjectType, class FunctionType, class arg>
		void Delay(TimeType timeType, long long delay, ObjectType* object, FunctionType(ObjectType::*func)(arg), arg argument)
		{
			auto m_StartTimepoint = std::chrono::high_resolution_clock::now();

			// convert from microseconds
			switch (timeType)
			{
			case ms: delay = delay * 1000; break;
			case s: delay = delay * 1000000; break;
			case min: delay = delay * 60000000;	break;
			default: break;
			}

			while (true)
			{
				auto timeNow = std::chrono::high_resolution_clock::now();

				auto whenStarted = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(timeNow).time_since_epoch().count();

				if (whenStarted + delay < now)
				{
					return ((*object).*(func))(argument);
				}
			}
		}

		// Delay that takes a lambda func
		template <class FunctionType>
		void Delay(TimeType timeType, long long delay, std::function<FunctionType()>lambdaFuncToCall)
		{
			auto m_StartTimepoint = std::chrono::high_resolution_clock::now();

			// convert from microseconds
			switch (timeType)
			{
			case ms: delay = delay * 1000; break;
			case s: delay = delay * 1000000; break;
			case min: delay = delay * 60000000;	break;
			default: break;
			}

			while (true)
			{
				auto timeNow = std::chrono::high_resolution_clock::now();

				auto whenStarted = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(timeNow).time_since_epoch().count();

				if (whenStarted + delay < now)
				{
					lambdaFuncToCall();
					return; // break from the loop
				}
			}
		}

		// Delay that takes a func ptr with argument pack
		template <class ObjectType, class FunctionType, typename ...ArgType, typename ...Args>
		FunctionType Delay(TimeType timeType, long long delay, ObjectType* obj, FunctionType(ObjectType::*func)(ArgType... type), Args... args)
		{
			auto m_StartTimepoint = std::chrono::high_resolution_clock::now();

			// convert from microseconds
			switch (timeType)
			{
			case ms: delay = delay * 1000; break;
			case s: delay = delay * 1000000; break;
			case min: delay = delay * 60000000;	break;
			default: break;
			}

			while (true)
			{
				auto timeNow = std::chrono::high_resolution_clock::now();

				auto whenStarted = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(timeNow).time_since_epoch().count();

				if (whenStarted + delay < now)
				{
					return ((*obj).*(func))(&args...);
				}
			}
		}

		// Delay without arguments in function called
		template <class ObjectType, class FunctionType>
		void Delay(TimeType timeType, long long delay, ObjectType* object, FunctionType(ObjectType::*func)())
		{
			auto m_StartTimepoint = std::chrono::high_resolution_clock::now();

			// convert from microseconds
			switch (timeType)
			{
			case ms: delay = delay * 1000; break;
			case s: delay = delay * 1000000; break;
			case min: delay = delay * 60000000;	break;
			default: break;
			}

			while (true)
			{
				auto timeNow = std::chrono::high_resolution_clock::now();

				auto whenStarted = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(timeNow).time_since_epoch().count();

				if (whenStarted + delay < now)
				{
					return ((*object).*(func))();
				}
			}
		}

		// Delay - no function pointer
		void Delay(TimeType timeType, long long delay)
		{
			auto m_StartTimepoint = std::chrono::high_resolution_clock::now();

			// convert from microseconds
			switch (timeType)
			{
			case ms: delay = delay * 1000; break;
			case s: delay = delay * 1000000; break;
			case min: delay = delay * 60000000;	break;
			default: break;
			}

			while (true)
			{
				auto timeNow = std::chrono::high_resolution_clock::now();

				auto whenStarted = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(timeNow).time_since_epoch().count();

				if (whenStarted + delay < now)
				{
					return;
				}
			}
		}
	}

	namespace benchmarking
	{
		class Timer
		{
		public:
			Timer(std::string functionName)
			{
				m_StartTimepoint = std::chrono::high_resolution_clock::now();
				nameOfTheFunction = functionName;
			}

			~Timer()
			{
				Stop();
			}

			void Stop()
			{
				auto endTimepoint = std::chrono::high_resolution_clock::now();

				auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
				auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

				auto duration = end - start;
				double ms = duration * 0.001;

				std::cout << "[" << nameOfTheFunction << "] took " << duration << "us (" << ms << "ms)\n\n";
			}

		private:
			std::chrono::time_point< std::chrono::high_resolution_clock > m_StartTimepoint;
			std::string nameOfTheFunction = "Name Not Set";
		};
	}

}