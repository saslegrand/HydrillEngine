#include <Core/Time.hpp>

#include <chrono>

#define NANOSEC_TO_SEC 0.000000001f

Time::Time()
{
}

void Time::Update()
{
	//	Static variables declarations

	static auto prevTime = std::chrono::steady_clock::now();

	//	Local variables

	auto  currentTime = std::chrono::steady_clock::now();
	float  delta = (currentTime - prevTime).count() * NANOSEC_TO_SEC;
	float FPSlimit = 1.0f / m_FPSThreshold;

	//	Frame per seconds limitations
	//while (delta < FPSlimit)
	//{
	//	currentTime = std::chrono::steady_clock::now();
	//	delta = (currentTime - prevTime).count() * NANOSEC_TO_SEC;
	//}
	prevTime = currentTime;

	//	Set values
	m_unscaledDeltaTime = delta;
	m_deltaTime = m_unscaledDeltaTime * m_timeScale;
	m_FPS = (1.0f / (float)m_unscaledDeltaTime);
	
	//	Update date component
	date.Update();
}

//	DATE FUNCTIONS

Time::Date::Date()
{
	timeStruct = std::make_unique<tm>();

	time_t now = time(0);
	localtime_s(timeStruct.get(), &now);
}

Time::Date::~Date()
{
}

void Time::Date::Update()
{
	time_t now = time(0);
	localtime_s(timeStruct.get(), &now);
}


std::string Time::Date::GetDateString(const char* format)
{
	char buf[80];

	strftime(buf, sizeof(buf), format, timeStruct.get());

	std::string out = buf;

	return out;
}
