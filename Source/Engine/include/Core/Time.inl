
//	Gets functions of Time class

float Time::GetTimeScale() const
{
	return m_timeScale;
}

float Time::GetDeltaTime() const
{
	return m_deltaTime;
}

float Time::GetUnscaledDeltaTime() const
{
	return m_unscaledDeltaTime;
}

float Time::GetFixedDeltaTime() const
{
 return m_fixedDeltaTime; 
}

float  Time::GetFPS() const 
{
 return m_FPS; 
}

float  Time::GetFPSThreshold() const 
{
 return m_FPSThreshold; 
}

//	Sets functions of Time class

void Time::SetFPSThreshold(const float FPSLimit) 
{
	m_FPSThreshold = FPSLimit;
}

void Time::SetTimeScale(const float timeScale)
{
	m_timeScale = timeScale;
}



//	Gets functions of Time::Date class

int Time::Date::GetSecond() const
{
	return timeStruct->tm_sec;
}

int Time::Date::GetMinute() const 
{
	return timeStruct->tm_min;
}

int Time::Date::GetHour() const 
{
	return timeStruct->tm_hour;
}

int Time::Date::GetDay() const 
{
	return timeStruct->tm_mday;
}

int Time::Date::GetMonth() const 
{
	return timeStruct->tm_mon + 1;
}

int Time::Date::GetYear() const 
{
	return timeStruct->tm_year + 1900;
}

int Time::Date::GetDayOfTheWeek() const 
{ 
	return timeStruct->tm_wday;
}

int Time::Date::GetDayOfTheYear() const 
{ 
	return timeStruct->tm_yday;
}