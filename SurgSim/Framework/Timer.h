// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SURGSIM_FRAMEWORK_TIMER_H
#define SURGSIM_FRAMEWORK_TIMER_H

#include <boost/chrono.hpp>
#include <deque>

namespace SurgSim
{
namespace Framework
{

/// Timer class, measures execution times.  Multiple times can be stored as "frames" to provide an average rate or
/// period.
class Timer
{
public:
	/// The Clock used by the Timer class.
	typedef boost::chrono::steady_clock TimerClock;

	/// Durations used by the Timer class.
	typedef boost::chrono::duration<double> TimerDuration;

	/// Time points used by the Timer class.
	typedef boost::chrono::time_point<TimerClock, TimerDuration> TimerTimePoint;

	/// Instantiate a TimerClock and start a timing run.
	Timer();

	/// Begin a timing run by storing the current time and clearing out the stored frames.
	void start();

	/// Begin a frame by storing the current time.
	void beginFrame();

	/// End this frame by storing the duration since the last \c start or \c beginFrame.  Also stores the current time.
	/// Asserts if the timer is stopped.
	void endFrame();

	/// Return the time between frames, on average.  Asserts if there are no frames.
	/// \return Average period in seconds.
	double getAverageFramePeriod() const;

	/// \return The average frequency in Hz.
	double getAverageFrameRate() const;

	/// Return the time between the last frame step and the previous frame step or start.  Asserts if there are no
	/// frames.
	/// \return Most-recent period in seconds.
	double getLastFramePeriod() const;

	/// \return Most-recent frequency in Hz.
	double getLastFrameRate() const;

	/// Set the maximum number of frames to store.
	void setNumberOfFrames(size_t numberOfFrames);

	/// \return Number of frames currently stored (not the maximum number of frames).
	size_t getCurrentNumberOfFrames() const;

	/// \return Number of times the clock returned an error code since \c start.
	size_t getNumberOfClockFails() const;

private:
	/// Get the current time.  Handles any error codes from the clock.  May assert if the clock fails.
	/// \return Current time.
	TimerTimePoint now();

	/// The clock used to get the time.
	static const TimerClock m_clock;

	/// The time at last \c start or \c frameStep.
	TimerTimePoint m_lastTime;

	/// Is this Timer stopped?
	bool m_stopped;

	/// Number of frames to average.
	size_t m_maxNumberOfFrames;

	/// Durations of the frames.
	std::deque<TimerDuration> m_frameDurations;

	/// Number of clock errors since start.
	size_t m_clockFails;
};

} // Framework
} // SurgSim

#endif
