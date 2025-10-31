#pragma once

namespace ITF
{
	namespace sound
	{
		class [[nodiscard]] CallbackDisabler final
		{
		public:
			CallbackDisabler();
			~CallbackDisabler();
		};
	} // namespace sound
} // namespace ITF