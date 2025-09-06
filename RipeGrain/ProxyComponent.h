#pragma once
#include <concepts>
#include <type_traits>

namespace RipeGrain
{

	// specialize this template for components to be recognized
	// as proxy components by the engine
	// the specialization must inherit from std::true_type
	template <typename T>
	class ProxyComponent : public std::false_type 
	{
		// overload this type alias to the type of the proxy
		// in the specialization
		using proxy_type = void;
	};

	template <typename T>
	concept proxy_specialized = ProxyComponent<T>::value;

	template <typename T>
	concept has_proxy_type = requires 
	{
		typename ProxyComponent<T>::proxy_type;
	} && !std::is_same_v<typename ProxyComponent<T>::proxy_type, void>;

	template <typename T>
	concept proxy_constructable = std::constructible_from<typename ProxyComponent<T>::proxy_type , T&>;

	template <typename T>
	concept ProxyAbleComponent = proxy_specialized<T> && has_proxy_type<T> && proxy_constructable<T> && std::default_initializable<T>;
}