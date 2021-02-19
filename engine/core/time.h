#pragma once

namespace ce {
	class Time {
	 private:
		double m_now, m_last, m_dt, m_fps;

	 public:
		Time();
		~Time() {};

		void update();

		double getDeltaTime() { return m_dt; }
		double getFPS() { return m_fps; };

		static double getTime();
	};
}