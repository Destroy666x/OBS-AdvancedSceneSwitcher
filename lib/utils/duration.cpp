#include "duration.hpp"
#include "obs-module-helper.hpp"

#include <sstream>
#include <iomanip>

namespace advss {

Duration::Duration(double initialValueInSeconds) : _value(initialValueInSeconds)
{
}

void Duration::Save(obs_data_t *obj, const char *name) const
{
	auto data = obs_data_create();
	_value.Save(data, "value");
	obs_data_set_int(data, "unit", static_cast<int>(_unit));
	obs_data_set_int(data, "version", 1);
	obs_data_set_obj(obj, name, data);
	obs_data_release(data);
}

void Duration::Load(obs_data_t *obj, const char *name)
{
	auto data = obs_data_get_obj(obj, name);

	// TODO: remove this fallback
	if (!data || !obs_data_has_user_value(data, "version") ||
	    obs_data_get_int(data, "version") != 1) {
		bool usingDefaultArgs = strcmp("duration", name) == 0;
		if (usingDefaultArgs) {
			_value = obs_data_get_double(obj, "seconds");
		}
		if (_value.GetValue() == 0.0) {
			_value = obs_data_get_double(obj, name);
		}

		if (usingDefaultArgs) {
			_unit = static_cast<Duration::Unit>(
				obs_data_get_int(obj, "displayUnit"));
		} else if (_value.GetValue() >= 86400) {
			_unit = (_value.GetValue() / 60 >= 86400)
					? Duration::Unit::HOURS
					: Duration::Unit::MINUTES;
		} else {
			_unit = Duration::Unit::SECONDS;
		}

		_value = _value / ConvertUnitToMultiplier(_unit);

		obs_data_release(data);
		return;
	}

	_value.Load(data, "value");
	_unit = static_cast<Duration::Unit>(obs_data_get_int(data, "unit"));
	obs_data_release(data);
}

bool Duration::DurationReached()
{
	if (IsReset()) {
		_startTime = std::chrono::high_resolution_clock::now();
	}

	auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now() - _startTime);
	return runTime.count() >= Milliseconds();
}

bool Duration::IsReset() const
{
	return _startTime.time_since_epoch().count() == 0;
}

double Duration::Seconds() const
{
	return _value.GetValue() * ConvertUnitToMultiplier(_unit);
}

double Duration::Milliseconds() const
{
	return Seconds() * 1000.0;
}

double Duration::TimeRemaining() const
{
	if (IsReset()) {
		return Seconds();
	}
	auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now() - _startTime);

	if (runTime.count() >= Milliseconds()) {
		return 0;
	}
	return (Milliseconds() - runTime.count()) / 1000.0;
}

void Duration::SetTimeRemaining(double remaining)
{
	long long msPassed = (long long)((Seconds() - remaining) * 1000);
	_startTime = std::chrono::high_resolution_clock::now() -
		     std::chrono::milliseconds(msPassed);
}

void Duration::Reset()
{
	_startTime = {};
}

static std::string durationUnitToString(Duration::Unit u)
{
	switch (u) {
	case Duration::Unit::SECONDS:
		return obs_module_text("AdvSceneSwitcher.unit.seconds");
	case Duration::Unit::MINUTES:
		return obs_module_text("AdvSceneSwitcher.unit.minutes");
	case Duration::Unit::HOURS:
		return obs_module_text("AdvSceneSwitcher.unit.hours");
	case Duration::Unit::DAYS:
		return obs_module_text("AdvSceneSwitcher.unit.days");
	case Duration::Unit::WEEKS:
		return obs_module_text("AdvSceneSwitcher.unit.weeks");
	case Duration::Unit::MONTHS:
		return obs_module_text("AdvSceneSwitcher.unit.months");
	case Duration::Unit::YEARS:
		return obs_module_text("AdvSceneSwitcher.unit.years");
	default:
		break;
	}

	return "";
}

std::string Duration::ToString() const
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2) << _value.GetValue() << " "
	   << durationUnitToString(_unit);
	if (!_value.IsFixedType()) {
		ss << " [" << GetWeakVariableName(_value.GetVariable()) << "]";
	}
	return ss.str();
}

int Duration::ConvertUnitToMultiplier(Unit unit)
{
	switch (unit) {
	case Unit::SECONDS:
		return 1;
	case Unit::MINUTES:
		return 60;
	case Unit::HOURS:
		return 3600;
	case Unit::DAYS:
		return 86400;
	case Unit::WEEKS:
		return 604800;
	case Unit::MONTHS:
		return 2629746;
	case Unit::YEARS:
		return 31556952;
	default:
		break;
	}

	return 0;
}

void Duration::SetUnit(Unit u)
{
	double prevMultiplier = ConvertUnitToMultiplier(_unit);
	double newMultiplier = ConvertUnitToMultiplier(u);
	_unit = u;
	_value = _value * (prevMultiplier / newMultiplier);
}

void Duration::ResolveVariables()
{
	_value.ResolveVariables();
}

} // namespace advss
