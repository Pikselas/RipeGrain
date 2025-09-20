#pragma once
#include <tuple>

namespace RipeGrain
{
	namespace Helpers
	{
		template<typename T, typename... Rest>
		struct last_type
		{
			using type = typename last_type<Rest...>::type;
		};

		template<typename T>
		struct last_type<T>
		{
			using type = T;
		};

		template<typename... Params>
		using LastParamT = typename last_type<Params...>::type;

		template<typename... Params>
		inline LastParamT<Params...>&& GetLastParam(Params&&... p)
		{
			auto t = std::forward_as_tuple(std::forward<Params>(p)...);
			constexpr auto size = std::tuple_size<decltype(t)>::value;
			return std::get<size - 1>(std::move(t));
		}

		template<typename... ParamT, size_t... I>
		inline auto strip_tuple_helper(std::tuple<ParamT&&...>&& t, std::index_sequence<I...>)
		{
			return std::forward_as_tuple(std::get<I>(t)...);
		}

		template<typename... ParamT>
		inline auto strip_tuple(std::tuple<ParamT&&...>&& t)
		{
			constexpr size_t size = sizeof...(ParamT);
			return strip_tuple_helper<ParamT&&...>(std::move(t), std::make_index_sequence<size - 1>{});
		}
	}
}