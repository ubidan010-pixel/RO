#pragma once

#if defined(ITF_PASTA_STEAMBOT) && defined(ITF_SNOWDROP)
#include "MC_Core.h"
#endif

namespace ITF
{
	namespace sound
	{
		class FilterParams final
		{

		public:
			enum class Type
			{
				off,
				lowPass,
				highPass
			};

		public:
			FilterParams() = default;
			explicit FilterParams(Type _type);
			FilterParams(Type _type, f32 _frequency);
			FilterParams(const FilterParams& _other) = default;

			const FilterParams& operator=(const FilterParams& _other);

			bool operator==(const FilterParams& _other) const;
			bool operator!=(const FilterParams& _other) const;

			bool operator>(const FilterParams& _other) const;
			bool operator<(const FilterParams& _other) const;

			// 0.f return this, 1.f return _other
			FilterParams mix(const FilterParams& _other, const f32 _alpha);

			Type getType() const { return m_type; }
			f32 getFrequency() const { return m_frequency; }
			void setFilter(Type _type, f32 _frequency)
			{
				m_type = _type;
				m_frequency = _frequency;
			}

		private:
			constexpr static f32 FREQ_ACCUARACY = 1.f;

			constexpr static f32 getDefaultFrequency(const Type _type)
			{
				// values are coming from raki
				switch (_type)
				{
				case Type::lowPass:
					return 24000.f;

				case Type::highPass:
					return 10.f;

				default:
					return 0.f;
				}
			}

		private:
			Type m_type{ Type::off };
			f32 m_frequency{ 0.f };
		};
	} // namespace sound
} // namespace ITF