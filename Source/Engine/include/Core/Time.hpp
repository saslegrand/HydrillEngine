#pragma once

#include <memory>
#include <string>

#include <EngineDLL.hpp>

/**
This class manage all time related variables and functions
*/
class Time
{
	//	Allow engine to call its private functions
	friend class Engine;
	
private:

	class Date
	{
		//	Let Time use its private functions
		friend Time;

	//	Constructors & Destructors

	public:

		Date();
		virtual ~Date();

	//	Variables

	private:

		std::unique_ptr<struct tm> timeStruct;

	//	Functions

	private:

		/**
		@brief Update date's datas
		*/
		void Update();

	public:

		/**
		@brief Get current second in the current minute

		@return Int : Current second
		*/
		inline int GetSecond() const;

		/**
		@brief Get current minute in the current hour
		*/
		inline int GetMinute() const;

		/**
		@brief Get current hour in the current day

		@return Int : Current hour
		*/
		inline int GetHour() const;

		/**
		@brief Get current day in the current month

		@return Int : Current day
		*/
		inline int GetDay() const;

		/**
		@brief Get current month in the current year

		@return Int : Current month
		*/
		inline int GetMonth() const;

		/**
		@brief Get current year in Common Era

		@return Int : Current year (CE)
		*/
		inline int GetYear() const;

		/**
		@brief Get this current day number in this week

		@return Int : Day of the week
		*/
		inline int GetDayOfTheWeek() const;

		/**
		@brief Get this day number in this year

		@return Int : Day of the year
		*/
		inline int GetDayOfTheYear() const;

		/**
		@brief Get date character string

		@param format : string format of the date

		@return String : Date in string with a chosen format
		*/
		ENGINE_API std::string GetDateString(const char* format = "%Y-%m-%d.%X");
	};

//	Constructors & Destructors

public:

	Time();

//	Variables

private:

	float m_timeScale = 1.0f;

	float m_unscaledDeltaTime = 0.0f;
	float m_deltaTime = 0.0f;

	float m_fixedDeltaTime = 1.0f / 60.f;

	float  m_FPSThreshold = 2000.f;
	float  m_FPS = 0.0f;

public:

	/**
	@brief This variable contains date's data like days, months, years, hour, minutes and seconds...
	*/
	Date date;

//	Functions

private:

	/**
	@brief Update time variables
	Must be called one time at the beginning of each frame
	*/
	void Update();

public:

	/**
	@brief Set time scale (default value = 1)
	
	@param New time scale
	*/
	inline void SetTimeScale(const float timeScale = 1.f);

	/**
	@brief Return the current time scale

	@param timeScale : New time scale

	@return Float : TimeScale
	*/
	inline float GetTimeScale() const;

	/**
	@brief Return scaled deltatime which is the time in seconds between two frame multiplied by the time scale

	@return Float : DeltaTime
	*/
	inline float GetDeltaTime() const;

	/**
	@brief Return deltatime which is the time between two frame in seconds

	@return Float : UnscaledDeltaTime
	*/
	inline float GetUnscaledDeltaTime() const;

	/**
	@brief Return the fixed deltatime, time between each fixed updates

	@return Float : FixedDeltaTime
	*/
	inline float GetFixedDeltaTime() const;

	/**
	@brief Return current Frame per second value, useful to check performance

	@return Float : FPS
	*/
	inline float GetFPS() const;

	/**
	@brief Return the FPS threshold/limitation, FPS should not be able to reach greater values than the FPS threshold/limitation

	@return Float : FPS Threshold
	*/
	inline float GetFPSThreshold() const;

	/**
	@brief Set FPS threshold/limitation, it could be used to gain some performances

	@param FPSLimit : New FPS threshold
	*/
	inline void SetFPSThreshold(const float FPSLimit);
};

#include "Time.inl"