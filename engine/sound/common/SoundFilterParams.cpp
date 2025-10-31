#include "precompiled_engine.h"
#include "engine/sound/common/SoundFilterParams.h"

namespace ITF
{
	namespace sound
	{
		FilterParams::FilterParams(Type _type, f32 _frequency)
			: m_type{ _type }
			, m_frequency{ _frequency }
		{
		}

		FilterParams::FilterParams(Type _type)
			: FilterParams(_type, getDefaultFrequency(_type))
		{
		}

		const ITF::sound::FilterParams& FilterParams::operator=(const FilterParams& _other)
		{
			m_frequency = _other.m_frequency;
			m_type = _other.m_type;

			return *this;
		}

		bool FilterParams::operator==(const FilterParams& _other) const
		{
			return (m_type == _other.m_type) && F32_ALMOST_EQUALS(m_frequency, _other.m_frequency, FREQ_ACCUARACY);
		}

		bool FilterParams::operator!=(const FilterParams& _other) const
		{
			return !(*this == _other);
		}

		bool FilterParams::operator>(const FilterParams& _other) const
		{
			if (m_type != _other.m_type)
				return m_type > _other.m_type;

			switch (m_type)
			{
			case Type::lowPass:
				return m_frequency < _other.m_frequency;

			case Type::highPass:
				return m_frequency > _other.m_frequency;
			default:
				// ITF_PASTA-wSwitch
				break;
			}

			return false;
		}

		bool FilterParams::operator<(const FilterParams& _other) const
		{
			return !(*this > _other);
		}

		FilterParams FilterParams::mix(const FilterParams& _other, const f32 _alpha)
		{
			if (_alpha <= 0.f)
				return *this;

			if (_alpha >= 1.f)
				return _other;

			if (getType() == _other.getType())
			{
				f32 mixFrequency = getFrequency() + _alpha * (_other.getFrequency() - getFrequency());

				return FilterParams(getType(), mixFrequency);
			}

			if (getType() == Type::off)
			{
				FilterParams filterParams(_other.getType());

				return filterParams.mix(_other, _alpha);
			}

			if (_other.getType() == Type::off)
			{
				FilterParams filterParams(getType());

				return mix(filterParams, _alpha);
			}

			if (_alpha < 0.5f)
			{
				f32 alphaToOff = 2.f * _alpha;

				FilterParams filterParams(Type::off);

				return mix(filterParams, alphaToOff);
			}

			f32 alphaFromOff = 2.f * (_alpha - 0.5f);

			FilterParams filterParams(Type::off);

			return filterParams.mix(_other, alphaFromOff);
		}
	} // namespace sound
} // namespace ITF
